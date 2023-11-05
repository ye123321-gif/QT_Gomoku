#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <qdebug.h>
#include <QMessageBox>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#define TIME_LIMIT 10
mainwidget::mainwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainwidget)
{
    ui->setupUi(this);
    currentRole = BLACK;
    timelimit = TIME_LIMIT;
    ui->lcdNumberBlack->display(timelimit);
    ui->lcdNumberWhite->display(timelimit);
    memset(gameBoard, EMPTY, sizeof(gameBoard));
    srand(time(NULL));

#if TESTING_FOR_PATTERN
    //gameBoard[7][7] = BLACK;
    //gameBoard[8][8] = BLACK;

    //gameBoard[10][10] = BLACK;
    //gameBoard[11][11] = BLACK;
#endif

    ui->chessBoard->setBoard(-1,-1,gameBoard);

    connect(ui->chessBoard, &ChessBoard::chessGameOver, this, stopExistingTimers);

    //Player clicks a position on the board
    connect(ui->chessBoard, &ChessBoard::pressedCoordinateXY, [&](int x, int y){
        if(myRole != currentRole && mode != TwoPlayer){
            return;
        }

        if(gameBoard[x][y] == EMPTY){
            gameBoard[x][y] = currentRole;
            ui->chessBoard->setBoard(x, y, gameBoard);
            if(mode == Online){
                emit onlineCoordinateXY(x, y);
            }
            if(bool win = ui->chessBoard->winCheck(gameBoard, currentRole)){
                return;
            }
            changeRole();
        }
    });


    connect(&BlackTimer, &QTimer::timeout, [&](){
        timelimit--;
        ui->lcdNumberBlack->display(timelimit);

        if(timelimit == 0){
            ui->chessBoard->setDirectWinner(WHITE);
            BlackTimer.stop();
        }

        if(mode == Machine && myRole != BLACK){
            machinePlay();
        }


    });
    connect(&WhiteTimer, &QTimer::timeout, [&](){
        timelimit--;
        ui->lcdNumberWhite->display(timelimit);

        if(timelimit == 0){
            ui->chessBoard->setDirectWinner(BLACK);
            WhiteTimer.stop();
        }

        if(mode == Machine && myRole != WHITE){
            machinePlay();
        }
    });
}

//allows play from local area network to display on the board
void mainwidget::networkPlay(int x, int y)
{
    if(gameBoard[x][y] == EMPTY){
        gameBoard[x][y] = currentRole;
        ui->chessBoard->setBoard(x, y, gameBoard);
        if(bool win = ui->chessBoard->winCheck(gameBoard, currentRole)){
            return;
        }
        changeRole();
    }
}

//Dummy Machine play
//Finds available move by observing patterns
static std::pair<int, int> findEmptyPosition(void* gameboard, const std::vector<std::vector<Pieces>>& patterns, int rule) {
    //rule: 0=>horizontal, 1=>vertical, 2: diagonal bottom right to top left, 3: diagonal bottom left to top right
    Pieces (*board)[15];
    board = (Pieces (*)[15])gameboard;
    for (int y = 0; y < 15; ++y) {
        for (int x = 0; x < 15; ++x) {
            for (const auto& pattern : patterns) {
                bool isPatternMatch = true;
                for (int j = 0; j < pattern.size(); ++j) {

                    if (rule == 0 && (x + j >= 15 || board[x + j][y] != pattern[j])) { //0: horizontal
                        isPatternMatch = false;
                        break;
                    }
                    else if(rule == 1 && (x + j >= 15 || board[y][x + j] != pattern[j])){ //1: vertical
                        isPatternMatch = false;
                        break;
                    }
                    else if(rule == 2 && (x + j >= 15 || y + j >= 15 || board[x + j][y + j] != pattern[j])){//2: diagonal bottom right to top left
                        isPatternMatch = false;
                        break;
                    }
                    else if(rule == 3 && (x - j <= 0 || y + j >= 15 || board[x - j][y + j] != pattern[j])){//3: diagonal bottom left to top right
                        isPatternMatch = false;
                        break;
                    }
                }
                if (isPatternMatch) {
                    // Find the position of "empty" in the matched pattern
                    for (int j = 0; j < pattern.size(); ++j) {
                        if (pattern[j] == EMPTY) {
                            if(rule == 0){
                                return std::make_pair(x+j, y);
                            }
                            else if(rule == 1){
                                return std::make_pair(y, x+j);
                            }
                            else if(rule == 2){
                                return std::make_pair(x + j, y + j);
                            }
                            else if(rule == 3){
                                return std::make_pair(x - j, y + j);
                            }
                        }
                    }
                }
            }
        }
    }
    return std::make_pair(-1, -1);  // Pattern not found, return an invalid pair
}

