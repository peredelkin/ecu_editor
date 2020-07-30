#ifndef QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H
#define QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H

#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N 16
#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N 16

#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_MIN ign_angle_mg_by_cycle_rpm_scale[0]
#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_MAX ign_angle_mg_by_cycle_rpm_scale[IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N-1]-1

#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_MIN ign_angle_mg_by_cycle_mg_scale[0]
#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_MAX ign_angle_mg_by_cycle_mg_scale[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N-1]-1

#include <QAbstractTableModel>
#include <QTableView>
#include <QBrush>
#include <QFont>

class QTableModel_ign_angle_mg_by_cycle : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit QTableModel_ign_angle_mg_by_cycle(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    const uint16_t rpm_scale[IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N] = { 600,720,840,990,1170,1380,1560,1950,2310,2730,3210,3840,4530,5370,6360,7650 };
    const uint16_t mg_scale[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N] = { 29,57,85,113,141,169,197,225,253,281,309,337,365,393,421,449 };
    volatile float table[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N][IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N];
private:
    typedef struct {
        int r;
        int g;
        int b;
    } color_t;

    qreal min_max_qreal(const qreal data,const qreal min,const qreal max) const {
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

    const qreal scale_table[7] =  {
        -5,
        5,
        15,
        25,
        35,
        45,
        55};

    const QColor color_table[7] = {
        QColor(255,0,255,100),
        QColor(0,0,255,100),
        QColor(0,255,255,100),
        QColor(0,255,0,100),
        QColor(255,255,0,100),
        QColor(255,0,0,100),
        QColor(255,255,255,100),
    };

    qreal gradient_interpolation(const qreal color0,const qreal color1,const qreal factor) const {
        return (color0 + ((color1 - color0) * factor));
    }

    QColor gradient_by_value(const qreal value,const QColor* color,const qreal* scale,int scale_count) const  {
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
signals:

};

#endif // QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H
