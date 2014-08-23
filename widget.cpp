#include "widget.h"
#include "ui_widget.h"
#include<QTextCodec>
#include <QDebug>
#include <qdir>
#include <QRegExp>
#include <QtCore>
#include <QtGui>
#include <QFileDialog>
#include <math.h>


using namespace std;

//QProcess *proc;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->light_label->setStyleSheet("background-color:red");
}

Widget::~Widget()
{
    delete ui;
}


void Widget::catchOutput()
{
//    QString message(proc->readAll());
    while(proc->canReadLine())
    {
        QString message = proc->readLine();

//        QString message = "[23:21:05 INFO]: 已分配内存: 989 MB.";
        QRegExp memory_rx(".*\\]: (.*)内存: (.*) MB.");

        int pos = message.indexOf(memory_rx);

        if(pos>=0)
        {
            QString memory_type = memory_rx.cap(1);         //类型
            QString memory_size = memory_rx.cap(2);         //大小
            if(memory_type == "最大")
            {
                ui->state_maxLabel->setText(memory_size);
            }
            else if(memory_type == "已分配")
            {
                ui->state_allocLabel->setText(memory_size);
            }
            else if(memory_type == "空闲")
            {
                ui->state_freeLabel->setText(memory_size);
            }
        }


    qDebug()<<message;
    ui->textBrowser->append(message);
    QRegExp rx;
    rx.setPatternSyntax(QRegExp::Wildcard);
    rx.setPattern(QString("*Done*"));
    if(rx.exactMatch(message))
    {
        ui->light_label->setStyleSheet("background-color:green");
    }
    }
}


void Widget::on_pushButton_3_clicked()
{
//    QString state;
//    switch(proc->state())
//    {
//        case 0:state = "NotRunning";
//        case 1:state = "Starting";
//        case 2:state = "Running";
//    }
//    qDebug()<<state;
    proc->write("memory\n");



}

void Widget::on_startButton_clicked()
{
    //设置进程工作目录
    QDir::setCurrent("/Users/bin/Desktop/bukkitServer");
//    qDebug()<<QDir::currentPath();

    proc = new QProcess(this);
    connect(proc, SIGNAL(readyReadStandardOutput()),this,SLOT(catchOutput()));
    ui->light_label->setStyleSheet("background-color:yellow");

    QStringList arguments;
    arguments << QString("-Xms%1").arg(ui->minLabel->text()) << QString("-Xmx%1").arg(ui->maxLabel->text())<<"-Dfile.encoding=UTF-8"<<
                 "-jar" << "craftbukkit-1.7.9-R0.2.jar";
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start("java",arguments);

    if (!proc->waitForStarted())
    {
        qDebug()<< "启动失败\n";
    }
     qDebug()<<"ok";
}

void Widget::on_stopButton_clicked()
{
    proc->write("stop\n");
    ui->light_label->setStyleSheet("background-color:red");
}

void Widget::on_pathButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("选择服务端的JAR文件");
    fileDialog->setNameFilter("JAR Files(*.jar)");
    if(fileDialog->exec() == QDialog::Accepted)
    {
         QString path = fileDialog->selectedFiles()[0];
         qDebug()<<path;
         QRegExp rx;
         rx.setPatternSyntax(QRegExp::Wildcard);
         rx.setPattern(QString("*.jar"));
         if(rx.exactMatch(path))
         {
            ui->jarLineEdit->setText(path);
         }
    }
}

void Widget::on_minHorizontalSlider_valueChanged(int value)
{
    int to_value = 128 * pow(2,value);
    QString memory = QString("%1M").arg(to_value);
    ui->minLabel->setText(memory);
}


void Widget::on_maxHorizontalSlider_valueChanged(int value)
{
    int to_value = 128 * pow(2,value);
    QString memory = QString("%1M").arg(to_value);
    ui->maxLabel->setText(memory);
}





