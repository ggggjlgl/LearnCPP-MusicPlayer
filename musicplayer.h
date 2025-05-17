#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>

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

private:
    Ui::MusicPlayer *ui;
private slots:
    void on_pbFolder_clicked();
};
#endif // MUSICPLAYER_H
