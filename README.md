# 宝石迷阵游戏开发文档

## 1. 项目简介

### 1.1 项目背景
宝石迷阵是一款经典的消除类益智游戏，玩家通过交换相邻宝石形成横向或纵向连续3个及以上相同宝石进行消除，获取分数。本项目使用Qt框架开发，实现了一个功能完整、界面美观的宝石迷阵游戏。

### 1.2 项目目标
- 实现经典宝石迷阵游戏的核心玩法
- 提供美观友好的用户界面
- 支持多种游戏模式（无尽模式、挑战模式）
- 实现完整的游戏功能（计分、排行榜、音效、撤销等）
- 确保游戏运行流畅、响应迅速

### 1.3 技术栈
- **开发框架**：Qt 5+（C++11标准）
- **界面设计**：Qt Designer (.ui文件)
- **构建工具**：qmake
- **资源管理**：Qt资源系统(.qrc)
- **多媒体支持**：Qt Multimedia模块

## 2. 团队分工及贡献度

| 成员   | 分工                   | 贡献度 |
| ------ | ---------------------- | ------ |
| 刘烨   | 项目架构设计、交互逻辑实现 | 25%    |
| 王茂源 | 游戏界面设计、核心算法实现 | 25%    |
| 王泰   | 游戏界面设计、资源搜索    | 25%    |
| 王靖杭 | 菜单、排行榜功能、文档编写| 25%    |

## 3. 功能需求与性能需求分析

### 3.1 功能需求分析

#### 3.1.1 核心游戏功能
- **宝石交换**：玩家可以点击选择相邻的两个宝石进行交换
- **匹配检测**：自动检测横向或纵向连续3个及以上相同宝石
- **宝石消除**：消除匹配的宝石并播放消除动画
- **下落填充**：消除后上方宝石下落填充空缺，顶部生成新宝石
- **链式反应**：支持消除后的连锁反应，连续消除获得额外分数

#### 3.1.2 游戏模式
- **无尽模式**：无时间限制，玩家可以无限游戏直到无有效移动
- **挑战模式**：有时间限制和关卡目标，完成目标可进入下一关卡

#### 3.1.3 辅助功能
- **计分系统**：根据消除的宝石类型和数量计算分数
- **排行榜**：记录并显示玩家的最高分数
- **撤销功能**：支持撤销上一步操作
- **提示功能**：显示可消除的宝石组合
- **音效控制**：支持背景音乐和音效开关
- **游戏重置**：支持重新开始游戏

### 3.2 性能需求分析

#### 3.2.1 响应性能
- 玩家点击操作的响应时间不超过100ms
- 宝石消除和下落动画流畅，帧率不低于30fps

#### 3.2.2 内存占用
- 游戏运行时内存占用不超过50MB
- 资源加载和释放及时，避免内存泄漏

#### 3.2.3 稳定性
- 游戏在长时间运行过程中无崩溃现象
- 异常情况处理完善，避免程序异常终止

## 4. 系统设计

### 4.1 系统架构设计

本项目采用MVC（Model-View-Controller）架构模式，将游戏逻辑与界面显示分离，提高代码的可维护性和扩展性。

- **Model层**：负责游戏核心逻辑，包括地图管理、宝石交换、匹配检测等
- **View层**：负责游戏界面显示，包括宝石渲染、用户交互、动画效果等
- **Controller层**：通过信号槽机制连接Model和View，处理用户输入和状态更新

### 4.2 功能模块设计

#### 4.2.1 核心游戏逻辑模块

| 模块名称 | 主要职责 | 关键类/文件 |
|---------|---------|------------|
| 地图管理 | 初始化地图、管理宝石状态 | GameMap.h/GameMap.cpp |
| 宝石交换 | 处理宝石交换逻辑、检查有效性 | GameMap::swap() |
| 匹配检测 | 检测可消除的宝石组合 | GameMap::checkMatches() |
| 宝石消除 | 执行宝石消除、更新分数 | GameMap::eliminate() |
| 下落填充 | 处理宝石下落、生成新宝石 | GameMap::applyGravity() |
| 死局检测 | 检查是否还有有效移动 | GameMap::hasPossibleMove() |
| 撤销管理 | 保存和恢复游戏状态 | GameMap::saveState()/GameMap::undo() |

