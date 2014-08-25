#include "widget.h"
#include "ui_widget.h"
#include <QTextCodec>
#include <QDebug>
#include <qdir>
#include <QRegExp>

#include <QFileDialog>
#include <math.h>
#include <QMessageBox>

using namespace std;


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->light_label->setStyleSheet("background-color:red");
    updateTimer = new QTimer();
    timer = new QTimer();
    info_mark = 0;
    jarName = "";
    jarPath = "";

    connect(updateTimer,SIGNAL(timeout()),this,SLOT(updateInfo()));
    connect(timer,SIGNAL(timeout()),this,SLOT(updateRunningTime()));

    GUIsettings = new QSettings(QString("%1/settings.ini").arg(QDir::currentPath()),QSettings::IniFormat);
    if(GUIsettings->value("jarPath","").toString() == "")
    {
        ui->generalTextBrowser->append(QString("[%1]:     GUI配置文件读取失败").arg(QTime::currentTime().toString()));
    }
    else
    {
        ui->generalTextBrowser->append(QString("[%1]:     GUI配置文件读取成功").arg(QTime::currentTime().toString()));
        jarPath = GUIsettings->value("jarPath","").toString();
        jarName = GUIsettings->value("jarName","").toString();
        ui->jarLineEdit->setText(QString("%1/%2").arg(jarPath).arg(jarName));
        loadServerSettings();
    }


}

Widget::~Widget()
{
    delete ui;
}

void Widget::loadServerSettings()
{
    ui->serverSettingGroupBox->setEnabled(true);

    QString path = QString("%1/server.properties").arg(jarPath);
    settings = new QSettings(path,QSettings::IniFormat);
    if(settings->value("server-port","-1").toInt() == -1)
    {
        qDebug()<<"服务端配置文件读取失败";
        ui->generalTextBrowser->append(QString("[%1]:     服务端配置文件读取失败").arg(QTime::currentTime().toString()));
    }
    else
    {
        ui->generalTextBrowser->append(QString("[%1]:     服务端配置文件读取成功").arg(QTime::currentTime().toString()));

        ui->maxUserSpinBox->setValue(settings->value("max-players","0").toInt());
        ui->portLineEdit->setText(settings->value("server-port","0").toString());
        ui->monsterCheckBox->setChecked(settings->value("spawn-monsters","0").toBool());
        ui->animalCheckBox->setChecked(settings->value("spawn-animals","0").toBool());
        ui->npcCheckBox->setChecked(settings->value("spawn-npcs","0").toBool());
        ui->netherCheckBox->setChecked(settings->value("allow-nether","0").toBool());
        ui->pvpCheckBox->setChecked(settings->value("pvp","0").toBool());
        ui->onlineCheckBox->setChecked(settings->value("online-mode","0").toBool());
    }
}

void Widget::catchOutput()
{
//    QString message(proc->readAll());
    while(proc->canReadLine())
    {

        QString message = proc->readLine();

        // 屏蔽+获取 memory信息
        QRegExp memory_rx(".*/memory.*");
        if(memory_rx.exactMatch(message))
        {
            info_mark = 8;
            continue;
        }
        if(info_mark)
        {
            QRegExp memory_info_rx(".*\\]: (.*)内存: (.*) MB.");
            int pos = message.indexOf(memory_info_rx);
            if(pos>=0)
            {
                QString memory_type = memory_info_rx.cap(1);         //类型
                int memory_size = memory_info_rx.cap(2).toInt();         //大小
                if(memory_type == "最大")
                {
//                    ui->state_maxLabel->setText(memory_size);
                    max_memory = memory_size;
                }
                else if(memory_type == "已分配")
                {
//                    ui->state_allocLabel->setText(memory_size);
                    alloc_memory = memory_size;

                }
                else if(memory_type == "空闲")
                {
//                    ui->state_freeLabel->setText(memory_size);
                    free_memory = memory_size;

                }
            }
            info_mark--;
            if(info_mark == 0)
            {
                int JVMProgress_percent = alloc_memory * 100 / max_memory;
                int use_memory = alloc_memory - free_memory;
                int serverProgress_percent = use_memory * 100 / alloc_memory;
                ui->JVMProgressBar->setValue(JVMProgress_percent);
                ui->serverProgressBar->setValue(serverProgress_percent);
                ui->jvmProgressLabel->setText(QString("[%1M/%2M]").arg(alloc_memory).arg(max_memory));
                ui->serverProgressLabel->setText(QString("[%1M/%2M]").arg(use_memory).arg(alloc_memory));
            }
        }
        else
        {
            qDebug()<<message;

            // 获取version信息
            QRegExp version_rx(".*Implementing API version (.*)\\)");
            int pos = message.indexOf(version_rx);
            if(pos>=0)
                ui->versionLabel->setText(version_rx.cap(1));

            // 获取plugin信息
            QRegExp plugin_rx(".*Plugins \\((.*)\\).*");
            int pos2 = message.indexOf(plugin_rx);
            if(pos2>=0)
                ui->pluginLabel->setText(plugin_rx.cap(1));


            ui->textBrowser->append(message);
            QRegExp rx;
            rx.setPatternSyntax(QRegExp::Wildcard);
            rx.setPattern(QString("*Done*"));
            if(rx.exactMatch(message))
            {
                ui->light_label->setStyleSheet("background-color:green");
                ui->runningGroupBox->setEnabled(true);
                ui->stopButton->setEnabled(true);

                ui->generalTextBrowser->append(QString("[%1]:     服务端启动成功").arg(QTime::currentTime().toString()));


                proc->write("version\n");
                proc->write("pl\n");
                updateInfo();
                updateTimer->start(10000);
                runningTime = 0;
                timer->start(1000);
            }
        }
   }
}

