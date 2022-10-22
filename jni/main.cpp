#include <android/log.h>
#include <jni.h>
#include <optional>

#include <librealsense2/rs.hpp>

#define LOG(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, "T265", fmt, ##__VA_ARGS__)
#define JNI_FUNC      extern "C" JNIEXPORT jobject JNICALL

#undef NDEBUG
#include <assert.h>

namespace {
std::optional<rs2::pipeline> s_pipeline;
jclass s_pose2d = nullptr, s_rotation2d = nullptr;
jmethodID s_pose2d_init = nullptr, s_rotation2d_init = nullptr;
constexpr jint JNI_VERSION = JNI_VERSION_1_6;

jobject create_pose2d(JNIEnv* env, const rs2_pose& pose)
{
    auto rot = env->NewObject(s_rotation2d, s_rotation2d_init); // FIXME: Actually calculate this
    return env->NewObject(s_pose2d, s_pose2d_init, static_cast<jdouble>(pose.translation.x), static_cast<jdouble>(-pose.translation.z), rot);
}
}

jint JNI_OnLoad(JavaVM* vm, void*)
{
    JNIEnv* env = nullptr;
    vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION);

    auto pose2d = env->FindClass("com/arcrobotics/ftclib/geometry/Pose2d");
    s_pose2d = static_cast<jclass>(env->NewGlobalRef(pose2d));
    s_pose2d_init = env->GetMethodID(pose2d, "<init>", "(DDLcom/arcrobotics/ftclib/geometry/Rotation2d;)V");
    env->DeleteLocalRef(pose2d);

    auto rotation2d = env->FindClass("com/arcrobotics/ftclib/geometry/Rotation2d");
    s_rotation2d = static_cast<jclass>(env->NewGlobalRef(rotation2d));
    s_rotation2d_init = env->GetMethodID(rotation2d, "<init>", "()V");
    env->DeleteLocalRef(rotation2d);

    return JNI_VERSION;
}

void JNI_OnUnload(JavaVM* vm, void*)
{
    JNIEnv* env = nullptr;
    vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION);
    env->DeleteGlobalRef(s_pose2d);
    env->DeleteGlobalRef(s_rotation2d);
}

JNI_FUNC Java_org_firstinspires_ftc_teamcode_Utils_Odometry_start(JNIEnv*, jobject)
{
    assert(!s_pipeline.has_value());
    try {
        s_pipeline.emplace();
        rs2::config cfg;
        cfg.enable_stream(RS2_STREAM_POSE, RS2_FORMAT_6DOF);
        s_pipeline->start(cfg);
    } catch (const rs2::error& e) {
        LOG("Camera failure: %s(%s): %s", e.get_failed_function().c_str(), e.get_failed_args().c_str(), e.what());
    } catch (const std::exception& e) {
        LOG("Unknown failure: %s", e.what());
    }
    return nullptr;
}

JNI_FUNC Java_org_firstinspires_ftc_teamcode_Utils_Odometry_stop(JNIEnv*, jobject)
{
    assert(s_pipeline.has_value());
    s_pipeline->stop();
    s_pipeline.reset();
    return nullptr;
}

JNI_FUNC Java_org_firstinspires_ftc_teamcode_Utils_Odometry_getPoseMeters(JNIEnv* env, jobject)
{
    try {
        auto frames = s_pipeline->wait_for_frames();
        auto frame = frames.first(RS2_STREAM_POSE);
        auto pose = frame.as<rs2::pose_frame>().get_pose_data();
        LOG("%10.3f %10.3f %10.3f", pose.translation.x, pose.translation.y, pose.translation.z);
        return create_pose2d(env, pose);
    } catch (const rs2::error& e) {
        LOG("Camera failure: %s(%s): %s", e.get_failed_function().c_str(), e.get_failed_args().c_str(), e.what());
    } catch (const std::exception& e) {
        LOG("Unknown failure: %s", e.what());
    }
    return nullptr;
}
