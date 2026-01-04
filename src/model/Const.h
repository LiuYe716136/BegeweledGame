#ifndef CONST_H
#define CONST_H

// 游戏规则配置
const int ROW = 8;      // 地图行数
const int COL = 8;      // 地图列数
const int GEM_KIND = 7; // 宝石种类数 (7种颜色)

// 界面渲染配置
const int GEM_SIZE = 60; // 宝石尺寸 (像素)
const int SPACING = 0;   // 宝石间距

// 宝石类型
enum GemType {
  EMPTY = 0, // 空 (消除后)
  RED,
  ORANGE,
  YELLOW,
  GREEN,
  WHITE,
  BLUE,
  PURPLE,
};

// 宝石分值定义
const int GEM_SCORES[] = {
    0,  // EMPTY
    10, // RED
    20, // ORANGE
    15, // YELLOW
    25, // GREEN
    30, // WHITE
    35, // BLUE
    40  // PURPLE
};

// 游戏状态 (控制点击逻辑)
enum GameState {
  IDLE,      // 空闲 (允许玩家操作)
  ANIMATING, // 动画中 (禁止操作)
  GAME_OVER  // 游戏结束
};

// 游戏模式枚举
enum GameMode {
  ENDLESS,  ///< 无尽模式，无时间限制
  CHALLENGE ///< 挑战模式，有时间限制和关卡目标
};

#endif // CONST_H
