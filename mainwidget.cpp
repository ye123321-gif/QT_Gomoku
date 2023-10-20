#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <qdebug.h>
#include <QMessageBox>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
mainwidget::mainwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainwidget)
{
    ui->setupUi(this);
    currentRole = BLACK;
    memset(gameBoard, EMPTY, sizeof(gameBoard));
    srand(time(NULL));
    //gameBoard[1][1] = BLACK;
    //gameBoard[14][0] = BLACK;
    ui->chessBoard->setBoard(-1,-1,gameBoard);

    connect(ui->chessBoard, &ChessBoard::pressedCoordinateXY, [&](int x, int y){
        if(myRole != currentRole){
            return;
        }

        if(gameBoard[x][y] == EMPTY){
            gameBoard[x][y] = currentRole;
            ui->chessBoard->setBoard(x, y, gameBoard);
            if(mode == Online){
                //qDebug() << " hello" << endl;
                emit onlineCoordinateXY(x, y);
            }
            bool win = ui->chessBoard->winCheck(gameBoard, currentRole);
            if(win){
                QString str = "";
                switch (currentRole) {
                case WHITE:
                    str = "Player with white piece wins";
                    break;
                case BLACK:
                    str = "Player with black piece wins";
                    break;
                }
                QMessageBox::information(this, "Congratulations2!", str);
            }
            changeRole();
            //qDebug() << currentRole << endl;
        }
        if(mode == Machine){
            machineTimer.start(1000);
        }

    });

    connect(&machineTimer, &QTimer::timeout,
    [&]()
    {
        machinePlay();
    });
}

void mainwidget::networkPlay(int x, int y)
{
    if(gameBoard[x][y] == EMPTY){
        gameBoard[x][y] = currentRole;
        ui->chessBoard->setBoard(x, y, gameBoard);
        bool win = ui->chessBoard->winCheck(gameBoard, currentRole);
        qDebug() << "cur: "<< currentRole << endl;
        if(win){
            QString str = "";
            switch (currentRole) {
            case WHITE:
                str = "Player with white piece wins";
                break;
            case BLACK:
                str = "Player with black piece wins";
                break;
            }
            QMessageBox::information(this, "Congratulations!", str);
        }
        changeRole();
        //qDebug() << currentRole << endl;
    }
}

std::pair<int, int> findEmptyPosition(void* gameboard, const std::vector<std::vector<Pieces>>& patterns, int rule) { //rule: 0=>horizontal, 1=>vertical, 2: diagonal bottom right to top left, 3: //diagonal bottom left to top right
    Pieces (*board)[15];
    board = (Pieces (*)[15])gameboard;
    for (int y = 0; y < 15; ++y) {
        for (int x = 0; x < 15; ++x) {
            for (const auto& pattern : patterns) {
                bool isPatternMatch = true;
                for (int j = 0; j < pattern.size(); ++j) {

                    if (rule == 0 && (x + j >= 15 || board[x + j][y] != pattern[j])) {
                        qDebug() << x+j << " : " << y << endl;
                        isPatternMatch = false;
                        break;
                    }
                    else if(rule == 1 && (x + j >= 15 || board[y][x + j] != pattern[j])){
                        qDebug() << y << " : " << x + j << endl;
                        isPatternMatch = false;
                        break;
                    }
                }
                if (isPatternMatch) {
                    // Find the position of "empty" in the matched pattern
                    qDebug() << pattern.size() << endl;
                    for (int j = 0; j < pattern.size(); ++j) {
                        if (pattern[j] == EMPTY) {
                            if(rule == 0){
                                return std::make_pair(x+j, y);
                            }
                            else if(rule == 1){
                                return std::make_pair(y, x+j);
                            }
                        }
                    }
                }
            }
        }
    }
    return std::make_pair(-1, -1);  // Pattern not found, return an invalid pair
}
std::vector<std::vector<Pieces>> getPattern(Pieces MachineRole){
    std::vector<std::vector<Pieces>> patterns;
    if(MachineRole == WHITE){
         patterns = {
                {BLACK, BLACK, EMPTY, BLACK, BLACK},
                {WHITE, WHITE, EMPTY, WHITE, WHITE},
                {EMPTY, BLACK, BLACK, BLACK, EMPTY},
                {EMPTY, WHITE, WHITE, WHITE, EMPTY},
                {BLACK,BLACK,EMPTY},
                {EMPTY,BLACK,BLACK},
                {BLACK,EMPTY},
                {EMPTY,BLACK}
        };

    }
    else{
        patterns = {
                {WHITE, WHITE, EMPTY, WHITE, WHITE},
                {BLACK, BLACK, EMPTY, BLACK, BLACK},
                {EMPTY, WHITE, WHITE, WHITE, EMPTY},
                {EMPTY, BLACK, BLACK, BLACK, EMPTY},
                {WHITE,WHITE,EMPTY},
                {EMPTY,WHITE,WHITE},
                {WHITE,EMPTY},
                {EMPTY,WHITE}
        };
    }
    return patterns;
}

void mainwidget::machinePlay()
{
    int level = 2;

    machineTimer.stop();
    if(level == 2){
        /* random int between 0 and 3*/
        //int randomness = rand() % 4;
        int randomness = 1;
        Pieces machineRole = (Pieces) (myRole % 2 + 1);
        std::vector<std::vector<Pieces>> patterns = getPattern(machineRole);
        std::pair<int, int> emptyPosition = findEmptyPosition(gameBoard, patterns, randomness);

        if (emptyPosition.first != -1) {
            sleep(1);
            int x =  emptyPosition.first;
            int y = emptyPosition.second;
            qDebug() << "The empty position is at row " << x << " and column " << y << endl;
            gameBoard[x][y] = currentRole;
            ui->chessBoard->setBoard(x, y, gameBoard);
            changeRole();
        } else {
            qDebug() << "Pattern not found in the board." << endl;
        }
    }
}

void mainwidget::setMode(PlayMode mode)
{
    this->mode = mode;
}

void mainwidget::setMyRole(Pieces role)
{
    this->myRole = role;
}

mainwidget::~mainwidget()
{
    delete ui;
}

void mainwidget::changeRole()
{
    currentRole = (Pieces) (currentRole % 2 + 1);
}
