//-----------------------------------------------------------------------------
// File: businterfaceeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 05.04.2011
//
// Description:
// Container for editors to edit a bus interface.
//-----------------------------------------------------------------------------

#include "businterfaceeditor.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: BusInterfaceEditor::BusInterfaceEditor()
//-----------------------------------------------------------------------------
BusInterfaceEditor::BusInterfaceEditor(LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busif, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<BusInterfaceValidator> busInterfaceValidator, QWidget* parent, QWidget* parentWnd):
ItemEditor(component, libHandler, parent),
busif_(busif),
tabs_(this), 
generalEditor_(libHandler, busif, component, parameterFinder, expressionFormatter, expressionParser, &tabs_, parentWnd),
portmapsEditor_(libHandler, component, busif, expressionParser, expressionFormatter, parameterFinder,
    busInterfaceValidator->getPortMapValidator(), &tabs_)
{
	Q_ASSERT(component);
	Q_ASSERT(libHandler);
	Q_ASSERT(busif_);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&tabs_);
	layout->setContentsMargins(0, 0, 0, 0);

	tabs_.addTab(&generalEditor_, tr("General"));
	tabs_.addTab(&portmapsEditor_, tr("Port maps"));

	connect(&portmapsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portmapsEditor_, SIGNAL(errorMessage(QString const&)),
        this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
	connect(&portmapsEditor_, SIGNAL(noticeMessage(QString const&)),
        this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);
    connect(&portmapsEditor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

    connect(&portmapsEditor_, SIGNAL(increaseReferences(QString)), this,
        SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&portmapsEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	connect(&generalEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&generalEditor_, SIGNAL(errorMessage(QString const&)),
		this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
	connect(&generalEditor_, SIGNAL(noticeMessage(QString const&)),
		this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);
	connect(&generalEditor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

    connect(&generalEditor_, SIGNAL(increaseReferences(QString)), this,
        SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&generalEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&generalEditor_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

	connect(&tabs_, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)), Qt::UniqueConnection);

	refresh();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEditor::~BusInterfaceEditor()
//-----------------------------------------------------------------------------
BusInterfaceEditor::~BusInterfaceEditor()
{
	tabs_.disconnect();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEditor::onTabChange()
//-----------------------------------------------------------------------------
void BusInterfaceEditor::onTabChange(int index)
{
	// if port maps tab is selected
	if (index == 1)
    {
        General::InterfaceMode busMode = busif_->getInterfaceMode();
        if (busMode == General::MONITOR && busif_->getMonitor())
        {
            busMode = busif_->getMonitor()->interfaceMode_;
        }

		// update the abstraction type
        portmapsEditor_.setAbsType(generalEditor_.getAbsType(), busMode);
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEditor::refresh()
//-----------------------------------------------------------------------------
void BusInterfaceEditor::refresh()
{
	generalEditor_.refresh();
	portmapsEditor_.refresh();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEditor::setProtection()
//-----------------------------------------------------------------------------
void BusInterfaceEditor::setProtection(bool locked)
{
    generalEditor_.setDisabled(locked);
    portmapsEditor_.setDisabled(locked);
}
