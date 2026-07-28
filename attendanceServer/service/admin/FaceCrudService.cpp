#include "service/admin/FaceCrudService.hpp"

#include <ctime>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include "db/FaceDataRepository.hpp"
#include "db/OperationLogRepository.hpp"
#include "net/Session.hpp"
#include "protocol/AppError.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"
// AuditContext is defined in AdminCrudService.hpp; include it for the full
// struct definition (only forward-declared in FaceCrudService.hpp).
#include "service/AdminCrudService.hpp"
#include "service/IDbExecutor.hpp"
#include "util/ArcFaceEngine.hpp"
#include "util/Base64.hpp"

namespace service {

namespace {

std::string opt_str(const nlohmann::json& data, const char* key) {
  if (!data.contains(key) || !data[key].is_string()) {
    return {};
  }
  return data[key].get<std::string>();
}

bool opt_bool(const nlohmann::json& data, const char* key, bool default_val) {
  if (!data.contains(key) || !data[key].is_boolean()) {
    return default_val;
  }
  return data[key].get<bool>();
}

void send_db_error(const std::shared_ptr<net::ISession>& session,
                   std::exception_ptr ep, const std::string& msg_id,
                   const std::string& to) {
  try {
    std::rethrow_exception(ep);
  } catch (const mysqlx::Error& e) {
    const protocol::MappedError me = protocol::map_mysqlx_error(e);
    session->write_line(protocol::build_error(me.code, me.msg, msg_id, to));
  } catch (const protocol::AppError& e) {
    session->write_line(protocol::build_error(
        e.numeric_code(), e.what(), msg_id, to));
  } catch (const std::invalid_argument& e) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, e.what(), msg_id, to));
  } catch (const std::exception& ex) {
    session->write_line(protocol::build_error(protocol::kCodeDbError,
                                              ex.what(), msg_id, to));
  }
}

int map_face_extract_error(util::FaceExtractError error) {
  switch (error) {
  case util::FaceExtractError::EngineNotInitialized:
    return protocol::kCodeBusinessValidation;
  case util::FaceExtractError::ImageLoadFailed:
    return 4005;
  case util::FaceExtractError::NoFaceDetected:
    return 4001;
  case util::FaceExtractError::MultipleFacesDetected:
    return 4002;
  case util::FaceExtractError::FaceTooSmall:
    return 4003;
  case util::FaceExtractError::FeatureExtractFailed:
    return 4004;
  default:
    return protocol::kCodeBusinessValidation;
  }
}

}  // namespace

// ---------------------------------------------------------------------------
// FaceData query/delete/register
// ---------------------------------------------------------------------------

void admin_face_query(const protocol::ParsedEnvelope& env,
                      const std::string& reply_to, IDbExecutor& db,
                      const std::shared_ptr<net::ISession>& session) {
  const std::string employee_id = opt_str(env.data, "employeeId");
  if (employee_id.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing required field: employeeId",
        env.msg_id, reply_to));
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;

  auto result = std::make_shared<std::optional<db::FaceDataRecord>>();

  db.dispatch(
      [employee_id, result](mysqlx::Session& s) {
        *result = db::FaceDataRepository::findByEmployeeId(s, employee_id);
      },
      [session, msg_id, to, result](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        nlohmann::json data;
        if (result->has_value()) {
          const auto& r = result->value();
          data["found"] = true;
          data["employeeId"] = r.employee_id;
          data["featureSize"] = r.feature_size;
          data["createdAt"] = r.created_at;
          data["updatedAt"] = r.updated_at;
        } else {
          data["found"] = false;
        }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeFaceQueryResponse, msg_id, to, 0, "ok", &data));
      });
}

void admin_face_delete(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit) {
  const std::string employee_id = opt_str(env.data, "employeeId");
  if (employee_id.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing required field: employeeId",
        env.msg_id, reply_to));
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [employee_id, audit](mysqlx::Session& s) {
        db::FaceDataRepository::deleteByEmployeeId(s, employee_id);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "face.delete";
        log.target_type = "FaceData";
        log.target_id = employee_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeFaceDeleteResponse, msg_id, to, 0, "ok", nullptr));
      });
}

