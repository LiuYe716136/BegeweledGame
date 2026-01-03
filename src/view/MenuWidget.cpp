#include "MenuWidget.h"
#include "ui_MenuWidget.h"
#include <QDebug>
#include <QFileInfo>
#include <QPainter>
#include <QPixmap>

/**
 * @brief MenuWidget构造函数
 * 初始化UI、设置窗口属性
 * @param parent 父窗口部件
 */
MenuWidget::MenuWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::MenuWidget), m_bgMusicPlayer(nullptr),
      m_musicEnabled(true) {
  ui->setupUi(this);
  this->setObjectName("MenuWidget");

  // 连接音乐按钮信号
  connect(ui->btn_music, &QPushButton::clicked, this,
          &MenuWidget::onMusicBtnClicked);
}

/**
 * @brief MenuWidget析构函数
 * 释放UI资源
 */
MenuWidget::~MenuWidget() { delete ui; }

/**
 * @brief 设置背景音乐播放器
 * 配置背景音乐播放器并开始播放
 * @param player 音乐播放器指针
 */
void MenuWidget::setBgMusicPlayer(QMediaPlayer *player) {
  m_bgMusicPlayer = player;
  m_musicEnabled = true;
  if (ui->btn_music) {
    ui->btn_music->setText("♪");
  }
  if (m_bgMusicPlayer && m_musicEnabled) {
    m_bgMusicPlayer->play();
  }
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
      ui->btn_music->setText("♪");
    } else {
      m_bgMusicPlayer->pause();
      ui->btn_music->setText("✕");
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
  QPixmap bg(":/bgs/assets/images/menu_bg.jpg");
  if (!bg.isNull()) {
    // 按窗口大小拉伸绘制
    painter.drawPixmap(rect(), bg.scaled(size(), Qt::IgnoreAspectRatio,
                                         Qt::SmoothTransformation));
  }
}

/**
 * @brief 无尽模式按钮点击事件
 * 发送开始无尽模式的信号
 */
void MenuWidget::on_btn_endless_clicked() { emit startEndlessMode(); }

/**
 * @brief 挑战模式按钮点击事件
 * 发送开始挑战模式的信号
 */
void MenuWidget::on_btn_challenge_clicked() { emit startChallengeMode(); }

/**
 * @brief 排行榜按钮点击事件
 * 发送显示排行榜的信号
 */
void MenuWidget::on_btn_ranking_clicked() { emit showRanking(); }
