#include "GameWidget.h"
#include "MenuWidget.h"
#include "RankingWidget.h"

#include <QApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFile>
#include <QDebug>

/**
 * @brief 程序主函数
 * 初始化Qt应用程序，创建并连接所有窗口部件，设置背景音乐播放器，
 * 建立窗口之间的信号槽连接，并启动应用程序事件循环
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 应用程序退出代码
 */
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  
  // 创建所有窗口部件
  MenuWidget menuWidget;     ///< 游戏主菜单窗口
  GameWidget gameWidget;     ///< 游戏主界面窗口
  RankingWidget rankingWidget; ///< 排行榜窗口
  
  // 创建共享背景音乐播放器
  QMediaPlayer bgMusicPlayer; ///< 背景音乐播放器
  QAudioOutput audioOutput;   ///< 音频输出设备
  bgMusicPlayer.setAudioOutput(&audioOutput);
  QUrl musicUrl(QUrl("qrc:/sounds/assets/sounds/Peter Hajba - Bejeweled 2 Theme.mp3")); ///< 音乐资源URL
  bgMusicPlayer.setSource(musicUrl);
  audioOutput.setVolume(0.5); ///< 设置音量为50%
  bgMusicPlayer.setLoops(QMediaPlayer::Infinite); ///< 设置音乐循环播放
  
  // 连接音乐播放状态变化信号，确保音乐持续播放
  QObject::connect(&bgMusicPlayer, &QMediaPlayer::playbackStateChanged, [&](QMediaPlayer::PlaybackState state) {
      qDebug() << "播放状态变化:" << state;
      if (state == QMediaPlayer::StoppedState && bgMusicPlayer.hasAudio()) {
          bgMusicPlayer.play(); ///< 如果音乐停止且有音频可用，则重新开始播放
      }
  });
  
  // 连接音乐播放器错误信号
  QObject::connect(&bgMusicPlayer, &QMediaPlayer::errorOccurred, [](QMediaPlayer::Error error, const QString &errorString) {
      qDebug() << "媒体播放器错误:" << error << "-" << errorString;
  });
  
  // 调试信息输出
  qDebug() << "音乐URL:" << musicUrl;
  qDebug() << "音乐源有效:" << bgMusicPlayer.isAvailable();
  qDebug() << "有音频:" << bgMusicPlayer.hasAudio();
  
  // 开始播放背景音乐（如果有音频可用）
  if (bgMusicPlayer.hasAudio()) {
      bgMusicPlayer.play();
      qDebug() << "音乐开始播放";
  } else {
      qDebug() << "无音频可用";
  }
  
  // 为游戏窗口设置音乐播放器
  gameWidget.setBgMusicPlayer(&bgMusicPlayer);
  
  // 为菜单窗口设置音乐播放器
  menuWidget.setBgMusicPlayer(&bgMusicPlayer);
  
  // 更新排行榜窗口引用
  gameWidget.updateRankingWidget(&rankingWidget);
  
  // 连接菜单信号，显示对应的窗口
  QObject::connect(&menuWidget, &MenuWidget::startEndlessMode, [&]() {
      gameWidget.setGameMode(GameWidget::ENDLESS); ///< 设置无尽模式
      gameWidget.setChallengeLevel(1);
      menuWidget.hide();
      gameWidget.show();
  });
  
  QObject::connect(&menuWidget, &MenuWidget::startChallengeMode, [&]() {
      gameWidget.setGameMode(GameWidget::CHALLENGE); ///< 设置挑战模式
      gameWidget.setChallengeLevel(1);
      menuWidget.hide();
      gameWidget.show();
  });
  
  QObject::connect(&menuWidget, &MenuWidget::showRanking, [&]() {
      rankingWidget.loadRanking(); ///< 加载排行榜数据
      menuWidget.hide();
      rankingWidget.show();
  });
  
  // 连接游戏窗口信号
  QObject::connect(&gameWidget, &GameWidget::backToMenu, [&]() {
      gameWidget.hide();
      menuWidget.show(); ///< 返回主菜单
  });
  
  QObject::connect(&gameWidget, &GameWidget::gameOver, [&](int score, int level) {
      // 根据游戏模式更新对应的排行榜
      if (gameWidget.gameMode() == GameWidget::ENDLESS) {
          rankingWidget.updateRanking("endless", score);
      } else {
          rankingWidget.updateRanking("challenge", score, level);
      }
  });
  
  // 连接排行榜窗口信号
  QObject::connect(&rankingWidget, &RankingWidget::backToMenu, [&]() {
      rankingWidget.hide();
      menuWidget.show(); ///< 返回主菜单
  });
  
  // 首先显示主菜单窗口
  menuWidget.show();
  
  // 启动应用程序事件循环
  return a.exec();
}
