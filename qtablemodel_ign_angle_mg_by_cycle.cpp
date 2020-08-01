#include "qtablemodel_ign_angle_mg_by_cycle.h"
#include <string.h>

static const qreal scale_table[9] =  {
    -15,
    -5,
    5,
    15,
    25,
    35,
    45,
    55,
    65
};

static const QColor color_table[8] = {
    QColor(0,0,0,127),
    QColor(255,0,255,100),
    QColor(0,0,255,100),
    QColor(0,255,255,100),
    QColor(0,255,0,100),
    QColor(255,255,0,100),
    QColor(255,0,0,100),
    QColor(255,255,255,127),
};

QTableModel_ign_angle_mg_by_cycle::QTableModel_ign_angle_mg_by_cycle(QWidget *parent) : QAbstractTableModel(parent)
{
    memset((void*)table,0,sizeof (float)*IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N*IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N);
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

QVariant QTableModel_ign_angle_mg_by_cycle::headerData(int section, Qt::Orientation orientation,int role) const {
    switch (role) {
    case Qt::DisplayRole: {
        switch (orientation) {
        case Qt::Orientation::Horizontal:
            return rpm_scale[section];
        case Qt::Orientation::Vertical:
            return mg_scale[section];
        default:
            return QVariant();
        }
    }
    default:
        return QVariant();
    }
}

QVariant QTableModel_ign_angle_mg_by_cycle::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }

    float value = table[index.row()][index.column()];

    switch (role) {
    case Qt::DisplayRole:
        return value;
    case Qt::EditRole:
        return value;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::BackgroundColorRole:
        return QBrush (QColorGradientByValue::gradient(value,color_table,scale_table,9));
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
