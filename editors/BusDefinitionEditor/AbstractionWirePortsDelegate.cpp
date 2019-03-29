//-----------------------------------------------------------------------------
// File: AbstractionWirePortsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 27.6.2011
//
// Description:
// The delegate that provides editors for logical port properties in Abstraction Definition.
//-----------------------------------------------------------------------------

#include "AbstractionWirePortsDelegate.h"
#include "LogicalPortColumns.h"

#include <QComboBox>
#include <QStringList>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::AbstractionWirePortsDelegate()
//-----------------------------------------------------------------------------
AbstractionWirePortsDelegate::AbstractionWirePortsDelegate(LibraryInterface* libraryAccess, QObject *parent):
AbstractionPortsDelegate(libraryAccess, parent)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AbstractionWirePortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    const QModelIndex& index ) const
{
    if (index.column() == LogicalPortColumns::DIRECTION)
    {
        QComboBox* box = new QComboBox(parent);

        QStringList list;
        list.append("in");
        list.append("out");
        list.append("inout");
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::DRIVER)
    {
        QComboBox* box = new QComboBox(parent);

        QStringList list;
        list.append(QString("none"));
        list.append(QString("any"));
        list.append(QString("clock"));
        list.append(QString("singleShot"));
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else
    {
        return AbstractionPortsDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::qualifierColumn()
//-----------------------------------------------------------------------------
int AbstractionWirePortsDelegate::qualifierColumn() const
{
    return LogicalPortColumns::QUALIFIER;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::getQualifierList()
//-----------------------------------------------------------------------------
QStringList AbstractionWirePortsDelegate::getQualifierList() const
{
    QStringList qualifierList = AbstractionPortsDelegate::getQualifierList();
    qualifierList.append("clock");
    qualifierList.append("reset");

    return qualifierList;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::widthColumn()
//-----------------------------------------------------------------------------
int AbstractionWirePortsDelegate::widthColumn() const
{
    return LogicalPortColumns::WIDTH;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::modeColum()
//-----------------------------------------------------------------------------
int AbstractionWirePortsDelegate::modeColum() const
{
    return LogicalPortColumns::MODE;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::presenceColumn()
//-----------------------------------------------------------------------------
int AbstractionWirePortsDelegate::presenceColumn() const
{
    return LogicalPortColumns::PRESENCE;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::systemGroupColumn()
//-----------------------------------------------------------------------------
int AbstractionWirePortsDelegate::systemGroupColumn() const
{
    return LogicalPortColumns::SYSTEM_GROUP;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::nameColumn()
//-----------------------------------------------------------------------------
int AbstractionWirePortsDelegate::nameColumn() const
{
    return LogicalPortColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int AbstractionWirePortsDelegate::descriptionColumn() const
{
    return LogicalPortColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::editorIsLineEditor()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsDelegate::editorIsLineEditor(int indexColumn) const
{
    return AbstractionPortsDelegate::editorIsLineEditor(indexColumn) ||
        indexColumn == LogicalPortColumns::DEFAULT_VALUE;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsDelegate::editorIsComboBox()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsDelegate::editorIsComboBox(int indexColumn) const
{
    return AbstractionPortsDelegate::editorIsComboBox(indexColumn) ||
        indexColumn == LogicalPortColumns::DIRECTION || indexColumn == LogicalPortColumns::DRIVER;
}
