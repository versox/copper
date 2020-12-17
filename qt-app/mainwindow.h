#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPicture>
#include <QPainter>
#include <QPen>

namespace Ui {
class MainWindow;
}

#define DRAWMODE_COUNT 2
enum DrawMode {
    GRID = 0,
    SHAPE = 1,
    DRAW = 3 // todo
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void serialStarted();
    void serialError(QProcess::ProcessError e);
    void readSerial();
    void switchMode();
    void makeShape();
    void paint();

private:
    Ui::MainWindow *ui;
    QList<QWidget*> labels;
    DrawMode currentMode;
    QList<QPoint> points;
    QPicture picture;
    QPen pen;
    QPainter painter;
    QPolygon polygon;
    QProcess* serialP;
};

#endif // MAINWINDOW_H
