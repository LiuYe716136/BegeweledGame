#include "GameWidget.h"
#include "ui_GameWidget.h" // 必须包含由 .ui 文件生成的头文件
#include <QDebug>      // 用于调试输出
#include <QFile>       // 用于检查文件
#include <QFileInfo>   // 用于文件信息检查
#include <QImage>      // 用于图片加载

// ==========================================
// 构造与析构
// ==========================================

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::GameWidget),
      m_game(new GameMap()) // 实例化游戏逻辑
      ,
      m_timer(new QTimer(this)) // 实例化定时器
      ,
      m_selectedPos(-1, -1) // 初始化为无效坐标
      ,
      m_score(0),
      m_state(IDLE), // 初始化为空闲状态
      m_soundSwap(nullptr),
      m_soundEliminate(nullptr),
      m_soundClick(nullptr) { // 初始化音效指针为nullptr
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
    painter.setRenderHint(QPainter::SmoothPixmapTransform); // 抗锯齿，图片更清晰

    // ========== 第一步：优先绘制背景图片（核心新增逻辑） ==========
    QPixmap bgPixmap(":/bgs/assets/images/game_bg.jpg");
    if (!bgPixmap.isNull()) {
        // 绘制背景并适配窗口大小（铺满整个GameWidget）
        painter.drawPixmap(this->rect(), bgPixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    // ========== 原有宝石绘制逻辑（完全保留，仅调整顺序） ==========
    // 获取游戏板的位置和大小
    QRect boardRect = ui->frame_board->geometry();
    int boardX = boardRect.x();
    int boardY = boardRect.y();
    int boardWidth = boardRect.width();
    int boardHeight = boardRect.height();

    // 计算每个宝石的大小和偏移量
    int gemSize = boardWidth / COL;
    int offsetX = (boardWidth - gemSize * COL) / 2;
    int offsetY = (boardHeight - gemSize * ROW) / 2;

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
                int x = boardX + offsetX + c * gemSize;
                int y = boardY + offsetY + r * gemSize;

                // 绘制宝石（缩放到合适大小）
                painter.drawPixmap(x, y, gemSize, gemSize, gemPixmap.scaled(gemSize, gemSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    }

    // 如果有选中的宝石，绘制选中框
    if (m_selectedPos.x() >= 0 && m_selectedPos.x() < COL && m_selectedPos.y() >= 0 && m_selectedPos.y() < ROW) {
        int r = m_selectedPos.y();
        int c = m_selectedPos.x();
        int x = boardX + offsetX + c * gemSize;
        int y = boardY + offsetY + r * gemSize;

        // 绘制选中框（黄色边框）
        QPen pen(QColor(255, 215, 0), 3);
        painter.setPen(pen);
        painter.drawRect(x, y, gemSize, gemSize);
    }

    // ========== 第二步：修复label_score文字阴影（替代text-shadow） ==========
    if (ui->label_score) {
        // 1. 绘制阴影（黑色，右下偏移2px）
        painter.save(); // 保存当前画家状态
        painter.setPen(Qt::black);
        painter.setFont(ui->label_score->font());
        QRect shadowRect = ui->label_score->geometry().translated(2, 2);
        painter.drawText(shadowRect, Qt::AlignCenter, ui->label_score->text());
        painter.restore(); // 恢复状态

        // 2. 绘制金色主文字
        painter.setPen(QColor(255, 215, 0)); // 金色
        painter.setFont(ui->label_score->font());
        painter.drawText(ui->label_score->geometry(), Qt::AlignCenter, ui->label_score->text());
    }
}
void GameWidget::mousePressEvent(QMouseEvent *event) {
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
        int selectedR = m_selectedPos.y();
        int selectedC = m_selectedPos.x();
        bool isAdjacent = false;
        if ((abs(r - selectedR) == 1 && c == selectedC) || (abs(c - selectedC) == 1 && r == selectedR)) {
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
  // 重置游戏
  initGame();
  update(); // 强制重绘
}

void GameWidget::on_btn_hint_clicked() {
  // TODO: 调用 m_game->findBestMove() 或类似逻辑
  // 高亮显示提示
}

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
        m_game->eliminate(matches);
        // 每次消除的分数 = 匹配数量 * 10（确保单次计算正确）
        m_score += matches.size() * 10;
        ui->label_score->setText(QString::number(m_score));
        m_timer->start(500);
    } else {
        m_game->applyGravity();
        matches = m_game->checkMatches();
        if (!matches.empty()) {
            m_timer->start(500);
        } else {
            m_timer->stop();
        }
    }
    update();
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
  ui->label_score->setText("0");

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
  bool imageLoadSuccess = backgroundImage.load(":/bgs/assets/images/game_bg.jpg");
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
        "border-image: url(:/bgs/assets/images/game_bg.jpg) 0 0 0 0 stretch stretch; "
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
        "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 180, 0, 255), stop:1 rgba(0, 255, 0, 255)); "
        "border-radius: 3px; "
        "}"
    );
    qDebug() << "背景图片设置完成！";
  }
}

bool GameWidget::screenToRowCol(QPoint pt, int &r, int &c) {
  // 获取游戏板的位置和大小
  QRect boardRect = ui->frame_board->geometry();
  int boardX = boardRect.x();
  int boardY = boardRect.y();
  int boardWidth = boardRect.width();
  int boardHeight = boardRect.height();
  
  // 检查点击是否在游戏板内
  if (!boardRect.contains(pt)) {
    return false;
  }
  
  // 计算每个宝石的大小和偏移量
  int gemSize = boardWidth / COL;
  int offsetX = (boardWidth - gemSize * COL) / 2;
  int offsetY = (boardHeight - gemSize * ROW) / 2;
  
  // 计算行列坐标
  r = (pt.y() - boardY - offsetY) / gemSize;
  c = (pt.x() - boardX - offsetX) / gemSize;
  
  // 检查行列坐标是否在合法范围内
  if (r >= 0 && r < ROW && c >= 0 && c < COL) {
    return true;
  }
  
  return false;
}
