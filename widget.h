#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include <QtCore>
#include <QtGui>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    QProcess *proc;
    void updateSettings(QString key,bool value);
    ~Widget();

private slots:
    void catchOutput();

    void on_pushButton_3_clicked();

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_pathButton_clicked();

    void on_minHorizontalSlider_valueChanged(int value);

    void on_maxHorizontalSlider_valueChanged(int value);

    void updateInfo();

    void updateRunningTime();


private:
    Ui::Widget *ui;
    int info_mark;
    int max_memory;
    int alloc_memory;
    int free_memory;
    QTimer *updateTimer;
    QTimer *timer;
    int runningTime;

    QString serverPath;
    QSettings *settings;
};

#endif // WIDGET_H
