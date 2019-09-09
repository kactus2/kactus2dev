//-----------------------------------------------------------------------------
// File: TagSelectorContainer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2019
//
// Description:
// Container for displayed tags.
//-----------------------------------------------------------------------------

#include "TagSelectorContainer.h"

#include <common/widgets/tagEditor/TagLabel.h>
#include <common/widgets/tagEditor/TagSelector.h>

#include <QLayout>

//-----------------------------------------------------------------------------
// Function: TagSelectorContainer::TagSelectorContainer()
//-----------------------------------------------------------------------------
TagSelectorContainer::TagSelectorContainer(QWidget* parent):
TagContainer(parent)
{

}

//-----------------------------------------------------------------------------
// Function: TagSelectorContainer::constructTagEditor()
//-----------------------------------------------------------------------------
TagDisplay* TagSelectorContainer::constructTagEditor(TagLabel* editedLabel)
{
    TagSelector* newTagEditor(new TagSelector(editedLabel, this));

    connect(newTagEditor, SIGNAL(acceptChanges(TagLabel*, TagDisplay*)),
        this, SLOT(itemChangesAccepted(TagLabel*, TagDisplay*)), Qt::UniqueConnection);
    connect(newTagEditor, SIGNAL(deleteItem(TagLabel*, TagDisplay*)),
        this, SLOT(itemDeleted(TagLabel*, TagDisplay*)), Qt::UniqueConnection);

    layout()->addWidget(newTagEditor);

    return newTagEditor;
}
