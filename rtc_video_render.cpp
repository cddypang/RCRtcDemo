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

void rtc_video_render::attachVideoRender(rcrtc::RCRTCEngine* engine) {
    if (engine) {
        rcrtc::IRCRTCView* rtc_view = nullptr;
        HANDLE viewId = nullptr;
        viewId = (HANDLE)ui->widget->winId();//窗口

        rtc_view = rcrtc::IRCRTCView::create((HANDLE)viewId);
        rtc_view->setFitType(rcrtc::RCRTCViewFitType::FIT);
        int32_t error_code = engine->setLocalView(rtc_view);
        if (error_code != 0) {
          emit sigSendSdkResult(QString(CUtils::formatSdkResult(error_code, "set local view failed.\n").c_str()));
        }
    }
}
