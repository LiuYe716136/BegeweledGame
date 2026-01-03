#include "GameWidget.h"
#include "ui_GameWidget.h"
#include "RankingWidget.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStyleOption>
#include <QMessageBox>

/**
 * @brief GameWidget构造函数
 * 初始化所有成员变量、UI和游戏资源
 * @param parent 父窗口部件
 */
GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::GameWidget),
    m_game(new GameMap()),
    m_timer(new QTimer(this)),
    m_timeTimer(new QTimer(this)),  // 初始化计时定时器
    // 音效变量 - 暂时未实现
    // m_soundSwap(nullptr),
    // m_soundEliminate(nullptr),
    // m_soundClick(nullptr),
    m_selectedPos(-1, -1),
    m_state(IDLE),
    m_score(0),
    m_gameMode(ENDLESS),
    m_challengeLevel(1),
    m_targetScore(1000),
    m_bgMusicPlayer(nullptr),
    m_musicEnabled(true),
    m_rankingWidget(nullptr),
    m_musicBtn(nullptr),
    m_isHinting(false) {
    ui->setupUi(this);

    // 加载资源和其他初始化...
    loadResources();
    initGame();

    // 连接定时器信号
    connect(m_timer, &QTimer::timeout, this, &GameWidget::updateGameState);

    // 初始化计时相关
    ui->progressBar_time->setFormat("%v s"); // %v 表示当前值，后面拼接 " s"
    connect(m_timeTimer, &QTimer::timeout, this, &GameWidget::updateTime);
}
/**
 * @brief GameWidget析构函数
 * 释放所有资源
 */
GameWidget::~GameWidget() {
  delete m_game;
  delete m_timer;
  delete m_timeTimer;
  // 删除音效变量 - 暂时未实现
  // delete m_soundSwap;
  // delete m_soundEliminate;
  // delete m_soundClick;
  delete ui;
}
/**
 * @brief 更新时间槽函数
 * 挑战模式下每秒更新剩余时间，处理关卡完成和时间耗尽逻辑
 */
void GameWidget::updateTime() {
    if (m_gameMode != CHALLENGE) {
        return;
    }
    
    m_remainingTime--;
    ui->progressBar_time->setValue(m_remainingTime);

    if (m_score >= m_targetScore) {
        int completedLevel = m_challengeLevel;
        int nextLevel = completedLevel + 1;
        
        QMessageBox msgBox;
        msgBox.setWindowTitle("关卡完成");
        msgBox.setText(QString("恭喜！你完成了第%1关！\n进入第%2关！\n目标分数：%3").arg(completedLevel).arg(nextLevel).arg(getChallengeTargetScore(nextLevel)));
        msgBox.setStyleSheet("QLabel { color: black; } QPushButton { color: black; }");
        msgBox.exec();
        
        m_challengeLevel = nextLevel;
        m_targetScore = getChallengeTargetScore(nextLevel);
        m_remainingTime = getChallengeTime(nextLevel);
        ui->progressBar_time->setRange(0, m_remainingTime);
        ui->progressBar_time->setValue(m_remainingTime);
        return;
    }

    if (m_remainingTime <= 0) {
        m_timeTimer->stop();
        m_timer->stop();
        m_state = GAME_OVER;

        QMessageBox msgBox;
        msgBox.setWindowTitle("游戏结束");
        msgBox.setText(QString("时间已到！\n你完成了第%1关！\n最终得分是：%2").arg(m_challengeLevel).arg(m_score));
        msgBox.setStyleSheet("QLabel { color: black; } QPushButton { color: black; }");
        msgBox.exec();
        
        emit gameOver(m_score, m_challengeLevel);
        emit backToMenu();
    }
}

