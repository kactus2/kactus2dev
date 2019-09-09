//-----------------------------------------------------------------------------
// File: TagDisplay.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2019
//
// Description:
// Display for tags.
//-----------------------------------------------------------------------------

#ifndef TAGDISPLAY_H
#define TAGDISPLAY_H

#include <QFrame>
#include <QObject>
#include <QPushButton>

class TagLabel;
class ColorBox;

//-----------------------------------------------------------------------------
//! Display for tags.
//-----------------------------------------------------------------------------
class TagDisplay : public QFrame
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] tagLabel    The tag label being edited.
     *      @param [in] parent      The parent widget.
     */
    TagDisplay(TagLabel* tagLabel, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~TagDisplay() = default;

    /*!
     *  The event filter.
     *
     *      @param [in] watched     The monitored object.
     *      @param [in] event       The occurring event.
     *
     *      @return True, if the event is caught, false otherwise.
     */
    virtual bool eventFilter(QObject *watched, QEvent *event) override final;

protected:

    /*!
     *  Handler for focus out events.
     *
     *      @param [in] event   The focus event.
     */
    virtual void focusOutEvent(QFocusEvent* event) override final;

    /*!
     *  Handler for key press events.
     *
     *      @param [in] event   The key event.
     */
    virtual void keyPressEvent(QKeyEvent *event) override final;
    
    /*!
     *  Get the color display button.
     *
     *      @return The color display button.
     */
    ColorBox* getColorButton();

    /*!
     *  Get the edited tag label.
     *
     *      @return The edited tag label.
     */
    TagLabel* getEditedLabel() const;

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

protected slots:

    /*!
     *  Handler for changing color through completer.
     *
     *      @param [in] newColor    The color of the selected completer item.
     */
    void completerColorChange(QColor const& newColor);

signals:

    /*!
     *  Signals the accepted changes made in the editor.
     *
     *      @param [in] editedLabel     The edited tag label.
     *      @param [in] tagEditor       This editor.
     */
    void acceptChanges(TagLabel* editedLabel, TagDisplay* tagEditor);

    /*!
     *  Signals the removal of this tag label.
     *
     *      @param [in] editedLabel     The edited tag label.
     *      @param [in] tagEditor       This editor.
     */
    void deleteItem(TagLabel* editedLabel, TagDisplay* tagEditor);

private slots:

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
    TagDisplay(TagDisplay const& rhs);
    TagDisplay& operator=(TagDisplay const& rhs);

    /*!
     *  Setup the layout.
     *
     *      @param [in] nameEditor  Name editor of the tag.
     */
    void setupLayout(QWidget* nameEditor);

    /*!
     *  Get the current name from the name editor.
     *
     *      @return The current name of the name editor.
     */
    virtual QString getNewName() const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Tag color editor.
    ColorBox* colorButton_;

    //! Delete tag button.
    QPushButton* deleteButton_;

    //! The edited tag label.
    TagLabel* editedLabel_;
};

//-----------------------------------------------------------------------------

#endif // TAGDISPLAY_H
