#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include "musicplayer.h"
#include "./ui_musicplayer.h"

MusicPlayer::MusicPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicPlayer), m_settings("MyCompany", "MyApp")
{
    setWindowIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->setupUi(this);

    QString lastFolderPath = m_settings.value("lastFolderPath", QDir().homePath()).toString();
    QMessageBox::information(this, "提示", "您选择的文件夹是:\n" + lastFolderPath);
}

MusicPlayer::~MusicPlayer()
{
    delete ui;
}

void MusicPlayer::on_pbFolder_clicked() {
    QString folderPath = QFileDialog::getExistingDirectory(nullptr, "选择文件夹", "");
    if (folderPath.isEmpty()) {
        QMessageBox::information(this, "取消", "您未选择任何文件夹");

    } else {
        m_settings.setValue("lastFolderPath", folderPath);
        QMessageBox::information(this, "提示", "您选择的文件夹是:\n" + m_settings.value("lastFolderPath").toString());

    }

}
