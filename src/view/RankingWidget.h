#ifndef RANKINGWIDGET_H
#define RANKINGWIDGET_H

#include <QWidget>
#include <vector>

/**
 * @brief 排行榜条目结构体
 * 用于存储玩家的游戏排名信息
 */
struct RankingItem {
    QString name;      ///< 玩家名称
    int score;         ///< 得分
    int level;         ///< 关卡数（仅挑战模式有效）
};

QT_BEGIN_NAMESPACE
namespace Ui {
class RankingWidget;
}
QT_END_NAMESPACE

/**
 * @brief 排行榜界面类
 * 负责显示和管理无尽模式和挑战模式的玩家排名信息
 */
class RankingWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件
     */
    explicit RankingWidget(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~RankingWidget();
    
    /**
     * @brief 加载排行榜数据
     * 从文件加载无尽模式和挑战模式的排名数据
     */
    void loadRanking();
    
    /**
     * @brief 更新排行榜
     * 将新的游戏记录添加到对应模式的排行榜中
     * @param mode 游戏模式（"endless"/"challenge"）
     * @param score 得分
     * @param level 关卡数（仅挑战模式有效）
     */
    void updateRanking(const QString &mode, int score, int level = 0);
    
    /**
     * @brief 获取无尽模式排行榜
     * @return 无尽模式排行榜数据
     */
    std::vector<RankingItem> getEndlessRanking() const;
    
    /**
     * @brief 获取挑战模式排行榜
     * @return 挑战模式排行榜数据
     */
    std::vector<RankingItem> getChallengeRanking() const;

private slots:
    /**
     * @brief 返回菜单按钮点击槽函数
     */
    void on_btn_back_clicked();
    
    /**
     * @brief 清空排行榜按钮点击槽函数
     */
    void on_btn_clearRanking_clicked();

signals:
    /**
     * @brief 返回菜单信号
     */
    void backToMenu();

private:
    Ui::RankingWidget *ui;                     ///< UI 指针
    std::vector<RankingItem> m_endlessRanking;  ///< 无尽模式排行榜数据
    std::vector<RankingItem> m_challengeRanking; ///< 挑战模式排行榜数据
    
    /**
     * @brief 加载无尽模式排行榜
     */
    void loadEndlessRanking();
    
    /**
     * @brief 加载挑战模式排行榜
     */
    void loadChallengeRanking();
    
    /**
     * @brief 保存无尽模式排行榜
     */
    void saveEndlessRanking();
    
    /**
     * @brief 保存挑战模式排行榜
     */
    void saveChallengeRanking();
    
    /**
     * @brief 显示无尽模式排行榜
     */
    void displayEndlessRanking();
    
    /**
     * @brief 显示挑战模式排行榜
     */
    void displayChallengeRanking();
    
    /**
     * @brief 加载背景图片
     */
    void loadBackground();
};

#endif // RANKINGWIDGET_H