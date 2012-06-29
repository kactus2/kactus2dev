//-----------------------------------------------------------------------------
// File: ApiInterfaceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.4.2012
//
// Description:
// API interface editor for the component editor.
//-----------------------------------------------------------------------------

#include "ApiInterfaceEditor.h"

#include <LibraryManager/libraryinterface.h>
#include <models/ApiInterface.h>

#include <QScrollArea>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::ApiInterfaceEditor()
//-----------------------------------------------------------------------------
ApiInterfaceEditor::ApiInterfaceEditor(LibraryInterface* libHandler,
                                       QSharedPointer<Component> component,
                                       QSharedPointer<ApiInterface> APIInterface,
									   QWidget *parent):
ItemEditor(component, parent),
libInterface_(libHandler),
apiIf_(APIInterface.data()),
nameEditor_(APIInterface->getNameGroup(), this, tr("Name and description")),
apiType_(VLNV::APIDEFINITION, libHandler, this, this),
detailsGroup_(tr("Details"), this),
dependencyCombo_(this)
{
    Q_ASSERT(APIInterface != 0);
    Q_ASSERT(libHandler != 0);
    
    // Set VLNV editor settings.
    apiType_.setTitle(tr("API definition"));
    apiType_.setMandatory(false);

    // Initialize the details group.
    QLabel* directionLabel = new QLabel(tr("Dependency:"), &detailsGroup_);
    dependencyCombo_.addItem(tr("requester"));
    dependencyCombo_.addItem(tr("provider"));
    dependencyCombo_.setCurrentIndex(0);

    QGridLayout* detailsLayout = new QGridLayout(&detailsGroup_);
    detailsLayout->addWidget(directionLabel, 0, 0, 1, 1);
    detailsLayout->addWidget(&dependencyCombo_, 0, 1, 1, 1);

    detailsLayout->setColumnMinimumWidth(1, 100);
    detailsLayout->setColumnStretch(2, 1);

    // Create the scroll area.
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
	scrollLayout->setContentsMargins(0, 0, 0, 0);

    // Create the top widget and set it under the scroll area.
    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Create the layout for the actual editor.
    QVBoxLayout* layout = new QVBoxLayout(topWidget);
    layout->addWidget(&nameEditor_);
    layout->addWidget(&apiType_);
    layout->addWidget(&detailsGroup_);
    layout->addStretch();
	layout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(topWidget);

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&apiType_, SIGNAL(vlnvEdited()),
		this, SLOT(onAPITypeChange()), Qt::UniqueConnection);
	connect(&dependencyCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onDependencyChange(int)), Qt::UniqueConnection);

    refresh();
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::~ApiInterfaceEditor()
//-----------------------------------------------------------------------------
ApiInterfaceEditor::~ApiInterfaceEditor()
{
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::isValid()
//-----------------------------------------------------------------------------
bool ApiInterfaceEditor::isValid() const
{
    return (nameEditor_.isValid() &&
            (apiType_.isEmpty() || (apiType_.isValid() && libInterface_->contains(apiType_.getVLNV()))));
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::makeChanges()
//-----------------------------------------------------------------------------
void ApiInterfaceEditor::makeChanges()
{
    // TODO remove this in final
}

void ApiInterfaceEditor::refresh() {
	nameEditor_.refresh();

	apiType_.setVLNV(apiIf_->getApiType());

	// the signal must be disconnected for the duration of changing the selected index
	disconnect(&dependencyCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onDependencyChange(int)));
	dependencyCombo_.setCurrentIndex(apiIf_->getDependencyDirection());
	connect(&dependencyCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onDependencyChange(int)), Qt::UniqueConnection);
}

void ApiInterfaceEditor::onAPITypeChange() {
	apiIf_->setApiType(apiType_.getVLNV());
	emit contentChanged();
}

void ApiInterfaceEditor::onDependencyChange( int index ) {
	apiIf_->setDependencyDirection(static_cast<DependencyDirection>(dependencyCombo_.currentIndex()));
	emit contentChanged();
}
