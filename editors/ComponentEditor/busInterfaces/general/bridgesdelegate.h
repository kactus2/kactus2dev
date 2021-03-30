//-----------------------------------------------------------------------------
// File: BridgesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2012
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef BRIDGESDELEGATE_H
#define BRIDGESDELEGATE_H

#include <QStyledItemDelegate>
#include <QSharedPointer>

class Component;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
// Data.
//-----------------------------------------------------------------------------
class BridgesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] busInterface    Interface for accessing bus interfaces.
	 *      @param [in] parent          The owner of this delegate.
     */
    BridgesDelegate(BusInterfaceInterface* busInterface, QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~BridgesDelegate() = default;

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent Owner for the editor.
	 *      @param [in] option Contains options for the editor.
	 *      @param [in] index Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor The editor where the data is to be set.
	 *      @param [in] index Model index identifying the item that's data is to be set.
     */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor The editor that contains the data to store.
	 *      @param [in] model Model that contains the data structure where data is to be saved to.
	 *      @param [in] index Model index identifying the item that's data is to be saved.
     */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

private:
	
    //! No copying. No assignment.
	BridgesDelegate(const BridgesDelegate& other);
	BridgesDelegate& operator=(const BridgesDelegate& other);

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;

	//! Boolean for ad-hoc group modify.
	bool opaqueGroupModify_;

	//! The new state for the group modify.
	Qt::CheckState opaque;
};

#endif // BRIDGESDELEGATE_H
