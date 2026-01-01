#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MenuWidget;
}
QT_END_NAMESPACE

class MenuWidget : public QWidget {
    Q_OBJECT

public:
    explicit MenuWidget(QWidget *parent = nullptr);
    ~MenuWidget();
    void paintEvent(QPaintEvent *event) override;

    void setBgMusicPlayer(QMediaPlayer *player);

signals:
    void startEndlessMode();
    void startChallengeMode();
    void showRanking();

private slots:
    void on_btn_endless_clicked();
    void on_btn_challenge_clicked();
    void on_btn_ranking_clicked();
    void onMusicBtnClicked();

private:
    Ui::MenuWidget *ui;
    void loadBackground();
    void createMusicButton(); 
    QMediaPlayer *m_bgMusicPlayer;
    QPushButton *m_musicBtn;
    bool m_musicEnabled;
};

#endif // MENUWIDGET_H
