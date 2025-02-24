//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCPUDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.10.2021
//
// Description:
// The delegate that provides editors to edit Linux Device Tree CPU details.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREECPUDELEGATE_H
#define LINUXDEVICETREECPUDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit Linux Device Tree CPU details.
//-----------------------------------------------------------------------------
class LinuxDeviceTreeCPUDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     */
    LinuxDeviceTreeCPUDelegate(QObject* parent = 0);
	
	/*!
     *  The destructor.
     */
    virtual ~LinuxDeviceTreeCPUDelegate() = default;

	/*!
	 *  Renders an item in the given index.
	 *
	 *    @param [in] painter     The painter to use for rendering.
	 *    @param [in] option      The style options for the rendering.
	 *    @param [in] index       The index to the data being rendered.
	 */
	virtual void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

    // Disable copying.
    LinuxDeviceTreeCPUDelegate(LinuxDeviceTreeCPUDelegate const& rhs) = delete;
    LinuxDeviceTreeCPUDelegate& operator=(LinuxDeviceTreeCPUDelegate const& rhs) = delete;

protected:
    
    /*!
     *  Handler for preprocessing events that starts editing.
     *
     *    @param [in] event   The event that triggered the editing.
     *    @param [in] model   The underlying model to edit.
     *    @param [in] option  Style options for rendering the item.
     *    @param [in] index   The index being edited.
     *
     *    @return True, if the delegate handles the event, otherwise false.
     */
    virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option, 
        QModelIndex const& index);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Boolean for boolean group modify.
    bool booleanModify_;

    //! The new state for the boolean modify.
    Qt::CheckState booleanState_;
};

#endif // LINUXDEVICETREECPUDELEGATE_H
