#include "GameMap.h"
#include <cstdlib> // 用于 rand() 和 srand()
#include <ctime>   // 用于 time()

/**
 * @brief GameMap构造函数实现
 * 初始化随机数种子和撤销分数
 */
GameMap::GameMap() : m_currentScore(0), m_lastUndoScore(0) {
  srand(static_cast<unsigned int>(time(nullptr)));
}

/**
 * @brief 获取指定位置宝石的分值
 * @param r 行坐标
 * @param c 列坐标
 * @return 宝石的分值，如果坐标无效返回0
 */
int GameMap::getGemScore(int r, int c) const {
  if (!isValid(r, c))
    return 0;
  return GEM_SCORES[static_cast<int>(m_map[r][c].type)];
}

/**
 * @brief 获取宝石类型实现
 * 返回指定位置的宝石类型，如果坐标无效则返回EMPTY
 * @param r 行坐标
 * @param c 列坐标
 * @return 宝石类型
 */
GemType GameMap::getGemType(int r, int c) const {
  if (!isValid(r, c)) {
    // 如果越界，返回空值
    return EMPTY;
  }
  // 返回实际类型
  return m_map[r][c].type;
}

/**
 * @brief 初始化地图实现
 * 随机填充宝石，并保证初始状态下没有可直接消除的组合
 */
void GameMap::init() {
  // 初始化地图，随机生成宝石
  for (int r = 0; r < ROW; r++) {
    for (int c = 0; c < COL; c++) {
      // 随机生成宝石类型（1-7，对应7种颜色）
      int randomType = rand() % GEM_KIND + 1;
      m_map[r][c].type = static_cast<GemType>(randomType);
      m_map[r][c].isMatched = false;
    }
  }

  // 检查初始生成是否有可消除的组合，如果有则重新生成
  while (!checkMatches().empty()) {
    for (int r = 0; r < ROW; r++) {
      for (int c = 0; c < COL; c++) {
        int randomType = rand() % GEM_KIND + 1;
        m_map[r][c].type = static_cast<GemType>(randomType);
        m_map[r][c].isMatched = false;
      }
    }
  }
}

/**
 * @brief 交换两个宝石实现
 * 在数据层面交换两个指定位置的宝石
 * @param r1 第一个宝石的行坐标
 * @param c1 第一个宝石的列坐标
 * @param r2 第二个宝石的行坐标
 * @param c2 第二个宝石的列坐标
 */
void GameMap::swap(int r1, int c1, int r2, int c2) {
  // 检查坐标是否有效
  if (!isValid(r1, c1) || !isValid(r2, c2)) {
    return;
  }

  // 交换两个宝石的位置
  Gem temp = m_map[r1][c1];
  m_map[r1][c1] = m_map[r2][c2];
  m_map[r2][c2] = temp;
}

/**
 * @brief 检查全图匹配实现，滑动窗口算法
 * 检测游戏地图中所有可以消除的宝石组合（横向或纵向连续3个及以上相同宝石）
 * @return 返回所有需要消除的宝石坐标集合
 */
std::vector<QPoint> GameMap::checkMatches() {
  std::vector<QPoint> matches;

  // 标记已经匹配的位置，避免重复计算
  bool visited[ROW][COL] = {false};

  // 1. 横向检测：连续3个及以上相同
  for (int r = 0; r < ROW; r++) {
    int start = 0;
    while (start < COL) {
      int end = start + 1; // 待检测
      // 找到连续相同的宝石
      while (end < COL && m_map[r][end].type != EMPTY &&
             m_map[r][end].type == m_map[r][start].type) {
        end++;
      }
      // 如果连续数量>=3，则添加到匹配列表
      if (end - start >= 3) {
        for (int c = start; c < end; c++) {
          if (!visited[r][c]) {
            visited[r][c] = true;
            matches.push_back(QPoint(c, r));
          }
        }
      }
      start = end;
    }
  }

  // 2. 纵向检测：连续3个及以上相同
  for (int c = 0; c < COL; c++) {
    int start = 0;
    while (start < ROW) {
      int end = start + 1;
      // 找到连续相同的宝石
      while (end < ROW && m_map[end][c].type != EMPTY &&
             m_map[end][c].type == m_map[start][c].type) {
        end++;
      }
      // 如果连续数量>=3，则添加到匹配列表
      if (end - start >= 3) {
        for (int r = start; r < end; r++) {
          if (!visited[r][c]) {
            visited[r][c] = true;
            matches.push_back(QPoint(c, r));
          }
        }
      }
      start = end;
    }
  }

  return matches;
}

/**
 * @brief 执行消除实现
 * 将指定坐标的宝石消除（设为EMPTY），并标记为已匹配
 * @param points 需要消除的宝石坐标集合
 */
void GameMap::eliminate(const std::vector<QPoint> &points) {
  // 遍历所有匹配的宝石位置
  for (const auto &point : points) {
    int r = point.y();
    int c = point.x();

    // 检查位置是否有效
    if (isValid(r, c)) {
      // 将宝石类型设置为空
      m_map[r][c].type = EMPTY;
      m_map[r][c].isMatched = true; // 标记为已匹配
    }
  }
}

/**
 * @brief 下落填充算法实现
 * 处理消除宝石后的下落填充逻辑：让上方的宝石下落填补空缺，顶部生成随机新宝石
 */
