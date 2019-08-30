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

#include <QFrame>
#include <QObject>
#include <QLineEdit>
#include <QPushButton>

class TagLabel;
class ColorBox;

//-----------------------------------------------------------------------------
//! Editor for tags.
//-----------------------------------------------------------------------------
class TagEditor : public QFrame
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

    /*!
     *  The event filter.
     *
     *      @param [in] watched     The monitored object.
     *      @param [in] event       The occurring event.
     *
     *      @return True, if the event is caught, false otherwise.
     */
    virtual bool eventFilter(QObject *watched, QEvent *event) final;

protected:

    /*!
     *  Handler for focus out events.
     *
     *      @param [in] event   The focus event.
     */
    virtual void focusOutEvent(QFocusEvent* event) final;

    /*!
     *  Handler for key press events.
     *
     *      @param [in] event   The key event.
     */
    virtual void keyPressEvent(QKeyEvent *event) final;
    
signals:

    /*!
     *  Signals the accepted changes made in the editor.
     *
     *      @param [in] editedLabel     The edited tag label.
     *      @param [in] tagEditor       This editor.
     */
    void acceptChanges(TagLabel* editedLabel, TagEditor* tagEditor);

    /*!
     *  Signals the removal of this tag label.
     *
     *      @param [in] editedLabel     The edited tag label.
     *      @param [in] tagEditor       This editor.
     */
    void deleteItem(TagLabel* editedLabel, TagEditor* tagEditor);

private slots:

    /*!
     *  Handler for changing the color.
     */
    void changeColor();

    /*!
     *  Handler for accepting changes.
     */
    void onAcceptChanges();

    /*!
     *  Handler for deleting item.
     */
    void onDeleteItem();

private:
    // Disable copying.
    TagEditor(TagEditor const& rhs);
    TagEditor& operator=(TagEditor const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    /*!
     *  Connect signals.
     */
    void connectSignals();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Tag name editor.
    QLineEdit* nameEdit_;

    //! Tag color editor.
    ColorBox* colorButton_;

    //! Delete tag button.
    QPushButton* deleteButton_;

    //! The edited tag label.
    TagLabel* editedLabel_;
};

//-----------------------------------------------------------------------------

#endif // TAGEDITOR_H
