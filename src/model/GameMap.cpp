#include "GameMap.h"
#include <cstdlib> // 用于 rand() 和 srand()
#include <ctime>   // 用于 time()

// ==========================================
// 构造与初始化
// ==========================================

GameMap::GameMap() {
  // 初始化随机数生成器
  srand(static_cast<unsigned int>(time(nullptr)));
}

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

// ==========================================
// 公共接口
// ==========================================

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

std::vector<QPoint> GameMap::checkMatches() {
  std::vector<QPoint> matches;
  
  // 标记已经匹配的位置，避免重复计算
  bool visited[ROW][COL] = {false};

  // 1. 横向检测：连续3个及以上相同
  for (int r = 0; r < ROW; r++) {
    int count = 1;
    for (int c = 1; c < COL; c++) {
      if (m_map[r][c].type != EMPTY && m_map[r][c].type == m_map[r][c-1].type) {
        count++;
      } else {
        if (count >= 3) {
          // 标记连续匹配的宝石
          for (int i = c - count; i < c; i++) {
            if (!visited[r][i]) {
              visited[r][i] = true;
              matches.push_back(QPoint(i, r));
            }
          }
        }
        count = 1;
      }
    }
    // 检查行末是否有匹配
    if (count >= 3) {
      for (int i = COL - count; i < COL; i++) {
        if (!visited[r][i]) {
          visited[r][i] = true;
          matches.push_back(QPoint(i, r));
        }
      }
    }
  }

  // 2. 纵向检测：连续3个及以上相同
  for (int c = 0; c < COL; c++) {
    int count = 1;
    for (int r = 1; r < ROW; r++) {
      if (m_map[r][c].type != EMPTY && m_map[r][c].type == m_map[r-1][c].type) {
        count++;
      } else {
        if (count >= 3) {
          // 标记连续匹配的宝石
          for (int i = r - count; i < r; i++) {
            if (!visited[i][c]) {
              visited[i][c] = true;
              matches.push_back(QPoint(c, i));
            }
          }
        }
        count = 1;
      }
    }
    // 检查列末是否有匹配
    if (count >= 3) {
      for (int i = ROW - count; i < ROW; i++) {
        if (!visited[i][c]) {
          visited[i][c] = true;
          matches.push_back(QPoint(c, i));
        }
      }
    }
  }

  return matches;
}

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

void GameMap::applyGravity() {
  // 1. 从下往上、从左往右遍历
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
    
    // 3. 顶部的空位随机生成新宝石
    for (int r = 0; r < emptyCount; r++) {
      int randomType = rand() % GEM_KIND + 1;
      m_map[r][c].type = static_cast<GemType>(randomType);
      m_map[r][c].isMatched = false;
    }
  }
}

bool GameMap::reset() {
  // 重置地图，重新生成宝石
  init();
  return true;
}

bool GameMap::hasPossibleMove() {
  // TODO: 死局检测算法（BFS 或 暴力模拟）
  // 模拟所有可能的交换，看是否能产生消除
  return true; // 暂时返回 true 保证游戏能运行
}

// ==========================================
// 拓展功能：撤销
// ==========================================


bool GameMap::isValid(int r, int c) const {
  // TODO: 判断坐标 (r, c) 是否在 [0, ROW) 和 [0, COL) 范围内
  return (r >= 0 && r < ROW && c >= 0 && c < COL);
}

GemType GameMap::getType(int r, int c) const {
  if (!isValid(r, c)) {
    // 如果越界，返回空值
    return EMPTY;
  }
  // 返回实际类型
  return m_map[r][c].type;
}

// ==========================================
// 内部辅助
// ==========================================

void GameMap::saveState() {
    Step step;
    // 保存地图快照
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            step.mapSnapshot[r][c] = m_map[r][c];
        }
    }
    // 保存分数快照
    step.scoreSnapshot = m_currentScore;
    m_historyStack.push(step);
}

// 实现撤销功能
bool GameMap::undo() {
    if (m_historyStack.empty()) {
        return false; // 没有历史记录可撤销
    }

    // 取出栈顶元素并恢复状态
    Step step = m_historyStack.top();
    m_historyStack.pop();

    // 恢复地图快照
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            m_map[r][c] = step.mapSnapshot[r][c];
        }
    }

    // 恢复分数（需要在 GameMap 中添加分数成员变量）
    // m_currentScore = step.scoreSnapshot;

    return true;
}