void GameMap::applyGravity() {
  // 从下往上、从左往右遍历，拆分八个列
  for (int c = 0; c < COL; c++) {
    int emptyCount = 0;

    // 从底部开始往上遍历
    for (int r = ROW - 1; r >= 0; r--) {
      if (m_map[r][c].type == EMPTY) {
        // 遇到空位，计数加1
        emptyCount++;
      } else if (emptyCount > 0) {
        // 遇到非空位且下方有空位，将宝石移动到下方
        m_map[r + emptyCount][c] = m_map[r][c];
        m_map[r][c].type = EMPTY;
        m_map[r][c].isMatched = false;
      }
    }

    // 顶部的空位随机生成新宝石
    for (int r = 0; r < emptyCount; r++) {
      int randomType = rand() % GEM_KIND + 1;
      m_map[r][c].type = static_cast<GemType>(randomType);
      m_map[r][c].isMatched = false;
    }
  }
}

/**
 * @brief 重置游戏实现
 * 重新初始化地图，生成新的宝石布局
 * @return true表示重置成功
 */
bool GameMap::reset() {
  init();
  return true;
}

/**
 * @brief 死局检测实现
 * 检查游戏中是否还有可能的有效移动（交换相邻宝石后能产生匹配）
 * @return true表示还有可移动的宝石，false表示死局
 */
bool GameMap::hasPossibleMove() {
  // 遍历所有宝石，尝试交换每个宝石与其右侧、下方的相邻宝石
  for (int r = 0; r < ROW; r++) {
    for (int c = 0; c < COL; c++) {
      // 尝试与右侧宝石交换
      if (c + 1 < COL) {
        // 保存原始状态
        Gem temp = m_map[r][c];
        m_map[r][c] = m_map[r][c + 1];
        m_map[r][c + 1] = temp;

        // 检查交换后是否有可消除组合
        if (!checkMatches().empty()) {
          // 恢复原始状态
          m_map[r][c + 1] = m_map[r][c];
          m_map[r][c] = temp;
          return true; // 存在有效交换
        }

        // 恢复原始状态
        m_map[r][c + 1] = m_map[r][c];
        m_map[r][c] = temp;
      }

      // 尝试与下方宝石交换
      if (r + 1 < ROW) {
        // 保存原始状态
        Gem temp = m_map[r][c];
        m_map[r][c] = m_map[r + 1][c];
        m_map[r + 1][c] = temp;

        // 检查交换后是否有可消除组合
        if (!checkMatches().empty()) {
          // 恢复原始状态
          m_map[r + 1][c] = m_map[r][c];
          m_map[r][c] = temp;
          return true; // 存在有效交换
        }

        // 恢复原始状态
        m_map[r + 1][c] = m_map[r][c];
        m_map[r][c] = temp;
      }
    }
  }

  // 所有交换都无法产生可消除组合，判定为死局
  return false;
}

/**
 * @brief 撤销
 * @return true 表示撤销成功
 */
bool GameMap::undo() {
  if (m_historyStack.empty()) {
    return false;
  }
  Step step = m_historyStack.top();
  m_historyStack.pop();

  // 恢复地图
  for (int r = 0; r < ROW; r++) {
    for (int c = 0; c < COL; c++) {
      m_map[r][c] = step.mapSnapshot[r][c];
    }
  }

  // 记录恢复的分数（供外部获取）
  m_lastUndoScore = step.scoreSnapshot;
  return true;
}

/**
 * @brief 检查坐标有效性实现
 * 判断指定的坐标是否在游戏地图的有效范围内
 * @param r 行坐标
 * @param c 列坐标
 * @return true表示坐标有效，false表示坐标无效
 */
bool GameMap::isValid(int r, int c) const {
  return (r >= 0 && r < ROW && c >= 0 && c < COL);
}

/**
 * @brief 保存当前状态
 * 将当前地图和分数保存到历史记录栈中，用于撤销操作
 * @param currentScore 当前游戏分数
 */
void GameMap::saveCurState(int currentScore) {
  Step step;
  // 保存地图快照
  for (int r = 0; r < ROW; r++) {
    for (int c = 0; c < COL; c++) {
      step.mapSnapshot[r][c] = m_map[r][c];
    }
  }
  // 保存当前分数（交换前的分数）
  step.scoreSnapshot = currentScore;
  m_historyStack.push(step);
}

/**
 * @brief 移除最后一个状态
 * 从历史记录栈中移除无效的状态
 */
void GameMap::popLastState() {
  if (!m_historyStack.empty()) {
    m_historyStack.pop();
  }
}

// 获取最近撤销的分数
int GameMap::getLastUndoScore() const { return m_lastUndoScore; }

/**
 * @brief 获取上一步的分数
 * @return 上一步的分数，如果没有历史记录返回-1
 */
int GameMap::getLastStepScore() const {
  if (!m_historyStack.empty()) {
    return m_historyStack.top().scoreSnapshot;
  }
  return -1; // 表示无历史记录
}

/**
 * @brief 清除历史记录
 * 清空撤销历史栈并重置撤销分数
 */
void GameMap::clearHistory() {
  while (!m_historyStack.empty()) {
    m_historyStack.pop();
  }
  m_lastUndoScore = 0;
}
