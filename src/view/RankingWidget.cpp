#include "RankingWidget.h"
#include "ui_RankingWidget.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QPixmap>
#include <QDebug>

RankingWidget::RankingWidget(QWidget *parent) : QWidget(parent), ui(new Ui::RankingWidget) {
    ui->setupUi(this);
    loadBackground();
    loadRanking();
}

RankingWidget::~RankingWidget() {
    delete ui;
}

void RankingWidget::loadBackground() {
    QPixmap backgroundPixmap;
    bool loadSuccess = backgroundPixmap.load(":/bgs/assets/images/login_bg.jpg");
    
    if (loadSuccess) {
        this->setStyleSheet(
            "#RankingWidget { "
            "border-image: url(:/bgs/assets/images/login_bg.jpg) 0 0 0 0 stretch stretch; "
            "background-color: #000000; "
            "}"
            "QLabel { "
            "color: white; "
            "font-family: 'Microsoft YaHei'; "
            "font-weight: bold; "
            "}"
            "QPushButton { "
            "background-color: rgba(255, 255, 255, 200); "
            "border: 2px solid #8f8f91; "
            "border-radius: 10px; "
            "padding: 5px; "
            "font-size: 16px; "
            "color: #333; "
            "font-weight: bold; "
            "}"
            "QPushButton:hover { "
            "background-color: rgba(255, 255, 255, 240); "
            "border-color: #ffffff; "
            "}"
            "QPushButton:pressed { "
            "background-color: rgba(200, 200, 200, 200); "
            "}"
            "QListWidget { "
            "background-color: rgba(0, 0, 0, 180); "
            "color: white; "
            "font-size: 16px; "
            "border: 2px solid gold; "
            "border-radius: 5px; "
            "}"
            "QTabWidget { "
            "background-color: rgba(0, 0, 0, 0); "
            "}"
            "QTabBar::tab { "
            "background-color: rgba(255, 255, 255, 150); "
            "color: #333; "
            "font-size: 16px; "
            "font-weight: bold; "
            "padding: 10px; "
            "}"
            "QTabBar::tab:selected { "
            "background-color: rgba(255, 215, 0, 200); "
            "}"
        );
    }
}

void RankingWidget::loadRanking() {
    loadEndlessRanking();
    loadChallengeRanking();
    displayEndlessRanking();
    displayChallengeRanking();
}

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

void RankingWidget::saveEndlessRanking() {
    QFile file("endless_ranking.txt");
    
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto& item : m_endlessRanking) {
            out << item.name << "," << item.score << "\n";
        }
        file.close();
    }
}

void RankingWidget::saveChallengeRanking() {
    QFile file("challenge_ranking.txt");
    
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto& item : m_challengeRanking) {
            out << item.name << "," << item.score << "," << item.level << "\n";
        }
        file.close();
    }
}

void RankingWidget::displayEndlessRanking() {
    ui->list_endless->clear();
    for (size_t i = 0; i < m_endlessRanking.size(); i++) {
        const auto& item = m_endlessRanking[i];
        QString text = QString("%1. %2 - %3分").arg(i + 1).arg(item.name).arg(item.score);
        ui->list_endless->addItem(text);
    }
    
    if (m_endlessRanking.empty()) {
        ui->list_endless->addItem("暂无记录");
    }
}

void RankingWidget::displayChallengeRanking() {
    ui->list_challenge->clear();
    for (size_t i = 0; i < m_challengeRanking.size(); i++) {
        const auto& item = m_challengeRanking[i];
        QString text = QString("%1. %2 - 第%3关，%4分").arg(i + 1).arg(item.name).arg(item.level).arg(item.score);
        ui->list_challenge->addItem(text);
    }
    
    if (m_challengeRanking.empty()) {
        ui->list_challenge->addItem("暂无记录");
    }
}

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

std::vector<RankingItem> RankingWidget::getEndlessRanking() const {
    return m_endlessRanking;
}

std::vector<RankingItem> RankingWidget::getChallengeRanking() const {
    return m_challengeRanking;
}

void RankingWidget::on_btn_back_clicked() {
    emit backToMenu();
}