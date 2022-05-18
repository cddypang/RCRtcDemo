#include "rcrtcmeeting.h"
#include "ui_rcrtcmeeting.h"

#include <iostream>
#include <sstream>
#include <QDialog>
#include <QMessageBox>

#include "cutils.h"
#include "rcrtc_device_manger.h"
#include "rtc_video_render.h"

RCRTCMeeting::RCRTCMeeting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RCRTCMeeting),
    rcrtc_engine_(nullptr)
{
    ui->setupUi(this);
    this->resize(800,600);

    connect(ui->pushButton_PubStream, SIGNAL(clicked()), this, SLOT(onPublishLocalStreamButton()));
    connect(ui->pushButton_SubRemote, &QPushButton::clicked, this, &RCRTCMeeting::onSubRemoteStreamButton);
    connect(ui->pushButton_SubCustomStream, &QPushButton::clicked, this, &RCRTCMeeting::onSubRemoteCustomStreamButton);
    connect(ui->pushButton_PubDesktop, &QPushButton::clicked, this, &RCRTCMeeting::onPubDesktopButton);
    connect(ui->pushButton_SwitchRole, &QPushButton::clicked, this, &RCRTCMeeting::onSwitchRoleButton);
    connect(ui->pushButton_Quit, &QPushButton::clicked, this, &RCRTCMeeting::onLeavRoom);

    ui->pushButton_PubStream->setEnabled(true);
    ui->pushButton_SubRemote->setEnabled(false);
    ui->pushButton_PubDesktop->setEnabled(true);
    ui->pushButton_SwitchRole->setEnabled(false);

    ui->pushButton_SubMix->setEnabled(false);  // todo
    ui->pushButton_SetMix->setEnabled(false);  // todo
}

RCRTCMeeting::~RCRTCMeeting()
{
    delete ui;
}

void RCRTCMeeting::onRecvSdkResultLog(const QString& line) {
    emit sigSendSdkResult(line);
}

// 根据实际情况设置视频参数，推荐根据分辨率获取码率范围
bool RCRTCMeeting::SetDefaultVideoConfig() {
    if(nullptr == rcrtc_engine_){
        return false;
    }
    rcrtc::RCRTCVideoConfig *config = rcrtc::RCRTCVideoConfig::create();
    config->setFps(rcrtc::RCRTCVideoFps::FPS_15);
    config->setResolution(rcrtc::RCRTCVideoResolution::RESOLUTION_480_640);
    int32_t minBitrate, maxBitrate;
    config->getMaxAndMinBitrate(&maxBitrate, &minBitrate);
    config->setMinBitrate(minBitrate);
    config->setMaxBitrate(maxBitrate);
    config->setMirror(false);
    int32_t err_code = rcrtc_engine_->setVideoConfig(config);
    rcrtc::RCRTCVideoConfig::destroy(&config);
    if(err_code !=0) {
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "set video config failed.").c_str()));
    }
    return 0 == err_code;
}

// 加入指定房间
bool RCRTCMeeting::EnterRoom(rcrtc::RCRTCEngine* engine, const std::string& roomid,
                             const rcrtc::RCRTCRole& userRole, const rcrtc::RCRTCJoinType& joinType,
                             const rcrtc::RCRTCMediaType& mediaType) {
    int32_t err_code = -1;
    if (engine && !roomid.empty()) {
        rcrtc_engine_ = engine;
        rtc_roomid_ = roomid;
    } else {
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(-1, "invalid parameters to rtcroom dialog").c_str()));
        return false;
    }

    rcrtc::RCRTCRoomSetup* setup = rcrtc::RCRTCRoomSetup::create();
    if(nullptr == setup){
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RCRTCRoomSetup::create failed.").c_str()));
        err_code = rcrtc_engine_->joinRoom(rtc_roomid_);
    } else {
        setup->setRole(userRole);
        setup->setJoinType(joinType);
        setup->setMediaType(mediaType);
        user_role_ = userRole;
        user_select_joinType_ = joinType;
        user_select_mediaType_ = mediaType;
        err_code = rcrtc_engine_->joinRoom(rtc_roomid_,setup);
        rcrtc::RCRTCRoomSetup::destroy(&setup);
    }
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "joinRoom call.").c_str()));

    if (0 == err_code) {
        std::ostringstream ostr;
        ostr << "ROOMID: " << roomid << " " << CUtils::rtcUserRoleName(userRole)
             << " " << CUtils::rtcMediaTypeName(mediaType);
        QString win_title = QString(ostr.str().c_str());
        this->setWindowTitle(win_title);

        if (user_role_ != rcrtc::RCRTCRole::MEETING_MEMBER) {
            ui->pushButton_SwitchRole->setEnabled(true);
        }
    }

    return err_code == 0;
}

