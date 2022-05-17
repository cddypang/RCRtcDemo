#include "rcrtcmeeting.h"
#include "ui_rcrtcmeeting.h"

#include <iostream>
#include <QDialog>
#include <QMessageBox>

#include "cutils.h"
#include "rcrtc_device_manger.h"
#include "rtc_video_render.h"

RCRTCMeeting::RCRTCMeeting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RCRTCMeeting),
    rcrtc_engine_(nullptr)
{
    ui->setupUi(this);

    connect(ui->pushButton_PubStream, SIGNAL(clicked()), this, SLOT(onPublishLocalStreamButton()));

    ui->pushButton_PubStream->setEnabled(true);
}

RCRTCMeeting::~RCRTCMeeting()
{
    delete ui;
}

// 根据实际情况设置视频参数，推荐根据分辨率获取码率范围
bool RCRTCMeeting::SetDefaultVideoConfig() {
    if(nullptr == rcrtc_engine_){
        return false;
    }
    rcrtc::RCRTCVideoConfig *config = rcrtc::RCRTCVideoConfig::create();
    config->setFps(rcrtc::RCRTCVideoFps::FPS_15);
    config->setResolution(rcrtc::RCRTCVideoResolution::RESOLUTION_480_640);
    int32_t minBitrate, maxBitrate;
    config->getMaxAndMinBitrate(&maxBitrate, &minBitrate);
    config->setMinBitrate(minBitrate);
    config->setMaxBitrate(maxBitrate);
    config->setMirror(false);
    int32_t err_code = rcrtc_engine_->setVideoConfig(config);
    rcrtc::RCRTCVideoConfig::destroy(&config);
    if(err_code !=0) {
        CUtils::showResult(err_code, "set video config failed.");
    }
    return 0 == err_code;
}

// 加入指定房间
bool RCRTCMeeting::EnterRoom(){
    int32_t ret =-1;
    rcrtc::RCRTCRoomSetup* setup = rcrtc::RCRTCRoomSetup::create();
    if(nullptr == setup){
        ret = rcrtc_engine_->joinRoom(rtc_roomid_);
    } else {
        setup->setRole(rcrtc::RCRTCRole::MEETING_MEMBER);
        setup->setJoinType(rcrtc::RCRTCJoinType::KICK_OTHER_DEVICE);
        setup->setMediaType(rcrtc::RCRTCMediaType::AUDIO_VIDEO);
        ret = rcrtc_engine_->joinRoom(rtc_roomid_,setup);
        rcrtc::RCRTCRoomSetup::destroy(&setup);
    }
    return ret == 0;
}

void RCRTCMeeting::onPublishLocalStreamButton() {
    int32_t err_code = -1;
    if (rcrtc_engine_) {
        {
            SetDefaultVideoConfig();

//            std::list<rcrtc::RCRTCCamera*> camera_list;
//            RCRTCDeviceManger::getCameraList(rcrtc_engine_, camera_list);

//            rcrtc_engine_->enableCamera(camera_list.front());

            err_code = rcrtc_engine_->publish(rcrtc::RCRTCMediaType::AUDIO_VIDEO);
            if (err_code != 0) {
              CUtils::showResult(err_code, "local call publish.\n");
              return;
            }

            // ui->pushButton_PubStream->setWindowTitle("取消发布本地视频流");

            rtc_video_render* render = new rtc_video_render(this);
            // render->setWindowTitle("223334");
            render->setMinimumSize(400,300);
            render->setMaximumSize(400,300);
            render->attachVideoRender(rcrtc_engine_);
            ui->verticalLayout->addWidget(render);

        }
    }
}

void RCRTCMeeting::onRoomJoined(int32_t code, const std::string& errMsg)
    {
//        QMessageBox::information(this, "Info", QString("welcome, roomid is: %1").arg(rtc_roomid_.c_str()));
//        this->setWindowTitle(QString("welcome, roomid is: %1").arg(rtc_roomid_.c_str()));
        CUtils::showResult(code, "onRoomJoined errMsg: " + errMsg);
        if (0 == code) {
            ui->pushButton_PubStream->setEnabled(true);
        }
    }


// 创建窗口，并设置本地视频窗口
//rcrtc::IRCRTCView* local_view = nullptr;
//void RCRTCMeeting::setLocalView() {
//  HANDLE viewId = (HANDLE)ui->local_video_view->winId();//窗口
//  local_view = rcrtc::IRCRTCView::create((HANDLE)viewId);
//  if (engine_->setLocalView(local_view) != 0) {
//    LOG("set local view failed.\n");
//    return;
//  }
//}

// 移除本地视频窗口，并释放窗口
//void RCRTCMeeting::removeLocalView() {
//  if (engine_->removeLocalView() != 0) {
//    LOG("remove local view failed.\n");
//    return;
//  }
//  rcrtc::IRCRTCView::destroy(&local_view);
//}

// 创建窗口，并设置远端视频窗口
// 当要订阅远端视频之前可以调用该接口
//rcrtc::IRCRTCView* remote_view = nullptr;
//void RCRTCMeeting::setRemoteView() {
//  HANDLE viewId = (HANDLE)ui->remote_video_view->winId();//窗口
//  remove_view = rcrtc::IRCRTCView::create((HANDLE)viewId);
//  std::string remote_user = "remote_user_1";
//  if (engine_->setRemoteView(remote_user, remote_view) != 0) {
//    LOG("set remote view failed.\n");
//    return;
//  }
//}

// 移除远端视频窗口，并释放窗口
// 当取消订阅远端视频后可以调用该接口
// 如果收到远端用户下线或远端用户停止发布视频流后，sdk 内部会自动移除远端视频窗口，
// 此时您只需调用 destroy 释放之前创建的渲染资源即可
//void RCRTCMeeting::removeRemoteView() {
//  std::string remote_user = "remote_user_1";
//  if (engine_->removeRemoteView(remote_user) != 0) {
//    LOG("remove remote view failed.\n");
//    return;
//  }
//  rcrtc::IRCRTCView::destroy(&remote_view);
//}
