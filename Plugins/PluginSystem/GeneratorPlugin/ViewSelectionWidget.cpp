//-----------------------------------------------------------------------------
// File: ViewSelectionWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 14.06.2016
//
// Description:
// Widget for selecting a view, component instantiation and file set for generation.
//-----------------------------------------------------------------------------

#include "ViewSelectionWidget.h"
#include "ViewSelection.h"

#include <common/KactusColors.h>

#include <QFormLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::ViewSelectionWidget()
//-----------------------------------------------------------------------------
ViewSelectionWidget::ViewSelectionWidget(QSharedPointer<ViewSelection> model) : 
	model_(model),
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
    // Select an instantiation and the file set in conjunction with view.
    viewSelectionLayout->addRow(tr("The component instantiation:"), instantiationSelection_);
    viewSelectionLayout->addRow(tr("Language of the instantiation:"), instantiationLanguage_);

    // The names of available views are the view selection items.
    viewSelection_->addItems(model_->viewNames());
    // The names of available file sets are the file set selection items.
    fileSetSelection_->addItems(model_->fileSetNames());

    // Checkable group box used to include generated file in the IP-XACT component.
    addToFileset_->setCheckable(true);
    addToFileset_->setChecked(model->getSaveToFileset() != 0);

    // Is editable, in case a new entry is desired.
    fileSetSelection_->setEditable(true);

    QPalette p = fileSetSelection_->palette();
    p.setColor(QPalette::Base, KactusColors::MANDATORY_FIELD);
    fileSetSelection_->setPalette(p);

	// It will have its own sub layout.
	QFormLayout* filesetLayout = new QFormLayout();
    addToFileset_->setLayout(filesetLayout);
    filesetLayout->addRow(tr("Select file set:"), fileSetSelection_);

	// Add everything it their proper position in the final layout.
	QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(viewSelectionLayout);
    topLayout->addWidget(addToFileset_);
    // Widget for warning messages.
    topLayout->addWidget(fileSetWarningLabel_);

	// Connect the relevant events to their handler functions.
    connect(addToFileset_, SIGNAL(toggled(bool)), 
        this, SLOT(onFileSetStateChanged(bool)), Qt::UniqueConnection);
    connect(viewSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onViewChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);

	// Get the selected view.
    int defaultViewIndex = viewSelection_->findText(model_->getViewName());

    if (defaultViewIndex == -1)
    {
        // If none, just pick the topmost.
	    viewSelection_->setCurrentIndex(0);
    }
    else
    {
        // Else set the selection accordingly.
        viewSelection_->setCurrentIndex(defaultViewIndex);
    }

    // Affects the selected file set.
    int defaultFileSetIndex = fileSetSelection_->findText(model_->getFileSetName());

    if (defaultFileSetIndex == -1)
    {
        // If none, just pick the topmost.
        fileSetSelection_->setCurrentIndex(0);
    }
    else
    {
        // Else set the selection accordingly.
        fileSetSelection_->setCurrentIndex(defaultFileSetIndex);
    }

    // Set the instantiation and the language.
    instantiationSelection_->setText(model_->getInstantiationName());
    setLanguage(model_->getCurrentLanguage());
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
void ViewSelectionWidget::onFileSetStateChanged(bool on)
{
    addToFileset_->setChecked(on);
    model_->setSaveToFileset(on);
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onViewChanged()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::onViewChanged(QString const& selectedViewName)
{
	// Pass the selection to model, get the component instantiation reference.
	model_->setView(selectedViewName);

    // Affects the instantiation.
    instantiationSelection_->setText(model_->getInstantiationName());

    // Update the language.
    setLanguage(model_->getCurrentLanguage());

    // Affects the selected file set.
    int defaultFileSetIndex = fileSetSelection_->findText(model_->getFileSetName());

    if (defaultFileSetIndex == -1)
    {
        // If none, just pick the topmost.
        fileSetSelection_->setCurrentIndex(0);
    }
    else
    {
        // Else set the selection accordingly.
        fileSetSelection_->setCurrentIndex(defaultFileSetIndex);
    }

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
    if (model_->getTargetLanguage().isEmpty())
    {
        return;
    }

	// Select color depending on match.
	if (selectedLanguage.toLower() == model_->getTargetLanguage().toLower())
	{
		instantiationLanguage_->setStyleSheet("QLabel { color : green; }");
	}
	else
	{
		instantiationLanguage_->setStyleSheet("QLabel { color : rgb(255, 102, 45); }");
	}
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::onFileSetChanged()
//-----------------------------------------------------------------------------
void ViewSelectionWidget::onFileSetChanged(QString const& fileSetName)
{
	// Set to the configuration.
	if (!model_->setFileSet(fileSetName) && !fileSetName.isEmpty())
	{
		// Warn user that a new file set will be created.
		fileSetWarningLabel_->setText(tr("New file set '%1' will be created.").arg(fileSetName));
	}
	else
	{
		fileSetWarningLabel_->setText("");
	}
}
