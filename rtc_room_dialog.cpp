#include "rtc_room_dialog.h"
#include "ui_rtc_room_dialog.h"

#include <iostream>
#include <sstream>
#include <QDialog>
#include <QMessageBox>

#include "cutils.h"
#include "rtc_device_manger.h"
#include "rtc_video_render.h"

RtcRoomDialog::RtcRoomDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RtcRoomDialog),
    rcrtc_engine_(nullptr)
{
    ui->setupUi(this);
    this->resize(800,600);
    // 去掉用 &~ 显示用 |
    //setWindowFlags(windowFlags() &~ Qt::WindowCloseButtonHint &~ Qt::WindowMinimizeButtonHint &~ Qt::WindowMaximizeButtonHint);
    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    connect(ui->pushButton_PubStream, &QPushButton::clicked, this, &RtcRoomDialog::onPublishLocalStreamButton);
    connect(ui->pushButton_SubRemote, &QPushButton::clicked, this, &RtcRoomDialog::onSubRemoteStreamButton);
    connect(ui->pushButton_SubCustomStream, &QPushButton::clicked, this, &RtcRoomDialog::onSubRemoteCustomStreamButton);
    connect(ui->pushButton_PubDesktop, &QPushButton::clicked, this, &RtcRoomDialog::onPubDesktopButton);
    connect(ui->pushButton_SwitchRole, &QPushButton::clicked, this, &RtcRoomDialog::onSwitchRoleButton);
    connect(ui->pushButton_SubLiveMix, &QPushButton::clicked, this, &RtcRoomDialog::onSubLiveMixButton);
    connect(ui->pushButton_SetLiveMix, &QPushButton::clicked, this, &RtcRoomDialog::onSetLiveMixButton);
    connect(ui->pushButton_Quit, &QPushButton::clicked, this, &RtcRoomDialog::onLeavRoom);

    ui->pushButton_PubStream->setEnabled(true);
    ui->pushButton_SubRemote->setEnabled(false);
    ui->pushButton_SubCustomStream->setEnabled(false);
    ui->pushButton_PubDesktop->setEnabled(true);
    ui->pushButton_SwitchRole->setEnabled(false);

    ui->pushButton_SubLiveMix->setEnabled(false);
    ui->pushButton_SetLiveMix->setEnabled(false);
}

RtcRoomDialog::~RtcRoomDialog()
{
    delete ui;
}

void RtcRoomDialog::onRecvSdkResultLog(const QString& line) {
    emit sigSendSdkResult(line);
}

// 根据实际情况设置视频参数，推荐根据分辨率获取码率范围
bool RtcRoomDialog::SetDefaultVideoConfig() {
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
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call setVideoConfig").c_str()));
    return 0 == err_code;
}

// 加入指定房间
bool RtcRoomDialog::EnterRoom(rcrtc::RCRTCEngine* engine, const std::string& roomid,
                             const rcrtc::RCRTCRole& userRole, const rcrtc::RCRTCJoinType& joinType,
                             const rcrtc::RCRTCMediaType& mediaType) {
    int32_t err_code = -1;
    if (engine && !roomid.empty()) {
        rcrtc_engine_ = engine;
        rtc_roomid_ = roomid;
    } else {
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(-1, "RoomDialog invalid parameters to rtcroom dialog").c_str()));
        return false;
    }

    rcrtc::RCRTCRoomSetup* setup = rcrtc::RCRTCRoomSetup::create();
    if(nullptr == setup){
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call RCRTCRoomSetup::create failed").c_str()));
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
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call joinRoom").c_str()));

    if (0 == err_code) {
        std::ostringstream ostr;
        ostr << "ROOMID: " << roomid << " " << CUtils::rtcUserRoleName(userRole)
             << " " << CUtils::rtcMediaTypeName(mediaType);
        QString win_title = QString(ostr.str().c_str());
        this->setWindowTitle(win_title);

        if (user_role_ != rcrtc::RCRTCRole::MEETING_MEMBER) {
            ui->pushButton_SwitchRole->setEnabled(true);
            if (user_role_ == rcrtc::RCRTCRole::LIVE_BROADCASTER) {
                ui->pushButton_SetLiveMix->setEnabled(true);
            } else {
                ui->pushButton_SubLiveMix->setEnabled(true);
            }
        }
    }

    return err_code == 0;
}

