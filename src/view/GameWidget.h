#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "GameMap.h"
#include <QMouseEvent>
#include <QPainter>
#include <QSoundEffect>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QMediaPlayer>
#include <QPushButton>

// 前置声明
class RankingWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class GameWidget;
}
QT_END_NAMESPACE

/**
 * @brief 游戏主界面类
 * 负责游戏的核心交互和显示，包括宝石矩阵渲染、用户点击处理、游戏状态更新等
 */
class GameWidget : public QWidget {
  Q_OBJECT

public:
  /**
   * @brief 游戏模式枚举
   */
  enum GameMode {
      ENDLESS,   ///< 无尽模式，无时间限制
      CHALLENGE  ///< 挑战模式，有时间限制和关卡目标
  };
  
  /**
   * @brief 构造函数
   * @param parent 父窗口部件
   */
  explicit GameWidget(QWidget *parent = nullptr);
  
  /**
   * @brief 析构函数
   */
  ~GameWidget();
  
  /**
   * @brief 设置游戏模式
   * @param mode 游戏模式（无尽模式/挑战模式）
   */
  void setGameMode(GameMode mode);
  
  /**
   * @brief 设置挑战模式的关卡
   * @param level 关卡数
   */
  void setChallengeLevel(int level);
  
  /**
   * @brief 获取当前游戏模式
   * @return 当前游戏模式
   */
  GameMode gameMode() const;

  /**
   * @brief 设置背景音乐播放器
   * @param player 音乐播放器指针
   */
  void setBgMusicPlayer(QMediaPlayer *player);

  /**
   * @brief 更新排行榜部件
   * @param rankingWidget 排行榜部件指针
   */
  void updateRankingWidget(RankingWidget *rankingWidget);

signals:
  /**
   * @brief 游戏结束信号
   * @param score 最终得分
   * @param level 完成的关卡数（仅挑战模式有效）
   */
  void gameOver(int score, int level = 0);
  
  /**
   * @brief 返回菜单信号
   */
  void backToMenu();

protected:
  /**
   * @brief 绘图事件
   * 负责将游戏地图数据绘制为宝石图片
   */
  void paintEvent(QPaintEvent *event) override;

  /**
   * @brief 鼠标点击事件
   * 处理宝石的选中和交换逻辑
   */
  void mousePressEvent(QMouseEvent *event) override;

private slots:
  /**
   * @brief 重置按钮点击槽函数
   */
  void on_btn_reset_clicked();
  
  /**
   * @brief 提示按钮点击槽函数
   */
  void on_btn_hint_clicked();
  
  /**
   * @brief 撤销按钮点击槽函数
   */
  void on_btn_undo_clicked();
  
  /**
   * @brief 时间更新槽函数
   * 挑战模式下更新剩余时间
   */
  void updateTime();
  
  /**
   * @brief 游戏主循环 (核心)
   * 由定时器触发，处理消除->下落->生成的流程
   */
  void updateGameState();

private:
  Ui::GameWidget *ui;         ///< UI 指针
  GameMap *m_game;            ///< 游戏逻辑模型指针
  QTimer *m_timer;            ///< 动画流程定时器

  // 游戏状态
  QPoint m_selectedPos;       ///< 当前选中宝石的数组行列坐标 (-1,-1 表示未选)
  GameState m_state;          ///< 当前游戏阶段
  int m_score;                ///< 当前得分
  GameMode m_gameMode;        ///< 游戏模式
  int m_challengeLevel;       ///< 当前关卡
  int m_targetScore;          ///< 目标分数（闯关模式）

  // 音效
  QSoundEffect *m_soundSwap;      ///< 交换音效
  QSoundEffect *m_soundEliminate; ///< 消除音效
  QSoundEffect *m_soundClick;     ///< 点击音效
  QMediaPlayer *m_bgMusicPlayer;  ///< 背景音乐播放器
  bool m_musicEnabled;            ///< 音乐开关状态

  // 辅助函数
  void initGame();      ///< 游戏初始化
  void loadResources(); ///< 加载资源
  QTimer* m_timeTimer;       ///< 计时定时器
  int m_remainingTime;       ///< 剩余时间（秒）
  
  RankingWidget *m_rankingWidget; ///< 排行榜部件指针
  QPushButton *m_musicBtn;        ///< 音乐控制按钮
  QPushButton *m_endGameBtn;      ///< 结束游戏按钮

  /**
   * @brief 获取挑战模式下的关卡时间
   * @param level 关卡数
   * @return 关卡时间（秒）
   */
  int getChallengeTime(int level) const;
  
  /**
   * @brief 获取挑战模式下的关卡目标分数
   * @param level 关卡数
   * @return 目标分数
   */
  int getChallengeTargetScore(int level) const;

  // 提示功能相关
  void findBestMove();       ///< 查找最佳移动
  QPoint m_hintPos1;         ///< 提示位置1
  QPoint m_hintPos2;         ///< 提示位置2
  bool m_isHinting;          ///< 是否正在显示提示

  /**
   * @brief 坐标转换：屏幕像素坐标 -> 数组行列坐标
   * @param pt 屏幕像素坐标
   * @param r 数组行坐标（输出参数）
   * @param c 数组列坐标（输出参数）
   * @return true 表示转换成功
   */
  bool screenToRowCol(QPoint pt, int &r, int &c);
};

#endif // GAMEWIDGET_H
