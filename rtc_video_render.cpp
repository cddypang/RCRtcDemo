#include "rtc_video_render.h"
#include "ui_rtc_video_render.h"

#include "cutils.h"

RtcVideoRender::RtcVideoRender(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RtcVideoRender)
{
    ui->setupUi(this);
}

RtcVideoRender::~RtcVideoRender()
{
    delete ui;
}

// userId emplty is local
void RtcVideoRender::attachVideoRender(rcrtc::RCRTCEngine* engine, const std::string& userId) {
    if (engine) {
        rcrtc::IRCRTCView* rtc_view = nullptr;
        HANDLE viewId = nullptr;
        viewId = (HANDLE)ui->widget->winId();//窗口

        rtc_view = rcrtc::IRCRTCView::create((HANDLE)viewId);
        rtc_view->setFitType(rcrtc::RCRTCViewFitType::FIT);
        int32_t err_code = -1;
        if (userId.empty()) {
            err_code = engine->setLocalView(rtc_view);
        } else {
            err_code = engine->setRemoteView(userId, rtc_view);
        }
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "set local view failed.\n").c_str()));
    }
}

// userId emplty is local
void RtcVideoRender::attachCustomVideoRender(rcrtc::RCRTCEngine* engine, const std::string& tag, const std::string& userId) {
    if (engine && !tag.empty()) {
        rcrtc::IRCRTCView* rtc_view = nullptr;
        HANDLE viewId = nullptr;
        viewId = (HANDLE)ui->widget->winId();//窗口

        rtc_view = rcrtc::IRCRTCView::create((HANDLE)viewId);
        rtc_view->setFitType(rcrtc::RCRTCViewFitType::FIT);
        int32_t err_code = -1;
        if (userId.empty()) {
            err_code = engine->setLocalCustomStreamView(tag, rtc_view);
        } else {
            err_code = engine->setRemoteCustomStreamView(userId, tag, rtc_view);
        }
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "set local|remote custom stream view failed.\n").c_str()));
    }
}

void RtcVideoRender::attachLiveMixVideoRender(rcrtc::RCRTCEngine* engine) {
    if (engine) {
        rcrtc::IRCRTCView* rtc_view = nullptr;
        HANDLE viewId = nullptr;
        viewId = (HANDLE)ui->widget->winId();//窗口

        rtc_view = rcrtc::IRCRTCView::create((HANDLE)viewId);
        rtc_view->setFitType(rcrtc::RCRTCViewFitType::FIT);
        int32_t err_code = engine->setLiveMixView(rtc_view);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call attachVideoRender.\n").c_str()));
    }
}
