#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDir>
#include "cutils.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , handle_im_(nullptr)
    , engine_setup_(nullptr)
    , rcrtc_engine_(nullptr)
    , rtc_dialog_(nullptr)
{
    ui->setupUi(this);

    connect(ui->pushButton_Enter, SIGNAL(clicked()), this, SLOT(onEnterRoomButton()));
    connect(ui->pushButton_InitSDK, SIGNAL(clicked()), this, SLOT(onInitSDK()));
    connect(ui->pushButton_UninitSDK, SIGNAL(clicked()), this, SLOT(onUninitSDK()));
    connect(ui->checkBox_allowVideo, &QCheckBox::stateChanged, this, &MainWindow::onVideoCheckboxStateChanged);
    connect(ui->checkBox_allowAudio, &QCheckBox::stateChanged, this, &MainWindow::onAudioCheckboxStateChanged);
    connect(this, &MainWindow::logToPlainText, this, &MainWindow::onLogToPlainText);

    ui->lineEdit_Navigation->setText("nav.cn.ronghub.com");  // 信令服务器地址
    ui->lineEdit_Navigation->setReadOnly(true);
    // role
    radio_group_ = new QButtonGroup();
    radio_group_->setExclusive(true);
    radio_group_->addButton(ui->radioButton_MeetingMember);
    radio_group_->addButton(ui->radioButton_LiveBroadCaster);
    radio_group_->addButton(ui->radioButton_LiveAudience);
    ui->radioButton_MeetingMember->setChecked(true);

    ui->checkBox_kickOthers->setChecked(true);
    ui->checkBox_allowVideo->setChecked(true);
    ui->checkBox_allowAudio->setChecked(true);

    ui->pushButton_UninitSDK->setEnabled(false);
    ui->pushButton_Enter->setEnabled(false);
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

    //todo debug
    ui->lineEdit_AppKey->setText("x18ywvqfxyg1c");
    //{"code":200,"userId":"host-qt-app","token":"Xab4I8wRR+q4jOxnQrCGXmXUKtIzjgOpeZpJSxiKKcEoqEfFYn7evw==@5ysv.cn.rongnav.com;5ysv.cn.rongcfg.com"}
    ui->lineEdit_Token->setText("Xab4I8wRR+q4jOxnQrCGXmXUKtIzjgOpeZpJSxiKKcEoqEfFYn7evw==@5ysv.cn.rongnav.com;5ysv.cn.rongcfg.com");
    ui->lineEdit_RoomID->setText("2233");
    ui->lineEdit_UserName->setText("debug");
}

MainWindow::~MainWindow()
{
    if (engine_setup_)
    {
        rcrtc::RCRTCEngineSetup::destroy(&engine_setup_);
        engine_setup_ = nullptr;
    }
    if (rcrtc_engine_)
    {
        rcrtc_engine_->destroy();
        rcrtc_engine_ = nullptr;
    }
    if (handle_im_)
    {
        rcim_uninit(handle_im_);
        handle_im_ = nullptr;
    }
    delete radio_group_;
    delete ui;
}

void MainWindow::onLogToPlainText(const QString& line)
{
    // todo mutex, 错误日志变红
    std::cout << "LOGTEXT22 >>> " << line.toStdString() << std::endl;
    ui->plainTextEdit->appendPlainText(line + "\n");
    emit ui->plainTextEdit->textChanged();
}

void MainWindow::onRecvSdkResultLog(const QString& line) {
    std::cout << "LOGTEXT11 >>> " << line.toStdString() << std::endl;
    emit logToPlainText(line);
}

void MainWindow::onAudioCheckboxStateChanged(int state) {
    if (state == Qt::Unchecked && !ui->checkBox_allowVideo->isChecked()) {
        ui->pushButton_Enter->setEnabled(false);
    }
    if (!ui->pushButton_InitSDK->isEnabled() && (ui->checkBox_allowAudio->isChecked() || ui->checkBox_allowVideo->isChecked())) {
        ui->pushButton_Enter->setEnabled(true);
    }
}

void MainWindow::onVideoCheckboxStateChanged(int state) {
    if (state == Qt::Unchecked && !ui->checkBox_allowAudio->isChecked()) {
        ui->pushButton_Enter->setEnabled(false);
    }
    if (!ui->pushButton_InitSDK->isEnabled() && (ui->checkBox_allowAudio->isChecked() || ui->checkBox_allowVideo->isChecked())) {
        ui->pushButton_Enter->setEnabled(true);
    }
}

