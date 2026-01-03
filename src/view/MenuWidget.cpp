#include "MenuWidget.h"
#include "ui_MenuWidget.h"
#include <QFileInfo>
#include <QPixmap>
#include <QDebug>
#include <QPainter>

/**
 * @brief MenuWidget构造函数
 * 初始化UI、设置窗口属性、加载背景和创建音乐按钮
 * @param parent 父窗口部件
 */
MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MenuWidget),
    m_bgMusicPlayer(nullptr),
    m_musicBtn(nullptr),
    m_musicEnabled(true) {
    ui->setupUi(this);
    this->setObjectName("MenuWidget");
    loadBackground();
    createMusicButton();
}

/**
 * @brief MenuWidget析构函数
 * 释放UI资源
 */
MenuWidget::~MenuWidget() {
    delete ui;
}

/**
 * @brief 设置背景音乐播放器
 * 配置背景音乐播放器并开始播放
 * @param player 音乐播放器指针
 */
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

/**
 * @brief 创建音乐控制按钮
 * 初始化音乐按钮的位置、样式和信号槽连接
 */
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

/**
 * @brief 音乐按钮点击事件处理
 * 切换音乐播放状态（播放/暂停）
 */
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

/**
 * @brief 重绘事件处理
 * 绘制背景图片
 * @param event 绘图事件
 */
void MenuWidget::paintEvent(QPaintEvent *event) {
    // 先调用父类方法确保正常绘制
    QWidget::paintEvent(event);

    // 绘制背景图
    QPainter painter(this);
    QPixmap bg(":/bgs/assets/images/login_bg.jpg");
    if (!bg.isNull()) {
        // 按窗口大小拉伸绘制
        painter.drawPixmap(rect(), bg.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}

/**
 * @brief 加载背景样式
 * 设置窗口和子控件的样式表
 */
void MenuWidget::loadBackground() {
    this->setObjectName("MenuWidget");
    this->setStyleSheet(
        // 仅保留子控件样式，删除 #MenuWidget 的背景设置
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

/**
 * @brief 无尽模式按钮点击事件
 * 发送开始无尽模式的信号
 */
void MenuWidget::on_btn_endless_clicked() {
    emit startEndlessMode();
}

/**
 * @brief 挑战模式按钮点击事件
 * 发送开始挑战模式的信号
 */
void MenuWidget::on_btn_challenge_clicked() {
    emit startChallengeMode();
}

/**
 * @brief 排行榜按钮点击事件
 * 发送显示排行榜的信号
 */
void MenuWidget::on_btn_ranking_clicked() {
    emit showRanking();
}
