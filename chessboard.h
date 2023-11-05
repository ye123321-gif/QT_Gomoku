#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#define GOMOKU_ROW 15
#define GOMOKU_COL 15
#include <QWidget>
enum Pieces{EMPTY, WHITE, BLACK};
class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent = 0);
    void setBoard(int curX = -1, int curY = -1, void *ChessBoard = NULL);
    bool winCheck(void *ChessBoard, Pieces curRole);
    void setDirectWinner(Pieces curRole);
protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;
signals:
    void pressedCoordinateXY(int x, int y);
    void chessGameOver();
public slots:

private:
    int spaceX; //spaces among axisX
    int spaceY; //spaces among axisY

    int curX; //current piece pos on axisX
    int curY; //current piece pos on axisY
    bool gameOver = false; //true if winner occur
    void* chessBoard;
};

#endif // CHESSBOARD_H
