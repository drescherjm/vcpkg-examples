#include "QGraphicsView0.h"
#include "ui_QGraphicsView0.h"

QGraphicsView0::QGraphicsView0(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QGraphicsView0)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    QBrush greenBrush(Qt::green);
    QBrush blueBrush(Qt::blue);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);

    rectangle = scene->addRect(100, 0, 80, 100, outlinePen, blueBrush);

    // addEllipse(x,y,w,h,pen,brush)
    ellipse = scene->addEllipse(0, -100, 300, 60, outlinePen, greenBrush);

    text = scene->addText("bogotobogo.com", QFont("Arial", 20) );
    // movable text
    text->setFlag(QGraphicsItem::ItemIsMovable);
}

QGraphicsView0::~QGraphicsView0()
{
    delete ui;
}