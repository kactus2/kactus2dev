//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 8.8.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICIESMODEL_H
#define FIELDACCESSPOLICIESMODEL_H

#include <KactusAPI/include/FieldInterface.h>

#include <QAbstractTableModel>

class FieldAccessPoliciesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
        
    FieldAccessPoliciesModel(QString const& fieldName, FieldInterface* fieldInterface, QObject* parent);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex& index) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);


private:

    FieldInterface* fieldInterface_;

    QString fieldName_;
};


#endif // FIELDACCESSPOLICIESMODEL_H
