#include "chessboard.h"
#include <QPainter>
#include <iostream>
#include <QMouseEvent>
#include <qdebug.h>

using namespace std;
static int bwidth = 0 ;
static int bheight = 0;
ChessBoard::ChessBoard(QWidget *parent) : QWidget(parent)
{
    curX = -1;
    curY = -1;
    chessBoard = NULL;
}

bool ChessBoard::winCheck(void *ChessBoard, Pieces curRole)
{
    Pieces (*chessBoard)[15];
    chessBoard = (Pieces (*)[15])this->chessBoard;
    //qDebug() << "here " << curRole << endl;

    int check = 0; //In gomoku, we only need to ensure there are 5 pieces with same color connected together
    //horizontal & vertical check
    int left = curX;
    int right = curX;
    int up = curY;
    int down = curY;



    int diagLeft = curX;
    int diagRight = curX;
    int tempYLeft = curY;
    int tempYRight = curY;

    int diagLeftBtoTright = curX;
    int diagRightBtoTright = curX;
    int tempYLeftBtoTright = curY;
    int tempYRightBtoTright = curY;
    while(check < 5){
        //horizontal
        if(left >=0 && chessBoard[left][curY] == curRole){
            left--;
        }
        if(right <= 14 && chessBoard[right][curY] == curRole){
            right++;
        }
        //vertical
        if(up >= 0 && chessBoard[curX][up] == curRole){
            up--;
        }
        if(down <= 14 && chessBoard[curX][down] == curRole){
            down++;
        }
        //diagonal bottom right to top left
        if(diagLeft >=0 && tempYLeft >= 0 && chessBoard[diagLeft][tempYLeft] == curRole){
            diagLeft--;
            tempYLeft--;
        }
        if(diagRight <=14 && tempYRight <= 14 &&chessBoard[diagRight][tempYRight] == curRole){
            diagRight++;
            tempYRight++;
        }

        //diagonal bottom left to top right
        if(diagLeftBtoTright >=0 && tempYLeftBtoTright <= 14 && chessBoard[diagLeftBtoTright][tempYLeftBtoTright] == curRole){
            diagLeftBtoTright--;
            tempYLeftBtoTright++;
        }
        if(diagRightBtoTright <= 14 && tempYRightBtoTright >= 0 && chessBoard[diagRightBtoTright][tempYRightBtoTright] == curRole){
            diagRightBtoTright++;
            tempYRightBtoTright--;
        }
        //qDebug() << left << ":" << right << "-" << up << ":" << down << "-" << diagLeft << ":" << diagRight<< endl;
        check++;
    }
    if(right - left - 1 >= 5){ // -1 because above process counted current piece twice
        return true;
    }
    else if(down - up - 1 >= 5){
        return true;
    }
    else if(diagRight - diagLeft - 1 >= 5){
        return true;
    }
    else if(diagRightBtoTright - diagLeftBtoTright - 1 >= 5){
        return true;
    }

    return false;
    //vertical
    //diagonal
}

void ChessBoard::setBoard(int curX, int curY, void *chessBoard)
{
    this->curX = curX;
    this->curY = curY;
    this->chessBoard = chessBoard;
    update();
}

