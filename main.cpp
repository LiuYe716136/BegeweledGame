#include "GameWidget.h"
#include "MenuWidget.h"
#include "RankingWidget.h"

#include <QApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  
  // Create all widgets
  MenuWidget menuWidget;
  GameWidget gameWidget;
  RankingWidget rankingWidget;
  
  // Create shared background music player
  QMediaPlayer bgMusicPlayer;
  QAudioOutput audioOutput;
  bgMusicPlayer.setAudioOutput(&audioOutput);
  QUrl musicUrl(QUrl("qrc:/sounds/assets/sounds/Peter Hajba - Bejeweled 2 Theme.mp3"));
  bgMusicPlayer.setSource(musicUrl);
  audioOutput.setVolume(0.5);
  bgMusicPlayer.setLoops(QMediaPlayer::Infinite);
  
  QObject::connect(&bgMusicPlayer, &QMediaPlayer::playbackStateChanged, [&](QMediaPlayer::PlaybackState state) {
      qDebug() << "Playback state changed:" << state;
      if (state == QMediaPlayer::StoppedState && bgMusicPlayer.hasAudio()) {
          bgMusicPlayer.play();
      }
  });
  
  QObject::connect(&bgMusicPlayer, &QMediaPlayer::errorOccurred, [](QMediaPlayer::Error error, const QString &errorString) {
      qDebug() << "Media Player Error:" << error << "-" << errorString;
  });
  
  qDebug() << "Music URL:" << musicUrl;
  qDebug() << "Music source is valid:" << bgMusicPlayer.isAvailable();
  qDebug() << "Has audio:" << bgMusicPlayer.hasAudio();
  
  if (bgMusicPlayer.hasAudio()) {
      bgMusicPlayer.play();
      qDebug() << "Music started playing";
  } else {
      qDebug() << "No audio available";
  }
  
  // Set music player for game widget
  gameWidget.setBgMusicPlayer(&bgMusicPlayer);
  
  // Set music player for menu widget
  menuWidget.setBgMusicPlayer(&bgMusicPlayer);
  
  // Update ranking widget reference
  gameWidget.updateRankingWidget(&rankingWidget);
  
  // Connect menu signals to show corresponding widgets
  QObject::connect(&menuWidget, &MenuWidget::startEndlessMode, [&]() {
      gameWidget.setGameMode(GameWidget::ENDLESS);
      gameWidget.setChallengeLevel(1);
      menuWidget.hide();
      gameWidget.show();
  });
  
  QObject::connect(&menuWidget, &MenuWidget::startChallengeMode, [&]() {
      gameWidget.setGameMode(GameWidget::CHALLENGE);
      gameWidget.setChallengeLevel(1);
      menuWidget.hide();
      gameWidget.show();
  });
  
  QObject::connect(&menuWidget, &MenuWidget::showRanking, [&]() {
      rankingWidget.loadRanking();
      menuWidget.hide();
      rankingWidget.show();
  });
  
  // Connect game widget signals
  QObject::connect(&gameWidget, &GameWidget::backToMenu, [&]() {
      gameWidget.hide();
      menuWidget.show();
  });
  
  QObject::connect(&gameWidget, &GameWidget::gameOver, [&](int score, int level) {
      if (gameWidget.gameMode() == GameWidget::ENDLESS) {
          rankingWidget.updateRanking("endless", score);
      } else {
          rankingWidget.updateRanking("challenge", score, level);
      }
  });
  
  // Connect ranking widget signal
  QObject::connect(&rankingWidget, &RankingWidget::backToMenu, [&]() {
      rankingWidget.hide();
      menuWidget.show();
  });
  
  // Show menu widget first
  menuWidget.show();
  
  return a.exec();
}
