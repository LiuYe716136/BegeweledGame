#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "Gem.h"
#include <QPoint>
#include <stack>
#include <vector>

class GameMap {
public:
  // ==========================================
  // 构造与初始化
  // ==========================================
  GameMap();

  /**
   * @brief 初始化地图
   * 随机填充宝石，并保证初始状态下没有可直接消除的组合
   */
  void init();

  // ==========================================
  // 核心数据 (实训为了方便可设为public，严谨点用getter)
  // ==========================================
  Gem m_map[ROW][COL];

  // ==========================================
  // 公共接口 (Widget调用这些)
  // ==========================================

  /**
   * @brief 交换两个宝石 (纯数据层面的交换)
   */
  void swap(int r1, int c1, int r2, int c2);

  /**
   * @brief 检查全图是否有可消除项
   * @return 返回所有需要消除的坐标点集合
   */
  std::vector<QPoint> checkMatches();

  /**
   * @brief 执行消除
   * @param points 要消除的坐标列表
   * 将这些位置设为 EMPTY
   */
  void eliminate(const std::vector<QPoint> &points);

  /**
   * @brief 下落填充算法
   * 让上方宝石下落填补空缺，顶部生成随机新宝石
   */
  void applyGravity();

  /**
   * @brief 死局/提示检测 (拓展算法)
   * @return true 表示玩家还有步子可以走
   */
  bool hasPossibleMove();

  // ==========================================
  // 拓展功能：撤销 (栈的应用)
  // ==========================================
  struct Step {
    Gem mapSnapshot[ROW][COL]; // 地图快照
    int scoreSnapshot;         // 分数快照
  };

  bool undo(); // 执行撤销

  // 获取当前状态是否有效 (给UI用的辅助)
  bool isValid(int r, int c) const;
  GemType getType(int r, int c) const;

private:
  // ==========================================
  // 内部辅助
  // ==========================================
  std::stack<Step> m_historyStack; // 历史记录栈
  void saveState();                // 保存当前状态到栈
};

#endif // GAMEMAP_H
