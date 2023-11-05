#ifndef MODESELECT_H
#define MODESELECT_H

#include <QWidget>
#include "mainwidget.h"
#include <QUdpSocket>
namespace Ui {
class modeSelect;
}

class modeSelect : public QWidget
{
    Q_OBJECT

public:
    explicit modeSelect(QWidget *parent = 0);
    void initUdpSocket(short port); //binding port to current host ip
    void interpretUdpPacket(char *buf);
    ~modeSelect();

private slots:
    void on_Exit_clicked();

    void on_TwoPlayer_clicked();

    void on_Machine_clicked();

    void on_Online_clicked();

    void on_pushButton_submit_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_Black_clicked();

    void on_pushButton_White_clicked();

private:
    Ui::modeSelect *ui;
    mainwidget mw;
    QUdpSocket *udpsocket;
};

#endif // MODESELECT_H