/**
 * @brief 绘图事件处理函数
 * 负责绘制游戏界面的所有元素：背景、宝石、选中框、提示和分数
 * @param event 绘图事件
 */
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

  // 如果有提示，绘制提示框（黄色虚线框）
  if (m_isHinting && m_hintPos1 != QPoint(-1, -1) && m_hintPos2 != QPoint(-1, -1)) {
      // 绘制第一个提示位置的框
      int r1 = m_hintPos1.y();
      int c1 = m_hintPos1.x();
      int x1 = boardX + offsetX + c1 * GEM_SIZE;
      int y1 = boardY + offsetY + r1 * GEM_SIZE;

      QPen hintPen(QColor(255, 255, 0), 4, Qt::DashLine);
      painter.setPen(hintPen);
      painter.drawRect(x1, y1, GEM_SIZE, GEM_SIZE);

      // 绘制第二个提示位置的框
      int r2 = m_hintPos2.y();
      int c2 = m_hintPos2.x();
      int x2 = boardX + offsetX + c2 * GEM_SIZE;
      int y2 = boardY + offsetY + r2 * GEM_SIZE;

      painter.setPen(hintPen);
      painter.drawRect(x2, y2, GEM_SIZE, GEM_SIZE);

      // 在两个位置之间画一条连接线
      painter.drawLine(x1 + GEM_SIZE / 2, y1 + GEM_SIZE / 2, x2 + GEM_SIZE / 2, y2 + GEM_SIZE / 2);
  }

  // 绘制分数
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

/**
 * @brief 鼠标点击事件处理函数
 * 处理宝石的选中和交换逻辑
 * @param event 鼠标事件
 */
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
    // 第一次点击，选中宝石
    m_selectedPos = QPoint(c, r);
  } else {
    // 第二次点击，检查是否相邻
    int selectedR = m_selectedPos.y();
    int selectedC = m_selectedPos.x();
    bool isAdjacent = false;
    if ((abs(r - selectedR) == 1 && c == selectedC) ||
        (abs(c - selectedC) == 1 && r == selectedR)) {
      isAdjacent = true;
    }

    if (isAdjacent) {
      // 交换前保存当前状态和分数
      m_game->saveState(m_score);
      m_game->swap(selectedR, selectedC, r, c);

      std::vector<QPoint> matches = m_game->checkMatches();

      if (matches.empty()) {
        // 无匹配时交换回来，并删除无效快照
        m_game->swap(r, c, selectedR, selectedC);
        m_game->popLastState(); // 关键：清除无效状态
      } else {
        // 有匹配时开始消除流程
        m_timer->start(500);
      }
    }
    // 清除选中状态
    m_selectedPos = QPoint(-1, -1);
  }

  repaint();
}

/**
 * @brief 重置按钮点击槽函数
 * 停止当前计时并重新初始化游戏
 */
void GameWidget::on_btn_reset_clicked() {
    m_timeTimer->stop();  // 先停止当前计时
    initGame();           // 重新初始化游戏（会重新开始计时）
    update();
}

/**
 * @brief 提示按钮点击槽函数
 * 查找最佳移动并显示提示，3秒后自动隐藏
 */
void GameWidget::on_btn_hint_clicked() {
    findBestMove();
    update();
    QTimer::singleShot(3000, [this]() {
        m_isHinting = false;
        m_hintPos1 = QPoint(-1, -1);
        m_hintPos2 = QPoint(-1, -1);
        update();
    });
}

/**
 * @brief 查找最佳移动
 * 遍历所有可能的移动，找到能获得最高分数的交换
 */
void GameWidget::findBestMove() {
    m_isHinting = false;
    m_hintPos1 = QPoint(-1, -1);
    m_hintPos2 = QPoint(-1, -1);
    
    int bestScore = 0;
    
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            const int dr[4] = {-1, 1, 0, 0}; // 上下左右四个方向
            const int dc[4] = {0, 0, -1, 1};
            
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d];
                int nc = c + dc[d];
                
                if (nr < 0 || nr >= ROW || nc < 0 || nc >= COL) continue;
                
                GemType type1 = m_game->getType(r, c);
                GemType type2 = m_game->getType(nr, nc);
                
                if (type1 == type2) continue; // 相同类型宝石交换无意义
                
                // 尝试交换
                m_game->swap(r, c, nr, nc);
                
                // 检查是否有匹配
                std::vector<QPoint> matches = m_game->checkMatches();
                
                // 计算得分
                int moveScore = 0;
                for (const auto& match : matches) {
                    int matchR = match.y();
                    int matchC = match.x();
                    moveScore += m_game->getGemScore(matchR, matchC);
                }
                
                // 交换回来
                m_game->swap(r, c, nr, nc);
                
                // 更新最佳移动
                if (moveScore > bestScore) {
                    bestScore = moveScore;
                    m_hintPos1 = QPoint(c, r);
                    m_hintPos2 = QPoint(nc, nr);
                    m_isHinting = true;
                }
            }
        }
    }
}

