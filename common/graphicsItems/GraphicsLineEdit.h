//-----------------------------------------------------------------------------
// File: GraphicsLineEdit.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 25.9.2012
//
// Description:
// Declares the graphics line edit class.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSLINEEDIT_H
#define GRAPHICSLINEEDIT_H

#include <QGraphicsTextItem>
#include <QValidator>

//-----------------------------------------------------------------------------
//! Graphics item for representing a single-line text editor.
//-----------------------------------------------------------------------------
class GraphicsLineEdit : public QGraphicsTextItem
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *  
     *      @param [in] parent  The parent.
     */
    GraphicsLineEdit(QGraphicsItem* parent);

    /*!
     *  Destructor.
     */
    ~GraphicsLineEdit();

    /*!
     *  Sets the editor read only.
     *  
     *      @param [in] readOnly    True for read only.
     */
    void setReadOnly(bool readOnly);

    /*!
     *  Sets a validator for editing.
     *  
     *  @param [in] validator The validator.
     */
    void setValidator(QValidator const* validator);

signals:
    //! Signaled when the text has been edited (and accepted with enter or tab).
    void textEdited();

    //! Signaled when invalid input has been entered.
    void invalidInputEntered();

private slots:
    //! Updates the validation when the text has been modified but not committed.
    void onContentChanged();

protected:
    bool sceneEvent(QEvent* event);

private:
    // Disable copying.
    GraphicsLineEdit(GraphicsLineEdit const& rhs);
    GraphicsLineEdit& operator=(GraphicsLineEdit const& rhs);

    /*!
     *  Saves the previous text and enters the edit mode.
     */
    void beginEdit();

    /*!
     *  Cancels the edit and leaves edit mode.
     */
    void cancelEdit();

    /*!
     *  Commits the edit and leaves edit mode.
     */
    void commitEdit();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, the editor is read only and the text cannot be edited interactively.
    bool readOnly_;

    //! Old text before edit.
    QString oldText_;

    //! The validator.
    QValidator const* validator_;
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSLINEEDIT_H
