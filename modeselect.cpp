#include "modeselect.h"
#include "ui_modeselect.h"
#include <qdebug.h>
#include <QMessageBox>
#include <iostream>
using namespace std;
modeSelect::modeSelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::modeSelect)
{
    ui->setupUi(this);
    initUdpSocket(8888);
    QPixmap backgroundImage(":/image/background.jpg");
    ui->background->setPixmap(backgroundImage);
    ui->background->setScaledContents(true);

    connect(&mw, &mainwidget::onlineCoordinateXY, [&](int x, int y){
        //qDebug() << "play" << endl;
        //"cmd=play&x=1&y=1"
        QString str = QString("cmd=play&x=%1&y=%2").arg(x).arg(y);

        QString ip = ui->lineEdit_ip->text();
        short port = ui->lineEdit_port->text().toShort();

        //sending move info
        udpsocket->writeDatagram(str.toUtf8(), QHostAddress(ip), port);
    });
}

void modeSelect::initUdpSocket(short port)
{

    udpsocket = new QUdpSocket(this);  //Store in the children list of modeSelect class
                                 //When freeing the modeSelect Class object, it will call the destructors for the udpsocket
    if(udpsocket == NULL)
    {
        cout << "new QUdpSocket err";
        return;
    }

    //binding ports and make it reusable
    if( false == udpsocket->bind(port, QUdpSocket::ShareAddress) )
    {
        cout << "bind err";
        return;
    }


    //Receiving udp packets
    connect(udpsocket, &QUdpSocket::readyRead,
            [&](){
                char buf[1500] = {0};
                QHostAddress addr;
                quint16 port;
                udpsocket->readDatagram(buf, sizeof(buf), &addr, &port); //接收数据
                //qDebug() << "buf: " << buf << endl;
                ui->lineEdit_ip->setText(addr.toString());
                ui->lineEdit_port->setText(QString::number(port));

                qDebug() << "buf = " << buf << endl;
                interpretUdpPacket(buf);//handle udp packets
            }
    );
}

void modeSelect::interpretUdpPacket(char *buf)
{
    //Connection request          "cmd=connect&role=black"
    //Connection has established      "cmd=ok"
    //play moves            "cmd=play&x=1&y=1"
    QString str = buf;

    QString tmp = str.section("&", 0, 0);
    QString cmd = tmp.section("=", 1, 1);
    //qDebug() << "cmd = " << cmd;

    if(cmd == "connect")
    { //connection request

        tmp = str.section("&", 1, 1); // "role=black"
        str = tmp.section("=", 1, 1); //"black"

        QString ip = ui->lineEdit_ip->text();
        short port = ui->lineEdit_port->text().toShort();
        QString s;
        if(str == "black")
        {
            s = QString("[%1:%2]Request for a Gomoku game，opponent selects black piece").arg(ip).arg(port);
            ui->radioButton_white->setChecked(true);
            mw.setMyRole(WHITE);
        }
        else
        {
            s = QString("[%1:%2]Request for a Gomoku game，opponent selects white piece").arg(ip).arg(port);
            ui->radioButton_black->setChecked(true);
            mw.setMyRole(BLACK);
        }

        int ret = QMessageBox::question(this, "Request for a Gomoku game", s, QMessageBox::Yes | QMessageBox::No);
        if(ret == QMessageBox::Yes){
            mw.setMode(Online);
            this->hide();
            mw.show();
            //inform other play that connection has established
            udpsocket->writeDatagram("cmd=ok", strlen("cmd=ok"), QHostAddress(ip), port);
        }
    }
    else if(cmd == "ok")
    {//Player has established connection
        QMessageBox::information(this, "Request for a Gomoku game", "Other player has connected");
        //qDebug() << "ok" << endl;
        this->hide();

        if(ui->radioButton_black->isChecked())
        {//set role only after connection has been established
            mw.setMyRole(BLACK);
        }
        else if(ui->radioButton_white->isChecked())
        {
             mw.setMyRole(WHITE);
        }
        this->hide();
        mw.show();
    }
    else if(cmd == "play")
    {//play move, "cmd=play&x=1&y=1"
        tmp = str.section("&", 1, 1); //"x=1"
        int x = tmp.section("=", 1, 1).toInt();

        tmp = str.section("&", 2, 2); //"y=1";
        int y = tmp.section("=", 1, 1).toInt();

        mw.networkPlay(x, y);
    }

    if(udpsocket->hasPendingDatagrams()){
        qDebug() << "error" << endl;
    }
}

modeSelect::~modeSelect()
{
    delete ui;
}

void modeSelect::on_Exit_clicked()
{
    this->close();
}

void modeSelect::on_TwoPlayer_clicked()
{
    mw.show();
    this->hide();
}

void modeSelect::on_Machine_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->PageSelect);
    mw.setMode(Machine);
}

void modeSelect::on_Online_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageOnlineInfo);
    mw.setMode(Online);
}

void modeSelect::on_pushButton_submit_clicked()
{

    QString ip = ui->lineEdit_ip->text(); //Get player ip
    short port = ui->lineEdit_port->text().toShort(); //get player port

    //"cmd=connect&role=black"
    QString str;
    if(ui->radioButton_black->isChecked())
    {//Player sending connection selects black
        str = "cmd=connect&role=black";
        //mw.setMyRole(BLACK);
    }
    else if(ui->radioButton_white->isChecked())
    {//selects white
        str = "cmd=connect&role=white";
        //mw.setMyRole(WHITE);
    }
    //qDebug() << str << endl;
    //Sending connection request
    udpsocket->writeDatagram(str.toUtf8(), QHostAddress(ip), port);
}

void modeSelect::on_pushButton_clear_clicked()
{
    ui->lineEdit_ip->clear();
    ui->lineEdit_port->clear();
}

void modeSelect::on_pushButton_Black_clicked()
{
    mw.setMyRole(BLACK);
    mw.show();
    this->hide();
}

void modeSelect::on_pushButton_White_clicked()
{
    mw.setMyRole(WHITE);
    mw.show();
    this->hide();
}
