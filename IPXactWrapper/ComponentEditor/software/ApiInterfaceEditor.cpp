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
									   QWidget *parent)
    : ItemEditor(component, parent),
      libInterface_(libHandler),
      apiIf_(APIInterface.data()),
      nameGroup_(this),
      apiType_(VLNV::APIDEFINITION, libHandler, this, this),
      detailsGroup_(tr("Details"), this),
      dependencyCombo_(this)
{
    Q_ASSERT(APIInterface != 0);
    Q_ASSERT(libHandler != 0);

    connect(&nameGroup_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&nameGroup_, SIGNAL(nameChanged(const QString&)),
        this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
    connect(&apiType_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dependencyCombo_, SIGNAL(currentIndexChanged(int)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    
    // Set name group and VLNV editor settings.
    nameGroup_.setTitle(tr("Name and description"));
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

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);

    // Create the top widget and set it under the scroll area.
    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Create the layout for the actual editor.
    QVBoxLayout* layout = new QVBoxLayout(topWidget);
    layout->addWidget(&nameGroup_);
    layout->addWidget(&apiType_);
    layout->addWidget(&detailsGroup_);
    layout->addStretch();

    scrollArea->setWidget(topWidget);

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
    return (nameGroup_.isValid() &&
            (apiType_.isEmpty() || (apiType_.isValid() && libInterface_->contains(apiType_.getVLNV()))));
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::removeModel()
//-----------------------------------------------------------------------------
void ApiInterfaceEditor::removeModel()
{
    component()->removeApiInterface(apiIf_);
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::makeChanges()
//-----------------------------------------------------------------------------
void ApiInterfaceEditor::makeChanges()
{
    apiIf_->setName(nameGroup_.getName());
    apiIf_->setDisplayName(nameGroup_.getDisplayName());
    apiIf_->setDescription(nameGroup_.getDescription());
    apiIf_->setApiType(apiType_.getVLNV());
    apiIf_->setDependencyDirection(static_cast<DependencyDirection>(dependencyCombo_.currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::restoreChanges()
//-----------------------------------------------------------------------------
void ApiInterfaceEditor::restoreChanges()
{
    nameGroup_.setName(apiIf_->getName());
    nameGroup_.setDisplayName(apiIf_->getDisplayName());
    nameGroup_.setDescription(apiIf_->getDescription());
    apiType_.setVLNV(apiIf_->getApiType());
    dependencyCombo_.setCurrentIndex(apiIf_->getDependencyDirection());
}

void ApiInterfaceEditor::refresh() {
	nameGroup_.setName(apiIf_->getName());
	nameGroup_.setDisplayName(apiIf_->getDisplayName());
	nameGroup_.setDescription(apiIf_->getDescription());
	apiType_.setVLNV(apiIf_->getApiType());
	dependencyCombo_.setCurrentIndex(apiIf_->getDependencyDirection());
}
