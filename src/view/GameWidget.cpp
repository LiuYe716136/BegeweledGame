#include "GameWidget.h"
#include "ui_GameWidget.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStyleOption>
#include <QMessageBox>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::GameWidget),
    m_game(new GameMap()),
    m_timer(new QTimer(this)),
    m_timeTimer(new QTimer(this)),  // 初始化计时定时器
    m_selectedPos(-1, -1),
    m_state(IDLE),
    m_score(0),
    m_soundSwap(nullptr),
    m_soundEliminate(nullptr),
    m_soundClick(nullptr) {
    ui->setupUi(this);

    // 加载资源和其他初始化...
    loadResources();
    initGame();

    // 连接定时器信号
    connect(m_timer, &QTimer::timeout, this, &GameWidget::updateGameState);

    // 初始化计时相关
    m_remainingTime = TOTAL_TIME;
    ui->progressBar_time->setRange(0, TOTAL_TIME);
    ui->progressBar_time->setValue(TOTAL_TIME);
    connect(m_timeTimer, &QTimer::timeout, this, &GameWidget::updateTime);
    // 设置进度条显示格式为 "剩余秒数s"（关键代码）
    ui->progressBar_time->setFormat("%v s"); // %v 表示当前值，后面拼接 " s"
    ui->progressBar_time->setRange(0, TOTAL_TIME); // 范围 0-120 秒
}
GameWidget::~GameWidget() {
  delete m_game;
  delete m_timer;
  delete ui;
}
void GameWidget::updateTime() {
    m_remainingTime--;
    ui->progressBar_time->setValue(m_remainingTime);

    // 时间到，游戏结束
    if (m_remainingTime <= 0) {
        m_timeTimer->stop();
        m_timer->stop();  // 停止游戏逻辑定时器
        m_state = GAME_OVER;

        // 显示游戏结束弹窗
        QMessageBox::information(this, "游戏结束",
                                 QString("时间已到！你的最终得分是：%1").arg(m_score));

        // 重置游戏（分数归零，地图重置）
        initGame();
    }
}

void GameWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::SmoothPixmapTransform); // 抗锯齿

  // 绘制默认样式
  QStyleOption opt;
  opt.initFrom(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

  // 绘制宝石
  // 获取游戏板的位置和大小
  QRect boardRect = ui->frame_board->geometry();
  int boardX = boardRect.x();
  int boardY = boardRect.y();
  int boardWidth = boardRect.width();
  int boardHeight = boardRect.height();

  // 计算宝石偏移量
  int offsetX = (boardWidth - GEM_SIZE * COL) / 2;
  int offsetY = (boardHeight - GEM_SIZE * ROW) / 2;

  // 遍历地图，绘制所有宝石
  for (int r = 0; r < ROW; r++) {
    for (int c = 0; c < COL; c++) {
      // 获取宝石类型
      GemType gemType = m_game->getType(r, c);

      // 根据宝石类型获取图片路径
      QString imagePath;
      switch (gemType) {
      case RED:
        imagePath = ":/gems/assets/images/red.png";
        break;
      case ORANGE:
        imagePath = ":/gems/assets/images/orange.png";
        break;
      case YELLOW:
        imagePath = ":/gems/assets/images/yellow.png";
        break;
      case GREEN:
        imagePath = ":/gems/assets/images/green.png";
        break;
      case WHITE:
        imagePath = ":/gems/assets/images/white.png";
        break;
      case BLUE:
        imagePath = ":/gems/assets/images/blue.png";
        break;
      case PURPLE:
        imagePath = ":/gems/assets/images/purple.png";
        break;
      default:
        continue; // 跳过空宝石
      }

      // 加载并绘制宝石图片
      QPixmap gemPixmap(imagePath);
      if (!gemPixmap.isNull()) {
        // 计算宝石的绘制位置
        int x = boardX + offsetX + c * GEM_SIZE;
        int y = boardY + offsetY + r * GEM_SIZE;

        // 绘制宝石
        painter.drawPixmap(x, y, GEM_SIZE, GEM_SIZE,
                           gemPixmap.scaled(GEM_SIZE, GEM_SIZE,
                                            Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation));
      }
    }
  }

  // 如果有选中的宝石，绘制选中框
  if (m_selectedPos != QPoint(-1, -1)) {
    int r = m_selectedPos.y();
    int c = m_selectedPos.x();
    int x = boardX + offsetX + c * GEM_SIZE;
    int y = boardY + offsetY + r * GEM_SIZE;

    // 绘制选中框（黄色边框）
    QPen pen(QColor(255, 215, 0), 3);
    painter.setPen(pen);
    painter.drawRect(x, y, GEM_SIZE, GEM_SIZE);
  }

  // 绘画分数
  if (ui->label_score) {
    // 绘制阴影（黑色，右下偏移2px）
    painter.save(); // 保存当前画家状态
    painter.setPen(Qt::black);
    painter.setFont(ui->label_score->font());
    QRect shadowRect = ui->label_score->geometry().translated(2, 2);
    painter.drawText(shadowRect, Qt::AlignCenter, ui->label_score->text());
    painter.restore(); // 恢复状态

    // 绘制金色主文字
    painter.setPen(QColor(255, 215, 0)); // 金色
    painter.setFont(ui->label_score->font());
    painter.drawText(ui->label_score->geometry(), Qt::AlignCenter,
                     ui->label_score->text());
  }
}

