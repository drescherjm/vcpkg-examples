#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>

namespace Ui {
class QGraphicsView0;
}

class QGraphicsView0 : public QDialog
{
    Q_OBJECT
    
public:
    explicit QGraphicsView0(QWidget *parent = 0);
    ~QGraphicsView0();
    
private:
    Ui::QGraphicsView0 *ui;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangle;
    QGraphicsTextItem *text;
};