#ifndef GEM_H
#define GEM_H

#include "Const.h" // 引用常量定义

struct Gem {
  GemType type;   // 宝石颜色
  bool isMatched; // 标记是否处于待消除状态 (用于UI闪烁特效)

  // 构造函数
  Gem(GemType t = EMPTY) : type(t), isMatched(false) {}

  // 重载 == 操作符，方便比较两个宝石是否颜色相同
  bool operator==(const Gem &other) const { return this->type == other.type; }

  bool operator!=(const Gem &other) const { return this->type != other.type; }
};

#endif // GEM_H