void GameWidget::mousePressEvent(QMouseEvent *event) {
    // 如果游戏结束则不响应点击
    if (m_state == GAME_OVER) {
        return;
    }
  if (event->button() != Qt::LeftButton) {
    return;
  }

  int r, c;
  if (!screenToRowCol(event->pos(), r, c)) {
    return;
  }

  if (m_selectedPos == QPoint(-1, -1)) {
    m_selectedPos = QPoint(c, r);
  } else {
    // 检查是否相邻，确保交换有效（避免斜向交换）
    int selectedR = m_selectedPos.y();
    int selectedC = m_selectedPos.x();
    bool isAdjacent = false;
    if ((abs(r - selectedR) == 1 && c == selectedC) ||
        (abs(c - selectedC) == 1 && r == selectedR)) {
      isAdjacent = true;
    }

    if (isAdjacent) {
      // 交换前保存当前状态和分数（核心修正）
      m_game->saveState(m_score);
      m_game->swap(selectedR, selectedC, r, c);

      std::vector<QPoint> matches = m_game->checkMatches();

      if (matches.empty()) {
        // 无匹配时交换回来，并删除无效快照
        m_game->swap(r, c, selectedR, selectedC);
        m_game->popLastState(); // 关键：清除无效状态
      } else {
        m_timer->start(500);
      }
    }
    m_selectedPos = QPoint(-1, -1);
  }

  repaint();
}

void GameWidget::on_btn_reset_clicked() {
    m_timeTimer->stop();  // 先停止当前计时
    initGame();           // 重新初始化游戏（会重新开始计时）
    update();
}

void GameWidget::on_btn_hint_clicked() {}

void GameWidget::on_btn_undo_clicked() {
  if (m_game->undo()) {
    // 从GameMap获取撤销前的分数并更新
    m_score = m_game->getLastUndoScore();
    ui->label_score->setText(QString::number(m_score));
    update();
  }
}

void GameWidget::updateGameState() {
    std::vector<QPoint> matches = m_game->checkMatches();

    if (!matches.empty()) {
        // 1. 计算本次消除的总分（按宝石颜色累加分值）
        int roundScore = 0;
        for (const auto& point : matches) {
            int r = point.y();   // QPoint的y对应行，x对应列
            int c = point.x();
            roundScore += m_game->getGemScore(r, c); // 调用获取对应颜色分值的方法
        }

        // 2. 执行消除操作
        m_game->eliminate(matches);

        // 3. 更新总分数并刷新UI
        m_score += roundScore;
        ui->label_score->setText(QString::number(m_score));

        m_timer->start(500);
    } else {
        m_game->applyGravity();
        matches = m_game->checkMatches();
        if (!matches.empty()) {
            m_timer->start(500);
        } else {
            // 下落完成后仍无匹配，检查是否为死局
            if (!m_game->hasPossibleMove()) {
                // 1. 显示死局提示对话框
                QMessageBox::information(this, "游戏提示", "当前已死局，即将重置地图！分数将保留。");

                // 2. 重置地图但不重置分数
                m_game->reset();  // 仅重置地图宝石布局

                // 3. 分数保持不变，无需修改m_score和label_score
                qDebug() << "死局！已重置地图，分数保留";
            }
            m_timer->stop();
        }
    }
    update();
}
// 内部辅助函数