void RtcRoomDialog::onLeavRoom() {
    int32_t err_code = -1;
    err_code = rcrtc_engine_->leaveRoom();
    if (err_code != 0) {
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call leaveRoom").c_str()));
    }
    emit sigLeaveRtcRoom();
}

void RtcRoomDialog::onPublishLocalStreamButton() {
    static bool pub_flag = false;
    pub_flag = !pub_flag;

    // todo 先使用静态方式存储，便于取消发布时能够删除对应窗体
    static RtcVideoRender* render_widget = nullptr;

    int32_t err_code = -1;
    if (rcrtc_engine_) {
        if (pub_flag) {
            if (user_select_mediaType_ != rcrtc::RCRTCMediaType::AUDIO) {
                SetDefaultVideoConfig();
            }
            err_code = rcrtc_engine_->publish(user_select_mediaType_);
            if (err_code != 0) {
                pub_flag = false;
                emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call publish").c_str()));
                return;
            }

            ui->pushButton_PubStream->setText("取消发布本地资源");

            if (user_select_mediaType_ != rcrtc::RCRTCMediaType::AUDIO) {
                render_widget = new RtcVideoRender(this);
                connect(render_widget, &RtcVideoRender::sigSendSdkResult, this, &RtcRoomDialog::onRecvSdkResultLog);

                // render->setWindowTitle("223334");
                render_widget->setFixedSize(400,300);
                render_widget->attachVideoRender(rcrtc_engine_);
                ui->verticalLayout->addWidget(render_widget);
            }
        } else {
            err_code = rcrtc_engine_->removeLocalView();
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call removeLocalView").c_str()));

            err_code = rcrtc_engine_->unpublish(user_select_mediaType_);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call unpublish").c_str()));
            if (err_code != 0) {
                pub_flag = false;
            }

            ui->verticalLayout->removeWidget(render_widget);
            delete render_widget;
            render_widget = nullptr;
            ui->pushButton_PubStream->setText("发布本地资源");
        }
    }
}

void RtcRoomDialog::onSubRemoteStreamButton() {
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
    mtx_sub_render_widgets_.lock();
    for (auto it=streams.begin(); it!=streams.end(); ++it) {
        if (it->second == rcrtc::RCRTCMediaType::VIDEO) {
            // note: 要先创建窗口再订阅视频流，否则接口返回正确但无视频渲染输出 【开发文档未提及】
            RtcVideoRender* render = new RtcVideoRender(this);
            connect(render, &RtcVideoRender::sigSendSdkResult, this, &RtcRoomDialog::onRecvSdkResultLog);

            // render->setWindowTitle("223334");
            render->setFixedSize(400,300);
            render->attachVideoRender(rcrtc_engine_, it->first.user_id);
            ui->verticalLayout->addWidget(render);
            sub_render_widgets_.insert(std::pair<rcrtc::RCRTCStreamKey, RtcVideoRender*>(it->first, render));
            err_code = rcrtc_engine_->subscribe(it->first.user_id, it->second);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call subscribe rtc_video").c_str()));
        }
        if (it->second == rcrtc::RCRTCMediaType::AUDIO) {
            err_code = rcrtc_engine_->subscribe(it->first.user_id, it->second);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call subscribe rtc_audio").c_str()));
        }
    }
    mtx_sub_render_widgets_.unlock();

    ui->pushButton_SubRemote->setEnabled(false);
}

void RtcRoomDialog::onRemotePublished(const QString& userId, qint32 mediaType) {
    mtx_remote_streams_.lock();
    rcrtc::RCRTCStreamKey sk(userId.toStdString(), CUtils::rtcMediaTypeName((rcrtc::RCRTCMediaType)mediaType));
    auto it = remote_streams_.find(sk);
    if (it == remote_streams_.end()) {
        remote_streams_.insert(std::pair<rcrtc::RCRTCStreamKey, rcrtc::RCRTCMediaType>(sk, (rcrtc::RCRTCMediaType)mediaType));
    }
    mtx_remote_streams_.unlock();
    // 有新资源发布使能订阅按钮
    ui->pushButton_SubRemote->setEnabled(true);
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(0, "RoomDialog cache remote user pub meidainfo").c_str()));
}

