#include "deepvik.h"
#include "ui_deepvik.h"

deepvik::deepvik(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::deepvik)
{
    ui->setupUi(this);
    for (int i = 0; i < 32; ++i) {
        QString name = QString("blackTile%1").arg(i);
        blackTiles[i] = findChild<QPushButton*>(name);
        connect(blackTiles[i], &QPushButton::clicked, this, &deepvik::handleTileClicked);
        blackTiles[i] -> setIcon(QIcon(QString(":/img/B.png")));
        blackTiles[i] -> setIconSize(QSize(80, 80));

        name = QString("whiteTile%1").arg(i);
        whiteTiles[i] = findChild<QPushButton*>(name);
        whiteTiles[i] -> setIcon(QIcon(QString(":/img/W.png")));
        whiteTiles[i] -> setIconSize(QSize(80, 80));
    }
    Freeze = true;
    COLOR = false;
    gameType = 0;
    userInput = "";
    ui -> surrenderButton -> setVisible(false);
    ui -> newGameButton -> setCursor(Qt::PointingHandCursor);
    ui -> surrenderButton -> setCursor(Qt::PointingHandCursor);
    ui -> rotateButton -> setCursor(Qt::PointingHandCursor);
    ui -> selectColorButton -> setCursor(Qt::PointingHandCursor);

    ui -> evalBar -> setOrientation(Qt::Vertical);
    ui->evalBar->setRange(0, 600);
    ui->evalBar->setValue(300);
    ui->evalBar->setAlignment(Qt::AlignCenter);
    INVERTED = false;
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
    return res;
}
void deepvik::changeCursor(bool type){
    for(uint32_t i = 0; i < 32; i++){
        if(type == 0) blackTiles[i] -> setCursor(Qt::ArrowCursor);
        else blackTiles[i] -> setCursor(Qt::PointingHandCursor);
    }
}
void deepvik::handleTileClicked()
{
    if(Freeze) return;
    QPushButton* clicked = qobject_cast<QPushButton*>(sender());
    if (!clicked) return;
    Freeze = true;
    changeCursor(false);
    int idx = 0;
    for (int i = 0; i < 32; i++) {
        if (blackTiles[i] == clicked) {
            idx = i;
            userInput += translateTile(i);
            break;
        }
    }
    if(legalMoves.empty()) legalMoves = generateMoves(gameState, COLOR);
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
        if(generateMoves(gameState, !COLOR).empty()){
            QMessageBox::about(this, "END", "You won!");
            endGame();
            return;
        }
        auto [evalAnswer, myBotAnswer] = BestMove(gameState, DIFFICULTY, !COLOR);
        ui -> evalBar -> setValue(clamp(evalAnswer * 3 + 300, 0, 600));
        for(int i = 0; i < (int)myBotAnswer.size() - 2; i += 2){
            make_move_from_string(gameState, myBotAnswer.substr(i, 4));
            wait(250);
            refresh();
            int nr = numToTile(myBotAnswer.substr(i + 2, 2));
            blackTiles[nr] -> setStyleSheet("border: 3px solid blue;");
            blackTiles[nr] -> setIconSize(QSize(77, 77));
        }
        if(generateMoves(gameState, COLOR).empty()){
            QMessageBox::about(this, "END", "You lost!");
            endGame();
            return;
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
            blackTiles[idx]->setStyleSheet("border: 3px solid yellow;");
            blackTiles[idx] -> setIconSize(QSize(77, 77));
        }
        else{
            if(userInput.size() >= 6) userInput.resize(userInput.size() - 2);
            else{
                userInput = "";
                refresh();
            }
            QMessageBox::about(this, "ERROR", "Incorrect move!");
        }
    }


    Freeze = false;
    changeCursor(true);
}

