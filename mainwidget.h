#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QTimer>
#include "chessboard.h"
namespace Ui {
class mainwidget;
}
enum PlayMode{TwoPlayer, Machine, Online};
class mainwidget : public QWidget
{
    Q_OBJECT

public:
    explicit mainwidget(QWidget *parent = 0);
    void setMode(PlayMode mode);
    void setMyRole(Pieces role);
    void networkPlay(int x, int y);
    void machinePlay();
    void stopExistingTimers();
    void clearBoard();
    ~mainwidget();
signals:
    void closeMainWidget();
    void onlineCoordinateXY(int x, int y);
public slots:
   void closeEvent(QCloseEvent *);
   void changeRole();
   void startBlackTimer(int ms);
   void startWhiteTimer(int ms);
private:
    Pieces gameBoard[15][15];
    Ui::mainwidget *ui;
    Pieces currentRole;
    Pieces myRole;
    QTimer BlackTimer;
    QTimer WhiteTimer;
    int timelimit; //Total allowable time limit to draw for each player
    PlayMode mode;
};

#endif // MAINWIDGET_H
