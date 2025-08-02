#ifndef DEEPVIK_H
#define DEEPVIK_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
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

private:
    void handleTileClicked();
    void refresh();
    QPushButton* tiles[32];
    Ui::deepvik *ui;
    bool Freeze;
    int gameType;
    string userInput;
    Game gameState;
    vector<string> legalMoves;
};
#endif // DEEPVIK_H
