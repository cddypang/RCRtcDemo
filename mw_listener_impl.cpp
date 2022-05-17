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
