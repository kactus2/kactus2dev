//-----------------------------------------------------------------------------
// File: TagEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.08.2019
//
// Description:
// Editor for tags.
//-----------------------------------------------------------------------------

#ifndef TAGEDITOR_H
#define TAGEDITOR_H

#include <common/widgets/tagEditor/TagDisplay.h>

#include <QLineEdit>

class TagLabel;

//-----------------------------------------------------------------------------
//! Editor for tags.
//-----------------------------------------------------------------------------
class TagEditor : public TagDisplay
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] tagLabel    The tag label being edited.
     *    @param [in] parent      The parent widget.
     */
    TagEditor(TagLabel* tagLabel, QWidget* parent = nullptr);

    /*!
     *  The destructor.
     */
    virtual ~TagEditor() = default;

    // Disable copying.
    TagEditor(TagEditor const& rhs) = delete;
    TagEditor& operator=(TagEditor const& rhs) = delete;

protected:

    /*!
     *  Check if color selection button should be shown.
     *
     *    @return True, if button should be shown, otherwise false.
     */
    bool showColorButton() const noexcept final;

    /*!
     *  Connect signals.
     */
    virtual void connectSignals();

    /*!
     *  Setup the tag editors.
     *
     *    @param [in] nameEditor  The name editor for the tag.
     */
    virtual void setupEditors(QWidget* nameEditor);

private slots:

    /*!
     *  Handler for changing the color.
     */
    void changeColor();

private:

    /*!
     *  Get the current name from the name editor.
     *
     *    @return The current name of the name editor.
     */
    virtual QString getNewName() const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Tag name editor.
    QLineEdit* nameEdit_ = new QLineEdit(this);
};

//-----------------------------------------------------------------------------

#endif // TAGEDITOR_H
