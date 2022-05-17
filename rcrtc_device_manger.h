#ifndef RCRTCDEVICEMANGER_H
#define RCRTCDEVICEMANGER_H

#include <list>
#include "rcrtc_device.h"
#include "rcrtc_engine.h"

#include "cutils.h"

class RCRTCDeviceManger
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

    // 开启指定的摄像头
    /*
    void enableCamera(rcrtc::RCRTCCamera* camera) {
      if (engine_->getCameraList(camera_list) != 0) {
        LOG("get camera list failed.\n");
        return;
      }

      cur_camera.setName(select_camera->getCStringName());
      cur_camera.setId(select_camera->getId());
      cur_camera.setIndex(select_camera->getIndex());
      if (engine_->enableCamera(&cur_camera) != 0) {
        LOG("enable camera failed.\n");
        return;
      }
    }

    // 关闭指定的摄像头
    void Demo::disableCamera() {
      if (engine_->disableCamera(&cur_camera) != 0) {
        LOG("disable camera failed.\n");
        return;
      }
    }

    // 切换摄像头
    void Demo::switchToCamera() {
      if (engine_->switchToCamera(&cur_camera) != 0) {
        LOG("switch camera failed.\n");
        return;
      }
    }*/
};

#endif // RCRTCDEVICEMANGER_H
