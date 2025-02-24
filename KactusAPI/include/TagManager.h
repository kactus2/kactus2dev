//-----------------------------------------------------------------------------
// File: TagManager.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.08.2019
//
// Description:
// Manager for loaded tags.
//-----------------------------------------------------------------------------

#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include <IPXACTmodels/common/TagData.h>

#include <KactusAPI/KactusAPIGlobal.h>

#include <QVector>

//-----------------------------------------------------------------------------
//! Manager for loaded tags.
//-----------------------------------------------------------------------------
class KACTUS2_API TagManager
{
public:

    /*!
     *  The destructor.
     */
    ~TagManager() = default;

    /*!
     *  Add new tags from the selected list.
     *
     *    @param [in] newTags     The selected list of new tags.
     */
    void addNewTags(QVector<TagData> newTags);

    /*!
     *  Set new tags.
     *
     *    @param [in] newTags     The selected list of new tags.
     */
    void setTags(QVector<TagData> newTags);

    /*!
     *  Get a list of the active tags.
     *
     *    @return List of the active tags.
     */
    QVector<TagData> getTags() const;

    /*!
     *  Clear the list of tags.
     */
    void clearTags();

    /*!
     *  Get the singleton instance of the tag manager.
     *
     *    @return The tag manager instance.
     */
    static TagManager& getInstance();

private:
    // Disable copying.
    TagManager(TagManager const& rhs);
    TagManager& operator=(TagManager const& rhs);
     
    /*!
     *  The constructor. Private to enforce the use of getInstance() to get access.
     */
    TagManager();
    
    /*!
     *  Check if the selected tag exists in the list.
     *
     *    @param [in] tag     The selected tag.
     *
     *    @return True, if the selected tag exists in the list, false otherwise.
     */
    bool tagExists(TagData const& tag) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The list of loaded tags.
    QVector<TagData> tags_;
};

//-----------------------------------------------------------------------------

#endif // TAGMANAGER_H
