//-----------------------------------------------------------------------------
// File: TagSelectorContainer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2019
//
// Description:
// Container for tag selectors.
//-----------------------------------------------------------------------------

#ifndef TAGSELECTORCONTAINER_H
#define TAGSELECTORCONTAINER_H

#include <common/widgets/tagEditor/TagContainer.h>

//-----------------------------------------------------------------------------
//! Container for tag selectors.
//-----------------------------------------------------------------------------
class TagSelectorContainer : public TagContainer
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent widget.
     */
    TagSelectorContainer(QWidget* parent);

    /*!
     *  The destructor.
     */
    virtual ~TagSelectorContainer() = default;

private:
    // Disable copying.
    TagSelectorContainer(TagSelectorContainer const& rhs);
    TagSelectorContainer& operator=(TagSelectorContainer const& rhs);

    /*!
     *  Construct a tag editor for the selected tag label.
     *
     *      @param [in] editedLabel     The selected tag label.
     *
     *      @return The created tag editor.
     */
    virtual TagDisplay* constructTagEditor(TagLabel* editedLabel) final;
};

//-----------------------------------------------------------------------------

#endif // TAGSELECTORCONTAINER_H
