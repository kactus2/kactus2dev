/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceeditor.cpp
 *		Project: Kactus 2
 */

#include "addressspaceeditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QWidget>

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::AddressSpaceEditor()
//-----------------------------------------------------------------------------
AddressSpaceEditor::AddressSpaceEditor( QSharedPointer<Component> component, 
										LibraryInterface* handler,
									   QSharedPointer<AddressSpace> addrSpace,
                                       QSharedPointer <ParameterFinder> parameterFinder,
                                       QSharedPointer <ExpressionFormatter> expressionFormatter,
									   QWidget* parent /*= 0*/ ):
ItemEditor(component, handler, parent),
addrSpace_(addrSpace),
nameEditor_(addrSpace->getNameGroup(), this),
general_(addrSpace, this),
segments_(addrSpace, component, handler, this),
localMemMap_(addrSpace->getLocalMemoryMap(), component, handler, parameterFinder, expressionFormatter, this)
{
	Q_ASSERT(addrSpace_);

	// create the scroll area
	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);

	QVBoxLayout* scrollLayout = new QVBoxLayout(this);
	scrollLayout->addWidget(scrollArea);
	scrollLayout->setContentsMargins(0, 0, 0, 0);

	// create the top widget and set it under the scroll area
	QWidget* topWidget = new QWidget(scrollArea);
	topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layout = new QVBoxLayout(topWidget);

	layout->addWidget(&nameEditor_);
	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	layout->addWidget(&general_);
	connect(&general_, SIGNAL(contentChanged()), 
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	layout->addWidget(&segments_);
	connect(&segments_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&segments_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&segments_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);


	layout->addWidget(&localMemMap_);
	connect(&localMemMap_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&localMemMap_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&localMemMap_, SIGNAL(itemAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&localMemMap_, SIGNAL(itemRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(&localMemMap_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&localMemMap_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	layout->setContentsMargins(0, 0, 0, 0);

	scrollArea->setWidget(topWidget);

	refresh();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::~AddressSpaceEditor()
//-----------------------------------------------------------------------------
AddressSpaceEditor::~AddressSpaceEditor()
{
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::isValid()
//-----------------------------------------------------------------------------
bool AddressSpaceEditor::isValid() const
{
	if (!nameEditor_.isValid()) {
		return false;
	}
	else if (!general_.isValid()) {
		return false;
	}
	else if (!segments_.isValid()) {
		return false;
	}
	else if (!localMemMap_.isValid()) {
		return false;
	}
	else {
		return true;
	}
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::refresh()
//-----------------------------------------------------------------------------
void AddressSpaceEditor::refresh()
{
	nameEditor_.refresh();

	general_.refresh();
	segments_.refresh();
	localMemMap_.refresh();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::showEvent()
//-----------------------------------------------------------------------------
void AddressSpaceEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/addressspace.html");
}
