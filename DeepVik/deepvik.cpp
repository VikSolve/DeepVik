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
    }
    gameType = 0;
}

deepvik::~deepvik()
{
    delete ui;
}
void wait(int milliseconds=50) {
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec();
}
void deepvik::handleTileClicked()
{
    QPushButton* clicked = qobject_cast<QPushButton*>(sender());
    if (clicked) {
        for (int i = 0; i < 32; ++i) {
            if (tiles[i] == clicked) {
                qDebug() << "Kliknięto tile" << i;
                break;
            }
        }
    }
}

void deepvik::on_pushButton_clicked()
{
    for(int i = 0; i < 12; i++){
        tiles[i] -> setIcon(QIcon(":/img/piece_W"));
        tiles[i] -> setIconSize(QSize(80, 80));
    }
    for(int i = 20; i < 32; i++){
        tiles[i] -> setIcon(QIcon(":/img/piece_B"));
        tiles[i] -> setIconSize(QSize(80, 80));
    }
}

