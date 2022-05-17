#ifndef RCRTCMEETING_H
#define RCRTCMEETING_H

#include <QWidget>
#include <map>
#include <mutex>

#include "rcrtc_engine.h"
#include "cutils.h"

namespace Ui {
class RCRTCMeeting;
}

class RCRTCMeeting : public QWidget
{
    Q_OBJECT

public:
    enum ERTCRoomType {
        ERTCRoomMeet = 1,
        ERTCRoomLive = 2
    };

    explicit RCRTCMeeting(QWidget *parent = nullptr);
    ~RCRTCMeeting();

    inline void SetRTCEngine(rcrtc::RCRTCEngine* engine,
                             const std::string& roomid) {
        if (engine && !roomid.empty()) {
            rcrtc_engine_ = engine;
            rtc_roomid_ = roomid;
        } else {
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(-1, "invalid parameters to rtcroom dialog").c_str()));
        }
    }
    // 根据实际情况设置视频参数，推荐根据分辨率获取码率范围
    bool SetDefaultVideoConfig();

    bool EnterRoom();
    bool PublishedStream();


public slots:
    void onLeavRoom();
    void onRecvSdkResultLog(const QString& line);
    void onPublishLocalStreamButton();
    void onSubRemoteStreamButton();
    void onPubDesktopButton();

    void onRemotePublished(const QString& userId, qint32 mediaType);
    void onRemoteUnpublished(const QString& userId, qint32 mediaType);

signals:
    void sigSendSdkResult(const QString&);

private:
    Ui::RCRTCMeeting *ui;

    rcrtc::RCRTCEngine* rcrtc_engine_;
    std::string user_token_;
    std::string rtc_roomid_;

    std::map<rcrtc::RCRTCStreamKey, rcrtc::RCRTCMediaType> remote_streams_;
    std::mutex mtx_remote_streams_;

};

#endif // RCRTCMEETING_H
