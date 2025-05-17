#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>
#include <QSettings>
#include <QListWidgetItem>
#include <QtMultimedia/QMediaPlayer>
#include <QFileInfo>

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
    void loadAllAudioFilePath(QString folderPath);
    void loadRank();
    void saveRank();
    void showRank();

private:
    Ui::MusicPlayer *ui;
    QSettings m_settings;
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    QFileInfoList m_allAudioList;
    QMap<QString, int> m_songPlayCount;
private slots:
    void on_pbFolder_clicked();
    void onAudioItemDoubleClicked(QListWidgetItem*);
    void updateTimeDisplay();
    void onSliderMoved(int value);
};
#endif // MUSICPLAYER_H
