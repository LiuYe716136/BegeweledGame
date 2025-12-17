#include "GameWidget.h"
#include "ui_widget.h" // 必须包含由 .ui 文件生成的头文件
#include <QDebug>      // 用于调试输出

// ==========================================
// 构造与析构
// ==========================================

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget),
      m_game(new GameMap()) // 实例化游戏逻辑
      ,
      m_timer(new QTimer(this)) // 实例化定时器
      ,
      m_selectedPos(-1, -1) // 初始化为无效坐标
      ,
      m_score(0) {
  ui->setupUi(this);

  // 加载图片和音效资源
  loadResources();

  // 初始化游戏数据
  initGame();

  // 连接定时器信号 (核心循环)
  connect(m_timer, &QTimer::timeout, this, &GameWidget::updateGameState);
}

GameWidget::~GameWidget() {
  delete m_game; // 释放非 QObject 对象
  delete ui;
}

// ==========================================
// Qt 事件重写
// ==========================================

void GameWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);

  // TODO: 绘制背景
  // painter.drawPixmap(...);

  // TODO: 绘制游戏区域 (遍历 m_game->m_map)
  // 双重循环 check 每一个 Gem 的类型，计算坐标并绘制图片
  // 如果是 m_selectedPos，可以画一个选中框
}

void GameWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() != Qt::LeftButton) {
    return;
  }

  int r, c;
  // 将屏幕坐标转换为矩阵行列
  if (!screenToRowCol(event->pos(), r, c)) {
    return; // 点击在游戏区域外
  }

  // TODO: 处理点击逻辑
  // 1. 如果当前没有选中 (m_selectedPos == -1,-1) -> 选中当前 (r,c)
  // 2. 如果已经选中了 -> 判断是否相邻 -> 尝试交换
  // 3. 触发 repaint() 重绘界面
}

// ==========================================
// 界面交互槽函数
// ==========================================

void GameWidget::on_btn_reset_clicked() {
  // 重置游戏
  initGame();
  update(); // 强制重绘
}

void GameWidget::on_btn_hint_clicked() {
  // TODO: 调用 m_game->findBestMove() 或类似逻辑
  // 高亮显示提示
}

void GameWidget::on_btn_undo_clicked() {
  // TODO: 调用 m_game->undo()
  // 如果成功，更新界面
  // if (m_game->undo()) { update(); }
}

void GameWidget::updateGameState() {
  // 这是一个动画状态机，由 m_timer 触发

  // TODO: 编写消除流程
  // 1. 检查是否有消除项 m_game->checkMatches()
  // 2. 如果有 -> 播放音效 -> m_game->eliminate() -> 增加分数
  // 3. 如果无消除 -> m_game->applyGravity() (下落)
  // 4. 如果静止且无消除 -> 停止定时器，等待玩家输入

  update(); // 每次状态更新都要重绘
}

// ==========================================
// 内部辅助函数
// ==========================================

void GameWidget::initGame() {
  // 初始化逻辑数据
  m_game->init();

  m_score = 0;
  m_selectedPos = QPoint(-1, -1);

  // UI 更新
  // ui->label_score->setText("0");

  update();
}

void GameWidget::loadResources() {
  // TODO: 加载图片资源到成员变量（如果有 pixmap 缓存）

  // TODO: 初始化音效
  // m_soundSwap = new QSoundEffect(this);
  // m_soundSwap->setSource(QUrl::fromLocalFile(":/res/swap.wav"));
}

bool GameWidget::screenToRowCol(QPoint pt, int &r, int &c) {
  // TODO: 根据界面布局计算
  // 假设每个格子大小是 GEM_SIZE，起始偏移是 OFFSET_X, OFFSET_Y

  // r = (pt.y() - OFFSET_Y) / GEM_SIZE;
  // c = (pt.x() - OFFSET_X) / GEM_SIZE;

  // 检查 r, c 是否在合法范围内
  // if (r >= 0 && r < ROW && c >= 0 && c < COL) return true;

  return false; // 默认返回 false
}
