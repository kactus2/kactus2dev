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

#include <editors/BusDefinitionEditor/LogicalPortColumns.h>

#include <QComboBox>
#include <QStringList>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::AbstractionTransactionalPortsDelegate()
//-----------------------------------------------------------------------------
AbstractionTransactionalPortsDelegate::AbstractionTransactionalPortsDelegate(LibraryInterface* libraryAccess,
    QObject *parent):
AbstractionPortsDelegate(libraryAccess, parent)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AbstractionTransactionalPortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    const QModelIndex& index ) const
{
    if (index.column() == LogicalPortColumns::INITIATIVE)
    {
        QComboBox* initiativeCombo = new QComboBox(parent);

        QStringList list{ "none", "requires", "provides", "requires/provides" };
        initiativeCombo->addItems(list);

        connect(initiativeCombo, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return initiativeCombo;
    }
    else if (index.column() == LogicalPortColumns::KIND)
    {
        QComboBox* kindCombo = new QComboBox(parent);
        kindCombo->setEditable(true);

        QStringList list = { "tlm_port", "tlm_socket", "simple_socket", "multi_socket" };
        kindCombo->addItems(list);

        connect(kindCombo, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return kindCombo;
    }
    else if (index.column() == LogicalPortColumns::PROTOCOLTYPE)
    {
        QComboBox* protocolTypeCombo = new QComboBox(parent);
        protocolTypeCombo->setEditable(true);

        QStringList list = { "", "tlm" };
        protocolTypeCombo->addItems(list);

        connect(protocolTypeCombo, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return protocolTypeCombo;
    }
    else if (index.column() == LogicalPortColumns::PAYLOADTYPE)
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
