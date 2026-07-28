#include "ArcFaceEngine.hpp"

#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace util {
	ArcFaceEngine::ArcFaceEngine() : m_engine(nullptr), m_initialized(false) {}
	ArcFaceEngine::~ArcFaceEngine() { uninitialize(); }

	ArcFaceEngine& ArcFaceEngine::instance() {
		static ArcFaceEngine inst;
		return inst;
	}

	bool ArcFaceEngine::initialize(const std::string& app_id, const std::string& sdk_key)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_initialized.load(std::memory_order_acquire)) {
			return true;
		}

		// 在线激活（若服务器无法访问外网，可改用离线激活：
	//   1. 在可联网机器上使用 ASFOfflineActivation 生成授权文件
	//   2. 将授权文件部署至服务器，使用 ASFOfflineActivationFile 进行激活）
		MRESULT result = ASFOnlineActivation(
			const_cast<char*>(app_id.c_str()),
			const_cast<char*>(sdk_key.c_str())
		);

		if (MOK != result && MERR_ASF_ALREADY_ACTIVATED != result) {
			return false;
		}

		MInt32 combinedMask = ASF_FACE_DETECT | ASF_FACERECOGNITION;

		result = ASFInitEngine(
			ASF_DETECT_MODE_IMAGE,
			ASF_OP_0_ONLY,
			kDetectFaceScaleVal,
			kDetectFaceMaxNum,
			combinedMask,
			&m_engine
		);

		if (result != MOK) {
			return false;
		}

		m_initialized.store(true, std::memory_order_release);
		return true;
	}

	void ArcFaceEngine::uninitialize() {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_initialized.load(std::memory_order_acquire) && m_engine) {
			ASFUninitEngine(m_engine);
			m_engine = nullptr;
			m_initialized.store(false, std::memory_order_release);
		}
	}

	bool ArcFaceEngine::is_initialized() const {
		return m_initialized.load(std::memory_order_acquire);
	}

	FaceExtractResult ArcFaceEngine::extract_feature_from_image_file(const std::string& image_path) {
		if (!m_initialized.load(std::memory_order_acquire)) {
			return { false,FaceExtractError::EngineNotInitialized,"ArcFace engine not initialized,{},{}" };
		}

		int width, height, channels;
		unsigned char* img_data = stbi_load(image_path.c_str(), &width, &height, &channels, 3);

		if (!img_data) {
			return { false,FaceExtractError::ImageLoadFailed,"Cannot open or decode image file:" + image_path,{},{} };
		}

		std::vector<unsigned char> bgr_data(width * height * 3);
		for (int i = 0; i < width * height; ++i) {
			bgr_data[i * 3 + 0] = img_data[i * 3 + 2];
			bgr_data[i * 3 + 1] = img_data[i * 3 + 1];
			bgr_data[i * 3 + 2] = img_data[i * 3 + 0];
		}

		stbi_image_free(img_data);

		int aligned_width = (width / 4) * 4;
		if (aligned_width != width) {
			std::vector<unsigned char> aligned_data(aligned_width * height * 3);
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < aligned_width; ++x) {
					for (int c = 0; c < 3; ++c) {
						aligned_data[(y * aligned_width + x) * 3 + c] =
							bgr_data[(y * width + x) * 3 + c];
					}
				}
			}
			return do_extract_feature(aligned_data.data(), aligned_width, height);
		}

		return do_extract_feature(bgr_data.data(), width, height);
	}

	FaceExtractResult ArcFaceEngine::do_extract_feature(void* img_data, int width, int height) {
		std::lock_guard<std::mutex> lock(m_mutex);

		ASVLOFFSCREEN asvl = { 0 };
		asvl.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
		asvl.i32Width = width;
		asvl.i32Height = height;
		asvl.ppu8Plane[0] = static_cast<MUInt8*>(img_data);
		asvl.pi32Pitch[0] = width * 3;

		ASF_MultiFaceInfo detected_faces = { 0 };
		MRESULT result = ASFDetectFacesEx(m_engine, &asvl, &detected_faces, ASF_DETECT_MODEL_RGB);

		if (result != MOK) {
			return { false, FaceExtractError::FeatureExtractFailed,
					"Face detection failed", {}, {} };
		}

		if (detected_faces.faceNum == 0) {
			return { false, FaceExtractError::NoFaceDetected,
					"No face detected in image", {}, {} };
		}

		if (detected_faces.faceNum > 1) {
			return { false, FaceExtractError::MultipleFacesDetected,
					"Multiple faces detected, expected single face", {}, {} };
		}

		int face_width = detected_faces.faceRect[0].right - detected_faces.faceRect[0].left;
		int face_height = detected_faces.faceRect[0].bottom - detected_faces.faceRect[0].top;
		if (face_width < kMinFaceSize || face_height < kMinFaceSize) {
			return { false, FaceExtractError::FaceTooSmall,
					"Face size too small, minimum " + std::to_string(kMinFaceSize) + " pixels", {}, {} };
		}

		ASF_SingleFaceInfo single_face_info = { 0 };
		single_face_info.faceRect = detected_faces.faceRect[0];
		single_face_info.faceOrient = detected_faces.faceOrient[0];

		ASF_FaceFeature sdk_feature = { 0 };
		result = ASFFaceFeatureExtractEx(m_engine, &asvl, &single_face_info, &sdk_feature);

		if (result != MOK) {
			return { false, FaceExtractError::FeatureExtractFailed,
					"Feature extraction failed", {}, {} };
		}

		FaceFeature feature;
		feature.size = sdk_feature.featureSize;
		feature.data.assign(
			reinterpret_cast<char*>(sdk_feature.feature),
			reinterpret_cast<char*>(sdk_feature.feature) + sdk_feature.featureSize
		);

		FaceInfo face_info;
		face_info.left = detected_faces.faceRect[0].left;
		face_info.top = detected_faces.faceRect[0].top;
		face_info.right = detected_faces.faceRect[0].right;
		face_info.bottom = detected_faces.faceRect[0].bottom;
		face_info.orient = detected_faces.faceOrient[0];

		return { true, FaceExtractError::None, "Success", feature, face_info };
	}
} // namespace util