//-----------------------------------------------------------------------------
// File: TransactionalPortsFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.05.2019
//
// Description:
// Sorting proxy model for transactional ports.
//-----------------------------------------------------------------------------

#include "TransactionalPortsFilter.h"

#include <IPXACTmodels/Component/Port.h>

#include <editors/ComponentEditor/ports/TransactionalPortColumns.h>
#include <KactusAPI/include/PortsInterface.h>

//-----------------------------------------------------------------------------
// Function: TransactionalPortsFilter::TransactionalPortsFilter()
//-----------------------------------------------------------------------------
TransactionalPortsFilter::TransactionalPortsFilter(QSharedPointer<PortsInterface> portInterface, QObject* parent):
PortsFilter(portInterface, parent)
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsFilter::nameColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsFilter::nameColumn() const
{
    return TransactionalPortColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsFilter::portIsAccepted()
//-----------------------------------------------------------------------------
bool TransactionalPortsFilter::portIsAccepted(QString const& portName) const
{
    return getInterface()->portIsTransactional(portName.toStdString());
}
