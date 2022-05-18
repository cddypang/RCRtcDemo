#ifndef CUTILS_H
#define CUTILS_H

#include <iostream>

#include "rcim.h"
#include "rcrtc_constants.h"

class CUtils
{
public:
    static std::string formatSdkResult(const int32_t code, const std::string& msg = "", const std::string& head = "RTC Result");

    static std::string rtcMediaTypeName(const rcrtc::RCRTCMediaType& type) {
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

    static std::string rtcUserRoleName(const rcrtc::RCRTCRole& role) {
        switch (role) {
        case rcrtc::RCRTCRole::MEETING_MEMBER:
            return "meeting_member";
        case rcrtc::RCRTCRole::LIVE_BROADCASTER:
            return "live_broadcaster";
        case rcrtc::RCRTCRole::LIVE_AUDIENCE:
            return "live_audience";
        }
        return "unknown";
    }
};

#endif // CUTILS_H
