#include "mainwindow.h"
#include "cutils.h"

// context: MainWindow*
void rcim_connection_callback_impl(HANDLE context, rc_connection_status code)
{
    MainWindow* mw = (MainWindow*)context;
    std::ostringstream ostr;
    if (mw) {
        ostr << "user_token_: " << mw->user_token_;
        std::string line = CUtils::formatSdkResult(code, ostr.str(), "IM Result");
        emit mw->logToPlainText(QString(line.c_str()));
    }
}

/**
 * 本地用户加入房间回调
 *
 * @param code   0: 调用成功, 非0: 失败
 */
void MainWindow::onRoomJoined(int32_t code, const std::string& errMsg)
{
    emit logToPlainText(QString(CUtils::formatSdkResult(code, "onRoomJoined errMsg: " + errMsg).c_str()));
}

/**
 * 本地用户离开房间回调
 *
 * @param code   0: 调用成功, 非0: 失败
 */
void MainWindow::onRoomLeft(int32_t code, const std::string& errMsg)
{
    emit logToPlainText(QString(CUtils::formatSdkResult(code, "onRoomLeft errMsg: " + errMsg).c_str()));
}

/**
 * 远端用户加入房间操作回调
 *
 * @param userId 用户 id
 */
void MainWindow::onUserJoined(const std::string& userId)
{
    emit logToPlainText(QString(CUtils::formatSdkResult(0, "onUserJoined userId: " + userId).c_str()));
    emit remoteUserJoined(QString(userId.c_str()));
}

/**
 * 远端用户因离线离开房间操作回调
 *
 * @param userId 用户 id
 */
void MainWindow::onUserOffline(const std::string& userId)
{
    emit logToPlainText(QString(CUtils::formatSdkResult(0, "onUserOffline userId: " + userId).c_str()));
    emit remoteUserLeft(QString(userId.c_str()));
}

/**
 * 远端用户离开房间操作回调
 *
 * @param userId 用户 id
 */
void MainWindow::onUserLeft(const std::string& userId)
{
    emit logToPlainText(QString(CUtils::formatSdkResult(0, "onUserLeft userId: " + userId).c_str()));
    emit remoteUserLeft(QString(userId.c_str()));
}

/**
 * 远端用户发布资源操作回调
 *
 * @param userId 远端用户 UserId
 * @param type   资源类型
 */
void MainWindow::onRemotePublished(const std::string& userId,
                               rcrtc::RCRTCMediaType type)
{
    emit remotePublished(QString(userId.c_str()), (qint32)type);
    emit logToPlainText(QString(CUtils::formatSdkResult(0, "onRemotePublished userId: " + userId \
                                                        + ", mediaType: " + CUtils::rtcMediaTypeName(type)).c_str()));
}

/**
 * 远端用户取消发布资源操作回调
 *
 * @param userId 远端用户 UserId
 * @param type   资源类型
 */
void MainWindow::onRemoteUnpublished(const std::string& userId,
                                 rcrtc::RCRTCMediaType type)
{
    emit remoteUnpublished(QString(userId.c_str()), (int32_t)type);
    emit logToPlainText(QString(CUtils::formatSdkResult(0, "onRemoteUnpublished userId: " + userId \
                                                        + ", mediaType: " + CUtils::rtcMediaTypeName(type)).c_str()));
}

/**
 * 本地用户发布资源回调
 *
 * @param type   资源类型
 * @param code   0: 调用成功, 非0: 失败
 */
void MainWindow::onPublished(rcrtc::RCRTCMediaType type,
                         int32_t code,
                         const std::string& errMsg)
{
    QString msg = QString("onPublished %1, errmsg: %2")
            .arg(CUtils::rtcMediaTypeName(type).c_str(), errMsg.c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(code, msg.toStdString()).c_str()));
}

/**
 * 本地用户取消发布资源回调
 *
 * @param type   资源类型
 * @param code   0: 调用成功, 非0: 失败
 */
void MainWindow::onUnpublished(rcrtc::RCRTCMediaType type,
                           int32_t code,
                           const std::string& errMsg)
{
    QString msg = QString("onUnpublished %1, errmsg: %2")
            .arg(CUtils::rtcMediaTypeName(type).c_str(), errMsg.c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(code, msg.toStdString()).c_str()));
}

/**
 * 订阅远端用户发布的资源操作回调
 *
 * @param userId 远端用户UserId
 * @param type   资源类型
 * @param code   0: 调用成功, 非0: 失败
 */
void MainWindow::onSubscribed(const std::string& userId,
                          rcrtc::RCRTCMediaType type,
                          int32_t code,
                          const std::string& errMsg)
{
    QString msg = QString("onSubscribed userId: %1, %2, errmsg: %3")
            .arg(userId.c_str(), CUtils::rtcMediaTypeName(type).c_str(), errMsg.c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(code, msg.toStdString()).c_str()));
}

