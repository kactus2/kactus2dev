//-----------------------------------------------------------------------------
// File: InterfacePortMapDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.05.2016
//
// Description:
// Delegate for the visualizing port maps of a bus interface.
//-----------------------------------------------------------------------------

#ifndef INTERFACEPORTMAPDELEGATE_H
#define INTERFACEPORTMAPDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! Delegate for the visualizing port maps of a bus interface.
//-----------------------------------------------------------------------------
class InterfacePortMapDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
     *
	 *      @param [in] parent  Pointer to the owner of this delegate.
	 */
	InterfacePortMapDelegate(QObject *parent = 0);
	
	/*!
     *  The destructor.
     */
    virtual ~InterfacePortMapDelegate();

protected:

    /*!
     *  Paint the editor.
     *
     *      @param [in] painter     The used painter.
     *      @param [in] option      The style options.
     *      @param [in] index       The current index.
     */
    void paint(QPainter *painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

    /*!
     *  The editor events.
     *
     *      @param [in] event   The event itself.
     *      @param [in] model   The used item model.
     *      @param [in] option  The style options.
     *      @param [in] index   The current model index.
     */
    bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option,
        QModelIndex const& index);

private:

    // Disable copying.
    InterfacePortMapDelegate(InterfacePortMapDelegate const& rhs);
    InterfacePortMapDelegate& operator=(InterfacePortMapDelegate const& rhs);

    //! Boolean for port map group modify.
    bool portMapGroupModify_;

    //! The new state for the group modify.
    Qt::CheckState portMapGroupState_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCVISIBILITYDELEGATE_H
