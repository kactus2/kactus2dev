//-----------------------------------------------------------------------------
// File: InstanceComboBox.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Sampo Suokuisma
// Date: 05.06.2025
//
// Description:
// A custom QComboBox that disables mouse wheel scrolling.
//-----------------------------------------------------------------------------

#ifndef INSTANCEWIDGETS_H
#define INSTANCEWIDGETS_H

#include <QComboBox>
#include <QWheelEvent>

/*!
 *  A custom QComboBox that disables mouse wheel scrolling.
 *
 *  This subclass of QComboBox overrides the default wheel event
 *  behavior to prevent accidental value changes caused by mouse wheel movements,
 *  especially when the combo box is not focused.
 */
class InstanceComboBox : public QComboBox
{
    Q_OBJECT
public:
    /*!
     *  Constructs the combo box with an optional parent widget.
     *
     *  @param [in] parent The parent widget. Defaults to nullptr.
     */
    explicit InstanceComboBox(QWidget* parent = nullptr);

protected:
    /*!
     *  Overrides the default wheel event to disable scrolling behavior.
     *
     *  @param [in] event The wheel event being processed.
     */
    void wheelEvent(QWheelEvent* event) override;
};

#endif // INSTANCEWIDGETS_H
