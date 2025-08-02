#include "deepvik.h"
#include "ui_deepvik.h"

deepvik::deepvik(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::deepvik)
{
    ui->setupUi(this);
    for (int i = 0; i < 32; ++i) {
        QString name = QString("tile%1").arg(i);
        tiles[i] = findChild<QPushButton*>(name);
        connect(tiles[i], &QPushButton::clicked, this, &deepvik::handleTileClicked);
        tiles[i] -> setIcon(QIcon(QString(":/img/B.png")));
        tiles[i] -> setIconSize(QSize(80, 80));

        name = QString("whiteTile%1").arg(i + 1);
        QPushButton *whiteTile = findChild<QPushButton*>(name);
        whiteTile -> setIcon(QIcon(QString(":/img/W.png")));
        whiteTile -> setIconSize(QSize(80, 80));
    }
    Freeze = true;
    gameType = 0;
    userInput = "";
}

deepvik::~deepvik()
{
    delete ui;
}
void wait(int milliseconds) {
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec();
}
string translateTile(int tile){
    string res = "";
    res += ('A' + ((tile & 3) << 1) + ((tile >> 2) & 1));
    res += ('1' + (tile >> 2));
    return res;
}
int numToTile(string x){
    int res = ((x[1] - '1') * 4) + ((x[0] - 'A') >> 1);
    qDebug() << x[0] << " " << x[1] <<"\n";
    return res;
}
void deepvik::handleTileClicked()
{
    if(Freeze) return;
    QPushButton* clicked = qobject_cast<QPushButton*>(sender());
    if (!clicked) return;
    Freeze = true;
    int idx = 0;
    for (int i = 0; i < 32; i++) {
        if (tiles[i] == clicked) {
            idx = i;
            userInput += translateTile(i);
            break;
        }
    }
    if(legalMoves.empty()) legalMoves = generateMoves(gameState, false);
    qDebug() << userInput << "\n";
    bool correctFullMove = false;
    for(auto v : legalMoves){
        if(userInput == v){
            correctFullMove = true;
            break;
        }
    }
    if(correctFullMove){
        make_move_from_string(gameState, userInput.substr(userInput.size() - 4, 4));
        legalMoves.clear();
        userInput = "";
        refresh();
        string myBotAnswer = BestMove(gameState, true);
        for(int i = 0; i < (int)myBotAnswer.size() - 2; i += 2){
            make_move_from_string(gameState, myBotAnswer.substr(i, 4));
            wait(250);
            refresh();
            int nr = numToTile(myBotAnswer.substr(i + 2, 2));
            tiles[nr] -> setStyleSheet("border: 3px solid blue;");
            tiles[nr] -> setIconSize(QSize(77, 77));
        }
    }
    else{
        bool prefix = false;
        for(auto v : legalMoves){
            if(userInput == v.substr(0, userInput.size())){
                prefix = true;
                break;
            }
        }
        if(prefix){
            if(userInput.size() >= 4){
                make_move_from_string(gameState, userInput.substr(userInput.size() - 4, 4));
                refresh();
            }
            tiles[idx]->setStyleSheet("border: 3px solid yellow;");
            tiles[idx] -> setIconSize(QSize(77, 77));
        }
        else{
            if(userInput.size() >= 6) userInput.resize(userInput.size() - 2);
            else userInput = "";
            refresh();
            QMessageBox::about(this, "ERROR", "Incorrect move!");
        }
    }


    Freeze = false;
}

void deepvik::on_newGameButton_clicked()
{
    gameState.reset();
    refresh();
    Freeze = false;
    legalMoves.clear();
    userInput = "";
}
array<int, 32> getBoard(const Game &curr){
    array<int, 32> res;
    for(uint32_t i = 0; i < 32; i++){
        if(curr.black_pawn & (1 << i)) res[i] = 1;
        else if(curr.red_pawn & (1 << i)) res[i] = 2;
        else if(curr.black_king & (1 << i)) res[i] = 3;
        else if(curr.red_king & (1 << i)) res[i] = 4;
        else res[i] = 0;
    }
    return res;
}
void deepvik::refresh(){
    array<int, 32> board = getBoard(gameState);
    for(int i = 0; i < 32; i++){
        if(board[i] == 0) tiles[i] -> setIcon(QIcon(QString(":/img/B.png")));
        else if(board[i] == 1) tiles[i] -> setIcon(QIcon(QString(":/img/piece_W.png")));
        else if(board[i] == 2) tiles[i] -> setIcon(QIcon(QString(":/img/piece_B.png")));
        else if(board[i] == 3) tiles[i] -> setIcon(QIcon(QString(":/img/king_W.png")));
        else tiles[i] -> setIcon(QIcon(QString(":/img/king_B.png")));
        tiles[i] -> setIconSize(QSize(80, 80));
    }
}