void MainWindow::onEnterRoomButton()
{
    std::string roomid = ui->lineEdit_RoomID->text().toStdString();
    std::string username = ui->lineEdit_UserName->text().toStdString();

    if (roomid.empty() || username.empty())
    {
        QMessageBox::warning(this, "Warning", "参数不完整", QMessageBox::Button::Yes);
    }
    else
    {
        rtc_dialog_ = new RCRTCMeeting();
        if (!rtc_dialog_)
        {
            std::cout << "new rcrtc_listener_impl_ failed" << std::endl;
            return;
        }
        connect(rtc_dialog_, &RCRTCMeeting::sigSendSdkResult, this, &MainWindow::onRecvSdkResultLog);
        connect(this, &MainWindow::remotePublished, rtc_dialog_, &RCRTCMeeting::onRemotePublished);
        connect(this, &MainWindow::remoteUnpublished, rtc_dialog_, &RCRTCMeeting::onRemoteUnpublished);

        // invoke rtc window
        rcrtc::RCRTCRole role;
        rcrtc::RCRTCJoinType join_type;
        rcrtc::RCRTCMediaType media_type;
        if (ui->radioButton_MeetingMember->isChecked()) {
            role = rcrtc::RCRTCRole::MEETING_MEMBER;
        } else if (ui->radioButton_LiveBroadCaster->isChecked()) {
            role = rcrtc::RCRTCRole::LIVE_BROADCASTER;
        } else {
            role = rcrtc::RCRTCRole::LIVE_AUDIENCE;
        }

        if (ui->checkBox_kickOthers->isChecked()) {
            join_type = rcrtc::RCRTCJoinType::KICK_OTHER_DEVICE;
        } else {
            join_type = rcrtc::RCRTCJoinType::REFUSE_CURRENT_DEVICE;
        }

        if (ui->checkBox_allowAudio->isChecked() && ui->checkBox_allowVideo->isChecked()) {
            media_type = rcrtc::RCRTCMediaType::AUDIO_VIDEO;
        } else if (ui->checkBox_allowAudio->isChecked() && !ui->checkBox_allowVideo->isChecked()) {
            media_type = rcrtc::RCRTCMediaType::AUDIO;
        } else if (!ui->checkBox_allowAudio->isChecked() && ui->checkBox_allowVideo->isChecked()) {
            media_type = rcrtc::RCRTCMediaType::VIDEO;
        } else {
            // todo log
            return;
        }

        if (rtc_dialog_) {
            bool ret = rtc_dialog_->EnterRoom(rcrtc_engine_, roomid, role, join_type, media_type);
            if (ret) {
                rtc_dialog_->show();
            } else {
                delete rtc_dialog_;
                rtc_dialog_ = nullptr;
            }
        }
    }
}

void MainWindow::onInitSDK()
{
    std::string appkey = ui->lineEdit_AppKey->text().toStdString();
    user_token_ = ui->lineEdit_Token->text().toStdString();
    std::string navigation = ui->lineEdit_Navigation->text().toStdString();

    ui->lineEdit_AppKey->setReadOnly(true);
    ui->lineEdit_Token->setReadOnly(true);

    if (appkey.empty() || user_token_.empty() || navigation.empty())
    {
        QMessageBox::warning(this, "Warning", "参数不完整", QMessageBox::Button::Yes);
    }
    else
    {
        int32_t error_code = -1;
        handle_im_ = rcim_init(appkey.c_str(), navigation.c_str(), "./", &error_code);
           if(nullptr == handle_im_){
               emit logToPlainText(QString(CUtils::formatSdkResult(error_code).c_str()));
               return;
           }
           QDir dir;
           QString dir_path = QCoreApplication::applicationDirPath() + "/log";
           if (!dir.exists(dir_path)) {
               dir.mkdir(dir_path);  //创造一个文件夹，以目前的年月日来命名的
           }
           engine_setup_ = rcrtc::RCRTCEngineSetup::create();
           if (engine_setup_) {
               // https://rtc-media.rongcloud.net 个人开发者账号对应媒体服务器地址
               // 后台分为媒体服务和信令服务 ，导航是信令服务的地址
               // 媒体服务的地址会在登录成功后从信令服务器获取一个
               // 如果用户设置了则以用户设置的为准
               //engine_setup_->setMediaUrl(navigation);
               engine_setup_->setReconnectable(true);
               auto log = engine_setup_->CreateLogSetup();
               if (log) {
                   log->setLogFolder(dir_path.toStdString());
                   log->setLogLevel(1);
               }
               rcrtc_engine_ = rcrtc::RCRTCEngine::create(handle_im_, engine_setup_);
               if (nullptr == rcrtc_engine_) {
                   emit logToPlainText(QString(CUtils::formatSdkResult(-1, "RTCEngine init failed.").c_str()));
                   return;
               }

               rcrtc_engine_->setListener(this);
               emit logToPlainText(QString(CUtils::formatSdkResult(0, "RTCEngine init OK.").c_str()));

               // connect im
               error_code = rcim_set_connection_callback(handle_im_, rcim_connection_callback_impl, this);
               if(0 == error_code) {
                      error_code = rcim_connect(handle_im_, user_token_.c_str());
                      if (0 != error_code) {
                          emit logToPlainText(QString(CUtils::formatSdkResult(error_code, "rcim_connect error").c_str()));
                          return;
                      }
                      ui->pushButton_InitSDK->setEnabled(false);
                      ui->pushButton_UninitSDK->setEnabled(true);
                      ui->pushButton_Enter->setEnabled(true);
               } else {
                   emit logToPlainText(QString(CUtils::formatSdkResult(error_code, "rcim_set_connection_callback error").c_str()));
               }

           } else {
               emit logToPlainText(QString(CUtils::formatSdkResult(-1, "RCRTCEngineSetup::create() failed.").c_str()));
               return;
           }
    }
}

void MainWindow::onUninitSDK() {
    int32_t err_code = rcim_disconnect(handle_im_);
    if (0 != err_code) {
        emit logToPlainText(QString(CUtils::formatSdkResult(err_code, "rcim_disconnect error").c_str()));
    }
    if (rtc_dialog_)
    {
        delete rtc_dialog_;
        rtc_dialog_ = nullptr;
    }

    ui->pushButton_Enter->setEnabled(false);
    ui->pushButton_UninitSDK->setEnabled(false);
    ui->pushButton_InitSDK->setEnabled(true);
}
