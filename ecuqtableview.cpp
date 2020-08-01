#include "ecuqtableview.h"

EcuQTableView::EcuQTableView(QWidget *parent)
{
    pen.setColor(Qt::black);
    brush.setColor(Qt::green);
    brush.setStyle(Qt::BrushStyle::SolidPattern);
}

EcuQTableView::~EcuQTableView()
{

}