#### 4.2.2 界面显示模块

| 模块名称 | 主要职责 | 关键类/文件 |
|---------|---------|------------|
| 主菜单界面 | 游戏模式选择、背景音乐控制 | MenuWidget.h/MenuWidget.cpp |
| 游戏主界面 | 宝石矩阵渲染、用户交互 | GameWidget.h/GameWidget.cpp |
| 排行榜界面 | 显示和更新游戏排行榜 | RankingWidget.h/RankingWidget.cpp |
| 资源管理 | 加载和管理图片、音效资源 | resources.qrc |

#### 4.2.3 游戏模式模块

| 模块名称 | 主要职责 | 关键类/文件 |
|---------|---------|------------|
| 无尽模式 | 无时间限制的游戏模式 | GameWidget::ENDLESS |
| 挑战模式 | 有时间限制和关卡目标 | GameWidget::CHALLENGE |

### 4.3 数据结构设计

#### 4.3.1 核心数据结构

```cpp
// 宝石类型枚举
enum GemType {
    EMPTY = 0, // 空
    RED,       // 红色
    ORANGE,    // 橙色
    YELLOW,    // 黄色
    GREEN,     // 绿色
    WHITE,     // 白色
    BLUE,      // 蓝色
    PURPLE     // 紫色
};

// 宝石结构体
struct Gem {
    GemType type;      // 宝石类型
    bool isMatched;    // 是否匹配（用于消除动画）
};

// 游戏步骤结构体（用于撤销功能）
struct Step {
    Gem mapSnapshot[ROW][COL]; // 地图快照
    int scoreSnapshot;         // 分数快照
};

// 游戏地图类
class GameMap {
private:
    Gem m_map[ROW][COL];        // 8x8宝石矩阵
    int m_currentScore;         // 当前分数
    std::stack<Step> m_historyStack; // 历史记录栈
    // ...
};
```

### 4.4 代码结构设计

项目采用清晰的目录结构，将不同功能的代码分离，提高代码的可维护性。

```
BejeweledGame/
├── src/
│   ├── model/         # 游戏核心逻辑
│   │   ├── Const.h    # 常量定义
│   │   ├── Gem.h      # 宝石结构体定义
│   │   └── GameMap.h  # 游戏地图类定义
│   └── view/          # 界面显示
│       ├── GameWidget.h      # 游戏主界面
│       ├── MenuWidget.h      # 主菜单界面
│       └── RankingWidget.h   # 排行榜界面
├── assets/            # 资源文件
│   ├── images/        # 图片资源
│   └── sounds/        # 音效资源
├── BejeweledGame.pro  # 项目配置文件
├── main.cpp           # 程序入口
└── resources.qrc      # 资源配置文件
```

### 4.5 数据库设计

项目使用简单的文件存储方式管理排行榜数据，分为无尽模式和挑战模式两个排行榜文件。

- **无尽模式排行榜**：存储玩家的最高分数
- **挑战模式排行榜**：存储玩家的关卡和对应分数

### 4.6 核心算法设计

#### 4.6.1 匹配检测算法

**算法描述**：遍历游戏地图，分别检测横向和纵向是否存在连续3个及以上相同宝石。

**实现思路**：
1. 使用二维visited数组标记已匹配的宝石，避免重复计算
2. 横向检测：从左到右遍历每一行，统计连续相同宝石的数量
3. 纵向检测：从上到下遍历每一列，统计连续相同宝石的数量
4. 当连续数量达到3个及以上时，将对应的宝石坐标加入匹配列表