/**
 * @brief 撤销按钮点击槽函数
 * 撤销上一步操作，恢复到之前的游戏状态
 */
void GameWidget::on_btn_undo_clicked() {
  if (m_game->undo()) {
    // 从GameMap获取撤销前的分数并更新
    m_score = m_game->getLastUndoScore();
    ui->label_score->setText(QString::number(m_score));
    update();
  }
}

/**
 * @brief 结束游戏按钮点击槽函数
 * 处理结束游戏的逻辑，停止计时，显示最终得分，然后返回主菜单
 */
void GameWidget::on_btn_endGame_clicked() {
    // 停止计时器
    m_timeTimer->stop();
    m_timer->stop();
    
    // 弹出消息框显示最终得分
    QMessageBox msgBox;
    if (m_gameMode == CHALLENGE) {
        msgBox.setWindowTitle("游戏结束");
        msgBox.setText(QString("你结束了游戏！\n你完成了第%1关！\n最终得分是：%2").arg(m_challengeLevel).arg(m_score));
    } else {
        msgBox.setWindowTitle("游戏结束");
        msgBox.setText(QString("你结束了游戏！\n最终得分是：%1").arg(m_score));
    }
    msgBox.setStyleSheet("QLabel { color: black; } QPushButton { color: black; }");
    msgBox.exec();
    
    // 发出游戏结束信号和返回菜单信号
    emit gameOver(m_score, m_challengeLevel);
    emit backToMenu();
}

/**
 * @brief 游戏状态更新函数（核心）
 * 由定时器触发，处理消除->下落->生成的流程
 */
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

        // 继续下一个消除步骤
        m_timer->start(500);
    } else {
        // 无匹配时应用重力
        m_game->applyGravity();
        // 检查新的匹配
        matches = m_game->checkMatches();
        if (!matches.empty()) {
            // 有新匹配继续消除
            m_timer->start(500);
        } else {
            // 下落完成后仍无匹配，检查是否为死局
            if (!m_game->hasPossibleMove()) {
                // 1. 显示死局提示对话框
                QMessageBox msgBox;
                msgBox.setWindowTitle("游戏提示");
                msgBox.setText("当前已死局，即将重置地图！分数将保留。");
                msgBox.setStyleSheet("QLabel { color: black; } QPushButton { color: black; }");
                msgBox.exec();

                // 2. 重置地图但不重置分数
                m_game->reset();  // 仅重置地图宝石布局

                // 3. 分数保持不变，无需修改m_score和label_score
                qDebug() << "死局！已重置地图，分数保留";
            }
            // 停止定时器
            m_timer->stop();
        }
    }
    // 刷新界面
    update();
}
/**
 * @brief 初始化游戏
 * 重置游戏状态、地图、分数和UI
 */
void GameWidget::initGame() {
    // 初始化逻辑数据
    m_game->clearHistory();
    m_game->init();

    m_score = 0;
    m_selectedPos = QPoint(-1, -1);
    m_state = IDLE;  // 重置游戏状态

    // 设置闯关模式的剩余时间（每关递减）
    int totalTime = (m_gameMode == CHALLENGE) ? getChallengeTime(m_challengeLevel) : 0;
    m_remainingTime = totalTime;
    ui->progressBar_time->setRange(0, totalTime);
    ui->progressBar_time->setValue(totalTime);

    // UI 更新
    ui->label_score->setText("0");

    // 开始计时 - 仅在闯关模式下
    if (m_gameMode == CHALLENGE) {
        m_timeTimer->start(1000);  // 每秒触发一次
        ui->progressBar_time->setVisible(true);
    } else {
        m_timeTimer->stop();  // 无尽模式禁用计时
        ui->progressBar_time->setVisible(false);
    }

    update();
}

/**
 * @brief 加载游戏资源
 * 初始化音效、背景图片和界面按钮
 */
