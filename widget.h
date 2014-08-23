#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    QProcess *proc;

    ~Widget();

private slots:
    void catchOutput();


    void on_pushButton_3_clicked();

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_pathButton_clicked();

    void on_minHorizontalSlider_valueChanged(int value);


    void on_maxHorizontalSlider_valueChanged(int value);

private:
    Ui::Widget *ui;


};

#endif // WIDGET_H