void RtcRoomDialog::onRemoteUnpublished(const QString& userId, qint32 mediaType) {
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
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call unsubscribe").c_str()));

        if ((rcrtc::RCRTCMediaType)mediaType == rcrtc::RCRTCMediaType::VIDEO) {
            // todo del render
            err_code = rcrtc_engine_->removeRemoteView(userId.toStdString());
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call removeRemoteView").c_str()));
        }

        mtx_sub_render_widgets_.lock();
        auto it = sub_render_widgets_.find(sk);
        if (it != sub_render_widgets_.end()) {
            ui->verticalLayout->removeWidget(it->second);
            delete it->second;
        }
        mtx_sub_render_widgets_.unlock();
    }
}

void RtcRoomDialog::onSubRemoteCustomStreamButton() {
    if (!rcrtc_engine_) {
        return;
    }

    // todo: cache sub mediatypes
    std::map<rcrtc::RCRTCStreamKey, rcrtc::RCRTCMediaType> custom_streams;
    mtx_remote_custom_streams_.lock();
    custom_streams = remote_custom_streams_;
    mtx_remote_custom_streams_.unlock();

    int32_t err_code = -1;
    mtx_sub_render_widgets_.lock();
    for (auto it=custom_streams.begin(); it!=custom_streams.end(); ++it) {
        if (it->second == rcrtc::RCRTCMediaType::VIDEO) {
            // note: 要先创建窗口再订阅视频流，否则接口返回正确但无视频渲染输出 【开发文档未提及】
            RtcVideoRender* render = new RtcVideoRender(this);
            connect(render, &RtcVideoRender::sigSendSdkResult, this, &RtcRoomDialog::onRecvSdkResultLog);

            // render->setWindowTitle("223334");
            render->setFixedSize(400,300);
            render->attachCustomVideoRender(rcrtc_engine_, it->first.tag, it->first.user_id);
            ui->verticalLayout->addWidget(render);
            sub_render_widgets_.insert(std::pair<rcrtc::RCRTCStreamKey, RtcVideoRender*>(it->first, render));

            err_code = rcrtc_engine_->subscribeCustomStream(it->first.user_id, it->first.tag, it->second);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call subscribeCustomStream rtc_video").c_str()));
        }
        if (it->second == rcrtc::RCRTCMediaType::AUDIO) {
            err_code = rcrtc_engine_->subscribeCustomStream(it->first.user_id, it->first.tag, it->second);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call subscribeCustomStream rtc_audio").c_str()));
        }
    }
    mtx_sub_render_widgets_.unlock();

    ui->pushButton_SubCustomStream->setEnabled(false);
}

void RtcRoomDialog::onRemoteCustomStreamPublished(const QString& userId,
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
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(0, "RoomDialog cache remote user custom stream pub meidainfo").c_str()));
}

void RtcRoomDialog::onRemoteCustomStreamUnpublished(const QString& userId,
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
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call unsubscribeCustomStream").c_str()));

        if ((rcrtc::RCRTCMediaType)mediaType == rcrtc::RCRTCMediaType::VIDEO) {
            // todo del render
            err_code = rcrtc_engine_->removeRemoteCustomStreamView(userId.toStdString(), tag.toStdString());
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call removeRemoteCustomStreamView").c_str()));
        }
        mtx_sub_render_widgets_.lock();
        auto it = sub_render_widgets_.find(sk);
        if (it != sub_render_widgets_.end()) {
            ui->verticalLayout->removeWidget(it->second);
            delete it->second;
        }
        mtx_sub_render_widgets_.unlock();
    }
}

