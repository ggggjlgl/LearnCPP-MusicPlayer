#include <QFileDialog>
#include <QString>
#include <QFileInfo>
#include <QMessageBox>
#include <QCloseEvent>
#include <QAudioOutput>
#include "musicplayer.h"
#include "./ui_musicplayer.h"

MusicPlayer::MusicPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicPlayer), m_settings("settings.ini", QSettings::IniFormat), m_curIndex(0)
{
    setWindowIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->setupUi(this);
    m_curListWidget = ui->lwAll;
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.2);
    loadAllAudioFilePath(m_settings.value("lastFolderPath", QDir().homePath()).toString());
    loadRank();
    connect(ui->lwAll, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item){
        onAudioItemDoubleClicked(item, ui->lwAll);
    });
    connect(ui->lwLike, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item){
        onAudioItemDoubleClicked(item, ui->lwLike);
    });
    connect(m_player, &QMediaPlayer::durationChanged, this, &MusicPlayer::updateTimeDisplay);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MusicPlayer::updateTimeDisplay);
    connect(ui->hsTime, &QSlider::sliderMoved, this, &MusicPlayer::onSliderMoved);
    connect(ui->hsTime, &QSlider::sliderPressed, this, &MusicPlayer::onSliderMoved);
    connect(ui->toolBox, &QToolBox::currentChanged, [this](int index) {
        if (index==1) {
            showRank();
        }
    });
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::onSongEnd);
}

MusicPlayer::~MusicPlayer()
{
    delete ui;
}
void MusicPlayer::playNextPrev(bool rankFlag, int change) {
    if (m_curIndex + change < m_curSongs.count()) {
        m_curIndex += change;
        playSong(rankFlag);
    }
}
void MusicPlayer::onSongEnd(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        playNextPrev(false);
    }
}
void MusicPlayer::closeEvent(QCloseEvent *event) {
    m_settings.sync();
    event->accept();
}
void MusicPlayer::on_pbPlay_clicked() {
    if (m_curListWidget == ui->lwAll) {
        ui->toolBox->setCurrentIndex(0);
    } else if (m_curListWidget == ui->lwLike) {
        ui->toolBox->setCurrentIndex(1);
    }
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
        ui->pbPlay->setText("▶️");
        m_curListWidget->setCurrentRow(m_curIndex);
    } else if (m_player->playbackState() == QMediaPlayer::PausedState) {
        m_player->play();
        ui->pbPlay->setText("⏸️");
        m_curListWidget->setCurrentRow(m_curIndex);
    } else {
        if (ui->toolBox->currentIndex() == 0) {
            QList<QListWidgetItem*> selected = ui->lwAll->selectedItems();
            if (!selected.isEmpty()) {
                onAudioItemDoubleClicked(selected.first(), ui->lwAll);
            }
        } else if (ui->toolBox->currentIndex() == 1) {
            QList<QListWidgetItem*> selected = ui->lwLike->selectedItems();
            if (!selected.isEmpty()) {
                onAudioItemDoubleClicked(selected.first(), ui->lwLike);
            }
        }
    }
}
void MusicPlayer::on_pbFolder_clicked() {
    QString folderPath = QFileDialog::getExistingDirectory(nullptr, "选择文件夹", "");
    if (!folderPath.isEmpty()) {
        loadAllAudioFilePath(folderPath);
    }
}
void MusicPlayer::on_pbNext_clicked() {
    playNextPrev(false);
}
void MusicPlayer::on_pbPrev_clicked() {
    playNextPrev(false, -1);
}
void MusicPlayer::loadRank() {
    m_songPlayCount.clear();
    m_settings.beginGroup("SongPlayCount");
    const QStringList keys = m_settings.childKeys();
    foreach (const QString &key, keys) {
        QString decondedKey = QUrl::fromPercentEncoding(key.toUtf8());
        m_songPlayCount[decondedKey] = m_settings.value(key).toInt();
    }
    m_settings.endGroup();
}
void MusicPlayer::saveRank() {
    m_settings.beginGroup("SongPlayCount");
    for (auto it = m_songPlayCount.constBegin(); it != m_songPlayCount.constEnd(); ++it) {
        QString encondedKey = QUrl::toPercentEncoding(it.key().toUtf8());
        m_settings.setValue(encondedKey, it.value());
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
void MusicPlayer::playSong(bool rankFlag) {
    if (m_curIndex < m_curListWidget->count()) {
        m_curListWidget->setCurrentRow(m_curIndex);
    }
    if (m_curIndex < m_curSongs.count()) {
        QString filePath = m_curSongs[m_curIndex].absoluteFilePath();
        m_player->setSource(QUrl::fromLocalFile(filePath));
        m_player->play();
        ui->pbPlay->setText("⏸️");
        if (rankFlag) {
            m_songPlayCount[filePath]++;
            saveRank();
        }
    }
}
void MusicPlayer::onAudioItemDoubleClicked(QListWidgetItem* item, QListWidget *curListWidget) {
    m_curListWidget = curListWidget;
    m_curSongs.clear();
    for (int i=0; i<curListWidget->count(); ++i) {
        QListWidgetItem *itemTmp = curListWidget->item(i);
        m_curSongs.append(QFileInfo(itemTmp->data(Qt::UserRole).toString()));
        if (itemTmp == item) {
            m_curIndex = i;
        }
    }
    playSong(true);

}
void MusicPlayer::showRank() {
    ui->lwLike->clear();
    QList<QPair<QString, int>> sortedList;
    loadRank();
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
void MusicPlayer::onSliderMoved() {
    if (m_player->duration()) {
        qint64 position = (ui->hsTime->value() * m_player->duration()) / 100;
        m_player->setPosition(position);
    }
}

