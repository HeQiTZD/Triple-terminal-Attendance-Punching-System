#pragma once

#include <atomic>
#include <string>
#include <vector>
#include <mutex>

#include "../arcface/include/arcsoft_face_sdk.h"
#include "../arcface/include/merror.h"
namespace util {

	struct FaceInfo {
		int left;
		int top;
		int right;
		int bottom;
		int orient;
	};

	struct FaceFeature {
		std::vector<char> data;
		int size;
		bool is_valid() const { return size > 0 && !data.empty(); }
	};

	enum class FaceExtractError {
		None = 0,
		EngineNotInitialized,
		ImageLoadFailed,
		NoFaceDetected,
		MultipleFacesDetected,
		FaceTooSmall,
		FeatureExtractFailed,
	};

	struct FaceExtractResult {
		bool success;
		FaceExtractError error;
		std::string error_message;
		FaceFeature feature;
		FaceInfo face_info;
	};

	class ArcFaceEngine {
	public:
		static ArcFaceEngine& instance();

		bool initialize(const std::string& app_id, const std::string& sdk_key);
		void uninitialize();
		bool is_initialized() const;

		FaceExtractResult extract_feature_from_image_file(const std::string& image_path);

	private:
		ArcFaceEngine();
		~ArcFaceEngine();
		ArcFaceEngine(const ArcFaceEngine&) = delete;
		ArcFaceEngine& operator=(const ArcFaceEngine&) = delete;

		FaceExtractResult do_extract_feature(void* img_data, int width, int height);

		MHandle m_engine;
		std::atomic<bool> m_initialized;
		std::mutex m_mutex;

		static constexpr int kMinFaceSize = 80;
		static constexpr int kDetectFaceScaleVal = 32;
		static constexpr int kDetectFaceMaxNum = 2;
	};
}// namespace util