void RtcRoomDialog::onPubDesktopButton() {
    static bool pub_flag = false;
    if (!rcrtc_engine_) {
        return;
    }
    pub_flag = !pub_flag;

    // todo 先使用静态方式存储，便于取消发布时能够删除对应窗体
    static RtcVideoRender* render_widget = nullptr;

    //todo tag manage
    // note: tag 中不能包含空格
    std::string stream_tag = "default-monitor";
    int32_t err_code = -1;
    if (pub_flag) {
        std::list<rcrtc::RCRTCDesktopSource*> desktops;
        err_code = rcrtc_engine_->getMonitorList(desktops);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call getMonitorList").c_str()));

        if (0 == err_code && !desktops.empty()) {
            err_code = rcrtc_engine_->createCustomStreamByMonitor(desktops.front(), stream_tag);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call createCustomStreamByMonitor").c_str()));
        }
        if (0 != err_code) {
            pub_flag = false;
            return;
        }

        err_code = rcrtc_engine_->publishCustomStream(stream_tag, rcrtc::RCRTCMediaType::VIDEO);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call publishCustomStream").c_str()));

        ui->pushButton_PubDesktop->setText("取消屏幕共享");
        render_widget = new RtcVideoRender(this);
        connect(render_widget, &RtcVideoRender::sigSendSdkResult, this, &RtcRoomDialog::onRecvSdkResultLog);
        // render->setWindowTitle("223334");
        render_widget->setFixedSize(400,300);
        render_widget->attachCustomVideoRender(rcrtc_engine_, stream_tag);
        ui->verticalLayout->addWidget(render_widget);
    } else {
        err_code = rcrtc_engine_->unpublishCustomStream(stream_tag, rcrtc::RCRTCMediaType::VIDEO);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call unpublishCustomStream").c_str()));
        err_code = rcrtc_engine_->removeLocalCustomStreamView(stream_tag);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call removeLocalCustomStreamView").c_str()));
        std::list<std::string> destroy_tags;
        destroy_tags.push_back(stream_tag);
        err_code = rcrtc_engine_->destroyCustomStream(destroy_tags);
        emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call destroyCustomStream").c_str()));
        pub_flag = false;
        ui->pushButton_PubDesktop->setText("发布屏幕共享");
        ui->verticalLayout->removeWidget(render_widget);
        delete render_widget;
        render_widget = nullptr;
    }
}

void RtcRoomDialog::onSwitchRoleButton() {
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
            ui->pushButton_SetLiveMix->setEnabled(false);
            ui->pushButton_SubLiveMix->setEnabled(true);
        }
    }
    if (cur_role == rcrtc::RCRTCRole::LIVE_AUDIENCE) {
        to_role = rcrtc::RCRTCRole::LIVE_BROADCASTER;
        err_code = rcrtc_engine_->switchLiveRole(to_role);
        if (0 == err_code) {
            ui->pushButton_PubStream->setEnabled(true);
            ui->pushButton_PubDesktop->setEnabled(true);
            ui->pushButton_SetLiveMix->setEnabled(true);
            ui->pushButton_SubLiveMix->setEnabled(false);
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
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call switchLiveRole").c_str()));
}

void RtcRoomDialog::onSubLiveMixButton() {
    static bool sub_flag = false;
    sub_flag = !sub_flag;

    // todo 先使用静态方式存储，便于取消发布时能够删除对应窗体
    static RtcVideoRender* render_widget = nullptr;

    int32_t err_code = -1;
    if (rcrtc_engine_) {
        if (sub_flag) {
            err_code = rcrtc_engine_->subscribeLiveMix(rcrtc::RCRTCMediaType::AUDIO_VIDEO);
            if (err_code != 0) {
                sub_flag = false;
                emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call subscribeLiveMix rtc_video_audio").c_str()));
                return;
            }

            ui->pushButton_PubStream->setText("取消订阅直播合流");

            render_widget = new RtcVideoRender(this);
            connect(render_widget, &RtcVideoRender::sigSendSdkResult, this, &RtcRoomDialog::onRecvSdkResultLog);

            // render->setWindowTitle("223334");
            render_widget->setFixedSize(400,300);
            render_widget->attachLiveMixVideoRender(rcrtc_engine_);
            ui->verticalLayout->addWidget(render_widget);
        } else {
            err_code = rcrtc_engine_->unsubscribeLiveMix(rcrtc::RCRTCMediaType::AUDIO_VIDEO);
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call unsubscribeLiveMix video_audio").c_str()));
            if (err_code != 0) {
                sub_flag = false;
            }
            err_code = rcrtc_engine_->removeLiveMixView();
            emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call removeLiveMixView").c_str()));
            ui->pushButton_PubStream->setText("订阅直播合流");
            ui->verticalLayout->removeWidget(render_widget);
            delete render_widget;
            render_widget = nullptr;
        }
    }
}

void RtcRoomDialog::onSetLiveMixButton() {
    if (!rcrtc_engine_) {
        return;
    }

    int32_t err_code = rcrtc_engine_->setLiveMixBackgroundColor(50,200,50);
    emit sigSendSdkResult(QString(CUtils::formatSdkResult(err_code, "RoomDialog call setLiveMixBackgroundColor").c_str()));
}
