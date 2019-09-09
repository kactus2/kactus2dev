//-----------------------------------------------------------------------------
// File: TagEditorContainer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2019
//
// Description:
// Container for editable tags.
//-----------------------------------------------------------------------------

#ifndef TAGEDITORCONTAINER_H
#define TAGEDITORCONTAINER_H

#include <common/widgets/tagEditor/TagContainer.h>

//-----------------------------------------------------------------------------
//! Container for editable tags.
//-----------------------------------------------------------------------------
class TagEditorContainer : public TagContainer
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent widget.
     */
    TagEditorContainer(QWidget* parent);

    /*!
     *  The destructor.
     */
    virtual ~TagEditorContainer() = default;

private:
    // Disable copying.
    TagEditorContainer(TagEditorContainer const& rhs);
    TagEditorContainer& operator=(TagEditorContainer const& rhs);

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

#endif // TAGEDITORCONTAINER_H