void ChessBoard::paintEvent(QPaintEvent *)
{
    spaceX = this->width() / (GOMOKU_ROW + 1); //add 1 here to allow the pieces being fully draw to the board
    spaceY = this->height() / (GOMOKU_COL + 1);
    QPainter p(this);

    int lineWidth = 1;

    QPen pen;
    pen.setWidth(lineWidth);
    p.setPen(pen);

#if 1
    for(int i = 0; i < 15; i++)
    {
        //row line
        p.drawLine(spaceX, spaceY+i*spaceY, spaceX+(GOMOKU_ROW - 1)*spaceX, spaceY+i*spaceY); //GOMOKU_ROW - 1 => There are 15 lines and 14 spaces among them
        //column line
        p.drawLine(spaceX + i * spaceX, spaceY , spaceX + i * spaceX, spaceY+(GOMOKU_COL - 1)*spaceY);
    }
#endif

    Pieces (*chessBoard)[15];
    chessBoard = (Pieces (*)[15])this->chessBoard;

    pen.setWidth(5);
    p.setPen(pen);
    p.drawPoint(spaceX + 7 * spaceX, spaceY + 7 * spaceY); //Center point
   // p.drawLine(0, 0, width(), 0);
    //p.drawLine(spaceX, spaceY+0*spaceY, spaceX+15*spaceX, spaceY+0*spaceY);
    if(chessBoard == NULL){
        cout << "error" << endl;
        return;
    }

    QPixmap blackPiece(":/image/black.png");
    blackPiece = blackPiece.scaled(spaceX, spaceY, Qt::IgnoreAspectRatio);
    int blackPieceWidth = blackPiece.width() / 2;
    int blackPieceHeight = blackPiece.height() / 2;

    bwidth = blackPieceWidth;
    bheight = blackPieceHeight;
#if 1
    QPixmap whitePiece(":/image/white.png");
    whitePiece = whitePiece.scaled(spaceX, spaceY, Qt::IgnoreAspectRatio);
    int whitePieceWidth = whitePiece.width() / 2;
    int whitePieceHeight = whitePiece.height() / 2;

    for(int row = 0; row < 15; row++){
        for(int col = 0; col < 15; col++){
            if(chessBoard[row][col] == BLACK){
                p.drawPixmap(spaceX + row * spaceX - blackPieceWidth, spaceY + col * spaceY - blackPieceHeight, blackPiece);
            }
            if(chessBoard[row][col] == WHITE){
                p.drawPixmap(spaceX + row * spaceX - whitePieceWidth, spaceY + col * spaceY - whitePieceHeight, whitePiece);
            }
        }
    }
#endif

    pen.setWidth(1);
    pen.setColor(Qt::green);
    p.setPen(pen);
    //p.drawRect(spaceX*15 - (blackPiece.width() / 2),spaceY - (blackPiece.height() / 2),
    //                 blackPiece.width(),blackPiece.height());
#if EXAMPLE
    cout << spaceX << ":" << spaceY << " " << spaceX - (blackPiece.width() / 2) << ":" << spaceY - (blackPiece.height() / 2) << endl;
    p.drawPixmap(spaceX + spaceX - (blackPiece.width() / 2), spaceY + spaceY - (blackPiece.height() / 2), blackPiece);
    p.drawPixmap(spaceX - (blackPiece.width() / 2), spaceY + spaceY - (blackPiece.height() / 2), blackPiece);

    p.drawPixmap(spaceX + 14 * spaceX - (blackPiece.width() / 2), spaceY - (blackPiece.height() / 2), blackPiece);
    pen.setWidth(1);
    pen.setColor(Qt::green);
    p.setPen(pen);
    p.drawRect(spaceX - (blackPiece.width() / 2),spaceY + spaceY - (blackPiece.height() / 2),
                     blackPiece.width(),blackPiece.height());
#endif

    if(curX != -1 && curY != -1){
        p.drawRect(spaceX + spaceX * curX - bwidth,spaceY + spaceY * curY- bheight, blackPiece.width(),blackPiece.height()); // curX or curY can be 0
    }
}

void ChessBoard::mousePressEvent(QMouseEvent *e)
{
    //qDebug() << spaceX << ":" << spaceY << endl;
    //qDebug() << spaceX * 15 << ":" << spaceY * 15 << endl;
    if(e->button() == Qt::LeftButton){
        //qDebug() << "Left Mouse pressed" << e->x() <<"," << e->y()<< endl;

        //qDebug() << spaceX * 15 + bwidth << " " << spaceX * 15 - bwidth << " " << spaceY + bheight << spaceY - bheight << endl;
#if 0
        if(e->x() <= spaceX * 15 + bwidth &&  e->x() >= spaceX * 15 - bwidth
                && e->y() <= spaceY + bheight && e->y() >= spaceY - bheight ){
            a = 1;
            update();
#endif  }
        if(e->x() >= spaceX - bwidth && e->x() <= spaceX * 15 + bwidth
                && e->y() >= spaceY - bheight && e->y() <= spaceY * 15 + bheight)
        {
            //qDebug() << "available" << endl;
            qDebug() << "here: " << (e->x() - bwidth) / spaceX << " " << (e->y() - bheight) / spaceY << endl; // converts mouse pos to coordinates on the chessBoard
            emit(pressedCoordinateXY((e->x() - bwidth) / spaceX , (e->y() - bheight) / spaceY));
        }

    }
}
