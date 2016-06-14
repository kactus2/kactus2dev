//-----------------------------------------------------------------------------
// File: ViewSelectionWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 14.06.2016
//
// Description:
// Dialog for selecting a view, component instantiation and file set for generation.
//-----------------------------------------------------------------------------

#include "ViewSelectionWidget.h"
#include "GeneratorConfiguration.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::ViewSelectionWidget()
//-----------------------------------------------------------------------------
ViewSelectionWidget::ViewSelectionWidget(QSharedPointer<GeneratorConfiguration> configuration,
	QSharedPointer<QList<QSharedPointer<View> > > views, 
	QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations) : 
	configuration_(configuration),
    viewSelection_(new QComboBox(this)),
    addToFileset_(new QGroupBox(tr("Add file to fileset"))),
    instantiationSelection_(new QComboBox(this)),
    fileSetSelection_(new QComboBox(this)),
    instantiationWarningLabel_(new QLabel),
    fileSetWarningLabel_(new QLabel)
{
	// Make view selection its own layout.
	QFormLayout* viewSelectionLayout = new QFormLayout();
	viewSelectionLayout->addRow(tr("Select view:"), viewSelection_);

	// Set names as the of views as items, but also track the view objects.
	foreach (QSharedPointer<View> currentView, *views)
	{
		views_[currentView->name()] = currentView;
		viewSelection_->addItem(currentView->name());
	}

	// Checkable group box used to include generated file in the IP-XACT component.
	addToFileset_->setCheckable(true);
	addToFileset_->setChecked(configuration->getSaveToFileset() != 0);

	// It will have its own sub layout.
	QFormLayout* filesetLayout = new QFormLayout();
	addToFileset_->setLayout(filesetLayout);

	// Widgets for choosing configuration instantiation and the file set.
	filesetLayout->addRow(tr("Select component instantiation:"), instantiationSelection_);
	filesetLayout->addRow(tr("Select file set:"), fileSetSelection_);
	// Both are editable, in case a custom entry is desired.
	instantiationSelection_->setEditable(true);
	fileSetSelection_->setEditable(true);

	// Set names as the of instantiations as items, but also track the instantiation objects.
	foreach (QSharedPointer<ComponentInstantiation> cimp, *instantiations)
	{
		instantiations_[cimp->name()] = cimp;
		instantiationSelection_->addItem(cimp->name());
	}

	// Widgets for warning messages
	filesetLayout->addWidget(instantiationWarningLabel_);
	filesetLayout->addWidget(fileSetWarningLabel_);

	// Add everything it their proper position in the final layout.
	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(viewSelectionLayout);
    topLayout->addWidget(addToFileset_);

	// Connect the relevant events to their handler functions.
    connect(addToFileset_, SIGNAL(toggled(bool)), 
        this, SLOT(onFileSetStateChanged(bool)), Qt::UniqueConnection);
    connect(viewSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onViewChanged(QString const&)), Qt::UniqueConnection);
	connect(instantiationSelection_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onInstantiationInserted(QString const&)), Qt::UniqueConnection);
	connect(instantiationSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onInstantiationChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);

	// Initially at the top of the list.
	viewSelection_->setCurrentIndex(0);

	// Finally, evaluate the fields.
	onViewChanged(viewSelection_->currentText());
	onInstantiationChanged(instantiationSelection_->currentText());
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::~ViewSelectionWidget()
//-----------------------------------------------------------------------------
ViewSelectionWidget::~ViewSelectionWidget()
{

}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onFileSetStateChanged()
//-----------------------------------------------------------------------------
bool ViewSelectionWidget::validSelections() const
{
	if (configuration_->getSaveToFileset())
	{
		if (!configuration_->getInstantiation() || instantiationSelection_->currentText().isEmpty())
		{
			instantiationWarningLabel_->setText("Please define a component instantiation.");
			return false;
		}

		if (fileSetSelection_->currentText().isEmpty())
		{
			fileSetWarningLabel_->setText("Please define a file set.");
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onFileSetStateChanged()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::onFileSetStateChanged(bool on)
{
    configuration_->setSaveToFileset(on);
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onViewChanged()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::onViewChanged(QString const& selectedViewName)
{
	// Get matching view from the list.
	QSharedPointer<View> selectedView = views_[selectedViewName];

	// Should exist.
	if (selectedView)
	{
		// Set to the configuration.
		configuration_->setActiveView(selectedView);

		// For convenience, set the matching component instantiation as the default choice.
		int index = instantiationSelection_->findText(selectedView->getComponentInstantiationRef());

		if (index != -1)
		{
			instantiationSelection_->setCurrentIndex(index);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onInstantiationInserted()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::onInstantiationInserted(QString const& selectedInstantiationName)
{
	// Get matching instantiation from the list.
	QSharedPointer<ComponentInstantiation> selectedInstantiation =
		instantiations_.value(selectedInstantiationName);

	if (!selectedInstantiation && !selectedInstantiationName.isEmpty())
	{
		// Create a new one if does not exist.
		selectedInstantiation = QSharedPointer<ComponentInstantiation>( new ComponentInstantiation );
		selectedInstantiation->setName(selectedInstantiationName);

		// Set it as the selected instantiation.
		configuration_->setInstantiation(selectedInstantiation);

		// Warn user that a new instantiation will be created.
		instantiationWarningLabel_->setText(tr("New component instantiation '%1' will be created.").arg(
            selectedInstantiationName));
	}
	else
	{
		instantiationWarningLabel_->setText("");
	}
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onInstantiationChanged()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::onInstantiationChanged(QString const& selectedInstantiationName)
{
	// Get matching instantiation from the list.
	QSharedPointer<ComponentInstantiation> selectedInstantiation =
		instantiations_.value(selectedInstantiationName);

	if (selectedInstantiation)
	{
		// Set to the configuration.
		configuration_->setInstantiation(selectedInstantiation);

		// Affects the available file sets.
		fileSetSelection_->clear();
		fileSetSelection_->addItems(*(selectedInstantiation->getFileSetReferences()));
		fileSetSelection_->setCurrentIndex(0);
		onFileSetChanged(fileSetSelection_->currentText());
	}
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onFileSetChanged()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::onFileSetChanged(QString const& fileSetName)
{
	// Set to the configuration.
	configuration_->setFileSetRef(fileSetName);

	if (!fileSetName.isEmpty() && ( !configuration_->getInstantiation() ||
		!configuration_->getInstantiation()->getFileSetReferences()->contains(fileSetName)))
	{
		// Warn user that a new file set will be created.
		fileSetWarningLabel_->setText(tr("New file set '%1' will be created.").arg(fileSetName));
	}
	else
	{
		fileSetWarningLabel_->setText("");
	}
}
