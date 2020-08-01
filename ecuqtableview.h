#ifndef ECUQTABLEVIEW_H
#define ECUQTABLEVIEW_H

#include <QTableView>
#include <QPainter>
#include <QDebug>

class EcuQTableView : public QTableView
{
    Q_OBJECT
public:
    explicit EcuQTableView(QWidget *parent = nullptr);
    ~EcuQTableView();

    void set_x(float x) {
        x_val = x;
    }

    void set_y(float y) {
        y_val = y;
    }

private:
    QPen pen;
    QBrush brush;

    int diameter = 100;
    int radius = 0;

    float x_val = 0;
    float y_val = 0;

    void paintEvent(QPaintEvent * event)
    {
        QTableView::paintEvent(event);

        int x_point = static_cast<int>(x_val);
        int y_point = static_cast<int>(y_val);

        float x0 = columnViewportPosition(x_point);
        float x1 = x0 + columnWidth(x_point);

        float y0 = rowViewportPosition(y_point);
        float y1 = y0 + rowHeight(y_point);

        float x_factor = (x_val - static_cast<float>(x_point));
        float y_factor = (y_val - static_cast<float>(y_point));

        int x = static_cast<int>(x0 + ((x1 - x0) * x_factor));
        int y = static_cast<int>(y0 + ((y1 - y0) * y_factor));
        radius = diameter/2;

        QPainter painter(viewport());
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(pen);
        painter.setBrush(brush);
        painter.drawEllipse(x-radius,y-radius,diameter,diameter);
    }

};

#endif // ECUQTABLEVIEW_H
