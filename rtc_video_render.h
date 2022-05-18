#ifndef RTC_VIDEO_RENDER_H
#define RTC_VIDEO_RENDER_H

#include <QWidget>
#include <string>

#include "rcrtc_engine.h"

namespace Ui {
class RtcVideoRender;
}

class RtcVideoRender : public QWidget
{
    Q_OBJECT

public:
    explicit RtcVideoRender(QWidget *parent = nullptr);
    ~RtcVideoRender();

    // userId emplty is local
    void attachVideoRender(rcrtc::RCRTCEngine* engine, const std::string& userId = "");

    // userId emplty is local
    void attachCustomVideoRender(rcrtc::RCRTCEngine* engine, const std::string& tag, const std::string& userId = "");

    void attachLiveMixVideoRender(rcrtc::RCRTCEngine* engine);

signals:
    void sigSendSdkResult(const QString&);

private:
    Ui::RtcVideoRender *ui;
};

#endif // RTC_VIDEO_RENDER_H
