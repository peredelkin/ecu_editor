#ifndef QCOLORGRADIENTBYVALUE_H
#define QCOLORGRADIENTBYVALUE_H

#include <QColor>

class QColorGradientByValue
{
public:
    static QColor gradient_by_value(const qreal value,const QColor* color,const qreal* scale,int scale_count) {
        qreal val = min_max_qreal(value,scale[0],scale[scale_count-2]);
        qreal *val_point = (qreal*) bsearch(&val,scale,scale_count-1,sizeof (qreal),bsearch_gradient_compare);
        int val_index = (int)(val_point - scale);

        QColor color0 = color[val_index];
        QColor color1 = color[val_index+1];
        QColor result;

        float factor = ((float) (val - val_point[0])) / ((float) (val_point[1] - val_point[0]));
        result.setRedF(gradient_interpolation(color0.redF(),color1.redF(),factor));
        result.setGreenF(gradient_interpolation(color0.greenF(),color1.greenF(),factor));
        result.setBlueF(gradient_interpolation(color0.blueF(),color1.blueF(),factor));
        result.setAlphaF(gradient_interpolation(color0.alphaF(),color1.alphaF(),factor));
        return result;
    }
private:
    static qreal min_max_qreal(const qreal data,const qreal min,const qreal max) {
        if(data > max) {
            return max;
        } else {
            if(data < min) {
                return min;
            } else {
                return data;
            }
        }
    }
    static int bsearch_gradient_compare(const void *ap, const void *bp)
    {
        const qreal *a = (qreal *) ap;
        const qreal *b = (qreal *) bp;
        if(*a < b[0])
            return -1;
        else if(*a >= b[1])
            return 1;
        else
            return 0;
    }
    static qreal gradient_interpolation(const qreal color0,const qreal color1,const qreal factor) {
        return (color0 + ((color1 - color0) * factor));
    }
};

#endif // QCOLORGRADIENTBYVALUE_H
