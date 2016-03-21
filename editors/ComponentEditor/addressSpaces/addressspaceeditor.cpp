//-----------------------------------------------------------------------------
// File: addressspaceeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.2.2012
//
// Description:
// Editor to edit and save settings of an address space within component editor.
//-----------------------------------------------------------------------------

#include "addressspaceeditor.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>

#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QWidget>

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::AddressSpaceEditor()
//-----------------------------------------------------------------------------
AddressSpaceEditor::AddressSpaceEditor(QSharedPointer<Component> component, LibraryInterface* handler,
                                       QSharedPointer<AddressSpace> addrSpace,
                                       QSharedPointer<ParameterFinder> parameterFinder,
                                       QSharedPointer<ExpressionFormatter> expressionFormatter,
                                       QSharedPointer<ExpressionParser> expressionParser,
                                       QSharedPointer<AddressSpaceValidator> addressSpaceValidator,
                                       QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
addrSpace_(addrSpace),
nameEditor_(addrSpace, this),
generalEditor_(addrSpace, component->getMasterInterfaces(addrSpace_->name()), parameterFinder, expressionParser,
    this),
segmentsEditor_(addrSpace, component, handler->getDirectoryPath(component->getVlnv()), parameterFinder,
    expressionParser, expressionFormatter, this),
localMemMapEditor_(addrSpace, component, handler, parameterFinder, expressionFormatter,
                   addressSpaceValidator->getLocalMemoryMapValidator(), this)
{
	Q_ASSERT(addrSpace_);

    nameEditor_.setTitle(tr("Address space name and description"));

    connect(&nameEditor_, SIGNAL(contentChanged()),	this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&generalEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&generalEditor_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(&generalEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&generalEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(&segmentsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&segmentsEditor_, SIGNAL(contentChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(&segmentsEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&segmentsEditor_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(&segmentsEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&segmentsEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	connect(&localMemMapEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&localMemMapEditor_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(&localMemMapEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&localMemMapEditor_, SIGNAL(itemAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&localMemMapEditor_, SIGNAL(itemRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(&localMemMapEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&localMemMapEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(&generalEditor_, SIGNAL(assignNewAddressUnitBits(QString const&)),
        &localMemMapEditor_, SIGNAL(assignNewAddressUnitBits(QString const&)), Qt::UniqueConnection);

    connect(this, SIGNAL(assignNewAddressUnitBits(QString const&)),
        &localMemMapEditor_, SIGNAL(assignNewAddressUnitBits(QString const&)), Qt::UniqueConnection);

    connect(&generalEditor_, SIGNAL(assignNewAddressUnitBits(QString const&)),
        this, SIGNAL(newAddressUnitBitsForAddressSpaceChildItems()), Qt::UniqueConnection);

    setupLayout();

	refresh();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::~AddressSpaceEditor()
//-----------------------------------------------------------------------------
AddressSpaceEditor::~AddressSpaceEditor()
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::refresh()
//-----------------------------------------------------------------------------
void AddressSpaceEditor::refresh()
{
    QStringList masterInterfaceList = component()->getMasterInterfaces(addrSpace_->name());

    nameEditor_.refresh();
	generalEditor_.refresh(masterInterfaceList);
	segmentsEditor_.refresh();
	localMemMapEditor_.refresh();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::showEvent()
//-----------------------------------------------------------------------------
void AddressSpaceEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/addressspace.html");
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceEditor::setupLayout()
//-----------------------------------------------------------------------------
void AddressSpaceEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QVBoxLayout* scrollLayout = new QVBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
    
    QHBoxLayout* nameAndGeneralLayout = new QHBoxLayout();
    nameAndGeneralLayout->addWidget(&nameEditor_, 0, Qt::AlignTop);
    nameAndGeneralLayout->addWidget(&generalEditor_);
    topLayout->addLayout(nameAndGeneralLayout);
    topLayout->addWidget(&segmentsEditor_, 1);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* bottomWidget = new QWidget(scrollArea);
    bottomWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* bottomLayout = new QVBoxLayout(bottomWidget);
    bottomLayout->addWidget(&localMemMapEditor_);
    bottomLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topWidget);
    verticalSplitter->addWidget(bottomWidget);
    verticalSplitter->setStretchFactor(1, 1);

    QSplitterHandle* handle = verticalSplitter->handle(1);
    QVBoxLayout* handleLayout = new QVBoxLayout(handle);
    handleLayout->setSpacing(0);
    handleLayout->setMargin(0);

    QFrame* line = new QFrame(handle);
    line->setLineWidth(2);
    line->setMidLineWidth(2);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    handleLayout->addWidget(line);

    verticalSplitter->setHandleWidth(10);

    scrollArea->setWidget(verticalSplitter);
}
