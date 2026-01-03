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

/**
 * @brief 游戏主菜单界面类
 * 负责显示游戏主菜单，包含游戏模式选择、排行榜查看和背景音乐控制功能
 */
class MenuWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件
     */
    explicit MenuWidget(QWidget *parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~MenuWidget();
    /**
     * @brief 重绘事件处理
     * @param event 绘图事件
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief 设置背景音乐播放器
     * @param player 音乐播放器指针
     */
    void setBgMusicPlayer(QMediaPlayer *player);

signals:
    /**
     * @brief 开始无尽模式信号
     */
    void startEndlessMode();
    /**
     * @brief 开始挑战模式信号
     */
    void startChallengeMode();
    /**
     * @brief 显示排行榜信号
     */
    void showRanking();

private slots:
    /**
     * @brief 无尽模式按钮点击槽函数
     */
    void on_btn_endless_clicked();
    /**
     * @brief 挑战模式按钮点击槽函数
     */
    void on_btn_challenge_clicked();
    /**
     * @brief 排行榜按钮点击槽函数
     */
    void on_btn_ranking_clicked();
    /**
     * @brief 音乐按钮点击槽函数
     */
    void onMusicBtnClicked();

private:
    Ui::MenuWidget *ui; ///< UI对象指针
    QMediaPlayer *m_bgMusicPlayer; ///< 背景音乐播放器指针
    bool m_musicEnabled; ///< 音乐是否启用
};

#endif // MENUWIDGET_H
