//-----------------------------------------------------------------------------
// File: AddToFilesetWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.11.2025
//
// Description:
// Widget for selecting file set to add generated item into.
//-----------------------------------------------------------------------------

#include "AddToFilesetWidget.h"

#include <common/KactusColors.h>

#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: AddToFilesetWidget::AddToFilesetWidget()
//-----------------------------------------------------------------------------
AddToFilesetWidget::AddToFilesetWidget(QWidget* parent) : QGroupBox(tr("Add file to fileset"), parent)
{
    setCheckable(true);
    setChecked(false);

    // Is editable, in case a new entry is desired.
    fileSetSelection_.setEditable(true);

    QFormLayout* filesetLayout = new QFormLayout();
    setLayout(filesetLayout);
    filesetLayout->addRow(tr("Select or create new file set:"), &fileSetSelection_);
    filesetLayout->addWidget(&fileSetWarningLabel_);

    connect(&fileSetSelection_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);
    connect(&fileSetSelection_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AddToFilesetWidget::clear()
//-----------------------------------------------------------------------------
void AddToFilesetWidget::clear()
{
    setChecked(false);
    fileSetSelection_.clear();
}

//-----------------------------------------------------------------------------
// Function: AddToFilesetWidget::setExistingFileSets()
//-----------------------------------------------------------------------------
void AddToFilesetWidget::setExistingFileSets(QStringList const& fileSetNames)
{
    fileSetNames_ = fileSetNames;
    fileSetSelection_.clear();
    fileSetSelection_.addItems(fileSetNames);
}

//-----------------------------------------------------------------------------
// Function: AddToFilesetWidget::selectDefaultFileSet()
//-----------------------------------------------------------------------------
void AddToFilesetWidget::selectDefaultFileSet(QString const& defaultName)
{
    int defaultFileSetIndex = fileSetSelection_.findText(defaultName);

    if (defaultFileSetIndex == -1)
    {
        // If none, just pick the topmost.
        fileSetSelection_.setCurrentIndex(0);
    }
    else
    {
        // Else set the selection accordingly.
        fileSetSelection_.setCurrentIndex(defaultFileSetIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: AddToFilesetWidget::setChecked()
//-----------------------------------------------------------------------------
void AddToFilesetWidget::setChecked(bool checked)
{
    QGroupBox::setChecked(checked);
}

//-----------------------------------------------------------------------------
// Function: AddToFilesetWidget::currentSelection()
//-----------------------------------------------------------------------------
QString AddToFilesetWidget::currentSelection() const
{
    return fileSetSelection_.currentText();
}

//-----------------------------------------------------------------------------
// Function: AddToFilesetWidget::onFileSetChanged()
//-----------------------------------------------------------------------------
void AddToFilesetWidget::onFileSetChanged(QString const& fileSetName)
{
    if (!fileSetNames_.contains(fileSetName) && fileSetName.isEmpty() == false)
    {
        // Warn user that a new file set will be created.
        fileSetWarningLabel_.setText(tr("New file set '%1' will be created.").arg(fileSetName));
    }
    else
    {
        fileSetWarningLabel_.setText("");
    }

    emit fileSetChanged(fileSetName);
}
