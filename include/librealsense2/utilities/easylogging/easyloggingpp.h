// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2021 Intel Corporation. All Rights Reserved.

#pragma once

// When including this file outside LibRealSense you also need to:
// 1. Compile the easylogging++.cc file
// 2. With static linkage, ELPP is initialized by librealsense, so doing it here will
//    create errors. When we're using the shared .so/.dll, the two are separate and we have
//    to initialize ours if we want to use the APIs!
// Use this code snippet:
// 
//#include <easylogging++.h>
//#ifdef BUILD_SHARED_LIBS
//INITIALIZE_EASYLOGGINGPP
//#endif

// you can use 'include(easyloggingpp.cmake)' for setting required includes and sources variables,
// and then use '${ELPP_SOURCES}' and '${ELPP_INCLUDES}' CMake variable to add to your target

#if BUILD_EASYLOGGINGPP
#include <third-party/easyloggingpp/src/easylogging++.h>


#ifdef __ANDROID__  
#include <android/log.h>
#include <sstream>

#define ANDROID_LOG_TAG "librs"

#define LOG_IMPL(level, ...) do { std::ostringstream ss; ss << '[' << __FILE_NAME__ << ':' << __LINE__ << ' ' << __func__ << "] " << __VA_ARGS__; __android_log_write( ANDROID_LOG_##level, ANDROID_LOG_TAG, "%s", ss.str().c_str() ); } while(false)

#ifndef NDEBUG
    #define LOG_DEBUG(...) LOG_IMPL(DEBUG, __VA_ARGS__)
#else
    #define LOG_DEBUG(...)
#endif
#define LOG_INFO(...)    LOG_IMPL(INFO, __VA_ARGS__)
#define LOG_WARNING(...) LOG_IMPL(WARN, __VA_ARGS__)
#define LOG_ERROR(...)   LOG_IMPL(ERROR, __VA_ARGS__)
#define LOG_FATAL(...)   LOG_IMPL(ERROR, __VA_ARGS__)

#else //__ANDROID__  

#define LOG_DEBUG(...)   do { CLOG(DEBUG   ,"librealsense") << __VA_ARGS__; } while(false)
#define LOG_INFO(...)    do { CLOG(INFO    ,"librealsense") << __VA_ARGS__; } while(false)
#define LOG_WARNING(...) do { CLOG(WARNING ,"librealsense") << __VA_ARGS__; } while(false)
#define LOG_ERROR(...)   do { CLOG(ERROR   ,"librealsense") << __VA_ARGS__; } while(false)
#define LOG_FATAL(...)   do { CLOG(FATAL   ,"librealsense") << __VA_ARGS__; } while(false)

#endif // __ANDROID__  


#else // BUILD_EASYLOGGINGPP


#define LOG_DEBUG(...)   do { ; } while(false)
#define LOG_INFO(...)    do { ; } while(false)
#define LOG_WARNING(...) do { ; } while(false)
#define LOG_ERROR(...)   do { ; } while(false)
#define LOG_FATAL(...)   do { ; } while(false)


#endif // BUILD_EASYLOGGINGPP