void admin_face_register(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to,
                         IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit) {
    const std::string employee_id = opt_str(env.data, "employeeId");
    const std::string photo_base64 = opt_str(env.data, "photoBase64");
    const bool overwrite = opt_bool(env.data, "overwrite", false);

    if (employee_id.empty()) {
        session->write_line(protocol::build_error(
            protocol::kCodeBusinessValidation,
            "missing required field: employeeId",
            env.msg_id,reply_to));
        return;
    }

    if (photo_base64.empty()) {
        session->write_line(protocol::build_error(
            protocol::kCodeBusinessValidation,
            "missing required field: photoBase64",
            env.msg_id, reply_to));
        return;
    }

    std::vector<unsigned char> photo_data;
    if (!util::base64_decode(photo_base64, photo_data)) {
        session->write_line(protocol::build_error(
            protocol::kCodeBusinessValidation,
            "invalid base64 encoding for photoBase64",
            env.msg_id, reply_to));
        return;
    }

    const std::string msg_id = env.msg_id;
    const std::string to = reply_to;

    // 加入随机数避免同一秒内并发请求的临时文件冲突
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    std::string temp_dir = std::filesystem::temp_directory_path().string();
    std::string temp_file = temp_dir + "/face_register_" + employee_id + "_"
        + std::to_string(std::time(nullptr)) + "_" + std::to_string(dis(gen)) + ".jpg";

    {
        std::ofstream ofs(temp_file, std::ios::binary);
        if (!ofs.is_open()) {
            session->write_line(protocol::build_error(
                protocol::kCodeBusinessValidation,
                "failed to create temporary file",
                msg_id, to));
            return;
        }
        ofs.write(reinterpret_cast<const char*>(photo_data.data()), photo_data.size());
    }

    auto person_exists = std::make_shared<bool>(false);
    auto face_exists = std::make_shared<bool>(false);

    db.dispatch(
        [employee_id, person_exists, face_exists](mysqlx::Session& s) {
            auto result = s.sql("SELECT 1 FROM Person WHERE employee_id = ? LIMIT 1")
                .bind(employee_id)
                .execute();
            auto person_row = result.fetchOne();
            *person_exists = static_cast<bool>(person_row);

            if (*person_exists) {
                auto face_result = s.sql("SELECT 1 FROM face_data WHERE employee_id = ? LIMIT 1")
                    .bind(employee_id)
                    .execute();
                auto face_row = face_result.fetchOne();
                *face_exists = static_cast<bool>(face_row);
            }
        },
        [session, msg_id, to, employee_id, temp_file, overwrite,
        person_exists, face_exists, &db, audit]
        (std::exception_ptr ep) mutable {
            if (ep) {
                std::filesystem::remove(temp_file);
                send_db_error(session, ep, msg_id, to);
                return;
            }

            if (!*person_exists) {
                std::filesystem::remove(temp_file);
                session->write_line(protocol::build_error(
                    4006, "employee not found", msg_id, to));
                return;
            }

            if (*face_exists && !overwrite) {
                std::filesystem::remove(temp_file);
                session->write_line(protocol::build_error(
                    4007, "face data already exists for this employee, set overwrite=true to replace",
                    msg_id, to));
                return;
            }

            util::ArcFaceEngine& engine = util::ArcFaceEngine::instance();
            if (!engine.is_initialized()) {
                std::filesystem::remove(temp_file);
                session->write_line(protocol::build_error(
                    protocol::kCodeBusinessValidation,
                    "face recognition engine not initialized",
                    msg_id, to));
                return;
            }

            util::FaceExtractResult result = engine.extract_feature_from_image_file(temp_file);

            std::filesystem::remove(temp_file);

            if (!result.success) {
                int error_code = map_face_extract_error(result.error);
                session->write_line(protocol::build_error(
                    error_code, result.error_message, msg_id, to));
                return;
            }

            db::FaceDataUpsertInput input;
            input.employee_id = employee_id;
            input.feature_vector.assign(result.feature.data.begin(),
                result.feature.data.end());
            input.feature_size = result.feature.size;

            db.dispatch(
                [input, employee_id, audit](mysqlx::Session& s) {
                    db::FaceDataRepository::upsert(s, input);
                    db::OperationLogEntry log;
                    log.user_id = audit.user_id;
                    log.employee_id = audit.employee_id;
                    log.action = "face.register";
                    log.target_type = "FaceData";
                    log.target_id = employee_id;
                    log.ip_address = audit.ip_address;
                    log.result = "success";
                    db::OperationLogRepository::insertLog(s, log);
                },
                [session, msg_id, to, employee_id, result, face_exists](std::exception_ptr ep) {
                    if (ep) {
                        send_db_error(session, ep, msg_id, to);
                        return;
                    }

                    nlohmann::json data;
                    data["employeeId"] = employee_id;
                    data["featureSize"] = result.feature.size;
                    data["faceRect"] = {
                        {"left", result.face_info.left},
                        {"top", result.face_info.top},
                        {"right", result.face_info.right},
                        {"bottom", result.face_info.bottom}
                    };
                    // 注意：由于两次 DB dispatch 之间存在时间窗口，
                    // isNew 在极端并发场景下可能出现不准确的情况，
                    // 但 DB 层 ON DUPLICATE KEY UPDATE 保证数据不会丢失。
                    data["isNew"] = !*face_exists;

                    session->write_line(protocol::build_report_ack(
                        protocol::kTypeFaceRegisterResponse, msg_id, to, 0, "ok", &data));
                });
        });
}

}  // namespace service
