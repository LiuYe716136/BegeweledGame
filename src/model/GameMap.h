#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "Gem.h"
#include <QPoint>
#include <stack>
#include <vector>

/**
 * @brief 游戏地图类
 * 负责管理游戏的核心数据和逻辑：地图初始化、宝石交换、匹配检测、消除、下落填充等
 */
class GameMap {
public:
  /**
   * @brief 构造函数
   * 初始化随机数种子和游戏分数
   */
  GameMap();

  /**
   * @brief 获取指定位置宝石的分值
   * @param r 行坐标
   * @param c 列坐标
   * @return 宝石的分值，如果坐标无效返回0
   */
  int getGemScore(int r, int c) const;

  /**
   * @brief 获取指定位置的宝石类型
   * @param r 行坐标
   * @param c 列坐标
   * @return 宝石类型，如果坐标无效返回EMPTY
   */
  GemType getGemType(int r, int c) const;

  /**
   * @brief 初始化地图
   * 随机填充宝石，并保证初始状态下没有可直接消除的组合
   */
  void init();

  /**
   * @brief 交换两个宝石 (数据层面的交换)
   */
  void swap(int r1, int c1, int r2, int c2);

  /**
   * @brief 检查全图是否有可消除项
   * @return 返回所有需要消除的坐标点集合
   */
  std::vector<QPoint> checkMatches();

  /**
   * @brief 执行消除
   * @param points 要消除的坐标集合,将这些位置设为 EMPTY
   */
  void eliminate(const std::vector<QPoint> &points);

  /**
   * @brief 下落填充算法
   * 让上方宝石下落填补空缺，顶部生成随机新宝石
   */
  void applyGravity();

  /**
   * @brief 重置
   * @return true 表示重置成功
   */
  bool reset();

  /**
   * @brief 死局/提示检测
   * @return true 表示玩家还有步子可以走
   */
  bool hasPossibleMove();

  /**
   * @brief 撤销
   * @return true 表示撤销成功
   */
  bool undo();

  /**
   * @brief 检查坐标是否有效
   * @param r 行坐标
   * @param c 列坐标
   * @return true表示坐标在有效范围内
   */
  bool isValid(int r, int c) const;

  /**
   * @brief 保存当前状态
   * 将当前地图和分数保存到历史记录栈中，用于撤销操作
   * @param currentScore 当前游戏分数
   */
  void saveCurState(int currentScore);

  /**
   * @brief 移除最后一个状态
   * 从历史记录栈中移除无效的状态
   */
  void popLastState();

  /**
   * @brief 获取最近一次撤销的分数
   * @return 最近一次撤销的分数
   */
  int getLastUndoScore() const;

  /**
   * @brief 获取上一步的分数
   * @return 上一步的分数，如果没有历史记录返回-1
   */
  int getLastStepScore() const;

  /**
   * @brief 清除历史记录
   * 清空撤销历史栈并重置撤销分数
   */
  void clearHistory();

private:
  Gem m_map[ROW][COL]; ///< 游戏地图的二维数组

  /**
   * @brief 游戏步骤结构体
   * 用于保存游戏的历史状态，包括地图快照和分数快照
   */
  struct Step {
    Gem mapSnapshot[ROW][COL]; ///< 地图快照
    int scoreSnapshot;         ///< 分数快照
  };

  int m_currentScore;  ///< 当前游戏分数
  int m_lastUndoScore; ///< 最近一次撤销的分数

  std::stack<Step> m_historyStack; ///< 历史记录栈，保存游戏的历史状态
};

#endif // GAMEMAP_H