**关键代码**：
```cpp
std::vector<QPoint> GameMap::checkMatches() {
    std::vector<QPoint> matches;
    bool visited[ROW][COL] = {false};

    // 横向检测
    for (int r = 0; r < ROW; r++) {
        int count = 1;
        for (int c = 1; c < COL; c++) {
            if (m_map[r][c].type != EMPTY && m_map[r][c].type == m_map[r][c-1].type) {
                count++;
            } else {
                if (count >= 3) {
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
        // 处理行末的匹配
    }
    
    // 纵向检测（类似横向检测逻辑）
    // ...
    
    return matches;
}
```

#### 4.6.2 下落填充算法

**算法描述**：处理宝石消除后的下落填充逻辑，让上方宝石下落填补空缺，顶部生成新宝石。

**实现思路**：
1. 从下往上、从左往右遍历每一列
2. 统计每列中的空位数
3. 将非空宝石下移相应的空位数量
4. 在顶部空位随机生成新宝石

**关键代码**：
```cpp
void GameMap::applyGravity() {
    for (int c = 0; c < COL; c++) {
        int emptyCount = 0;
        
        // 从底部开始往上遍历
        for (int r = ROW - 1; r >= 0; r--) {
            if (m_map[r][c].type == EMPTY) {
                emptyCount++;
            } else if (emptyCount > 0) {
                // 非空位且下方有空位，下移宝石
                m_map[r + emptyCount][c] = m_map[r][c];
                m_map[r][c].type = EMPTY;
                m_map[r][c].isMatched = false;
            }
        }
        
        // 顶部空位生成新宝石
        for (int r = 0; r < emptyCount; r++) {
            int randomType = rand() % GEM_KIND + 1;
            m_map[r][c].type = static_cast<GemType>(randomType);
            m_map[r][c].isMatched = false;
        }
    }
}
```

#### 4.6.3 死局检测算法

**算法描述**：检查游戏地图是否还有可进行的有效移动（交换相邻宝石后能产生匹配）。

**实现思路**：
1. 遍历所有宝石位置
2. 尝试与右侧和下方宝石交换
3. 检查交换后是否产生可消除组合
4. 如果存在有效交换，返回true；否则返回false

**关键代码**：
```cpp
bool GameMap::hasPossibleMove() {
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            // 尝试与右侧宝石交换
            if (c + 1 < COL) {
                // 保存原始状态
                Gem temp = m_map[r][c];
                m_map[r][c] = m_map[r][c+1];
                m_map[r][c+1] = temp;
                
                // 检查交换后是否有匹配
                if (!checkMatches().empty()) {
                    // 恢复原始状态
                    m_map[r][c+1] = m_map[r][c];
                    m_map[r][c] = temp;
                    return true;
                }
                
                // 恢复原始状态
                m_map[r][c+1] = m_map[r][c];
                m_map[r][c] = temp;
            }
            
            // 尝试与下方宝石交换（类似右侧交换逻辑）
            // ...
        }
    }
    return false;
}
```

## 5. UI设计

### 5.1 界面布局

#### 5.1.1 主菜单界面
- 游戏标题显示
- 游戏模式选择按钮（无尽模式、挑战模式）
- 排行榜按钮
- 背景音乐控制按钮
- 背景图片

#### 5.1.2 游戏主界面
- 8x8宝石矩阵显示
- 分数显示
- 游戏控制按钮（重置、提示、撤销）
- 游戏状态显示（当前模式、剩余时间等）
- 背景音乐控制按钮
- 返回菜单按钮

#### 5.1.3 排行榜界面
- 无尽模式排行榜
- 挑战模式排行榜
- 返回菜单按钮

### 5.2 资源设计

#### 5.2.1 图片资源
- 7种颜色的宝石图片（red.png, orange.png, yellow.png, green.png, white.png, blue.png, purple.png）
- 背景图片（game_bg.jpg, login_bg.jpg）

#### 5.2.2 音效资源
- 背景音乐（Peter Hajba - Bejeweled 2 Theme.mp3）
- 交换音效
- 消除音效
- 点击音效

## 6. 实现效果

### 6.1 核心功能实现

