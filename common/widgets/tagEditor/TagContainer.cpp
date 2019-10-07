//-----------------------------------------------------------------------------
// File: TagContainer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.08.2019
//
// Description:
// Editor for tags.
//-----------------------------------------------------------------------------

#include "TagContainer.h"

#include <common/widgets/tagEditor/TagLabel.h>
#include <common/widgets/tagEditor/TagDisplay.h>
#include <common/widgets/tagEditor/FlowLayout.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: TagContainer::TagContainer()
//-----------------------------------------------------------------------------
TagContainer::TagContainer(QWidget* parent):
QWidget(parent),
additionTag_(new TagLabel(QLatin1String("+"), this)),
tags_()
{
    additionTag_->setToolTip(QLatin1String("Create new tag"));
    connect(additionTag_, SIGNAL(clicked(TagLabel*)), this, SLOT(createNewTag()), Qt::UniqueConnection);

    setupLayout();
    layout()->addWidget(additionTag_);
}

//-----------------------------------------------------------------------------
// Function: TagContainer::setupTags()
//-----------------------------------------------------------------------------
void TagContainer::setupTags(QVector<TagData> documentTags)
{
    layout()->removeWidget(additionTag_);
    for (auto oldTag : tags_)
    {
        layout()->removeWidget(oldTag);
        oldTag->deleteLater();
    }

    tags_.clear();

    for (auto tag : documentTags)
    {
        TagLabel* newTag(new TagLabel(tag.name_, this, tag.color_));
        connectTagLabel(newTag);

        tags_.append(newTag);
        layout()->addWidget(newTag);
    }

    layout()->addWidget(additionTag_);
}

//-----------------------------------------------------------------------------
// Function: TagContainer::connectTagLabel()
//-----------------------------------------------------------------------------
void TagContainer::connectTagLabel(QWidget* tagItem)
{
    connect(tagItem, SIGNAL(clicked(TagLabel*)), this, SLOT(itemClicked(TagLabel*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: TagContainer::setupLayout()
//-----------------------------------------------------------------------------
void TagContainer::setupLayout()
{
    FlowLayout* layout = new FlowLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: TagContainer::createNewTag()
//-----------------------------------------------------------------------------
void TagContainer::createNewTag()
{
    layout()->removeWidget(additionTag_);

    TagLabel* newLabel = new TagLabel(QLatin1String("unnamed"), this);
    connectTagLabel(newLabel);
    layout()->addWidget(newLabel);

    tags_.append(newLabel);

    layout()->addWidget(additionTag_);

    itemClicked(newLabel);
}

//-----------------------------------------------------------------------------
// Function: TagContainer::itemClicked()
//-----------------------------------------------------------------------------
void TagContainer::itemClicked(TagLabel* clickedItem)
{
    QVector<QWidget*> temporaryTags;

    int itemIndex = tags_.indexOf(clickedItem);
    for (int i = itemIndex + 1; i < tags_.size(); ++i)
    {
        temporaryTags.append(tags_.at(i));
        layout()->removeWidget(tags_.at(i));
    }

    layout()->removeWidget(additionTag_);

    clickedItem->hide();
    layout()->removeWidget(clickedItem);

    TagDisplay* newTagEditor = constructTagEditor(clickedItem);
    for (auto newTag : temporaryTags)
    {
        layout()->addWidget(newTag);
    }

    layout()->addWidget(additionTag_);
    tags_.replace(itemIndex, newTagEditor);

    newTagEditor->setFocus(Qt::PopupFocusReason);
}

//-----------------------------------------------------------------------------
// Function: TagContainer::itemChangesAccepted()
//-----------------------------------------------------------------------------
void TagContainer::itemChangesAccepted(TagLabel* editedTag, TagDisplay* tagEditor)
{
    if (tagExists(editedTag))
    {
        layout()->removeWidget(tagEditor);
        editedTag->deleteLater();
        tagEditor->deleteLater();

        tags_.removeAt(tags_.indexOf(tagEditor));
    }
    else
    {
        editedTag->show();

        int tagEditorIndex = tags_.indexOf(tagEditor);
        tags_.replace(tagEditorIndex, editedTag);

        QVector<QWidget*> temporaryTags;
        for (int i = tagEditorIndex; i < tags_.size(); ++i)
        {
            temporaryTags.append(tags_.at(i));
            layout()->removeWidget(tags_.at(i));
        }
        layout()->removeWidget(additionTag_);

        layout()->addWidget(editedTag);
        for (auto oldTag : temporaryTags)
        {
            layout()->addWidget(oldTag);
        }
        layout()->addWidget(additionTag_);

        tagEditor->deleteLater();
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: TagContainer::tagExists()
//-----------------------------------------------------------------------------
bool TagContainer::tagExists(TagLabel* editedTag) const
{
    for (auto comparisonWidget : tags_)
    {
        TagLabel* comparisonTag = dynamic_cast<TagLabel*>(comparisonWidget);
        if (comparisonTag && comparisonTag->text().compare(editedTag->text()) == 0)
        {
            comparisonTag->setPalette(editedTag->palette());
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: TagContainer::itemDeleted()
//-----------------------------------------------------------------------------
void TagContainer::itemDeleted(TagLabel* editedTag, TagDisplay* tagEditor)
{
    tags_.removeAt(tags_.indexOf(tagEditor));

    layout()->removeWidget(editedTag);
    layout()->removeWidget(tagEditor);
    editedTag->deleteLater();
    tagEditor->deleteLater();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: TagContainer::getTags()
//-----------------------------------------------------------------------------
QVector<TagData> TagContainer::getTags() const
{
    QVector<TagData> finishedTags;

    for (auto tagWidget : tags_)
    {
        TagLabel* tag = dynamic_cast<TagLabel*>(tagWidget);
        if (tag)
        {
            QColor tagColor = tag->palette().color(QPalette::Window);
            TagData newTagData({ tag->text(), tagColor.name() });

            finishedTags.append(newTagData);
        }
    }

    return finishedTags;
}
