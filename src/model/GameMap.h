#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "Gem.h"
#include <QPoint>
#include <stack>
#include <vector>

class GameMap {
public:
  // 初始化
  GameMap();
    // 获取宝石分值
    int getGemScore(int r, int c) const {
        if (!isValid(r, c)) return 0;
        return GEM_SCORES[static_cast<int>(m_map[r][c].type)];
    }
    void clearHistory() {
        while (!m_historyStack.empty()) {
            m_historyStack.pop();
        }
        m_lastUndoScore = 0;
    }

  /**
   * @brief 初始化地图
   * 随机填充宝石，并保证初始状态下没有可直接消除的组合
   */
  void init();

  int getCurrentScore() const { return m_currentScore; }

  void setCurrentScore(int score) { m_currentScore = score; }

  int getLastStepScore() const {
    if (!m_historyStack.empty()) {
      return m_historyStack.top().scoreSnapshot;
    }
    return -1; // 表示无历史记录
  }

  // 核心数据
  Gem m_map[ROW][COL];

  // 公共接口

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

  // 获取当前选中是否有效
  bool isValid(int r, int c) const;
  /**
   * @brief 获取宝石类型
   * @return
   */
  GemType getType(int r, int c) const;
  void saveState(int currentScore); // 保存当前状态（含分数）
  int getLastUndoScore() const;     // 获取最近一次撤销的分数
  void popLastState();              // 移除无效状态
private:
  struct Step {
    Gem mapSnapshot[ROW][COL]; // 地图快照
    int scoreSnapshot;         // 分数快照
  };

  int m_currentScore;
  int m_lastUndoScore;

  std::stack<Step> m_historyStack; // 历史记录栈
                                   // 保存当前状态到栈
};

#endif // GAMEMAP_H
