#include "components.h"



VolumeControl::VolumeControl(int volume, QWidget* parent): QPushButton("🔉", parent){
    menu = new QMenu(this);
    QSlider* slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 100);
    slider->setValue(volume);
    slider->setFixedHeight(100);
    slider->setFixedWidth(26);


    QWidgetAction* action = new QWidgetAction(menu);
    action->setDefaultWidget(slider);
    menu->addAction(action);

    connect(slider, &QSlider::valueChanged, this, &VolumeControl::volumeChanged);
    connect(this, &QPushButton::clicked, this, &VolumeControl::showMenu);
}

void VolumeControl::showMenu(int value) {
    if (menu->isVisible()) {
        menu->hide();
    } else {
        menu->exec(mapToGlobal(QPoint((this->width() - 26) / 2, -menu->sizeHint().height())));
    }
}
