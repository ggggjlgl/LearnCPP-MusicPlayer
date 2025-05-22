#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>
#include <QSettings>
#include <QListWidgetItem>
#include <QtMultimedia/QMediaPlayer>
#include <QFileInfo>
#include "components.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MusicPlayer;
}
QT_END_NAMESPACE

class MusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();
    void loadAll(QString folderPath);
    void loadRank();
    void saveRank();
    void showRank();
    void playSong(bool rankFlag);
    void playNextPrev(bool rankFlag, int change=1);
    void setUpCustomWidget();

private:
    Ui::MusicPlayer *ui;
    QSettings m_settings;
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    QFileInfoList m_allAudioList;
    QFileInfoList m_curSongs;
    QMap<QString, int> m_songPlayCount;
    int m_curIndex;
    QListWidget* m_curListWidget;
    float m_volume;
    VolumeControl* pbVolume;
private slots:
    void on_pbFolder_clicked();
    void on_pbNext_clicked();
    void on_pbPrev_clicked();
    void on_pbPlay_clicked();
    void onAudioItemDoubleClicked(QListWidgetItem* item, QListWidget *curListWidget);
    void updateTimeDisplay();
    void onSliderMoved();
    void onSongEnd(QMediaPlayer::MediaStatus status);
    void closeEvent(QCloseEvent *event);
    void setVolume(int value);
};
#endif // MUSICPLAYER_H
