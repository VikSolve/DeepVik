#ifndef DEEPVIK_H
#define DEEPVIK_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>

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
    void on_pushButton_clicked();

private:
    QPushButton* tiles[32];
    Ui::deepvik *ui;
    void handleTileClicked();
    int gameType;
    int MV1;
    int MV2;
};
#endif // DEEPVIK_H