void GameWidget::loadResources() {
  // 初始化音效变量 - 暂时未实现
  // m_soundSwap = new QSoundEffect(this);
  // m_soundEliminate = new QSoundEffect(this);
  // m_soundClick = new QSoundEffect(this);
  
  // 1. 尝试加载背景图片
  QPixmap backgroundPixmap;
  bool loadSuccess = backgroundPixmap.load(":/bgs/assets/images/game_bg.jpg");

  // 2. 设置背景图片
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
  }
  
  // 创建音乐控制按钮
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
  
  connect(m_musicBtn, &QPushButton::clicked, [this]() {
      m_musicEnabled = !m_musicEnabled;
      if (m_bgMusicPlayer) {
          if (m_musicEnabled) {
              m_bgMusicPlayer->play();
              m_musicBtn->setText("♪");
          } else {
              m_bgMusicPlayer->pause();
              m_musicBtn->setText("✕");
          }
      }
  });
  
  // 删除了返回菜单按钮
  
  // 创建结束游戏按钮
  QPushButton *btn_endGame = new QPushButton(this);
  btn_endGame->setGeometry(660, 530, 120, 50);
  btn_endGame->setText("结束游戏");
  btn_endGame->setStyleSheet(
      "QPushButton { "
      "background-color: rgba(255, 100, 100, 150); "
      "border: 2px solid red; "
      "border-radius: 10px; "
      "font-size: 14px; "
      "font-weight: bold; "
      "color: white; "
      "}"
      "QPushButton:hover { "
      "background-color: rgba(255, 100, 100, 200); "
      "}"
      "QPushButton:pressed { "
      "background-color: rgba(255, 100, 100, 100); "
      "}"
  );
  
  connect(btn_endGame, &QPushButton::clicked, [this]() {
      m_timeTimer->stop();
      m_timer->stop();
      m_state = GAME_OVER;
      
      QMessageBox msgBox;
      msgBox.setWindowTitle("游戏结束");
      msgBox.setText(QString("你的得分是：%1").arg(m_score));
      msgBox.setStyleSheet("QLabel { color: black; } QPushButton { color: black; }");
      msgBox.exec();
      
      emit gameOver(m_score, 1);
      
      emit backToMenu();
  });
  
  // 默认隐藏结束游戏按钮，在无尽模式中显示
  btn_endGame->hide();
}

/**
 * @brief 坐标转换：屏幕像素坐标 -> 数组行列坐标
 * @param pt 屏幕像素坐标
 * @param r 数组行坐标（输出参数）
 * @param c 数组列坐标（输出参数）
 * @return true 表示转换成功
 */
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

/**
 * @brief 设置游戏模式
 * @param mode 游戏模式（无尽模式/挑战模式）
 */
void GameWidget::setGameMode(GameMode mode) {
    m_gameMode = mode;
    
    m_challengeLevel = 1;
    m_targetScore = getChallengeTargetScore(1);
    
    initGame();
}

/**
 * @brief 设置挑战模式的关卡
 * @param level 关卡数
 */
void GameWidget::setChallengeLevel(int level) {
    m_challengeLevel = level;
    m_targetScore = getChallengeTargetScore(level);
}

/**
 * @brief 获取挑战模式下的关卡时间
 * @param level 关卡数
 * @return 关卡时间（秒），最低30秒
 */
int GameWidget::getChallengeTime(int level) const {
    int baseTime = 120;
    int reduction = (level - 1) * 5;  // 每关减少5秒，允许更多关卡
    int minTime = 30;
    return qMax(baseTime - reduction, minTime);
}

/**
 * @brief 获取挑战模式下的关卡目标分数
 * @param level 关卡数
 * @return 目标分数，每关递增且有随机波动
 */
int GameWidget::getChallengeTargetScore(int level) const {
    if (level == 1) {
        return 1000;
    }
    int baseScore = 1000;
    int rangeWidth = 500 * (level - 1);
    int randomIncrease = rand() % rangeWidth;
    return baseScore + (level - 1) * 500 + randomIncrease;
}

/**
 * @brief 获取当前游戏模式
 * @return 当前游戏模式
 */
GameWidget::GameMode GameWidget::gameMode() const {
    return m_gameMode;
}

/**
 * @brief 设置背景音乐播放器
 * @param player 音乐播放器指针
 */
void GameWidget::setBgMusicPlayer(QMediaPlayer *player) {
    m_bgMusicPlayer = player;
    m_musicEnabled = true;
}

/**
 * @brief 更新排行榜部件
 * @param rankingWidget 排行榜部件指针
 */
void GameWidget::updateRankingWidget(RankingWidget *rankingWidget) {
    m_rankingWidget = rankingWidget;
}
