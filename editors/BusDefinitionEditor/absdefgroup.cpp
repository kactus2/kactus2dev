//-----------------------------------------------------------------------------
// File: absdefgroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.8.2011
//
// Description:
// Editor for the logical ports of an abstraction definition.
//-----------------------------------------------------------------------------

#include "absdefgroup.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::AbsDefGroup()
//-----------------------------------------------------------------------------
AbsDefGroup::AbsDefGroup(QWidget *parent): 
QGroupBox(tr("Signals (Abstraction Definition)"), parent),
    portView_(this),
    portModel_(this),
    portDelegate_(this)
{
	portView_.setModel(&portModel_);
	portView_.setItemDelegate(&portDelegate_);
    portView_.setAllowImportExport(true);
    portView_.setItemsDraggable(false);

	connect(&portView_, SIGNAL(addSignalOptions()), this, SLOT(onAddSignalOptions()), Qt::UniqueConnection);
	
	connect(&portModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRenamed(const QString&, const QString&)), 
        this, SIGNAL(portRenamed(const QString&, const QString&)), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), 
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    connect(&portView_, SIGNAL(addItem(const QModelIndex&)), &portModel_, SLOT(addSignal()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(removeItem(const QModelIndex&)),
        &portModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

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
	portModel_.addSignalOptions(portView_.selected());
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::save()
//-----------------------------------------------------------------------------
void AbsDefGroup::save()
{
	portModel_.save();
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