/**
 * 取消订阅远端用户发布的资源操作回调
 *
 * @param userId 远端用户UserId
 * @param type   资源类型
 * @param code   0: 调用成功, 非0: 失败
 */
void MainWindow::onUnsubscribed(const std::string& userId,
                            rcrtc::RCRTCMediaType type,
                            int32_t code,
                            const std::string& errMsg)
{
    QString msg = QString("onUnsubscribed userId: %1, %2, errmsg: %3")
            .arg(userId.c_str(), CUtils::rtcMediaTypeName(type).c_str(), errMsg.c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(code, msg.toStdString()).c_str()));
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
void MainWindow::onCustomStreamPublished(const std::string& tag,
                                     rcrtc::RCRTCMediaType type,
                                     int32_t code,
                                     const std::string& errMsg)
{
    QString msg = QString("onCustomStreamPublished tag: %1, %2, errmsg: %3")
            .arg(tag.c_str(), CUtils::rtcMediaTypeName(type).c_str(), errMsg.c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(code, msg.toStdString()).c_str()));
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
void MainWindow::onCustomStreamUnpublished(const std::string& tag,
                                       rcrtc::RCRTCMediaType type,
                                       int32_t code,
                                       const std::string& errMsg)
{
    QString msg = QString("onCustomStreamUnpublished tag: %1, %2, errmsg: %3")
            .arg(tag.c_str(), CUtils::rtcMediaTypeName(type).c_str(), errMsg.c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(code, msg.toStdString()).c_str()));
}
/**
 * @brief 自定义流发布结束通知
 *
 * @param tags   自定义流唯一标志
 * @param type   媒体类型
 *
 * @return void
 */
void MainWindow::onCustomStreamPublishFinished(const std::string& tag,
                                           rcrtc::RCRTCMediaType type)
{
    QString msg = QString("onCustomStreamPublishFinished tag: %1, %2")
            .arg(tag.c_str(), CUtils::rtcMediaTypeName(type).c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(0, msg.toStdString()).c_str()));
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
void MainWindow::onCustomStreamPublished(const std::list<std::string>& tags,
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
void MainWindow::onCustomStreamUnpublished(const std::list<std::string>& tags,
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
void MainWindow::onCustomStreamPublishFinished(
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
void MainWindow::onRemoteCustomStreamPublished(const std::string& userId,
                                           const std::string& tag,
                                           rcrtc::RCRTCMediaType type)
{
    emit remoteCustomStreamPublished(QString(userId.c_str()), QString(tag.c_str()), (qint32)type);

    QString msg = QString("onRemoteCustomStreamPublished userId: %1, tag: %2, %3")
            .arg(userId.c_str(), tag.c_str(), CUtils::rtcMediaTypeName(type).c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(0, msg.toStdString()).c_str()));
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
void MainWindow::onRemoteCustomStreamUnpublished(const std::string& userId,
                                             const std::string& tag,
                                             rcrtc::RCRTCMediaType type)
{
    emit remoteCustomStreamUnpublished(QString(userId.c_str()), QString(tag.c_str()), (qint32)type);

    QString msg = QString("onRemoteCustomStreamUnpublished userId: %1, tag: %2, %3")
            .arg(userId.c_str(), tag.c_str(), CUtils::rtcMediaTypeName(type).c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(0, msg.toStdString()).c_str()));
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
void MainWindow::onRemoteCustomStreamStateChanged(const std::string& userId,
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
void MainWindow::onRemoteCustomStreamFirstFrame(const std::string& userId,
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
void MainWindow::onCustomStreamSubscribed(const std::string& userId,
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
void MainWindow::onCustomStreamUnsubscribed(const std::string& userId,
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
void MainWindow::onCustomStreamSubscribed(const std::string& userId,
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
void MainWindow::onCustomStreamUnsubscribed(const std::string& userId,
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
void MainWindow::onLiveRoleSwitched(rcrtc::RCRTCRole curRole,
                                int32_t code,
                                const std::string& errMsg)
{
    QString msg = QString("onLiveRoleSwitched cur_role: %1, errmsg: %2")
            .arg(CUtils::rtcUserRoleName(curRole).c_str(), errMsg.c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(code, msg.toStdString()).c_str()));
}

/**
 * 远端用户切换角色的回调
 *
 * @param roomId 房间 id
 * @param userId 远端用户 id
 * @param role 远端用户当前角色
 */
void MainWindow::onRemoteLiveRoleSwitched(const std::string& roomId,
                                      const std::string& userId,
                                      rcrtc::RCRTCRole role)
{
    QString msg = QString("onRemoteLiveRoleSwitched roomId: %1, userId: %2, cur_role: %3")
            .arg(roomId.c_str(), userId.c_str(), CUtils::rtcUserRoleName(role).c_str());
    emit logToPlainText(QString(CUtils::formatSdkResult(0, msg.toStdString()).c_str()));
}