void Widget::updateInfo()
{
    qDebug()<<"time";
    proc->write("memory\n");
}

void Widget::updateRunningTime()
{
    runningTime++;
    ui->runningTimeLabel->setText(QString("%1 时 %2 分 %3 秒").arg(runningTime/3600).arg(runningTime /60 % 60).arg(runningTime % 60 % 60));
}

void Widget::updateSettings(QString key,bool value)
{
    if(value)
        settings->setValue(key,"true");
    else
        settings->setValue(key,"false");
}

void Widget::on_startButton_clicked()
{
    if(ui->minHorizontalSlider->value() > ui->maxHorizontalSlider->value())
    {
        QMessageBox::warning(NULL, "警告", "最大内存应大于等于初始内存!", QMessageBox::Yes);
        return;
    }
    if(ui->jarLineEdit->text() == "")
    {
        QMessageBox::warning(NULL, "警告", "请选择服务端的jar文件!", QMessageBox::Yes);
        return;
    }

        //修改配置文件
        settings->setValue("max-players",ui->maxUserSpinBox->value());
        settings->setValue("server-port",ui->portLineEdit->text());
        updateSettings("spawn-monsters",ui->monsterCheckBox->isChecked());
        updateSettings("spawn-animals",ui->animalCheckBox->isChecked());
        updateSettings("spawn-monsters",ui->npcCheckBox->isChecked());
        updateSettings("allow-nether",ui->netherCheckBox->isChecked());
        updateSettings("pvp",ui->pvpCheckBox->isChecked());
        updateSettings("online-mode",ui->onlineCheckBox->isChecked());


    //设置进程工作目录
    QDir::setCurrent(jarPath);
    qDebug()<<QDir::currentPath();

    proc = new QProcess(this);
    connect(proc, SIGNAL(readyReadStandardOutput()),this,SLOT(catchOutput()));

    QStringList arguments;
    arguments << QString("-Xms%1").arg(ui->minLabel->text()) << QString("-Xmx%1").arg(ui->maxLabel->text())<<"-Dfile.encoding=UTF-8"<<
                 "-jar" << jarName;
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start("java",arguments);

    ui->startButton->setEnabled(false);
    ui->JVMSettingGroupBox->setEnabled(false);
    ui->serverSettingGroupBox->setEnabled(false);
    ui->light_label->setStyleSheet("background-color:yellow");

    ui->generalTextBrowser->append(QString("[%1]:     正在启动服务端").arg(QTime::currentTime().toString()));


    if (!proc->waitForStarted())
    {
        qDebug()<< "启动失败\n";
    }
     qDebug()<<"ok";
}

void Widget::on_stopButton_clicked()
{
    proc->write("stop\n");
    updateTimer->stop();
    timer->stop();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->runningGroupBox->setEnabled(false);

    ui->runningTimeLabel->setText("未开始");
    ui->JVMProgressBar->setValue(0);
    ui->serverProgressBar->setValue(0);
    ui->jvmProgressLabel->setText("");
    ui->serverProgressLabel->setText("");
    ui->pluginLabel->setText("0");
    ui->onlineNumberLabel->setText("0");
    ui->versionLabel->setText("0");
    ui->light_label->setStyleSheet("background-color:red");
    ui->JVMSettingGroupBox->setEnabled(true);
    ui->serverSettingGroupBox->setEnabled(true);

    ui->generalTextBrowser->append(QString("[%1]:     服务端已停止").arg(QTime::currentTime().toString()));

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
//    proc->write("memory\n");
    GUIsettings->setValue("jarPath",jarPath);
    GUIsettings->setValue("jarName",jarName);
}

void Widget::on_pathButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("选择服务端的JAR文件");
    fileDialog->setNameFilter("JAR Files(*.jar)");
    if(fileDialog->exec() == QDialog::Accepted)
    {
         QString path = fileDialog->selectedFiles()[0];
         QRegExp rx;
         rx.setPatternSyntax(QRegExp::Wildcard);
         rx.setPattern(QString("*.jar"));
         if(rx.exactMatch(path))
         {
            ui->jarLineEdit->setText(path);
            QFileInfo fileInfo = QFileInfo(path);
            jarPath = fileInfo.absolutePath();
            jarName = fileInfo.fileName();
            GUIsettings->setValue("jarPath",jarPath);
            GUIsettings->setValue("jarName",jarName);

            loadServerSettings();

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





