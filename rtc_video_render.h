#ifndef RTC_VIDEO_RENDER_H
#define RTC_VIDEO_RENDER_H

#include <QWidget>
#include <string>

#include "rcrtc_engine.h"

namespace Ui {
class rtc_video_render;
}

class rtc_video_render : public QWidget
{
    Q_OBJECT

public:
    explicit rtc_video_render(QWidget *parent = nullptr);
    ~rtc_video_render();

    // userId emplty is local
    void attachVideoRender(rcrtc::RCRTCEngine* engine, const std::string& userId = "");

    // userId emplty is local
    void attachCustomVideoRender(rcrtc::RCRTCEngine* engine, const std::string& tag, const std::string& userId = "");

signals:
    void sigSendSdkResult(const QString&);

private:
    Ui::rtc_video_render *ui;
};

#endif // RTC_VIDEO_RENDER_H
