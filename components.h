#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <QApplication>
#include <QPushButton>
#include <QMenu>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidgetAction>

class VolumeControl: public QPushButton {
    Q_OBJECT
public:
    VolumeControl(int volume, QWidget* parent = nullptr);

    void showMenu(int value);

private:
    QMenu* menu;
    QSlider* slider;
signals:
    void volumeChanged(int value);
};


#endif // COMPONENTS_H
