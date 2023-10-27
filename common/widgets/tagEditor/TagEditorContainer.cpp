//-----------------------------------------------------------------------------
// File: TagEditorContainer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2019
//
// Description:
// Container for editable tags.
//-----------------------------------------------------------------------------

#include "TagEditorContainer.h"

#include <common/widgets/tagEditor/TagLabel.h>
#include <common/widgets/tagEditor/TagEditor.h>

#include <QLayout>

//-----------------------------------------------------------------------------
// Function: TagEditorContainer::TagEditorContainer()
//-----------------------------------------------------------------------------
TagEditorContainer::TagEditorContainer(QWidget* parent):
TagContainer(parent)
{

}

//-----------------------------------------------------------------------------
// Function: TagEditorContainer::constructTagEditor()
//-----------------------------------------------------------------------------
TagDisplay* TagEditorContainer::constructTagEditor(TagLabel* editedLabel)
{
    auto newTagEditor(new TagEditor(editedLabel, this));

    connect(newTagEditor, SIGNAL(acceptChanges(TagLabel*, TagDisplay*)),
        this, SLOT(itemChangesAccepted(TagLabel*, TagDisplay*)), Qt::UniqueConnection);
    connect(newTagEditor, SIGNAL(deleteItem(TagLabel*, TagDisplay*)),
        this, SLOT(itemDeleted(TagLabel*, TagDisplay*)), Qt::UniqueConnection);

    layout()->addWidget(newTagEditor);

    return newTagEditor;
}
