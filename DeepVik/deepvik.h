#ifndef DEEPVIK_H
#define DEEPVIK_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QSlider>
#include <QLabel>
#include <QDialog>
#include <QVBoxLayout>
#include "src/myBot.h"
#include "src/moveGenerator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class deepvik;
}
QT_END_NAMESPACE

class deepvik : public QMainWindow
{
    Q_OBJECT

public:
    deepvik(QWidget *parent = nullptr);
    ~deepvik();

private slots:
    void on_newGameButton_clicked();

    void on_selectColorButton_clicked();

    void on_rotateButton_clicked();

    void on_surrenderButton_clicked();

private:
    void handleTileClicked();
    void refresh();
    void changeCursor(bool type);
    QPushButton* blackTiles[32];
    QPushButton* whiteTiles[32];
    Ui::deepvik *ui;
    bool Freeze;
    int gameType;
    int DIFFICULTY;
    bool COLOR;
    string userInput;
    Game gameState;
    vector<string> legalMoves;
};
#endif // DEEPVIK_H
