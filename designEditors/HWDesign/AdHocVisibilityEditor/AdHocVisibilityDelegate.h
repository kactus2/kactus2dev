//-----------------------------------------------------------------------------
// File: AdHocVisibilityDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// Delegate for the table visualizing ad-hoc visibility for component ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCVISIBILITYDELEGATE_H
#define ADHOCVISIBILITYDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! Delegate for the table visualizing ad-hoc visibility for component ports.
//-----------------------------------------------------------------------------
class AdHocVisibilityDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
	 *  Constructor.
     *
	 *      @param [in] parent Pointer to the owner of this instance.
	 */
	AdHocVisibilityDelegate(QObject *parent = 0);
	
	/*!
     *  Destructor.
     */
	virtual ~AdHocVisibilityDelegate();

	/*!
     *  Creates a new editor for the given item.
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

	/*!
     *  Sets the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*!
     *  Saves the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

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
    AdHocVisibilityDelegate(AdHocVisibilityDelegate const& rhs);
    AdHocVisibilityDelegate& operator=(AdHocVisibilityDelegate const& rhs);

    //! Boolean for ad-hoc group modify.
    bool adhocGroupModify_;

    //! The new state for the group modify.
    Qt::CheckState adhocGroupState_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCVISIBILITYDELEGATE_H
