#ifndef QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H
#define QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H

#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N 16
#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N 16

#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_MIN ign_angle_mg_by_cycle_rpm_scale[0]
#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_MAX ign_angle_mg_by_cycle_rpm_scale[IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N-1]-1

#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_MIN ign_angle_mg_by_cycle_mg_scale[0]
#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_MAX ign_angle_mg_by_cycle_mg_scale[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N-1]-1

#include <QAbstractTableModel>

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

signals:

};

#endif // QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H
