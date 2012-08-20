//-----------------------------------------------------------------------------
// File: LineEditEx.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.8.2012
//
// Description:
// Line edit with extended functionality.
//-----------------------------------------------------------------------------

#ifndef LINEEDITEX_H
#define LINEEDITEX_H

#include <common/widgets/assistedLineEdit/BasicLineContentMatcher.h>
#include <common/widgets/assistedLineEdit/AssistedLineEdit.h>

#include <QIcon>
#include <QLabel>

//-----------------------------------------------------------------------------
//! Line edit with extended functionality.
//-----------------------------------------------------------------------------
class LineEditEx : public AssistedLineEdit
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    LineEditEx(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~LineEditEx();

    /*!
     *  Sets the disallowed inputs.
     */
    void setDisallowedInputs(QStringList const& inputs);

    /*!
     *  Sets the notification message template which is used when the user inputs a
     *  string that is included in the disallowed inputs. Example: "View '%1' is already in use."
     *
     *      @param [in] template The template string. Must contain %1 to allow argumentation.
     */
    void setMessageTemplate(QString const& messageTemplate);

    /*!
     *  Sets the notification message icon.
     *
     *      @param [in] icon The icon to set.
     */
    void setMessageIcon(QPixmap const& icon);

    /*!
     *  Returns true if the input is valid.
     */
    bool isInputValid() const;

protected:
    //! Called when the line edit has been resized.
    virtual void resizeEvent(QResizeEvent* event);

private slots:
    /*!
     *  Validates the given current text and either shows or hides the notification icon.
     *
     *      @param [in] text The text to validate.
     */
    void validateText(QString const& text);

private:
    // Disable copying.
    LineEditEx(LineEditEx const& rhs);
    LineEditEx& operator=(LineEditEx const& rhs);
    void updateIconPos();
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of disallowed inputs.
    QStringList disallowedInputs_;

    //! The notification message template.
    QString messageTemplate_;

    //! The notification message icon.
    QIcon messageIcon_;

    //! The notification icon widget.
    QLabel* iconWidget_;    

    //! If true, the input is valid.
    bool valid_;
};

//-----------------------------------------------------------------------------

#endif // LINEEDITEX_H
