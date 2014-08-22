#include "widget.h"
#include "ui_widget.h"
#include <iostream>
#include<QTextCodec>
#include <QDebug>
#include <qdir>
#include <QRegExp>

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
//    proc.start("netstat", arguments);
void Widget::on_pushButton_clicked()
{
    //设置进程工作目录
    QDir::setCurrent("/Users/bin/Desktop/bukkitServer");
//    qDebug()<<QDir::currentPath();

    proc = new QProcess(this);
    connect(proc, SIGNAL(readyReadStandardOutput()),this,SLOT(catchOutput()));
    ui->light_label->setStyleSheet("background-color:yellow");

    QStringList arguments;
    arguments << "-Xms1024M" << "-Xms1024M" << "-jar" << "craftbukkit-1.7.9-R0.2.jar";
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start("java",arguments);
    if (!proc->waitForStarted())
    {
        cout << "启动失败\n";
    }
     cout<<"ok"<<endl;
}

void Widget::on_pushButton_2_clicked()
{
    proc->write("stop\n");
    ui->light_label->setStyleSheet("background-color:red");

}

void Widget::catchOutput()
{
    QString message(proc->readAll());
    qDebug()<< message;
    ui->textBrowser->append(message);
    QRegExp rx;
    rx.setPatternSyntax(QRegExp::Wildcard);
    rx.setPattern(QString("*Done*"));
    if(rx.exactMatch(message))
    {
        ui->light_label->setStyleSheet("background-color:green");
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

}
