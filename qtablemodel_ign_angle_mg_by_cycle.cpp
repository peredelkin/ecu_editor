#include "qtablemodel_ign_angle_mg_by_cycle.h"
#include <string.h>

QTableModel_ign_angle_mg_by_cycle::QTableModel_ign_angle_mg_by_cycle(QObject *parent) : QAbstractTableModel(parent)
{
    memset((void*)table,0,sizeof (float)*16*16);
}

Qt::ItemFlags QTableModel_ign_angle_mg_by_cycle::flags(const QModelIndex &index) const {
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool QTableModel_ign_angle_mg_by_cycle::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role == Qt::EditRole)
    table[index.row()][index.column()] = value.toFloat();
}

QVariant QTableModel_ign_angle_mg_by_cycle::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }

    float value = table[index.row()][index.column()];

    switch (role) {
    case Qt::DisplayRole:
        return value;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::BackgroundColorRole:
        return QBrush (gradient(value));
    case Qt::SizeHintRole:
        return Qt::MinimumSize;
    default:
        return QVariant();
    }
}

int QTableModel_ign_angle_mg_by_cycle::rowCount(const QModelIndex &parent) const {
    return IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N;
}

int QTableModel_ign_angle_mg_by_cycle::columnCount(const QModelIndex &parent) const {
    return IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N;
}
