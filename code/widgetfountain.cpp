#include "widgetfountain.h"
#include "ui_widgetfountain.h"

WidgetFountain::WidgetFountain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFountain)
{
    ui->setupUi(this);

    connect(ui->btnUpdate, &QPushButton::clicked, this,
            [=] (void) { emit updatedParameters(); });
}

WidgetFountain::~WidgetFountain()
{
    delete ui;
}

double WidgetFountain::getGravity() const {
    return ui->gravity->value();
}

bool WidgetFountain::attractionGravitatoryForceFlag() const {
    return ui->afCheckbox->isChecked();
}

double WidgetFountain::getkBounce() const {
    return ui->kBounce->value();
}

double WidgetFountain::getkFriction() const {
    return ui->kFriction->value();
}

void WidgetFountain::setGenerateParticles(int idx) const {
    ui->comboBoxGenerate->setCurrentIndex(idx);
}

int WidgetFountain::getGenerateParticles() const {
    return ui->comboBoxGenerate->currentIndex();
}
