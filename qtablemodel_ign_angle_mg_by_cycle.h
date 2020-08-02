#ifndef QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H
#define QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H

#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N 16
#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N 16

#include <QAbstractTableModel>
#include <QTableView>
#include <QBrush>
#include <QFont>
#include "qcolorgradientbyvalue.h"

class QTableModel_ign_angle_mg_by_cycle : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit QTableModel_ign_angle_mg_by_cycle(QWidget *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation,int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    volatile float table[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N][IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N];
private:

signals:

};

#endif // QTABLEMODEL_IGN_ANGLE_MG_BY_CYCLE_H
