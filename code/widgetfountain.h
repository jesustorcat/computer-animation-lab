#ifndef WIDGETFOUNTAIN_H
#define WIDGETFOUNTAIN_H

#include <QWidget>

namespace Ui {
class WidgetFountain;
}

class WidgetFountain : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetFountain(QWidget *parent = nullptr);
    ~WidgetFountain();

    double getGravity()    const;
    bool attractionGravitatoryForceFlag() const;
    double getkFriction() const;
    double getkBounce() const;
    void setGenerateParticles(int idx) const;
    int getGenerateParticles() const;

signals:
    void updatedParameters();

private:
    Ui::WidgetFountain *ui;
};

#endif // WIDGETFOUNTAIN_H
