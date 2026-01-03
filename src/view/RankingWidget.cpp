#include "RankingWidget.h"
#include "ui_RankingWidget.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QPixmap>
#include <QStringList>
#include <QTextStream>

/**
 * @brief RankingWidget构造函数
 * 初始化UI、加载背景图片和排行榜数据
 * @param parent 父窗口部件
 */
RankingWidget::RankingWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::RankingWidget), m_musicEnabled(true) {
  ui->setupUi(this);
  this->setObjectName("RankingWidget");
  loadRanking();
  connect(ui->btn_music, &QPushButton::clicked, this,
          &RankingWidget::on_btn_music_clicked);
}

/**
 * @brief RankingWidget析构函数
 * 释放UI资源
 */
RankingWidget::~RankingWidget() { delete ui; }

/**
 * @brief 音乐按钮点击槽函数
 * 切换音乐开关状态并发出信号
 */
void RankingWidget::on_btn_music_clicked() {
  m_musicEnabled = !m_musicEnabled;
  emit musicToggle(m_musicEnabled);
}

/**
 * @brief 加载排行榜数据
 * 加载并显示无尽模式和挑战模式的排行榜数据
 */
void RankingWidget::loadRanking() {
  loadEndlessRanking();
  loadChallengeRanking();
  displayEndlessRanking();
  displayChallengeRanking();
}

/**
 * @brief 加载无尽模式排行榜
 * 从文件中读取无尽模式的排名数据并按得分降序排序
 */
void RankingWidget::loadEndlessRanking() {
  m_endlessRanking.clear();
  QFile file("endless_ranking.txt");

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    while (!in.atEnd()) {
      QString line = in.readLine();
      QStringList parts = line.split(",");
      if (parts.size() >= 2) {
        RankingItem item;
        item.name = parts[0];
        item.score = parts[1].toInt();
        item.level = 0;
        m_endlessRanking.push_back(item);
      }
    }
    file.close();
  }

  // Sort by score (descending)
  for (size_t i = 0; i < m_endlessRanking.size(); i++) {
    for (size_t j = i + 1; j < m_endlessRanking.size(); j++) {
      if (m_endlessRanking[i].score < m_endlessRanking[j].score) {
        std::swap(m_endlessRanking[i], m_endlessRanking[j]);
      }
    }
  }
}

/**
 * @brief 加载挑战模式排行榜
 * 从文件中读取挑战模式的排名数据并按关卡和得分降序排序
 */
void RankingWidget::loadChallengeRanking() {
  m_challengeRanking.clear();
  QFile file("challenge_ranking.txt");

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    while (!in.atEnd()) {
      QString line = in.readLine();
      QStringList parts = line.split(",");
      if (parts.size() >= 2) {
        RankingItem item;
        item.name = parts[0];
        item.score = parts[1].toInt();
        item.level = parts.size() > 2 ? parts[2].toInt() : 0;
        m_challengeRanking.push_back(item);
      }
    }
    file.close();
  }

  // Sort by level (descending), then by score
  for (size_t i = 0; i < m_challengeRanking.size(); i++) {
    for (size_t j = i + 1; j < m_challengeRanking.size(); j++) {
      if (m_challengeRanking[i].level < m_challengeRanking[j].level ||
          (m_challengeRanking[i].level == m_challengeRanking[j].level &&
           m_challengeRanking[i].score < m_challengeRanking[j].score)) {
        std::swap(m_challengeRanking[i], m_challengeRanking[j]);
      }
    }
  }
}

/**
 * @brief 保存无尽模式排行榜
 * 将无尽模式的排名数据保存到文件中
 */
void RankingWidget::saveEndlessRanking() {
  QFile file("endless_ranking.txt");

  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    for (const auto &item : m_endlessRanking) {
      out << item.name << "," << item.score << "\n";
    }
    file.close();
  }
}

/**
 * @brief 保存挑战模式排行榜
 * 将挑战模式的排名数据保存到文件中
 */
void RankingWidget::saveChallengeRanking() {
  QFile file("challenge_ranking.txt");

  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    for (const auto &item : m_challengeRanking) {
      out << item.name << "," << item.score << "," << item.level << "\n";
    }
    file.close();
  }
}

/**
 * @brief 显示无尽模式排行榜
 * 在界面上显示无尽模式的排名数据
 */
