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
    explicit RCRTCMeeting(QWidget *parent = nullptr);
    ~RCRTCMeeting();

    // 根据实际情况设置视频参数，推荐根据分辨率获取码率范围
    bool SetDefaultVideoConfig();

    bool EnterRoom(rcrtc::RCRTCEngine* engine, const std::string& roomid,
                   const rcrtc::RCRTCRole& userRole, const rcrtc::RCRTCJoinType& joinType,
                   const rcrtc::RCRTCMediaType& mediaType);
    bool PublishedStream();


public slots:
    void onLeavRoom();
    void onRecvSdkResultLog(const QString& line);
    void onPublishLocalStreamButton();
    void onSubRemoteStreamButton();
    void onPubDesktopButton();
    void onSwitchRoleButton();

    void onRemotePublished(const QString& userId, qint32 mediaType);
    void onRemoteUnpublished(const QString& userId, qint32 mediaType);

signals:
    void sigSendSdkResult(const QString&);

private:
    Ui::RCRTCMeeting *ui;

    rcrtc::RCRTCEngine* rcrtc_engine_;
    std::string user_token_;
    std::string rtc_roomid_;
    rcrtc::RCRTCRole user_role_;
    rcrtc::RCRTCJoinType user_select_joinType_;
    rcrtc::RCRTCMediaType user_select_mediaType_;

    std::map<rcrtc::RCRTCStreamKey, rcrtc::RCRTCMediaType> remote_streams_;
    std::mutex mtx_remote_streams_;

};

#endif // RCRTCMEETING_H
