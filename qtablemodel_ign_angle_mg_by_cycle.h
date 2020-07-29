#ifndef QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H
#define QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H

#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N 16
#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N 16

#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_MIN ign_angle_mg_by_cycle_rpm_scale[0]
#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_MAX ign_angle_mg_by_cycle_rpm_scale[IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N-1]-1

#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_MIN ign_angle_mg_by_cycle_mg_scale[0]
#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_MAX ign_angle_mg_by_cycle_mg_scale[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N-1]-1

#include <QAbstractTableModel>
#include <QBrush>

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

    float min_max_float(const float data,const float min,const float max) const {
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

    static int bsearch_compare(const void *ap, const void *bp)
    {
        const float *a = (float *) ap;
        const float *b = (float *) bp;
        if(*a < b[0])
            return -1;
        else if(*a >= b[1])
            return 1;
        else
            return 0;
    }

    const float scale[5] = {-10,20,40,45,50};

    const int red[4] = {0,0,255,0};
    const int green[4] = {0,255,0,0};
    const int blue[4] = {255,0,0,0};

    color_t gradient(const float value) const  {
        float val = min_max_float(value,scale[0],scale[3]);
        float *val_point = (float*) bsearch(&val,scale,4,sizeof (float),bsearch_compare);
        int val_index = (int)(val_point - scale);

        int red0 = red[val_index];
        int red1 = red[val_index+1];

        int green0 = green[val_index];
        int green1 = green[val_index+1];

        int blue0 = blue[val_index];
        int blue1 = blue[val_index+1];

        float val_factor = ((float) (val - val_point[0])) / ((float) (val_point[1] - val_point[0]));
        int color_red = (red0 + ((red1 - red0) * val_factor));
        int color_green = (green0 + ((green1 - green0) * val_factor));
        int color_blue = (blue0 + ((blue1 - blue0) * val_factor));

        color_t result;
        result.r = color_red;
        result.g = color_green;
        result.b = color_blue;
        return result;
    }
signals:

};

#endif // QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H
