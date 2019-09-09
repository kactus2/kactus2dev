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
     *      @param [in] tagLabel    The tag label being edited.
     *      @param [in] parent      The parent widget.
     */
    TagEditor(TagLabel* tagLabel, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~TagEditor() = default;

protected:

    /*!
     *  Connect signals.
     */
    virtual void connectSignals();

    /*!
     *  Setup the tag editors.
     *
     *      @param [in] nameEditor  The name editor for the tag.
     */
    virtual void setupEditors(QWidget* nameEditor);

private slots:

    /*!
     *  Handler for changing the color.
     */
    void changeColor();

private:
    // Disable copying.
    TagEditor(TagEditor const& rhs);
    TagEditor& operator=(TagEditor const& rhs);

    /*!
     *  Get the current name from the name editor.
     *
     *      @return The current name of the name editor.
     */
    virtual QString getNewName() const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Tag name editor.
    QLineEdit* nameEdit_;
};

//-----------------------------------------------------------------------------

#endif // TAGEDITOR_H
