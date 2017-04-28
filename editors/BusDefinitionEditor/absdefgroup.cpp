//-----------------------------------------------------------------------------
// File: absdefgroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 15.8.2011
//
// Description:
// Editor for the logical ports of an abstraction definition.
//-----------------------------------------------------------------------------

#include "absdefgroup.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::AbsDefGroup()
//-----------------------------------------------------------------------------
AbsDefGroup::AbsDefGroup(QWidget *parent): 
QGroupBox(tr("Signals (Abstraction Definition)"), parent),
    portView_(this),
    portProxy_(this),
    portModel_(this),
    portDelegate_(this)
{

    portProxy_.setSourceModel(&portModel_);

	portView_.setModel(&portProxy_);
    portView_.setSortingEnabled(true);
	portView_.setItemDelegate(&portDelegate_);

    portView_.sortByColumn(0, Qt::AscendingOrder);

	connect(&portView_, SIGNAL(addSignalOptions()), this, SLOT(onAddSignalOptions()), Qt::UniqueConnection);
	
	connect(&portModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&portModel_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), 
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    connect(&portView_, SIGNAL(addItem(const QModelIndex&)), &portModel_, SLOT(onAddItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(&portView_, SIGNAL(removeItem(QModelIndex const&)), 
        &portModel_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);

	setupLayout();
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::~AbsDefGroup()
//-----------------------------------------------------------------------------
AbsDefGroup::~AbsDefGroup()
{

}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::onAddSignalOptions()
//-----------------------------------------------------------------------------
void AbsDefGroup::onAddSignalOptions()
{
    QModelIndexList selection;
    foreach (QModelIndex index, portView_.selectionModel()->selectedIndexes())
    {
        selection.append(portProxy_.mapToSource(index));
    }

	portModel_.addSignalOptions(selection);
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setAbsDef()
//-----------------------------------------------------------------------------
void AbsDefGroup::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
	portModel_.setAbsDef(absDef);
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setBusDef()
//-----------------------------------------------------------------------------
void AbsDefGroup::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    portDelegate_.setBusDef(busDefinition);
    portModel_.setBusDef(busDefinition);
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setupLayout()
//-----------------------------------------------------------------------------
void AbsDefGroup::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&portView_, 1);
}