void deepvik::on_newGameButton_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Select depth");

    QVBoxLayout layout(&dialog);

    QSlider slider(Qt::Horizontal);
    slider.setRange(1, 11);
    slider.setValue(1);

    QLabel label("Depth: 1");
    label.setAlignment(Qt::AlignCenter);

    layout.addWidget(&slider);
    layout.addWidget(&label);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout.addWidget(&buttonBox);

    QObject::connect(&slider, &QSlider::valueChanged, [&label](int v){
        label.setText(v == 11 ? "Depth: Unlimited" : QString("Depth: %1").arg(v));
    });
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if(dialog.exec() != QDialog::Accepted) {
        return;
    }
    DIFFICULTY = slider.value();
    if(DIFFICULTY == 11) DIFFICULTY = 60;
    ui -> depthLabel -> setText(QString("DEPTH: %1").arg(DIFFICULTY));
    gameState.reset();
    refresh();
    ui->evalBar->setValue(300);
    legalMoves.clear();
    userInput = "";

    if(COLOR){
        auto [evalAnswer, myBotAnswer] = BestMove(gameState, DIFFICULTY, false);
        ui -> evalBar -> setValue(clamp(evalAnswer * 3 + 300, 0, 600));
        for(int i = 0; i < (int)myBotAnswer.size() - 2; i += 2){
            make_move_from_string(gameState, myBotAnswer.substr(i, 4));
            wait(250);
            refresh();
            int nr = numToTile(myBotAnswer.substr(i + 2, 2));
            blackTiles[nr] -> setStyleSheet("border: 3px solid blue;");
            blackTiles[nr] -> setIconSize(QSize(77, 77));
        }
        if(generateMoves(gameState, true).empty()){
            QMessageBox::about(this, "END", "You lost!");
            Freeze = true;
            return;
        }
    }

    changeCursor(true);
    ui -> newGameButton -> setVisible(false);
    ui -> selectColorButton -> setVisible(false);
    ui -> surrenderButton -> setVisible(true);
    Freeze = false;
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
        if(board[i] == 0) blackTiles[i] -> setIcon(QIcon(QString(":/img/B.png")));
        else if(board[i] == 1) blackTiles[i] -> setIcon(QIcon(QString(":/img/piece_W.png")));
        else if(board[i] == 2) blackTiles[i] -> setIcon(QIcon(QString(":/img/piece_B.png")));
        else if(board[i] == 3) blackTiles[i] -> setIcon(QIcon(QString(":/img/king_W.png")));
        else blackTiles[i] -> setIcon(QIcon(QString(":/img/king_B.png")));
        blackTiles[i] -> setIconSize(QSize(80, 80));
    }
}


void deepvik::on_selectColorButton_clicked()
{
    QStringList colors = { "White", "Black" };
    bool ok;
    QString selected = QInputDialog::getItem(this, "Choose color", "Choose color:", colors, COLOR ? 1 : 0, false, &ok);
    if (ok) {
        bool tempColor = (selected == "Black");
        if(COLOR != tempColor){
            ui -> evalBar -> setValue(600 - ui -> evalBar -> value());
            COLOR = tempColor;
        }
        ui->ColorLabel->setText(QString("Your color: %1").arg(COLOR ? "black" : "white"));
    }
}


void deepvik::on_rotateButton_clicked()
{
    for (int i = 0; i < 16; ++i) {
        QPoint pos1 = blackTiles[i]->pos();
        QPoint pos2 = blackTiles[31 - i]->pos();
        blackTiles[i]->move(pos2);
        blackTiles[31 - i]->move(pos1);
    }
    INVERTED = !INVERTED;
    if (INVERTED) {
        ui->evalBar->setStyleSheet(R"(
            QProgressBar {
                background-color: #f1d6b8;
                border: none;
                color: rgb(85, 170, 255);
                font-size: 16pt;
                font-weight: bold;
                text-align: center;
            }
            QProgressBar::chunk {
                background-color: #2a1a1a;
            }
        )");
    } else {
        ui->evalBar->setStyleSheet(R"(
            QProgressBar {
                background-color: #2a1a1a;
                border: none;
                color: rgb(85, 170, 255);
                font-size: 16pt;
                font-weight: bold;
                text-align: center;
            }
            QProgressBar::chunk {
                background-color: #f1d6b8;
            }
        )");
    }
}
void deepvik::endGame(){
    Freeze = true;
    ui -> newGameButton -> setVisible(true);
    ui -> selectColorButton -> setVisible(true);
    ui -> surrenderButton -> setVisible(false);
    changeCursor(false);
}

void deepvik::on_surrenderButton_clicked()
{
    endGame();
    QMessageBox::about(this, "END", "You lost!");
}