//Patterns for machine play
static std::vector<std::vector<Pieces>> getPattern(Pieces MachineRole){
    std::vector<std::vector<Pieces>> patterns;
    if(MachineRole == WHITE){
         patterns = {
                {EMPTY, BLACK, BLACK, BLACK, BLACK,WHITE},
                {WHITE, BLACK, BLACK, BLACK, BLACK,EMPTY},
                {BLACK, BLACK, EMPTY, BLACK, BLACK},
                {WHITE, WHITE, EMPTY, WHITE, WHITE},
                {EMPTY, BLACK, BLACK, BLACK, EMPTY},
                {EMPTY, WHITE, WHITE, WHITE, EMPTY},
                {BLACK,BLACK,EMPTY},
                //{EMPTY,BLACK,BLACK}, //this might be prioritized than the first one
                {BLACK,EMPTY},
                //{EMPTY,BLACK}
        };

    }
    else{
        patterns = {
                {EMPTY, WHITE, WHITE, WHITE, WHITE, BLACK},
                {BLACK, WHITE, WHITE, WHITE, WHITE, EMPTY},
                {WHITE, WHITE, EMPTY, WHITE, WHITE},
                {BLACK, BLACK, EMPTY, BLACK, BLACK},
                {EMPTY, WHITE, WHITE, WHITE, EMPTY},
                {EMPTY, BLACK, BLACK, BLACK, EMPTY},
                {WHITE,WHITE,EMPTY},
                //{EMPTY,WHITE,WHITE},
                {WHITE,EMPTY},
                //{EMPTY,WHITE}
        };
    }
    return patterns;
}

void mainwidget::machinePlay()
{
    int level = 1;
    //Since there is nested loops and it can block the event loop and prevent UI updates
    //Adding this line below allows we to update UI within the excution of the function
    QCoreApplication::processEvents();

    if(level == 1){
        /* random int between 0 and 3*/
        int randomness = rand() % 4;
        Pieces machineRole = (Pieces) (myRole % 2 + 1);
        std::vector<std::vector<Pieces>> patterns = getPattern(machineRole);
        std::pair<int, int> emptyPosition = findEmptyPosition(gameBoard, patterns, randomness);

        if (emptyPosition.first != -1) {
            //slowing down machine from moving too quickly
            sleep(1);
            int x =  emptyPosition.first;
            int y = emptyPosition.second;
            //qDebug() << "The empty position is at row " << x << " and column " << y << endl;
            gameBoard[x][y] = currentRole;
            ui->chessBoard->setBoard(x, y, gameBoard);
            if(bool win = ui->chessBoard->winCheck(gameBoard, currentRole)){
                return;
            }

        } else {
            //mimic the center move
            //this case can occur only if player choose white under machine mode
            gameBoard[GOMOKU_ROW / 2][GOMOKU_COL / 2] = currentRole;
            ui->chessBoard->setBoard(GOMOKU_ROW / 2, GOMOKU_COL / 2, gameBoard);
        }
        changeRole();
    }
    else if(level == 2){
        /* Working on it */
    }
}

void mainwidget::stopExistingTimers()
{
    if(BlackTimer.isActive()){
        BlackTimer.stop();
    }
    if(WhiteTimer.isActive()){
        WhiteTimer.stop();
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

void mainwidget::closeEvent(QCloseEvent *)
{
    stopExistingTimers();
    emit this->closeMainWidget();
}

void mainwidget::changeRole()
{
    currentRole = (Pieces) (currentRole % 2 + 1);
    timelimit = TIME_LIMIT;
    if(currentRole == BLACK){
        WhiteTimer.stop();
        ui->lcdNumberWhite->display(timelimit);
        startBlackTimer(1000);
    }
    else{
        BlackTimer.stop();
        ui->lcdNumberBlack->display(timelimit);
        startWhiteTimer(1000);
    }
}


void mainwidget::startBlackTimer(int ms)
{
    BlackTimer.start(ms);
}

void mainwidget::startWhiteTimer(int ms)
{
    WhiteTimer.start(ms);
}
