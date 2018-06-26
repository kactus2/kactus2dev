//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.06.2018
//
// Description:
// The delegate that provides editors for transactional port properties in Abstraction Definition.
//-----------------------------------------------------------------------------

#include "AbstractionTransactionalPortsDelegate.h"

#include <editors/BusDefinitionEditor/AbstractionTransactionalPortColumns.h>

#include <QComboBox>
#include <QStringList>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::AbstractionTransactionalPortsDelegate()
//-----------------------------------------------------------------------------
AbstractionTransactionalPortsDelegate::AbstractionTransactionalPortsDelegate(QObject *parent):
AbstractionPortsDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AbstractionTransactionalPortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    const QModelIndex& index ) const
{
    if (index.column() == AbstractionTransactionalPortColumns::INITIATIVE)
    {
        QComboBox* initiativeCombo = new QComboBox(parent);

        QStringList list{ "none", "requires", "provides", "requires/provides" };
        initiativeCombo->addItems(list);

        connect(initiativeCombo, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return initiativeCombo;
    }
    else if (index.column() == AbstractionTransactionalPortColumns::KIND)
    {
        QComboBox* kindCombo = new QComboBox(parent);
        kindCombo->setEditable(true);

        QStringList list = { "tlm_port", "tlm_socket", "simple_socket", "multi_socket" };
        kindCombo->addItems(list);

        connect(kindCombo, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return kindCombo;
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PROTOCOLTYPE)
    {
        QComboBox* protocolTypeCombo = new QComboBox(parent);
        protocolTypeCombo->setEditable(true);

        QStringList list = { "", "tlm" };
        protocolTypeCombo->addItems(list);

        connect(protocolTypeCombo, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return protocolTypeCombo;
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADTYPE)
    {
        QComboBox* payloadTypeCombo = new QComboBox(parent);

        QStringList list = { "none", "generic", "specific" };
        payloadTypeCombo->addItems(list);

        connect(payloadTypeCombo, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return payloadTypeCombo;
    }
    else
    {
        return AbstractionPortsDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::qualifierColumn()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsDelegate::qualifierColumn() const
{
    return AbstractionTransactionalPortColumns::QUALIFIER;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::widthColumn()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsDelegate::widthColumn() const
{
    return AbstractionTransactionalPortColumns::BUSWIDTH;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::modeColum()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsDelegate::modeColum() const
{
    return AbstractionTransactionalPortColumns::MODE;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::presenceColumn()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsDelegate::presenceColumn() const
{
    return AbstractionTransactionalPortColumns::PRESENCE;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::systemGroupColumn()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsDelegate::systemGroupColumn() const
{
    return AbstractionTransactionalPortColumns::SYSTEM_GROUP;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::nameColumn()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsDelegate::nameColumn() const
{
    return AbstractionTransactionalPortColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsDelegate::descriptionColumn() const
{
    return AbstractionTransactionalPortColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::editorIsLineEditor()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsDelegate::editorIsLineEditor(int indexColumn) const
{
    return AbstractionPortsDelegate::editorIsLineEditor(indexColumn) ||
        indexColumn == AbstractionTransactionalPortColumns::PAYLOADNAME ||
        indexColumn == AbstractionTransactionalPortColumns::PAYLOADEXTENSION;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::editorIsComboBox()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsDelegate::editorIsComboBox(int indexColumn) const
{
    return AbstractionPortsDelegate::editorIsComboBox(indexColumn) ||
        indexColumn == AbstractionTransactionalPortColumns::INITIATIVE ||
        indexColumn == AbstractionTransactionalPortColumns::KIND ||
        indexColumn == AbstractionTransactionalPortColumns::PROTOCOLTYPE ||
        indexColumn == AbstractionTransactionalPortColumns::PAYLOADTYPE;
}
