#ifndef RCRTCMEETING_H
#define RCRTCMEETING_H

#include <QWidget>

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
            CUtils::showResult(-1, "invalid parameters to rtcroom dialog");
        }
    }
    // 根据实际情况设置视频参数，推荐根据分辨率获取码率范围
    bool SetDefaultVideoConfig();

    bool EnterRoom();
    bool PublishedStream();


public slots:
    void onPublishLocalStreamButton();

private:
    Ui::RCRTCMeeting *ui;

    rcrtc::RCRTCEngine* rcrtc_engine_;
    std::string user_token_;
    std::string rtc_roomid_;

public:

    /**
     * 本地用户加入房间回调
     *
     * @param code   0: 调用成功, 非0: 失败
     */
    virtual void onRoomJoined(int32_t code, const std::string& errMsg);

    /**
     * 本地用户离开房间回调
     *
     * @param code   0: 调用成功, 非0: 失败
     */
    virtual void onRoomLeft(int32_t code, const std::string& errMsg)
    {
        CUtils::showResult(code, "onRoomLeft errMsg: " + errMsg);
    }

    /**
     * 引擎内部错误回调
     *
     * @param code   0: 调用成功, 非0: 失败
     */
    virtual void onError(int32_t code)
    {
        CUtils::showResult(code, "Engine internal error!");
    }

    /**
     * 本地用户被踢出房间回调
     *
     * @param roomId 房间 id
     * @param errMsg 失败原因
     */
    virtual void onKicked(const std::string& roomId,
                          const std::string& errMsg)
    {
        CUtils::showResult(0, "onKicked, roomId: " + roomId + ", errMsg: " + errMsg);
    }

    /**
     * 远端用户加入房间操作回调
     *
     * @param userId 用户 id
     */
    virtual void onUserJoined(const std::string& userId)
    {
        CUtils::showResult(0, "onUserJoined userId: " + userId);
    }

    /**
     * 远端用户因离线离开房间操作回调
     *
     * @param userId 用户 id
     */
    virtual void onUserOffline(const std::string& userId)
    {
        CUtils::showResult(0, "onUserOffline userId: " + userId);
    }

    /**
     * 远端用户离开房间操作回调
     *
     * @param userId 用户 id
     */
    virtual void onUserLeft(const std::string& userId)
    {
        CUtils::showResult(0, "onUserLeft userId: " + userId);
    }

    /**
     * 远端用户发布资源操作回调
     *
     * @param userId 远端用户 UserId
     * @param type   资源类型
     */
    virtual void onRemotePublished(const std::string& userId,
                                   rcrtc::RCRTCMediaType type)
    {
        CUtils::showResult(0, "onRemotePublished userId: " + userId + ", mediaType: " + CUtils::rtcMediaTypeName(type));
    }

    /**
     * 远端用户取消发布资源操作回调
     *
     * @param userId 远端用户 UserId
     * @param type   资源类型
     */
    virtual void onRemoteUnpublished(const std::string& userId,
                                     rcrtc::RCRTCMediaType type)
    {
        CUtils::showResult(0, "onRemoteUnpublished userId: " + userId + ", mediaType: " + CUtils::rtcMediaTypeName(type));
    }

    /**
     * 远端用户资源状态操作回调
     *
     * @param userId   远端用户UserId
     * @param type     资源类型
     * @param disabled 是否停止相应资源采集
     */
    virtual void onRemoteStateChanged(const std::string& userId,
                                      rcrtc::RCRTCMediaType type,
                                      bool disabled)
    {
        // todo
    }

    /**
     * 收到远端用户第一帧数据
     *
     * @param userId 远端用户UserId
     * @param type   资源类型
     */
    virtual void onRemoteFirstFrame(const std::string& userId,
                                    rcrtc::RCRTCMediaType type)
    {
        // todo
    }

    /**
     * 远端直播合流发布资源操作回调
     *
     * @param type   资源类型
     */
    virtual void onRemoteLiveMixPublished(rcrtc::RCRTCMediaType type)
    {
        // todo
    }

    /**
     * 远端直播合流取消发布资源操作回调
     *
     * @param type   资源类型
     */
    virtual void onRemoteLiveMixUnpublished(rcrtc::RCRTCMediaType type)
    {
        // todo
    }

    /**
     * 收到直播合流第一帧数据
     *
     * @param type 资源类型
     */
    virtual void onRemoteLiveMixFirstFrame(rcrtc::RCRTCMediaType type)
    {
        // todo
    }

    /**
     * 本地用户发布资源回调
     *
     * @param type   资源类型
     * @param code   0: 调用成功, 非0: 失败
     */
    virtual void onPublished(rcrtc::RCRTCMediaType type,
                             int32_t code,
                             const std::string& errMsg)
    {
        QString msg = QString("roomid: %1, local publish %2, errcode: %3, errmsg: %4")
                .arg(rtc_roomid_.c_str()).arg(CUtils::rtcMediaTypeName(type).c_str()).arg(code).arg(errMsg.c_str());
        CUtils::showResult(code, msg.toStdString());
    }

    /**
     * 本地用户取消发布资源回调
     *
     * @param type   资源类型
     * @param code   0: 调用成功, 非0: 失败
     */
    virtual void onUnpublished(rcrtc::RCRTCMediaType type,
                               int32_t code,
                               const std::string& errMsg)
    {
        QString msg = QString("roomid: %1, local unPublish %2, errcode: %3, errmsg: %4")
                .arg(rtc_roomid_.c_str()).arg(CUtils::rtcMediaTypeName(type).c_str()).arg(code).arg(errMsg.c_str());
        CUtils::showResult(code, msg.toStdString());
    }

    /**
     * 订阅远端用户发布的资源操作回调
     *
     * @param userId 远端用户UserId
     * @param type   资源类型
     * @param code   0: 调用成功, 非0: 失败
     */
    virtual void onSubscribed(const std::string& userId,
                              rcrtc::RCRTCMediaType type,
                              int32_t code,
                              const std::string& errMsg)
    {
        // todo
    }

    /**
     * 取消订阅远端用户发布的资源操作回调
     *
     * @param userId 远端用户UserId
     * @param type   资源类型
     * @param code   0: 调用成功, 非0: 失败
     */
    virtual void onUnsubscribed(const std::string& userId,
                                rcrtc::RCRTCMediaType type,
                                int32_t code,
                                const std::string& errMsg)
    {
        // todo
    }

    /**
     * 订阅合流资源操作回调
     *
     * @param type   资源类型
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixSubscribed(rcrtc::RCRTCMediaType type,
                                     int32_t code,
                                     const std::string& errMsg)
    {
        // todo
    }

    /**
     * 取消订阅合流资源操作回调
     *
     * @param type   资源类型
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixUnsubscribed(rcrtc::RCRTCMediaType type,
                                       int32_t code,
                                       const std::string& errMsg)
    {
        // todo
    }

    /**
     * 使能摄像头操作回调
     *
     * @param type   资源类型
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onEnableCamera(bool enable,
                                int32_t code,
                                const std::string& errMsg)
    {
        // todo
    }

    /**
     * 切换摄像头操作回调
     *
     * @param camera 摄像头信息（函数调用结束，指针即刻销毁）
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg 错误描述
     */
    virtual void onSwitchCamera(const rcrtc::RCRTCCamera* camera,
                                int32_t code,
                                const std::string& errMsg)
    {
        // todo
    }

    virtual void onLiveCdnAdded(const std::string& url,
                                int32_t code,
                                const std::string& errMsg)
    {
        // todo
    }
    virtual void onLiveCdnRemoved(const std::string& url,
                                  int32_t code,
                                  const std::string& errMsg)
    {
        // todo
    }

    /**
     * 设置合流布局模式操作回调
     *
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixLayoutModeSet(int32_t code,
                                        const std::string& errMsg)
    {
        // todo
    }

    /**
     * 设置合流渲染模式操作回调
     *
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixRenderModeSet(int32_t code,
                                        const std::string& errMsg)
    {
        // todo
    }

    /**
     * 设置合流自定义布局操作回调
     *
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixCustomLayoutsSet(int32_t code,
                                           const std::string& errMsg)
    {
        // todo
    }

    /**
     * 设置合流中参与混音的主播操作回调
     *
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixCustomAudiosSet(int32_t code,
                                          const std::string& errMsg)
    {
        // todo
    }

    /**
     * 设置合流中音频输出码率操作回调
     *
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixAudioBitrateSet(int32_t code,
                                          const std::string& errMsg)
    {
        // todo
    }

    /**
     * 设置合流中视频输出码率操作回调
     *
     * @param tiny 是否是小流
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixVideoBitrateSet(bool tiny,
                                          int32_t code,
                                          const std::string& errMsg)
    {
        // todo
    }

    /**
     * 设置合流中视频输出分辨率操作回调
     *
     * @param tiny 是否是小流
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixVideoResolutionSet(bool tiny,
                                             int32_t code,
                                             const std::string& errMsg)
    {
        // todo
    }

    /**
     * 设置合流中视频输出帧率操作回调
     *
     * @param tiny 是否是小流
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixVideoFpsSet(bool tiny,
                                      int32_t code,
                                      const std::string& errMsg)
    {
        // todo
    }
    /**
     * 设置合流背景颜色操作结果回调
     *
     * @param code   0: 调用成功, 非0: 失败
     * @param errMsg  错误描述
     */
    virtual void onLiveMixBackgroundColorSet(int32_t code,
                                             const std::string& errMsg)
    {
        // todo
    }

    virtual void onAudioEffectCreated(int32_t effectId,
                                      int32_t code,
                                      const std::string& errMsg)
    {
        // todo
    }

    virtual void onAudioEffectFinished(int32_t effectId)
    {
        // todo
    }

    virtual void onAudioMixingStarted()
    {
        // todo
    }

    virtual void onAudioMixingPaused()
    {
        // todo
    }

    virtual void onAudioMixingStopped()
    {
        // todo
    }

    virtual void onAudioMixingFinished()
    {
        // todo
    }

    /**
     * @brief 发布自定义流结果通知
     *
     * @param tag  自定义流唯一标志
     * @param type 媒体类型
     * @param code 错误码
     * @param errMsg 错误类型
     *
     * @return void
     */
    virtual void onCustomStreamPublished(const std::string& tag,
                                         rcrtc::RCRTCMediaType type,
                                         int32_t code,
                                         const std::string& errMsg)
    {
        // todo
    }
    /**
     * @brief 取消发布自定义流结果通知
     *
     * @param tag    自定义流唯一标志
     * @param type   媒体类型
     * @param code   错误码
     * @param errMsg 错误类型
     *
     * @return void
     */
    virtual void onCustomStreamUnpublished(const std::string& tag,
                                           rcrtc::RCRTCMediaType type,
                                           int32_t code,
                                           const std::string& errMsg)
    {
        // todo
    }
    /**
     * @brief 自定义流发布结束通知
     *
     * @param tags   自定义流唯一标志
     * @param type   媒体类型
     *
     * @return void
     */
    virtual void onCustomStreamPublishFinished(const std::string& tag,
                                               rcrtc::RCRTCMediaType type)
    {
        // todo
    }
    /**
     * @brief 批量发布自定义流结果通知
     *
     * @param tag 自定义流唯一标志列表
     * @param code 错误码
     * @param errMsg 错误类型
     *
     * @return void
     */
    virtual void onCustomStreamPublished(const std::list<std::string>& tags,
                                         int32_t code,
                                         const std::string& errMsg)
    {
        // todo
    }
    /**
     * @brief 批量取消发布自定义流结果通知
     *
     * @param tag    自定义流唯一标志列表
     * @param code   错误码
     * @param errMsg 错误类型
     *
     * @return void
     */
    virtual void onCustomStreamUnpublished(const std::list<std::string>& tags,
                                           int32_t code,
                                           const std::string& errMsg)
    {
        // todo
    }
    /**
     * @brief 自定义流发布结束通知
     *
     * @param tags    自定义流唯一标志列表
     *
     * @return void
     */
    virtual void onCustomStreamPublishFinished(
        const std::list<std::string>& tags)
    {
        // todo
    }
    /**
     * @brief 远端用户发布自定义流通知
     *
     * @param userId  远端用户id
     * @param tag     自定义流唯一标志， 全局唯一。
     * @param type    自定义流媒体类型
     *
     * @return void
     */
    virtual void onRemoteCustomStreamPublished(const std::string& userId,
                                               const std::string& tag,
                                               rcrtc::RCRTCMediaType type)
    {
        // todo
    }
    /**
     * @brief 远端用户取消发布自定义流通知
     *
     * @param userId  远端用户Id
     * @param tag     自定义流唯一标志， 全局唯一。
     * @param type    自定义流媒体类型
     *
     * @return void
     */
    virtual void onRemoteCustomStreamUnpublished(const std::string& userId,
                                                 const std::string& tag,
                                                 rcrtc::RCRTCMediaType type)
    {
        // todo
    }
    /**
     * @brief 远端用户取消发布自定义流通知
     *
     * @param userId  远端用户Id
     * @param tag     自定义流唯一标志， 全局唯一。
     * @param type    自定义流媒体类型
     * @param disabled
     * - true 被禁止发送
     * - false 取消禁止发送
     *
     * @return void
     */
    virtual void onRemoteCustomStreamStateChanged(const std::string& userId,
                                                  const std::string& tag,
                                                  rcrtc::RCRTCMediaType type,
                                                  bool disabled)
    {
        // todo
    }
    /**
     * @brief 解码远端自定义视频流第一帧数据通知
     *
     * @param userId  远端用户Id
     * @param tag     自定义流唯一标志， 全局唯一。
     * @param type    自定义流媒体类型
     *
     * @return void
     */
    virtual void onRemoteCustomStreamFirstFrame(const std::string& userId,
                                                const std::string& tag,
                                                rcrtc::RCRTCMediaType type)
    {
        // todo
    }
    /**
     * @brief 订阅指定自定义流结果通知
     *
     * @param userId  远端用户Id
     * @param tag     自定义流唯一标志， 全局唯一。
     * @param type    自定义流媒体类型
     * @param code    错误码
     * @param errMsg  错误码信息描述
     *
     * @return void
     */
    virtual void onCustomStreamSubscribed(const std::string& userId,
                                          const std::string& tag,
                                          rcrtc::RCRTCMediaType type,
                                          int32_t code,
                                          const std::string& errMsg)
    {
        // todo
    }
    /**
     * @brief 取消订阅指定自定义流结果通知
     *
     * @param userId  远端用户Id
     * @param tag     自定义流唯一标志， 全局唯一。
     * @param type    自定义流媒体类型
     * @param code    错误码
     * @param errMsg  错误码信息描述
     *
     * @return void
     */
    virtual void onCustomStreamUnsubscribed(const std::string& userId,
                                            const std::string& tag,
                                            rcrtc::RCRTCMediaType type,
                                            int32_t code,
                                            const std::string& errMsg)
    {
        // todo
    }
    /**
     * @brief 批量订阅指定自定义流结果通知
     *
     * @param userId  远端用户Id
     * @param tags    自定义流唯一标志列表。
     * @param code    错误码
     * @param errMsg  错误码信息描述
     *
     * @return void
     */
    virtual void onCustomStreamSubscribed(const std::string& userId,
                                          const std::list<std::string>& tags,
                                          int32_t code,
                                          const std::string& errMsg)
    {
        // todo
    }
    /**
     * @brief 批量取消订阅指定自定义流结果通知
     *
     * @param userId  远端用户Id
     * @param tags    自定义流唯一标志列表。
     * @param code    错误码
     * @param errMsg  错误码信息描述
     *
     * @return void
     */
    virtual void onCustomStreamUnsubscribed(const std::string& userId,
                                            const std::list<std::string>& tags,
                                            int32_t code,
                                            const std::string& errMsg)
    {
        // todo
    }

    /**
     * 切换角色的回调
     *
     * @param role 当前角色
     * @param code 0: 成功，role 是切换后的角色；非0: 失败，role 保持原来的角色
     * @param errMsg 失败原因
     */
    virtual void onLiveRoleSwitched(rcrtc::RCRTCRole curRole,
                                    int32_t code,
                                    const std::string& errMsg)
    {
        // todo
    }

    /**
     * 远端用户切换角色的回调
     *
     * @param roomId 房间 id
     * @param userId 远端用户 id
     * @param role 远端用户当前角色
     */
    virtual void onRemoteLiveRoleSwitched(const std::string& roomId,
                                          const std::string& userId,
                                          rcrtc::RCRTCRole role)
    {
        // todo
    }

    /**
     * 获取用户加入的房间信息
     * @param roomInfo 用户加入的房间 id，使用的 设备 id 以及加入时间
     * @param code 0: 成功，；非0: 失败
     */
    virtual void onJoinedRoomGot(const std::list<rcrtc::RCRTCUserJoinedRoom>& roomInfo,
                                 int32_t code)
    {
        // todo
    }
};

#endif // RCRTCMEETING_H