void RCRTCMeeting::onLeavRoom() {
    int32_t err_code = -1;
    err_code = rcrtc_engine_->leaveRoom();
    if (err_code != 0) {
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call leaveRoom.\n").c_str()));
    }
    // todo send sig to mainwindow
}

void RCRTCMeeting::onPublishLocalStreamButton() {
    static bool pub_flag = false;
    pub_flag = !pub_flag;

    int32_t err_code = -1;
    if (rcrtc_engine_) {
        if (pub_flag) {
            if (user_select_mediaType_ != rcrtc::RCRTCMediaType::AUDIO) {
                SetDefaultVideoConfig();
            }
            err_code = rcrtc_engine_->publish(user_select_mediaType_);
            if (err_code != 0) {
                pub_flag = false;
                emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call publish.\n").c_str()));
                return;
            }

            ui->pushButton_PubStream->setText("取消发布本地资源");

            if (user_select_mediaType_ != rcrtc::RCRTCMediaType::AUDIO) {
                // todo
                rtc_video_render* render = new rtc_video_render(this);
                connect(render, &rtc_video_render::sigSendSdkResult, this, &RCRTCMeeting::onRecvSdkResultLog);

                // render->setWindowTitle("223334");
                render->setFixedSize(400,300);
                render->attachVideoRender(rcrtc_engine_);
                ui->verticalLayout->addWidget(render);
            }
        } else {
            err_code = rcrtc_engine_->unpublish(user_select_mediaType_);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call unpublish.\n").c_str()));
            if (err_code != 0) {
                pub_flag = false;
            }
            err_code = rcrtc_engine_->removeLocalView();
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call removeLocalView.\n").c_str()));
            ui->pushButton_PubStream->setText("发布本地资源");
        }
    }
}

void RCRTCMeeting::onSubRemoteStreamButton() {
    if (!rcrtc_engine_) {
        return;
    }

    // note: 订阅列表时，mediatype固定为一种，用的用户只发布音频不灵活，只能等接口报错？
    // todo: cache sub mediatypes
    std::map<rcrtc::RCRTCStreamKey, rcrtc::RCRTCMediaType> streams;
    mtx_remote_streams_.lock();
    streams = remote_streams_;
    mtx_remote_streams_.unlock();

    int32_t err_code = -1;
    for (auto it=streams.begin(); it!=streams.end(); ++it) {
        if (it->second == rcrtc::RCRTCMediaType::VIDEO) {
            // note: 要先创建窗口再订阅视频流，否则接口返回正确但无视频渲染输出 【开发文档未提及】
            // todo render del?
            rtc_video_render* render = new rtc_video_render(this);
            connect(render, &rtc_video_render::sigSendSdkResult, this, &RCRTCMeeting::onRecvSdkResultLog);

            // render->setWindowTitle("223334");
            render->setFixedSize(400,300);
            render->attachVideoRender(rcrtc_engine_, it->first.user_id);
            ui->verticalLayout->addWidget(render);

            err_code = rcrtc_engine_->subscribe(it->first.user_id, it->second);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call subscribe video.\n").c_str()));
        }
        if (it->second == rcrtc::RCRTCMediaType::AUDIO) {
            err_code = rcrtc_engine_->subscribe(it->first.user_id, it->second);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call subscribe audio.\n").c_str()));
        }
    }

    ui->pushButton_SubRemote->setEnabled(false);
}

void RCRTCMeeting::onRemotePublished(const QString& userId, qint32 mediaType) {
    mtx_remote_streams_.lock();
    rcrtc::RCRTCStreamKey sk(userId.toStdString(), CUtils::rtcMediaTypeName((rcrtc::RCRTCMediaType)mediaType));
    auto it = remote_streams_.find(sk);
    if (it == remote_streams_.end()) {
        remote_streams_.insert(std::pair<rcrtc::RCRTCStreamKey, rcrtc::RCRTCMediaType>(sk, (rcrtc::RCRTCMediaType)mediaType));
    }
    mtx_remote_streams_.unlock();
    // 有新资源发布使能订阅按钮
    ui->pushButton_SubRemote->setEnabled(true);
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(0, "cache remote user pub meidainfo.\n").c_str()));
}

