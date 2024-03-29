//-----------------------------------------------------------------------------
// File: ApiInterfaceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 16.4.2012
//
// Description:
// API interface editor for the component editor.
//-----------------------------------------------------------------------------

#include "ApiInterfaceEditor.h"

#include <KactusAPI/include/LibraryInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>

#include <QApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::ApiInterfaceEditor()
//-----------------------------------------------------------------------------
ApiInterfaceEditor::ApiInterfaceEditor(LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    QSharedPointer<ApiInterface> APIInterface,
    QWidget *parent):
ItemEditor(component, libHandler, parent),
    libInterface_(libHandler),
    apiIf_(APIInterface),
    nameEditor_(APIInterface, component->getRevision(), this, tr("Name and description")),
    detailsGroup_(tr("Details"), this),
    dependencyCombo_(this)
{
    Q_ASSERT(APIInterface != 0);
    Q_ASSERT(libHandler != 0);

	// find the main window for VLNV editor
	QWidget* parentWindow = nullptr;
	for (QWidget* widget : QApplication::topLevelWidgets())
    {
		auto mainWnd = dynamic_cast<QMainWindow*>(widget);
		if (mainWnd)
        {
			parentWindow = mainWnd;
			break;
		}
	}

	apiType_ = new VLNVEditor(VLNV::APIDEFINITION, libHandler, parentWindow, this);
    
    // Set VLNV editor settings.
    apiType_->setTitle(tr("API definition"));
    apiType_->setMandatory(false);

	setupLayout();


	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(apiType_, SIGNAL(vlnvEdited()),
		this, SLOT(onAPITypeChange()), Qt::UniqueConnection);
	connect(&dependencyCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onDependencyChange(int)), Qt::UniqueConnection);

    refresh();
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::isValid()
//-----------------------------------------------------------------------------
bool ApiInterfaceEditor::isValid() const
{
    return (nameEditor_.isValid() &&
            (apiType_->isEmpty() || (apiType_->isValid() && libInterface_->contains(apiType_->getVLNV()))));
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::refresh()
//-----------------------------------------------------------------------------
void ApiInterfaceEditor::refresh()
{
	nameEditor_.refresh();

	apiType_->setVLNV(apiIf_->getApiType());

	// the signal must be disconnected for the duration of changing the selected index
	disconnect(&dependencyCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onDependencyChange(int)));
	dependencyCombo_.setCurrentIndex(apiIf_->getDependencyDirection());
	connect(&dependencyCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onDependencyChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::onAPITypeChange()
//-----------------------------------------------------------------------------
void ApiInterfaceEditor::onAPITypeChange()
{
	apiIf_->setApiType(apiType_->getVLNV());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::onDependencyChange()
//-----------------------------------------------------------------------------
void ApiInterfaceEditor::onDependencyChange(int /*index*/)
{
	apiIf_->setDependencyDirection(static_cast<DependencyDirection>(dependencyCombo_.currentIndex()));
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::showEvent()
//-----------------------------------------------------------------------------
void ApiInterfaceEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/apiinterface.html");
}

//-----------------------------------------------------------------------------
// Function: ApiInterfaceEditor::ApiInterfaceEditor()
//-----------------------------------------------------------------------------
void ApiInterfaceEditor::setupLayout()
{
    // Initialize the details group.
    auto directionLabel = new QLabel(tr("Dependency:"), &detailsGroup_);
    dependencyCombo_.addItem(tr("requester"));
    dependencyCombo_.addItem(tr("provider"));
    dependencyCombo_.setCurrentIndex(0);

    detailsGroup_.setFlat(true);
    auto detailsLayout = new QGridLayout(&detailsGroup_);
    detailsLayout->addWidget(directionLabel, 0, 0, 1, 1);
    detailsLayout->addWidget(&dependencyCombo_, 0, 1, 1, 1);
    detailsLayout->setRowStretch(1, 1);

    detailsLayout->setColumnMinimumWidth(1, 100);
    detailsLayout->setColumnStretch(2, 1);

    // Create the scroll area.
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    // Create the top widget and set it under the scroll area.
    auto topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Create the layout for the actual editor.
    auto layout = new QGridLayout(topWidget);
    layout->addWidget(&nameEditor_, 0, 0, 2, 1);
    layout->addWidget(apiType_, 0, 1, 1, 1, Qt::AlignTop);
    layout->addWidget(&detailsGroup_, 1, 1, 1, 1);
    layout->setRowStretch(2, 1);
    layout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(topWidget);
}