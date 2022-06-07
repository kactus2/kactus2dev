//-----------------------------------------------------------------------------
// File: TagManager.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.08.2019
//
// Description:
// Manager for loaded tags.
//-----------------------------------------------------------------------------

#include "TagManager.h"

//-----------------------------------------------------------------------------
// Function: TagManager::addNewTags()
//-----------------------------------------------------------------------------
void TagManager::addNewTags(QVector<TagData> newTags)
{
    for (auto tag : newTags)
    {
        if (!tagExists(tag))
        {
            tags_.append(tag);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: TagManager::tagExists()
//-----------------------------------------------------------------------------
bool TagManager::tagExists(TagData const& tag) const
{
    for (auto existingTag : tags_)
    {
        if (tag.name_.compare(existingTag.name_) == 0 && tag.color_.compare(existingTag.color_) == 0)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: TagManager::setTags()
//-----------------------------------------------------------------------------
void TagManager::setTags(QVector<TagData> newTags)
{
    clearTags();

    for (auto tag : newTags)
    {
        if (!tagExists(tag))
        {
            tags_.append(tag);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: TagManager::getTags()
//-----------------------------------------------------------------------------
QVector<TagData> TagManager::getTags() const
{
    return tags_;
}

//-----------------------------------------------------------------------------
// Function: TagManager::clearTags()
//-----------------------------------------------------------------------------
void TagManager::clearTags()
{
    tags_.clear();
}

//-----------------------------------------------------------------------------
// Function: TagManager::getInstance()
//-----------------------------------------------------------------------------
TagManager& TagManager::getInstance()
{
    static TagManager instance;
    return instance;
}

//-----------------------------------------------------------------------------
// Function: TagManager::TagManager()
//-----------------------------------------------------------------------------
TagManager::TagManager():
tags_()
{
    
}
