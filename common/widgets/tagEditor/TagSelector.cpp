//-----------------------------------------------------------------------------
// File: TagSelector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2019
//
// Description:
// Selector for tags.
//-----------------------------------------------------------------------------

#include "TagSelector.h"

#include <common/widgets/tagEditor/TagLabel.h>
#include <common/widgets/tagEditor/TagManager.h>
#include <common/widgets/tagEditor/TagCompleterModel.h>

//-----------------------------------------------------------------------------
// Function: TagSelector::TagSelector()
//-----------------------------------------------------------------------------
TagSelector::TagSelector(TagLabel* tagLabel, QWidget* parent):
TagDisplay(tagLabel, parent),
nameEdit_(new QComboBox(this))
{
    setupEditors(nameEdit_);
}

//-----------------------------------------------------------------------------
// Function: TagSelector::setupEditors()
//-----------------------------------------------------------------------------
void TagSelector::setupEditors(QWidget* nameEditor)
{
    TagDisplay::setupEditors(nameEditor);

    TagCompleterModel* nameSelectorModel = new TagCompleterModel(TagManager::getInstance().getTags(), nameEdit_);

    connect(nameEdit_, SIGNAL(currentIndexChanged(int)),
        nameSelectorModel, SLOT(unfilteredItemSelected(int)), Qt::UniqueConnection);
    connect(nameSelectorModel, SIGNAL(selectedColor(QColor const&)),
        this, SLOT(completerColorChange(QColor const&)), Qt::UniqueConnection);

    nameEdit_->setModel(nameSelectorModel);
    nameEdit_->setFixedWidth(80);

    int currentItemIndex = nameEdit_->findText(getEditedLabel()->text());
    if (currentItemIndex < 0)
    {
        currentItemIndex = 0;
    }

    nameEdit_->setCurrentIndex(currentItemIndex);
}

//-----------------------------------------------------------------------------
// Function: TagSelector::getNewName()
//-----------------------------------------------------------------------------
QString TagSelector::getNewName() const
{
    return nameEdit_->currentText();
}
