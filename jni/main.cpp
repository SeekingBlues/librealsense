#include <android/log.h>
#include <errno.h>
#include <fcntl.h>
#include <jni.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <librealsense2/rs.hpp>

#define LOG(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, "T265", fmt, ##__VA_ARGS__)
#define JNI_FUNC      extern "C" JNIEXPORT auto JNICALL

namespace {
pthread_t s_thread;
std::atomic<bool> s_stop_thread = false;
}

JNI_FUNC Java_org_firstinspires_ftc_teamcode_Driving_Odometry_odometryStart(JNIEnv*, jobject)
{
    s_stop_thread = false;
    auto rc = pthread_create(
        &s_thread, nullptr, [](void*) -> void* {
            try {
                rs2::pipeline pipe;
                rs2::config cfg;
                cfg.enable_stream(RS2_STREAM_POSE, RS2_FORMAT_6DOF);
                pipe.start(cfg);

                while (!s_stop_thread) {
                    auto frames = pipe.wait_for_frames();
                    auto frame = frames.first_or_default(RS2_STREAM_POSE);
                    assert(frame);
                    auto pose = frame.as<rs2::pose_frame>().get_pose_data();
                    LOG("%10.3f %10.3f %10.3f", pose.translation.x, pose.translation.y, pose.translation.z);
                }
            } catch (const rs2::error& e) {
                LOG("Camera failure: %s(%s): %s", e.get_failed_function().c_str(), e.get_failed_args().c_str(), e.what());
            } catch (const std::exception& e) {
                LOG("Unknown failure: %s", e.what());
            }
            LOG("Exiting thread");
            return nullptr;
        },
        nullptr);
    if (rc != 0)
        LOG("Failed to create thread: %s", strerror(rc));
}

JNI_FUNC Java_org_firstinspires_ftc_teamcode_Driving_Odometry_odometryStop(JNIEnv*, jobject)
{
    LOG("Stopping thread");
    s_stop_thread = true;
}
