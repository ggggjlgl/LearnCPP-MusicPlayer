#include <QFileDialog>
#include <QString>
#include <QFileInfo>
#include <QMessageBox>
#include <QAudioOutput>
#include "musicplayer.h"
#include "./ui_musicplayer.h"

MusicPlayer::MusicPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicPlayer), m_settings("MyCompany", "MyApp")
{
    setWindowIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->setupUi(this);
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.2);
    loadAllAudioFilePath(m_settings.value("lastFolderPath", QDir().homePath()).toString());
    loadRank();
    connect(ui->lwAll, &QListWidget::itemDoubleClicked, this, &MusicPlayer::onAudioItemDoubleClicked);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MusicPlayer::updateTimeDisplay);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MusicPlayer::updateTimeDisplay);
    connect(ui->hsTime, &QSlider::sliderMoved, this, &MusicPlayer::onSliderMoved);
    connect(ui->toolBox, &QToolBox::currentChanged, [this](int index) {
        if (index==1) {
            showRank();
        }
    });
}

MusicPlayer::~MusicPlayer()
{
    delete ui;
}

void MusicPlayer::on_pbFolder_clicked() {
    QString folderPath = QFileDialog::getExistingDirectory(nullptr, "选择文件夹", "");
    if (!folderPath.isEmpty()) {
        loadAllAudioFilePath(folderPath);
    }
}
void MusicPlayer::loadRank() {
    m_settings.beginGroup("SongPlayCount");
    const QStringList keys = m_settings.childKeys();
    foreach (const QString &key, keys) {
        m_songPlayCount[key] = m_settings.value(key).toInt();
    }
    m_settings.endGroup();
}
void MusicPlayer::saveRank() {
    m_settings.beginGroup("SongPlayCount");
    QMapIterator<QString, int> it(m_songPlayCount);
    while(it.hasNext()) {
        it.next();
        m_settings.setValue(it.key(), it.value());
    }
    m_settings.endGroup();
}
void MusicPlayer::loadAllAudioFilePath(QString folderPath) {
    m_settings.setValue("lastFolderPath", folderPath);
    ui->lwAll->clear();
    QDir dir(folderPath);
    QStringList filters = {"*.mp3", "*.flac"};
    this->m_allAudioList = dir.entryInfoList(filters, QDir::Files);
    foreach (const QFileInfo& fileInfo, m_allAudioList) {
        QListWidgetItem* item = new QListWidgetItem(fileInfo.fileName(), ui->lwAll);
        item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
    }
}

void MusicPlayer::onAudioItemDoubleClicked(QListWidgetItem* item) {
    QString filePath = item->data(Qt::UserRole).toString();
    m_songPlayCount[filePath]++;
    saveRank();
    m_player->setSource(QUrl::fromLocalFile(filePath));
    m_player->play();
}
void MusicPlayer::showRank() {
    ui->lwLike->clear();
    QList<QPair<QString, int>> sortedList;
    QMapIterator<QString, int> it(m_songPlayCount);
    while (it.hasNext()) {
        it.next();
        sortedList.append(qMakePair(it.key(), it.value()));
    }
    std::sort(sortedList.begin(), sortedList.end(), [](const QPair<QString, int> &a, const QPair<QString, int> &b) {return a.second > b.second;});
    int count = qMin(10, sortedList.size());
    for (int i=0; i<count; ++i) {
        const auto &pair = sortedList[i];
        QListWidgetItem *item = new QListWidgetItem(QFileInfo(pair.first).fileName(), ui->lwLike);
        item->setData(Qt::UserRole, pair.first);
    }
}
void MusicPlayer::updateTimeDisplay() {
    qint64 duration = m_player->duration();
    qint64 position = m_player->position();
    QString durationTime = QString("%1:%2:%3")
                               .arg(duration / 3600000, 2, 10, QLatin1Char('0'))
                               .arg(duration % 3600000 / 60000, 2, 10, QLatin1Char('0'))
                               .arg(duration % 60000 / 1000, 2, 10, QLatin1Char('0'));
    QString positionTime = QString("%1:%2:%3")
                               .arg(position / 3600000, 2, 10, QLatin1Char('0'))
                               .arg(position % 3600000 / 60000, 2, 10, QLatin1Char('0'))
                               .arg(position % 60000 / 1000, 2, 10, QLatin1Char('0'));
    ui->lbStartTIme->setText(positionTime);
    ui->lbTotalTIme->setText(durationTime);
    if (!(ui->hsTime->isSliderDown())) {
        short process = static_cast<short>((position * 100) / (duration + 1));
        ui->hsTime->setValue(process);
    }

}
void MusicPlayer::onSliderMoved(int value) {
    qint64 position = (value * m_player->duration()) / 100;
    m_player->setPosition(position);
}