void RCRTCMeeting::onRemoteUnpublished(const QString& userId, qint32 mediaType) {
    bool unsub_flag = false;
    mtx_remote_streams_.lock();
    rcrtc::RCRTCStreamKey sk(userId.toStdString(), CUtils::rtcMediaTypeName((rcrtc::RCRTCMediaType)mediaType));
    auto it = remote_streams_.find(sk);
    if (it != remote_streams_.end()) {
        remote_streams_.erase(it);
        unsub_flag = true;
    }
    mtx_remote_streams_.unlock();

    // 检测到远端取消发布后取消订阅
    if (unsub_flag) {
        int32_t err_code = rcrtc_engine_->unsubscribe(userId.toStdString(), (rcrtc::RCRTCMediaType)mediaType);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call unsubscribe.\n").c_str()));

        if ((rcrtc::RCRTCMediaType)mediaType == rcrtc::RCRTCMediaType::VIDEO) {
            // todo del render
            err_code = rcrtc_engine_->removeRemoteView(userId.toStdString());
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call removeRemoteView.\n").c_str()));
        }
    }
}

void RCRTCMeeting::onSubRemoteCustomStreamButton() {
    if (!rcrtc_engine_) {
        return;
    }

    // todo: cache sub mediatypes
    std::map<rcrtc::RCRTCStreamKey, rcrtc::RCRTCMediaType> custom_streams;
    mtx_remote_custom_streams_.lock();
    custom_streams = remote_custom_streams_;
    mtx_remote_custom_streams_.unlock();

    int32_t err_code = -1;
    for (auto it=custom_streams.begin(); it!=custom_streams.end(); ++it) {
        if (it->second == rcrtc::RCRTCMediaType::VIDEO) {
            // note: 要先创建窗口再订阅视频流，否则接口返回正确但无视频渲染输出 【开发文档未提及】
            // todo render del?
            rtc_video_render* render = new rtc_video_render(this);
            connect(render, &rtc_video_render::sigSendSdkResult, this, &RCRTCMeeting::onRecvSdkResultLog);

            // render->setWindowTitle("223334");
            render->setFixedSize(400,300);
            render->attachCustomVideoRender(rcrtc_engine_, it->first.tag, it->first.user_id);
            ui->verticalLayout->addWidget(render);

            err_code = rcrtc_engine_->subscribeCustomStream(it->first.user_id, it->first.tag, it->second);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call subscribeCustomStream video.\n").c_str()));
        }
        if (it->second == rcrtc::RCRTCMediaType::AUDIO) {
            err_code = rcrtc_engine_->subscribeCustomStream(it->first.user_id, it->first.tag, it->second);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call subscribeCustomStream audio.\n").c_str()));
        }
    }

    ui->pushButton_SubCustomStream->setEnabled(false);
}

void RCRTCMeeting::onRemoteCustomStreamPublished(const QString& userId,
                                 const QString& tag,
                                   qint32 mediaType) {
    mtx_remote_custom_streams_.lock();
    rcrtc::RCRTCStreamKey sk(userId.toStdString(), tag.toStdString());
    auto it = remote_custom_streams_.find(sk);
    if (it == remote_custom_streams_.end()) {
        remote_custom_streams_.insert(std::pair<rcrtc::RCRTCStreamKey, rcrtc::RCRTCMediaType>(sk, (rcrtc::RCRTCMediaType)mediaType));
    }
    mtx_remote_custom_streams_.unlock();
    // 有新资源发布使能订阅按钮
    ui->pushButton_SubCustomStream->setEnabled(true);
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(0, "cache remote user custom stream pub meidainfo.\n").c_str()));
}

void RCRTCMeeting::onRemoteCustomStreamUnpublished(const QString& userId,
                                   const QString& tag,
                                     qint32 mediaType) {
    bool unsub_flag = false;
    mtx_remote_custom_streams_.lock();
    rcrtc::RCRTCStreamKey sk(userId.toStdString(), tag.toStdString());
    auto it = remote_custom_streams_.find(sk);
    if (it != remote_custom_streams_.end()) {
        remote_custom_streams_.erase(it);
        unsub_flag = true;
    }
    mtx_remote_custom_streams_.unlock();

    // 检测到远端取消发布后取消订阅
    if (unsub_flag) {
        int32_t err_code = rcrtc_engine_->unsubscribeCustomStream(userId.toStdString(), tag.toStdString(), (rcrtc::RCRTCMediaType)mediaType);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call unsubscribeCustomStream.\n").c_str()));

        if ((rcrtc::RCRTCMediaType)mediaType == rcrtc::RCRTCMediaType::VIDEO) {
            // todo del render
            err_code = rcrtc_engine_->removeRemoteCustomStreamView(userId.toStdString(), tag.toStdString());
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call removeRemoteCustomStreamView.\n").c_str()));
        }
    }
}

