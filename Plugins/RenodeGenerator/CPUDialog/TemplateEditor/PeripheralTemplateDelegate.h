//-----------------------------------------------------------------------------
// File: PeripheralTemplateDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 31.05.2023
//
// Description:
// The delegate that provides editors to edit peripheral template details.
//-----------------------------------------------------------------------------

#ifndef PERIPHERALTEMPLATEDELEGATE_H
#define PERIPHERALTEMPLATEDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit peripheral template details.
//-----------------------------------------------------------------------------
class PeripheralTemplateDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     */
    PeripheralTemplateDelegate(QString const& generationDestinationFolder, QWidget* parentWidget, QObject* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~PeripheralTemplateDelegate() = default;

    // Disable copying.
    PeripheralTemplateDelegate(PeripheralTemplateDelegate const& rhs) = delete;
    PeripheralTemplateDelegate& operator=(PeripheralTemplateDelegate const& rhs) = delete;

protected:
    
    /*!
     *  Handler for preprocessing events that starts editing.
     *
     *      @param [in] event   The event that triggered the editing.
     *      @param [in] model   The underlying model to edit.
     *      @param [in] option  Style options for rendering the item.
     *      @param [in] index   The index being edited.
     *
     *      @return True, if the delegate handles the event, otherwise false.
     */
    virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option, QModelIndex const& index);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The current folder location.
    QString currentFolder_;

    //! The parent widget.
    QWidget* parentWidget_;
};

#endif // PERIPHERALTEMPLATEDELEGATE_H
