#include "cutils.h"

//note QObject::setParent: Cannot set parent, new parent is in a different thread
// 多线程ui弹窗有问题，改成终端输出

void CUtils::showResult(const int32_t code, const std::string& msg)
{
    //if (!winHandle)
    {
        std::cout << "RtcDemo output, error_code: " << code << ", msg: " << msg << std::endl;
        return;
    }

//    if (0 == code) {
//        winHandle->information(winHandle, "Info", QString("error_code: %1, msg: %2").arg(code).arg(msg.c_str()));
//    } else {
//        winHandle->warning(winHandle, "Warning", QString("error_code: %1, msg: %2").arg(code).arg(msg.c_str()));
//    }
}

// context: user_token
void CUtils::rcim_connection_callback_impl(HANDLE context, rc_connection_status code)
{
    //if (0 != code) {
        // QMessageBox::warning(nullptr, "Warning", QString("error_code: %1, user_token: %2").arg(code).arg((const char*)context));
        std::cout << "rcim_connection error_code: " << code << ", user_token_: " << (const char*)context << std::endl;
    //}
}
