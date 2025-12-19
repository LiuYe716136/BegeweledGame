#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "GameMap.h"
#include <QMouseEvent>
#include <QPainter>
#include <QSoundEffect>
#include <QString>
#include <QTimer>
#include <QWidget>

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
  void on_btn_reset_clicked(); // 重置
  void on_btn_hint_clicked();  // 提示
  void on_btn_undo_clicked();  // 撤销
  void updateTime();
  /**
   * @brief 游戏主循环 (核心)
   * 由定时器触发，处理 消除->下落->生成 的流程
   */
  void updateGameState();

private:
  Ui::GameWidget *ui; // UI 指针
  GameMap *m_game;    // 游戏逻辑模型指针
  QTimer *m_timer;    // 动画流程定时器

  // 游戏状态
  QPoint m_selectedPos; // 当前选中宝石的数组行列坐标 (-1,-1 表示未选)
  GameState m_state;    // 当前游戏阶段
  int m_score;          // 当前得分

  // 音效
  QSoundEffect *m_soundSwap;
  QSoundEffect *m_soundEliminate;
  QSoundEffect *m_soundClick;
  QSoundEffect *m_soundBg;

  // 辅助函数
  void initGame();      // 游戏初始化
  void loadResources(); // 加载资源
  QTimer* m_timeTimer;       // 计时定时器
  int m_remainingTime;       // 剩余时间（秒）
  const int TOTAL_TIME = 120; // 总时间2分钟（120秒）

  /**
   * @brief 坐标转换：屏幕像素坐标 -> 数组行列坐标
   * @param pt 屏幕像素坐标
   * @param r 数组行坐标
   * @param c 数组列坐标
   * @return true 表示转换成功
   */
  bool screenToRowCol(QPoint pt, int &r, int &c);
};

#endif // GAMEWIDGET_H