void GameWidget::initGame() {
    // 初始化逻辑数据
    m_game->clearHistory();
    m_game->init();

    m_score = 0;
    m_selectedPos = QPoint(-1, -1);
    m_state = IDLE;  // 重置游戏状态

    // 重置计时
    m_remainingTime = TOTAL_TIME;
    ui->progressBar_time->setValue(TOTAL_TIME);

    // UI 更新
    ui->label_score->setText("0");

    // 开始计时
    m_timeTimer->start(1000);  // 每秒触发一次

    update();
}

void GameWidget::loadResources() {
  // 1. 检查资源路径是否存在
  QFileInfo resourceInfo(":/bgs/assets/images/game_bg.jpg");
  qDebug() << "资源文件存在吗？" << resourceInfo.exists();
  qDebug() << "资源文件路径：" << ":/bgs/assets/images/game_bg.jpg";

  // 2. 尝试使用QPixmap加载
  QPixmap backgroundPixmap;
  bool loadSuccess = backgroundPixmap.load(":/bgs/assets/images/game_bg.jpg");
  qDebug() << "QPixmap加载成功？" << loadSuccess;
  qDebug() << "图片尺寸：" << backgroundPixmap.size();

  // 3. 尝试使用QImage加载
  QImage backgroundImage;
  bool imageLoadSuccess =
      backgroundImage.load(":/bgs/assets/images/game_bg.jpg");
  qDebug() << "QImage加载成功？" << imageLoadSuccess;
  qDebug() << "图片尺寸：" << backgroundImage.size();

  // 4. 尝试使用QFile直接读取
  QFile resourceFile(":/bgs/assets/images/game_bg.jpg");
  if (resourceFile.open(QIODevice::ReadOnly)) {
    qDebug() << "QFile读取成功，文件大小：" << resourceFile.size() << " bytes";
    resourceFile.close();
  } else {
    qDebug() << "QFile读取失败：" << resourceFile.errorString();
  }

  // 5. 设置背景图片
  if (loadSuccess) {
    // 使用border-image代替background-image，确保能自适应窗口大小且完全覆盖
    this->setStyleSheet(
        "#GameWidget { "
        "border-image: url(:/bgs/assets/images/game_bg.jpg) 0 0 0 0 stretch "
        "stretch; "
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
        "font-size: 16px; "
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
        "QFrame { "
        "background-color: rgba(0, 0, 0, 0); "
        "border: 2px solid rgba(255, 255, 255, 50); "
        "border-radius: 5px; "
        "}"
        "QProgressBar { "
        "border: 2px solid grey; "
        "border-radius: 5px; "
        "text-align: center; "
        "background-color: rgba(0,0,0,150); "
        "color: white; "
        "}"
        "QProgressBar::chunk { "
        "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, "
        "stop:0 rgba(0, 180, 0, 255), stop:1 rgba(0, 255, 0, 255)); "
        "border-radius: 3px; "
        "}");
    qDebug() << "背景图片设置完成！";
  }
}

bool GameWidget::screenToRowCol(QPoint pt, int &r, int &c) {
  // 获取游戏板的位置和大小（像素）
  QRect boardRect = ui->frame_board->geometry();
  int boardX = boardRect.x();
  int boardY = boardRect.y();
  int boardWidth = boardRect.width();
  int boardHeight = boardRect.height();

  // 检查点击是否在游戏板内
  if (!boardRect.contains(pt)) {
    return false;
  }

  // 计算宝石偏移量
  int offsetX = (boardWidth - GEM_SIZE * COL) / 2;
  int offsetY = (boardHeight - GEM_SIZE * ROW) / 2;

  // 计算行列坐标
  r = (pt.y() - boardY - offsetY) / GEM_SIZE;
  c = (pt.x() - boardX - offsetX) / GEM_SIZE;

  // 检查行列坐标是否在合法范围内
  if (r >= 0 && r < ROW && c >= 0 && c < COL) {
    return true;
  }

  return false;
}
