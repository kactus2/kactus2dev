//-----------------------------------------------------------------------------
// File: AdHocDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// Delegate for the table visualizing ad-hoc visibility for component ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCDELEGATE_H
#define ADHOCDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
// Constants defining which column represents what kind of information.
//-----------------------------------------------------------------------------
enum PortEditorColumn
{
    ADHOC_COL_NAME = 0,          //!< Column for the port name.
    ADHOC_COL_DIRECTION,         //!< Column for the port direction.
    ADHOC_COL_VISIBILITY,        //!< Column for toggling ad-hoc visibility on/off.
    ADHOC_COL_COUNT
};

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit ad-hoc visibility of
//! of component ports.
//-----------------------------------------------------------------------------
class AdHocDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
	 *  Constructor.
     *
	 *      @param [in] parent Pointer to the owner of this instance.
	 */
	AdHocDelegate(QObject *parent = 0);
	
	/*!
     *  Destructor.
     */
	virtual ~AdHocDelegate();

	/*!
     *  Creates a new editor for the given item.
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
		                          QModelIndex const& index) const;

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
	 *
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		                      QModelIndex const& index) const;

protected:
    void paint(QPainter *painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option,
                     QModelIndex const& index);

private:
    // Disable copying.
    AdHocDelegate(AdHocDelegate const& rhs);
    AdHocDelegate& operator=(AdHocDelegate const& rhs);

    //! Boolean for ad-hoc group modify.
    bool adhocGroupModify_;

    //! The new state for the group modify.
    Qt::CheckState adhocGroupState_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCDELEGATE_H
