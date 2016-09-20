//-----------------------------------------------------------------------------
// File: ViewSelectionWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 14.06.2016
//
// Description:
// Dialog for selecting a view, component instantiation and file set for generation.
//-----------------------------------------------------------------------------

#include "ViewSelectionWidget.h"
#include "ViewSelection.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <IPXACTmodels/Component/View.h>

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::ViewSelectionWidget()
//-----------------------------------------------------------------------------
ViewSelectionWidget::ViewSelectionWidget(QSharedPointer<ViewSelection> configuration) : 
	configuration_(configuration),
    viewSelection_(new QComboBox(this)),
    addToFileset_(new QGroupBox(tr("Add file to fileset"))),
	instantiationSelection_(new QComboBox(this)),
	instantiationLanguage_(new QLabel),
    fileSetSelection_(new QComboBox(this)),
    instantiationWarningLabel_(new QLabel),
    fileSetWarningLabel_(new QLabel)
{
	// Make view selection its own layout.
	QFormLayout* viewSelectionLayout = new QFormLayout();
	viewSelectionLayout->addRow(tr("Select view:"), viewSelection_);

	// The names of available views are the view selection items.
	viewSelection_->addItems(configuration_->viewNames());

	// Checkable group box used to include generated file in the IP-XACT component.
	addToFileset_->setCheckable(true);
	addToFileset_->setChecked(configuration->getSaveToFileset() != 0);

	// It will have its own sub layout.
	QVBoxLayout* filesetLayout = new QVBoxLayout();
	addToFileset_->setLayout(filesetLayout);

	// Widgets for choosing the component instantiation and the file set.
	QFormLayout* selectionLayout = new QFormLayout();
	filesetLayout->addLayout(selectionLayout);
	selectionLayout->addRow(tr("Set component instantiation:"), instantiationSelection_);
	selectionLayout->addRow(tr("Language of the instantiation:"), instantiationLanguage_);
	selectionLayout->addRow(tr("Select file set:"), fileSetSelection_);
	// Both are editable, in case a custom entry is desired.
	instantiationSelection_->setEditable(true);
	fileSetSelection_->setEditable(true);

	// The names of available component instantiations are items.
	instantiationSelection_->addItems(configuration_->instantiationNames());

	// Widgets for warning messages.
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
		this, SLOT(onInstantiationChanged(QString const&)), Qt::UniqueConnection);
	connect(instantiationSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onInstantiationChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);

	// Initially at the top of the list.
    QString defaultView = configuration_->getDefaultView();

    if (defaultView.isEmpty())
    {
	    viewSelection_->setCurrentIndex(0);
    }
    else
    {
        viewSelection_->setCurrentIndex(viewSelection_->findText(defaultView));
    }

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
	// If the file is not saved to a file set, no selections needed.
	if (configuration_->getSaveToFileset())
	{
		// If the configuration is clueless about the instantiation name, that is bad.
		if (configuration_->getInstantiationName().isEmpty())
		{
			instantiationWarningLabel_->setText("Please define a component instantiation.");
			return false;
		}

		// Must have a file set as well.
		if (configuration_->getFileSetName().isEmpty())
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
	// Pass the selection to model, get the component instantiation reference.
	QString cimpRef = configuration_->setView(selectedViewName);

	// For convenience, set the referred component instantiation as the default choice.
	int index = instantiationSelection_->findText(cimpRef);

	if (index != -1)
    {
        instantiationSelection_->setItemData(instantiationSelection_->currentIndex(),
           QApplication::palette().brush(QPalette::Base), Qt::BackgroundRole);
		instantiationSelection_->setCurrentIndex(index);
		instantiationSelection_->setItemData(index, QColor(Qt::green), Qt::BackgroundRole);
    }

    emit viewChanged();
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onInstantiationChanged()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::onInstantiationChanged(QString const& selectedInstantiationName)
{
    if (!configuration_->setInstantiation(selectedInstantiationName) && !selectedInstantiationName.isEmpty())
    {
        // Warn user that a new instantiation will be created.
        instantiationWarningLabel_->setText(tr("<b>The view will be associated with new component instantiation '%1'.</b>").arg(
            selectedInstantiationName));
    }
    else if (configuration_->getView()->getComponentInstantiationRef() != selectedInstantiationName
        && !selectedInstantiationName.isEmpty())
    {
        // Clear the warning.
        instantiationWarningLabel_->setText(tr("<b>The view will be associated with component instantiation '%1'.</b>").arg(
            selectedInstantiationName));
    }
    else
    {
        // Clear the warning.
        instantiationWarningLabel_->setText("");
    }

    // Update the language.
    setLanguage(configuration_->getCurrentLanguage());

	// Affects the available file sets.
	fileSetSelection_->clear();
	fileSetSelection_->addItems(*(configuration_->fileSetNames()));
	fileSetSelection_->setCurrentIndex(0);
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::setLanguage()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::setLanguage(QString selectedLanguage)
{
    if (selectedLanguage.isEmpty())
    {
        // Make it more notable, if the selected language is empty
        instantiationLanguage_->setText("Not specified");
    }
    else
    {
        // Inform the language setting to user.
        instantiationLanguage_->setText(selectedLanguage);
    }

    // No target language means no further reaction.
    if (configuration_->getTargetLanguage().isEmpty())
    {
        return;
    }

	// Select color depending on match.
	if (selectedLanguage == configuration_->getTargetLanguage())
	{
		instantiationLanguage_->setStyleSheet("QLabel { color : green; }");
	}
	else
	{
		instantiationLanguage_->setStyleSheet("QLabel { color : red; }");
	}
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onFileSetChanged()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::onFileSetChanged(QString const& fileSetName)
{
	// Set to the configuration.
	if (!fileSetName.isEmpty() && !configuration_->setFileSet(fileSetName))
	{
		// Warn user that a new file set will be created.
		fileSetWarningLabel_->setText(tr("New file set '%1' will be created.").arg(fileSetName));
	}
	else
	{
		fileSetWarningLabel_->setText("");
	}
}
