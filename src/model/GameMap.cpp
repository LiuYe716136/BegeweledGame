#include "GameMap.h"

// ==========================================
// 构造与初始化
// ==========================================

GameMap::GameMap() {
  // 可以在这里做简单的成员变量初始化
}

void GameMap::init() {
  // TODO: 编写初始化逻辑
  // 1. 双重循环遍历 m_map
  // 2. 随机生成 GemType
  // 3. 检查是否有生成时就直接消除的情况（如果有则重新生成）
}

// ==========================================
// 公共接口
// ==========================================

void GameMap::swap(int r1, int c1, int r2, int c2) {
  // TODO: 交换 m_map[r1][c1] 和 m_map[r2][c2] 的数据
  // 注意：如果有选中状态等属性，也要一并处理
}

std::vector<QPoint> GameMap::checkMatches() {
  std::vector<QPoint> matches;

  // TODO: 编写消除检测算法
  // 1. 横向检测：连续3个及以上相同
  // 2. 纵向检测：连续3个及以上相同
  // 3. 将符合条件的坐标 push_back 到 matches 中
  // 4. 注意去重（如果横竖交叉的情况）

  return matches;
}

void GameMap::eliminate(const std::vector<QPoint> &points) {
  // TODO: 遍历 points
  // 将 m_map 对应位置的 GemType 设置为 EMPTY 或消除状态
}

void GameMap::applyGravity() {
  // TODO: 编写下落算法
  // 1. 从下往上、从左往右遍历
  // 2. 如果遇到空位，让上方的宝石掉下来
  // 3. 顶部的空位随机生成新宝石
}

bool GameMap::reset() {}

bool GameMap::hasPossibleMove() {
  // TODO: 死局检测算法（BFS 或 暴力模拟）
  // 模拟所有可能的交换，看是否能产生消除
  return true; // 暂时返回 true 保证游戏能运行
}

// ==========================================
// 拓展功能：撤销
// ==========================================

bool GameMap::undo() {
  if (m_historyStack.empty()) {
    return false;
  }

  // TODO: 取出栈顶元素
  // Step step = m_historyStack.top();
  // m_historyStack.pop();
  // 将 step 中的数据恢复到当前 m_map

  return true;
}

bool GameMap::isValid(int r, int c) const {
  // TODO: 判断坐标 (r, c) 是否在 [0, ROW) 和 [0, COL) 范围内
  return (r >= 0 && r < ROW && c >= 0 && c < COL);
}

GemType GameMap::getType(int r, int c) const {
  if (!isValid(r, c)) {
    // 如果越界，返回一个默认值或空值
    return (GemType)0;
  }
  // 返回实际类型 (假设 Gem 类中有 type 成员或类似的获取方式)
  // return m_map[r][c].type;
  return (GemType)0; // 占位返回
}

// ==========================================
// 内部辅助
// ==========================================

void GameMap::saveState() {
  // TODO: 创建 Step 对象
  // 将当前 m_map 的内容和分数复制到 Step 中
  // m_historyStack.push(step);
}
