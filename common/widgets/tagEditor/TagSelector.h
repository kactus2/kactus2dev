//-----------------------------------------------------------------------------
// File: TagSelector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2019
//
// Description:
// Selector for tags.
//-----------------------------------------------------------------------------

#ifndef TAGSELECTOR_H
#define TAGSELECTOR_H

#include <common/widgets/tagEditor/TagDisplay.h>

#include <QComboBox>

class TagLabel;

//-----------------------------------------------------------------------------
//! Selector for tags.
//-----------------------------------------------------------------------------
class TagSelector : public TagDisplay
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] tagLabel    The tag label being edited.
     *    @param [in] parent      The parent widget.
     */
    TagSelector(TagLabel* tagLabel, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~TagSelector() = default;

protected:

    /*!
     *  Setup the tag editors.
     *
     *    @param [in] nameEditor  The name editor for the tag.
     */
    virtual void setupEditors(QWidget* nameEditor);

private:
    // Disable copying.
    TagSelector(TagSelector const& rhs);
    TagSelector& operator=(TagSelector const& rhs);

    /*!
     *  Get the current name from the name editor.
     *
     *    @return The current name of the name editor.
     */
    virtual QString getNewName() const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Tag name selector.
    QComboBox* nameEdit_;
};

//-----------------------------------------------------------------------------

#endif // TAGSELECTOR_H
