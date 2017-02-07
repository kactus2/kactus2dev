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

#include <QFormLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::ViewSelectionWidget()
//-----------------------------------------------------------------------------
ViewSelectionWidget::ViewSelectionWidget(QSharedPointer<ViewSelection> configuration) : 
	configuration_(configuration),
    viewSelection_(new QComboBox(this)),
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
    // The names of available file sets are the file set selection items.
    fileSetSelection_->addItems(configuration_->fileSetNames());

    // Select an instantiation and the file set in conjunction with view.
    viewSelectionLayout->addRow(tr("The component instantiation:"), instantiationSelection_);
    viewSelectionLayout->addRow(tr("Language of the instantiation:"), instantiationLanguage_);
    viewSelectionLayout->addRow(tr("Select file set:"), fileSetSelection_);
    // Is editable, in case a new entry is desired.
    fileSetSelection_->setEditable(true);

    QPalette p = fileSetSelection_->palette();
    p.setColor(QPalette::Base, QColor("LemonChiffon"));
    fileSetSelection_->setPalette(p);

	// Add everything it their proper position in the final layout.
	QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(viewSelectionLayout);
    // Widget for warning messages.
    topLayout->addWidget(fileSetWarningLabel_);

	// Connect the relevant events to their handler functions.
    connect(viewSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onViewChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);

	// Get the selected view.
    int defaultViewIndex = viewSelection_->findText(configuration_->getViewName());

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
    int defaultFileSetIndex = fileSetSelection_->findText(configuration_->getFileSetName());

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
    instantiationSelection_->setText(configuration_->getInstantiationName());
    setLanguage(configuration_->getCurrentLanguage());
}

//-----------------------------------------------------------------------------
// Function: ViewSelectionWidget::~ViewSelectionWidget()
//-----------------------------------------------------------------------------
ViewSelectionWidget::~ViewSelectionWidget()
{
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

    // Affects the selected file set.
    int defaultFileSetIndex = fileSetSelection_->findText(configuration_->getFileSetName());

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
    if (configuration_->getTargetLanguage().isEmpty())
    {
        return;
    }

	// Select color depending on match.
	if (selectedLanguage.toLower() == configuration_->getTargetLanguage().toLower())
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
