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
