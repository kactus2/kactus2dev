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
	instantiationSelection_(new QLabel),
	instantiationLanguage_(new QLabel),
    fileSetSelection_(new QComboBox(this)),
    fileSetWarningLabel_(new QLabel)
{
	// Make view selection its own layout.
	QFormLayout* viewSelectionLayout = new QFormLayout();
    viewSelectionLayout->addRow(tr("Select view:"), viewSelection_);

	// The names of available views are the view selection items.
    viewSelection_->addItems(configuration_->viewNames());

    // Select an instantiation in conjunction with view.
    viewSelectionLayout->addRow(tr("The component instantiation:"), instantiationSelection_);
    viewSelectionLayout->addRow(tr("Language of the instantiation:"), instantiationLanguage_);

	// Checkable group box used to include generated file in the IP-XACT component.
	addToFileset_->setCheckable(true);
	addToFileset_->setChecked(configuration->getSaveToFileset() != 0);

	// It will have its own sub layout.
	QVBoxLayout* filesetLayout = new QVBoxLayout();
	addToFileset_->setLayout(filesetLayout);

	// Widgets for choosing the file set.
	QFormLayout* filesetSelectionLayout = new QFormLayout();
	filesetLayout->addLayout(filesetSelectionLayout);
	filesetSelectionLayout->addRow(tr("Select file set:"), fileSetSelection_);
	// Is editable, in case a custom entry is desired.
	fileSetSelection_->setEditable(true);

	// Widget for warning messages.
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
		// Must have a file set as well.
		if (configuration_->getFileSetName().isEmpty())
		{
			fileSetWarningLabel_->setText("<b>Please define a file set.</b>");
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
	configuration_->setView(selectedViewName);

    // Affects the instantiation.
    instantiationSelection_->setText(configuration_->getInstantiationName());

    // Update the language.
    setLanguage(configuration_->getCurrentLanguage());

    // Affects the available file sets.
    fileSetSelection_->clear();
    fileSetSelection_->addItems(*(configuration_->fileSetNames()));
    fileSetSelection_->setCurrentIndex(0);
    onFileSetChanged(fileSetSelection_->currentText());

    emit viewChanged();
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
	if (!configuration_->setFileSet(fileSetName) && !fileSetName.isEmpty())
	{
		// Warn user that a new file set will be created.
		fileSetWarningLabel_->setText(tr("New file set '%1' will be created.").arg(fileSetName));
	}
	else
	{
		fileSetWarningLabel_->setText("");
	}
}