void RCRTCMeeting::onPubDesktopButton() {
    static bool pub_flag = false;
    if (!rcrtc_engine_) {
        return;
    }
    pub_flag = !pub_flag;

    //todo tag manage
    std::string stream_tag = "default monitor";
    int32_t err_code = -1;
    if (pub_flag) {
        std::list<rcrtc::RCRTCDesktopSource*> desktops;
        err_code = rcrtc_engine_->getMonitorList(desktops);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call getMonitorList.\n").c_str()));

        if (0 == err_code && !desktops.empty()) {
            err_code = rcrtc_engine_->createCustomStreamByMonitor(desktops.front(), stream_tag);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call createCustomStreamByMonitor.\n").c_str()));
        }
        if (0 != err_code) {
            pub_flag = false;
            return;
        }

        ui->pushButton_PubDesktop->setText("取消屏幕共享");
        rtc_video_render* render = new rtc_video_render(this);
        connect(render, &rtc_video_render::sigSendSdkResult, this, &RCRTCMeeting::onRecvSdkResultLog);
        // render->setWindowTitle("223334");
        render->setFixedSize(400,300);
        render->attachCustomVideoRender(rcrtc_engine_, stream_tag);
        ui->verticalLayout->addWidget(render);

        err_code = rcrtc_engine_->publishCustomStream(stream_tag, rcrtc::RCRTCMediaType::VIDEO);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call publishCustomStream.\n").c_str()));
    } else {
        err_code = rcrtc_engine_->unpublishCustomStream(stream_tag, rcrtc::RCRTCMediaType::VIDEO);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call unpublishCustomStream.\n").c_str()));
        err_code = rcrtc_engine_->removeLocalCustomStreamView(stream_tag);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call removeLocalCustomStreamView.\n").c_str()));
        std::list<std::string> destroy_tags;
        destroy_tags.push_back(stream_tag);
        err_code = rcrtc_engine_->destroyCustomStream(destroy_tags);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call destroyCustomStream.\n").c_str()));
        pub_flag = false;
        ui->pushButton_PubDesktop->setText("发布屏幕共享");
    }
}

void RCRTCMeeting::onSwitchRoleButton() {
    static rcrtc::RCRTCRole cur_role = user_role_;
    if (cur_role == rcrtc::RCRTCRole::MEETING_MEMBER) {
        ui->pushButton_SwitchRole->setEnabled(false);
        return;
    }
    if (!rcrtc_engine_) {
        return;
    }
    int32_t err_code = -1;
    rcrtc::RCRTCRole to_role = cur_role;
    if (cur_role == rcrtc::RCRTCRole::LIVE_BROADCASTER) {
        to_role = rcrtc::RCRTCRole::LIVE_AUDIENCE;
        err_code = rcrtc_engine_->switchLiveRole(to_role);
        if (0 == err_code) {
            ui->pushButton_PubStream->setEnabled(false);
            ui->pushButton_PubDesktop->setEnabled(false);
        }
    }
    if (cur_role == rcrtc::RCRTCRole::LIVE_AUDIENCE) {
        to_role = rcrtc::RCRTCRole::LIVE_BROADCASTER;
        err_code = rcrtc_engine_->switchLiveRole(to_role);
        if (0 == err_code) {
            ui->pushButton_PubStream->setEnabled(true);
            ui->pushButton_PubDesktop->setEnabled(true);
        }
    }

    if (cur_role != to_role) {
        cur_role = to_role;
        std::ostringstream ostr;
        ostr << "ROOMID: " << rtc_roomid_ << " " << CUtils::rtcUserRoleName(cur_role)
             << " " << CUtils::rtcMediaTypeName(user_select_mediaType_);
        QString win_title = QString(ostr.str().c_str());
        this->setWindowTitle(win_title);
    }
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "local call switchLiveRole.\n").c_str()));
}
