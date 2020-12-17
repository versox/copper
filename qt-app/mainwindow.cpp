#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>
#include <QProcess>
#include <QDebug>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Find the label objects
    QRegularExpression re("l\\_\\d");
    labels = this->findChildren<QWidget*>(re);
    std::sort(labels.begin(), labels.end(),
              [](const QWidget* a, const QWidget* b) -> bool
    {
        // 2, 2 because the strings had extra random charactersSS??!?!?  and the 2nd param is length :P
        QString aname = a->objectName();
        QStringRef* ar = new QStringRef(&aname, 2, 2);
        QString bname = b->objectName();
        QStringRef* br = new QStringRef(&bname, 2, 2);
//        qInfo() << ar->toString();
//        qInfo() << br->toString();
//        qInfo() << br->toInt();
//        qInfo() << "AAHHHH";
        return ( ar->toInt() < br->toInt() );
    });

    // Drawing
    picture.setBoundingRect(ui->drawLabel->frameRect());
    ui->drawLabel->setPicture(picture);
    pen.setWidth(10);

    // Switch mode
    currentMode = DrawMode::GRID;
    QPushButton* switchBtn = ui->switchBtn;
    connect(switchBtn, &QPushButton::pressed, this, &MainWindow::switchMode);

    // Listen to serial
    serialP = new QProcess(this);
    serialP->setProgram("cat");
    serialP->setArguments(*new QStringList("/dev/ttymxc0"));
    connect(serialP, &QProcess::readyReadStandardOutput, this, &MainWindow::readSerial);
    connect(serialP, &QProcess::errorOccurred, this, &MainWindow::serialError);
    connect(serialP, &QProcess::started, this, &MainWindow::serialStarted);
    serialP->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::switchMode() {
    int nextMode = (int) currentMode + 1;
    if (nextMode >= DRAWMODE_COUNT) nextMode = 0;
    currentMode = (DrawMode) nextMode;

    bool labelsVisible = false;

    switch(currentMode) {
        case GRID:
            ui->drawLabel->hide();
            ui->switchBtn->setText("Grid");
            labelsVisible = true;
            break;
        case DRAW:
            ui->drawLabel->show();
            ui->switchBtn->setText("Draw");
            break;
        case SHAPE:
            ui->drawLabel->show();
            ui->switchBtn->setText("Shape");
//            QVector<QPoint> vec = points.toVector();
//            polygon = QPolygon(vec);
            makeShape();
            paint();
            break;
    }

    for (int i = 0; i < 16; i++) {
        QLabel* l = (QLabel*) labels.at(i);
        l->setVisible(labelsVisible);
    }
}

void MainWindow::readSerial() {
    QByteArray read = serialP->readAll();
    qInfo() << QString::fromStdString(read.toStdString());

    points.clear();

    for (int i = 0; i < 16; i++) {
        // Update labels
        QLabel* l = (QLabel*) labels.at(i);
        QChar c = read.at(i);
        QString s = c;
        l->setText(s);

        // Get points for shape
        if (s == "1") {
            points.append(l->pos());
        }
    }

    if (currentMode == DrawMode::SHAPE) {
//        QVector<QPoint> vec = points.toVector();
//        polygon = QPolygon(vec);
        makeShape();
        paint();
    }
}

void MainWindow::makeShape() {
    QVector<QPoint> vec = points.toVector();
    QPointF mean;
    for (int i = 0; i < vec.length(); i++) {
        mean = mean + vec.at(i);
    }
    mean = mean / vec.length();

    QVector<QPair<double, QPoint>> withAngle;

    for (int i = 0; i < vec.length(); i++) {
        QPointF trans = vec.at(i) - mean;
        withAngle.append(QPair<double, QPoint>(atan2(trans.y(), trans.x()), vec.at(i)));
    }

    // Sort by angle
    std::sort(withAngle.begin(), withAngle.end(),
              [](QPair<double, QPoint>& a,  QPair<double, QPoint>& b) -> bool
    {
        return a.first > b.first;
    });

    for (int i = 0; i < vec.length(); i++) {
        auto pair = withAngle.at(i);
        vec[i] = pair.second;
        qInfo() << pair.first;
    }

    polygon = QPolygon(vec);
}

void MainWindow::paint() {
    painter.begin(&picture);
    painter.setPen(pen);
    painter.drawPolygon(polygon);
    painter.end();
    ui->drawLabel->setPicture(picture);
}

void MainWindow::serialError(QProcess::ProcessError e) {
    qInfo() << "ERRAR";
    qInfo() << e;
}

void MainWindow::serialStarted() {
    qInfo() << "Ssserial started";
}
