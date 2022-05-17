#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "im/rcim.h"
#include "rtc/rcrtc_engine.h"

#include "rcrtcmeeting.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onEnterButton();
    void onInitSDK();
    void onUninitSDK();

private:
    Ui::MainWindow *ui;

    HANDLE_IM_ENGINE handle_im_;
    rcrtc::RCRTCEngineSetup* engine_setup_;
    rcrtc::RCRTCEngine* rcrtc_engine_;
    RCRTCMeeting* rtc_dialog_;
    std::string user_token_; // im 连接需要 token ，所以同时只能登录一个用户
};
#endif // MAINWINDOW_H
