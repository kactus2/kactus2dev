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

#include <common/widgets/colorBox/ColorBox.h>

class TagLabel;

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
     *    @param [in] tagLabel    The tag label being edited.
     *    @param [in] parent      The parent widget.
     */
    TagDisplay(TagLabel* tagLabel, QWidget* parent = nullptr);

    /*!
     *  The destructor.
     */
    virtual ~TagDisplay() = default;

    // Disable copying.
    TagDisplay(TagDisplay const& rhs) = delete;
    TagDisplay& operator=(TagDisplay const& rhs) = delete;

    /*!
     *  The event filter.
     *
     *    @param [in] watched     The monitored object.
     *    @param [in] event       The occurring event.
     *
     *    @return True, if the event is caught, false otherwise.
     */
    bool eventFilter(QObject *watched, QEvent *event) final;


protected:
    
    /*!
     *  Check if color selection button should be shown.
     *
     *    @return True, if button should be shown, otherwise false.
     */
    virtual bool showColorButton() const noexcept;

    /*!
     *  Handler for focus out events.
     *
     *    @param [in] event   The focus event.
     */
    void focusOutEvent(QFocusEvent* event) final;

    /*!
     *  Handler for key press events.
     *
     *    @param [in] event   The key event.
     */
    void keyPressEvent(QKeyEvent *event) final;
    
    /*!
     *  Get the color display button.
     *
     *    @return The color display button.
     */
    ColorBox* getColorButton();

    /*!
     *  Get the edited tag label.
     *
     *    @return The edited tag label.
     */
    TagLabel* getEditedLabel() const;

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

protected slots:

    /*!
     *  Handler for changing color through completer.
     *
     *    @param [in] newColor    The color of the selected completer item.
     */
    void completerColorChange(QColor const& newColor);

signals:

    /*!
     *  Signals the accepted changes made in the editor.
     *
     *    @param [in] editedLabel     The edited tag label.
     *    @param [in] tagEditor       This editor.
     */
    void acceptChanges(TagLabel* editedLabel, TagDisplay* tagEditor);

    /*!
     *  Signals the removal of this tag label.
     *
     *    @param [in] editedLabel     The edited tag label.
     *    @param [in] tagEditor       This editor.
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

    /*!
     *  Setup the layout.
     *
     *    @param [in] nameEditor  Name editor of the tag.
     */
    void setupLayout(QWidget* nameEditor);

    /*!
     *  Get the current name from the name editor.
     *
     *    @return The current name of the name editor.
     */
    virtual QString getNewName() const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    constexpr static int BUTTON_SIDE = 20;

    //! Tag color editor.
    ColorBox* colorButton_ =
         new ColorBox(QSize(BUTTON_SIDE, BUTTON_SIDE), this);

    //! Accept tag button.
    QPushButton* okButton_ = new QPushButton(
        QIcon(QStringLiteral(":/icons/common/graphics/checkMark.png")), QString(), this);

    //! Delete tag button.
    QPushButton* deleteButton_ = new QPushButton(
        QIcon(QStringLiteral(":/icons/common/graphics/cross.png")), QString(), this);

    //! The edited tag label.
    TagLabel* editedLabel_;
};

//-----------------------------------------------------------------------------

#endif // TAGDISPLAY_H
