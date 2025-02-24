//-----------------------------------------------------------------------------
// File: FloatingModeReferenceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 16.10.2023
//
// Description:
// A mode reference editor for tables containing mode references.
//-----------------------------------------------------------------------------

#ifndef FLOATINGMODEREFERENCEEDITOR_H
#define FLOATINGMODEREFERENCEEDITOR_H

#include <QFrame>

class ModeReferenceInterface;
class ModeReferenceEditor;

class FloatingModeReferenceEditor : public QFrame
{
    Q_OBJECT
public:

    /*!
     *	The constructor.
     *  
     *    @param [in] model      The mode reference interface to use.
     *    @param [in] parent     The parent widget.
     */
    FloatingModeReferenceEditor(ModeReferenceInterface* modeRefInterface, QWidget* parent);

    virtual ~FloatingModeReferenceEditor() = default;

    FloatingModeReferenceEditor(FloatingModeReferenceEditor& other) = delete;
    FloatingModeReferenceEditor& operator=(FloatingModeReferenceEditor& other) = delete;

    /*!
     *	Get the size hint of the mode reference table.
     *  
     *	    
     * 	    @return The size hint of the mode reference table.
     */
    QSize getTableSizeHint() const;

    /*!
     *	Get the margins of the mode reference table.
     *  
     * 	    @return The margins of the mode reference table.
     */
    QMargins getTableMargins() const;

signals:

    /*!
     *	Signal emitted when the user has pressed the accept button.
     */
    void finishEditing();

    /*!
     *	Signal emitted when user cancels editing by pressing the cross.
     */
    void cancelEditing();

private:

    /*!
     *	Setup the layout of the editor.
     */
    void setupLayout();

    //! The mode reference editor itself.
    ModeReferenceEditor* modeReferencesEditor_;

};

#endif // FLOATINGMODEREFERENCEEDITOR_H
