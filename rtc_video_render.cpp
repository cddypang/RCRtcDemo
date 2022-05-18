#include "rtc_video_render.h"
#include "ui_rtc_video_render.h"

#include "cutils.h"

rtc_video_render::rtc_video_render(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rtc_video_render)
{
    ui->setupUi(this);
}

rtc_video_render::~rtc_video_render()
{
    delete ui;
}

// userId emplty is local
void rtc_video_render::attachVideoRender(rcrtc::RCRTCEngine* engine, const std::string& userId) {
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
void rtc_video_render::attachCustomVideoRender(rcrtc::RCRTCEngine* engine, const std::string& tag, const std::string& userId) {
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

void rtc_video_render::attachLiveMixVideoRender(rcrtc::RCRTCEngine* engine) {
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
