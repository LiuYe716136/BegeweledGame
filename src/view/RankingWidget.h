#ifndef RANKINGWIDGET_H
#define RANKINGWIDGET_H

#include <QWidget>
#include <vector>

struct RankingItem {
    QString name;
    int score;
    int level; // For challenge mode
};

QT_BEGIN_NAMESPACE
namespace Ui {
class RankingWidget;
}
QT_END_NAMESPACE

class RankingWidget : public QWidget {
    Q_OBJECT

public:
    explicit RankingWidget(QWidget *parent = nullptr);
    ~RankingWidget();

    void loadRanking();
    void updateRanking(const QString &mode, int score, int level = 0);

    std::vector<RankingItem> getEndlessRanking() const;
    std::vector<RankingItem> getChallengeRanking() const;

private slots:
    void on_btn_back_clicked();

signals:
    void backToMenu();

private:
    Ui::RankingWidget *ui;
    std::vector<RankingItem> m_endlessRanking;
    std::vector<RankingItem> m_challengeRanking;

    void loadEndlessRanking();
    void loadChallengeRanking();
    void saveEndlessRanking();
    void saveChallengeRanking();
    void displayEndlessRanking();
    void displayChallengeRanking();
    void loadBackground();
};

#endif // RANKINGWIDGET_H