#### 6.1.1 游戏玩法
- ✅ 宝石交换功能
- ✅ 匹配检测功能
- ✅ 宝石消除动画
- ✅ 下落填充效果
- ✅ 链式反应支持

#### 6.1.2 游戏模式
- ✅ 无尽模式
- ✅ 挑战模式

#### 6.1.3 辅助功能
- ✅ 计分系统
- ✅ 排行榜功能
- ✅ 撤销功能
- ✅ 提示功能
- ✅ 音效控制
- ✅ 游戏重置

### 6.2 界面效果

- **主菜单界面**：简洁美观，提供清晰的游戏模式选择
- **游戏主界面**：宝石矩阵布局合理，操作流畅，状态信息清晰可见
- **排行榜界面**：排名信息清晰，支持不同模式的排行榜查看

### 6.3 性能表现

- 游戏运行流畅，动画效果自然
- 响应迅速，用户操作无明显延迟
- 内存占用合理，长时间运行稳定

## 7. 项目总结与心得

### 7.1 项目总结

本项目成功实现了一个功能完整的宝石迷阵游戏，包括：
- 完整的核心游戏逻辑（交换、匹配、消除、下落等）
- 多种游戏模式（无尽模式、挑战模式）
- 丰富的辅助功能（计分、排行榜、撤销、提示等）
- 美观友好的用户界面
- 流畅的游戏体验

### 7.2 遇到的问题及解决方案

#### 7.2.1 游戏死局问题
**问题**：游戏过程中可能出现没有有效移动的情况
**解决方案**：实现死局检测算法，在无有效移动时自动重置地图

#### 7.2.2 动画流畅性问题
**问题**：宝石消除和下落动画不够流畅
**解决方案**：使用Qt的定时器控制动画帧率，优化渲染逻辑

### 7.3 项目收获

- 深入理解了Qt框架的使用，包括界面设计、信号槽机制、资源管理等
- 掌握了游戏开发的基本流程和核心算法
- 提高了面向对象编程和代码组织能力
- 学会了如何解决实际开发中遇到的问题

### 7.4 改进方向

- 增加更多游戏模式和关卡设计
- 优化动画效果，增加更多特效
- 提供更丰富的游戏设置选项
- 实现网络排行榜功能
- 支持游戏存档和读取功能

## 8. 参考文献

1. Qt官方文档：https://doc.qt.io/
2. C++ Primer Plus（第六版）
3. 宝石迷阵游戏规则与玩法
4. Qt Designer使用指南

## 9. 附录

### 9.1 项目配置文件

**BejeweledGame.pro**
```pro
QT       += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# 包含路径
INCLUDEPATH += $$PWD/src/model \
               $$PWD/src/view

SOURCES += \
    main.cpp \
    src/model/GameMap.cpp \
    src/view/GameWidget.cpp \
    src/view/MenuWidget.cpp \
    src/view/RankingWidget.cpp

HEADERS += \
    src/model/Const.h \
    src/model/Gem.h \
    src/model/GameMap.h \
    src/view/GameWidget.h \
    src/view/MenuWidget.h \
    src/view/RankingWidget.h

FORMS += \
    src/view/GameWidget.ui \
    src/view/MenuWidget.ui \
    src/view/RankingWidget.ui

RESOURCES += \
    resources.qrc

DESTDIR = bin
```

### 9.2 核心常量定义

**src/model/Const.h**
```cpp
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

// 宝石分值 (每种颜色对应不同分值)
extern const int GEM_SCORES[];

// 游戏状态 (控制点击逻辑)
enum GameState {
  IDLE,      // 空闲 (允许玩家操作)
  ANIMATING, // 动画中 (禁止操作)
  GAME_OVER  // 游戏结束
};

#endif // CONST_H
```

### 9.3 编译和运行

1. 使用Qt Creator打开项目文件`BejeweledGame.pro`
2. 点击"构建"按钮编译项目
3. 点击"运行"按钮启动游戏

或者使用命令行编译：
```bash
qmake
make
bin/BejeweledGame.exe
```
