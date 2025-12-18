#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "GameMap.h" // 包含核心逻辑
#include <QMouseEvent>
#include <QPainter>
#include <QSoundEffect>
#include <QString>
#include <QTimer>
#include <QWidget>

// Qt 命名空间处理
QT_BEGIN_NAMESPACE
namespace Ui {
class GameWidget;
}
QT_END_NAMESPACE

class GameWidget : public QWidget {
  Q_OBJECT

public:
  explicit GameWidget(QWidget *parent = nullptr);
  ~GameWidget();

protected:
  // ==========================================
  // Qt 事件重写 (Protected)
  // ==========================================

  /**
   * @brief 绘图事件
   * 负责将 m_game->m_map 里的数据画成图片
   */
  void paintEvent(QPaintEvent *event) override;

  /**
   * @brief 鼠标点击事件
   * 处理选中、交换逻辑
   */
  void mousePressEvent(QMouseEvent *event) override;

private slots:
  // ==========================================
  // 界面交互槽函数 (Private Slots)
  // ==========================================

  // 对应 widget.ui 里的按钮
  void on_btn_reset_clicked(); // 重置
  void on_btn_hint_clicked();  // 提示 (拓展)
  void on_btn_undo_clicked();  // 撤销 (拓展)

  /**
   * @brief 游戏主循环 (核心)
   * 由定时器触发，处理 消除->下落->生成 的流程
   */
  void updateGameState();

private:
  // ==========================================
  // 内部成员变量
  // ==========================================
  Ui::GameWidget *ui;  // UI 指针
  GameMap *m_game; // 游戏逻辑模型指针
  QTimer *m_timer; // 动画流程定时器

  // 游戏状态
  QPoint m_selectedPos; // 当前选中的格子 (-1,-1 表示未选)
  GameState m_state;    // 当前游戏阶段
  int m_score;          // 当前得分

  // 音效 (Qt6/Qt5 兼容推荐 QSoundEffect)
  QSoundEffect *m_soundSwap;
  QSoundEffect *m_soundEliminate;
  QSoundEffect *m_soundClick;

  // ==========================================
  // 内部辅助函数
  // ==========================================

  void initGame();      // 游戏初始化
  void loadResources(); // 加载资源

  // 坐标转换：屏幕像素 -> 数组下标
  bool screenToRowCol(QPoint pt, int &r, int &c);
};

#endif // GAMEWIDGET_H
