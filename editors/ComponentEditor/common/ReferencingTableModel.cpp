//-----------------------------------------------------------------------------
// File: ReferencingTableModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.03.2015
//
// Description:
// Base class for table model properties for editable tables.
//-----------------------------------------------------------------------------

#include "ReferencingTableModel.h"

//-----------------------------------------------------------------------------
// Function: ReferencingTableModel::ReferencingTableModel()
//-----------------------------------------------------------------------------
ReferencingTableModel::ReferencingTableModel(QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
QAbstractTableModel(parent),
parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ReferencingTableModel::~ReferencingTableModel()
//-----------------------------------------------------------------------------
ReferencingTableModel::~ReferencingTableModel()
{

}

//-----------------------------------------------------------------------------
// Function: ReferencingTableModel::removeReferencesInItemOnRow()
//-----------------------------------------------------------------------------
void ReferencingTableModel::removeReferencesInItemOnRow(const int& row) const
{
    QStringList allParameterIds = parameterFinder_->getAllParameterIds();
    foreach (QString valueID, allParameterIds)
    {
        int totalreferences = getAllReferencesToIdInItemOnRow(row, valueID);
        for (int i = 0; i < totalreferences; ++i)
        {
            emit decreaseReferences(valueID);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ReferencingTableModel::removeReferencesFromSingleExpression()
//-----------------------------------------------------------------------------
void ReferencingTableModel::removeReferencesFromSingleExpression(QString const& expression) const
{
    QStringList allParameterIds = parameterFinder_->getAllParameterIds();
    foreach (QString valueID, allParameterIds)
    {
        int referencesToId = expression.count(valueID);
        for (int i = 0; i < referencesToId; ++i)
        {
            emit decreaseReferences(valueID);
        }
    }
}
