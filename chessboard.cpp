#include "chessboard.h"
#include <QPainter>
#include <iostream>
#include <QMouseEvent>
#include <qdebug.h>
#include <QMessageBox>
using namespace std;
static int bwidth = 0 ; //piece width
static int bheight = 0; //piece height
ChessBoard::ChessBoard(QWidget *parent) : QWidget(parent)
{
    curX = -1;
    curY = -1;
    chessBoard = NULL;
}

bool ChessBoard::winCheck(void *ChessBoard, Pieces curRole)
{
    Pieces (*chessBoard)[15];
    chessBoard = (Pieces (*)[15])ChessBoard;
    //In gomoku, we only need to ensure there are 5 pieces with same color connected together
    int check = 0;

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
    bool win = false;

    //checking if on a row/column/diagonal there is 5 same colour consecutive pieces
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
        check++;
    }
    if((right - left - 1 >= 5) ||
            (down - up - 1 >= 5) ||
            (diagRight - diagLeft - 1 >= 5) ||
            (diagRightBtoTright - diagLeftBtoTright - 1 >= 5)){ // -1 because above process counted current piece twice
        win = true;
    }

    if(win){
        QString str = "";
        switch (curRole) {
        case WHITE:
            str = "Player with white piece wins";
            break;
        case BLACK:
            str = "Player with black piece wins";
            break;
        }
        emit chessGameOver();
        QMessageBox::information(this, "Congratulations!", str);
    }
    gameOver = win;
    return win;
}

void ChessBoard::setDirectWinner(Pieces curRole)
{
    QString str = "";
    switch (curRole) {
    case WHITE:
        str = "Player with white piece wins";
        break;
    case BLACK:
        str = "Player with black piece wins";
        break;
    }
    QMessageBox::information(this, "Congratulations!", str);
    gameOver = true;
}

void ChessBoard::setBoard(int curX, int curY, void *chessBoard)
{
    this->curX = curX;
    this->curY = curY;
    this->chessBoard = chessBoard;
    update();
}

/* This function draws the chess board
 * It's called whenever there's update to the chess board
 */
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

    //Mark the previous move
    pen.setWidth(1);
    pen.setColor(Qt::green);
    p.setPen(pen);

    if(curX != -1 && curY != -1){
        p.drawRect(spaceX + spaceX * curX - bwidth,spaceY + spaceY * curY- bheight, blackPiece.width(),blackPiece.height()); // curX or curY can be 0
    }
}

void ChessBoard::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        //Shouldn't response to user mouse movement when the game is over
        if(gameOver == true){
            return;
        }

        if(e->x() >= spaceX - bwidth && e->x() <= spaceX * 15 + bwidth
                && e->y() >= spaceY - bheight && e->y() <= spaceY * 15 + bheight)
        {
            // converts mouse pos to coordinates on the chessBoard
            emit(pressedCoordinateXY((e->x() - bwidth) / spaceX , (e->y() - bheight) / spaceY));
        }

    }
}