void RankingWidget::displayEndlessRanking() {
  ui->list_endless->clear();
  for (size_t i = 0; i < m_endlessRanking.size(); i++) {
    const auto &item = m_endlessRanking[i];
    QString text =
        QString("%1. %2 - %3分").arg(i + 1).arg(item.name).arg(item.score);
    ui->list_endless->addItem(text);
  }

  if (m_endlessRanking.empty()) {
    ui->list_endless->addItem("暂无记录");
  }
}

/**
 * @brief 显示挑战模式排行榜
 * 在界面上显示挑战模式的排名数据
 */
void RankingWidget::displayChallengeRanking() {
  ui->list_challenge->clear();
  for (size_t i = 0; i < m_challengeRanking.size(); i++) {
    const auto &item = m_challengeRanking[i];
    QString text = QString("%1. %2 - 第%3关，%4分")
                       .arg(i + 1)
                       .arg(item.name)
                       .arg(item.level)
                       .arg(item.score);
    ui->list_challenge->addItem(text);
  }

  if (m_challengeRanking.empty()) {
    ui->list_challenge->addItem("暂无记录");
  }
}

/**
 * @brief 更新排行榜
 * 将新的游戏记录添加到对应模式的排行榜中并保存
 * @param mode 游戏模式（"endless"/"challenge"）
 * @param score 得分
 * @param level 关卡数（仅挑战模式有效）
 */
void RankingWidget::updateRanking(const QString &mode, int score, int level) {
  RankingItem newItem;
  newItem.name = "玩家";
  newItem.score = score;
  newItem.level = level;

  if (mode == "endless") {
    m_endlessRanking.push_back(newItem);

    // Sort again
    for (size_t i = 0; i < m_endlessRanking.size(); i++) {
      for (size_t j = i + 1; j < m_endlessRanking.size(); j++) {
        if (m_endlessRanking[i].score < m_endlessRanking[j].score) {
          std::swap(m_endlessRanking[i], m_endlessRanking[j]);
        }
      }
    }

    // Keep only top 10
    if (m_endlessRanking.size() > 10) {
      m_endlessRanking.resize(10);
    }

    saveEndlessRanking();
    displayEndlessRanking();
  } else if (mode == "challenge") {
    m_challengeRanking.push_back(newItem);

    // Sort again
    for (size_t i = 0; i < m_challengeRanking.size(); i++) {
      for (size_t j = i + 1; j < m_challengeRanking.size(); j++) {
        if (m_challengeRanking[i].level < m_challengeRanking[j].level ||
            (m_challengeRanking[i].level == m_challengeRanking[j].level &&
             m_challengeRanking[i].score < m_challengeRanking[j].score)) {
          std::swap(m_challengeRanking[i], m_challengeRanking[j]);
        }
      }
    }

    // Keep only top 10
    if (m_challengeRanking.size() > 10) {
      m_challengeRanking.resize(10);
    }

    saveChallengeRanking();
    displayChallengeRanking();
  }
}

/**
 * @brief 获取无尽模式排行榜
 * @return 无尽模式排行榜数据
 */
std::vector<RankingItem> RankingWidget::getEndlessRanking() const {
  return m_endlessRanking;
}

/**
 * @brief 获取挑战模式排行榜
 * @return 挑战模式排行榜数据
 */
std::vector<RankingItem> RankingWidget::getChallengeRanking() const {
  return m_challengeRanking;
}

/**
 * @brief 返回菜单按钮点击槽函数
 * 发出返回菜单信号
 */
void RankingWidget::on_btn_back_clicked() { emit backToMenu(); }

/**
 * @brief 清空排行榜按钮点击槽函数
 * 清空无尽模式和挑战模式的排行榜数据并更新UI
 */
void RankingWidget::on_btn_clearRanking_clicked() {
  // 显示确认对话框
  QMessageBox msgBox;
  msgBox.setWindowTitle(QString::fromUtf8("清空排行榜"));
  msgBox.setText(
      QString::fromUtf8("确定要清空所有排行榜数据吗？此操作不可恢复。"));
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);

  int reply = msgBox.exec();

  if (reply == QMessageBox::Yes) {
    // 清空排行榜数据
    m_endlessRanking.clear();
    m_challengeRanking.clear();

    // 保存空的排行榜数据到文件
    saveEndlessRanking();
    saveChallengeRanking();

    // 更新UI显示
    displayEndlessRanking();
    displayChallengeRanking();

    // 显示操作成功提示
    QMessageBox msgBox; // 指定父对象
    msgBox.setWindowTitle("提示");
    msgBox.setText("操作成功！");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QLabel { color: black; } QPushButton { color: black; }");

    msgBox.exec();
  }
}
