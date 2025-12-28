#include "MenuWidget.h"
#include "ui_MenuWidget.h"
#include <QFileInfo>
#include <QPixmap>
#include <QDebug>

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MenuWidget),
    m_bgMusicPlayer(nullptr),
    m_musicBtn(nullptr),
    m_musicEnabled(true) {
    ui->setupUi(this);
    this->setObjectName("MenuWidget");
    loadBackground();
    createMusicButton();
}

MenuWidget::~MenuWidget() {
    delete ui;
}

void MenuWidget::setBgMusicPlayer(QMediaPlayer *player) {
    m_bgMusicPlayer = player;
    m_musicEnabled = true;
    if (m_musicBtn) {
        m_musicBtn->setText("♪");
    }
    if (m_bgMusicPlayer && m_musicEnabled) {
        m_bgMusicPlayer->play();
    }
}

void MenuWidget::createMusicButton() {
    m_musicBtn = new QPushButton(this);
    m_musicBtn->setGeometry(720, 20, 50, 50);
    m_musicBtn->setText("♪");
    m_musicBtn->setStyleSheet(
        "QPushButton { "
        "background-color: rgba(255, 215, 0, 150); "
        "border: 2px solid gold; "
        "border-radius: 25px; "
        "font-size: 24px; "
        "font-weight: bold; "
        "color: #333; "
        "}"
        "QPushButton:hover { "
        "background-color: rgba(255, 215, 0, 200); "
        "}"
        "QPushButton:pressed { "
        "background-color: rgba(255, 215, 0, 100); "
        "}"
    );
    
    connect(m_musicBtn, &QPushButton::clicked, this, &MenuWidget::onMusicBtnClicked);
}

void MenuWidget::onMusicBtnClicked() {
    m_musicEnabled = !m_musicEnabled;
    if (m_bgMusicPlayer) {
        if (m_musicEnabled) {
            m_bgMusicPlayer->play();
            m_musicBtn->setText("♪");
        } else {
            m_bgMusicPlayer->pause();
            m_musicBtn->setText("♪");
        }
    }
}

void MenuWidget::loadBackground() {
    this->setObjectName("MenuWidget");
    this->setStyleSheet(
        "#MenuWidget { "
        "border-image: url(:/bgs/assets/images/login_bg.jpg) 0 0 0 0 stretch stretch; "
        "background-color: #000000; "
        "}"
        "QLabel { "
        "color: white; "
        "font-family: 'Microsoft YaHei'; "
        "font-weight: bold; "
        "}"
        "QPushButton { "
        "background-color: rgba(255, 255, 255, 200); "
        "border: 2px solid #8f8f91; "
        "border-radius: 10px; "
        "padding: 5px; "
        "font-size: 18px; "
        "color: #333; "
        "font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "background-color: rgba(255, 255, 255, 240); "
        "border-color: #ffffff; "
        "}"
        "QPushButton:pressed { "
        "background-color: rgba(200, 200, 200, 200); "
        "}"
    );
}

void MenuWidget::on_btn_endless_clicked() {
    emit startEndlessMode();
}

void MenuWidget::on_btn_challenge_clicked() {
    emit startChallengeMode();
}

void MenuWidget::on_btn_ranking_clicked() {
    emit showRanking();
}