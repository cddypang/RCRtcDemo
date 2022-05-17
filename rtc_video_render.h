#ifndef RTC_VIDEO_RENDER_H
#define RTC_VIDEO_RENDER_H

#include <QWidget>

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

    void attachVideoRender(rcrtc::RCRTCEngine* engine);

private:
    Ui::rtc_video_render *ui;
};

#endif // RTC_VIDEO_RENDER_H
