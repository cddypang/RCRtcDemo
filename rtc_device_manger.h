#ifndef RCRTCDEVICEMANGER_H
#define RCRTCDEVICEMANGER_H

#include <list>
#include "rcrtc_device.h"
#include "rcrtc_engine.h"

#include "cutils.h"

class RTCDeviceManger
{
public:
    // 获取摄像头信息列表，注意：不要对列表进行增减操作
    static int32_t getCameraList(rcrtc::RCRTCEngine* engine, std::list<rcrtc::RCRTCCamera*>& camera_list) {
        int32_t err_code = -1;
        if (engine) {
            err_code = engine->getCameraList(camera_list);
        }
        return err_code;
    }
};

#endif // RCRTCDEVICEMANGER_H
