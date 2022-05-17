#ifndef CUTILS_H
#define CUTILS_H

#include <iostream>

#include "rcim.h"
#include "rcrtc_constants.h"

class CUtils
{
public:
    static void showResult(const int32_t code, const std::string& msg = "");

    // context: user_token
    static void rcim_connection_callback_impl(HANDLE context, rc_connection_status code);

    static std::string rtcMediaTypeName(enum rcrtc::RCRTCMediaType type) {
        switch (type) {
        case rcrtc::RCRTCMediaType::VIDEO:
            return std::move("RTC_Video");
        case rcrtc::RCRTCMediaType::AUDIO:
            return "RTC_Audio";
        case rcrtc::RCRTCMediaType::AUDIO_VIDEO:
            return "RTC_Audio_Vedio";
        }
        return "unknown";
    }
};

#endif // CUTILS_H
