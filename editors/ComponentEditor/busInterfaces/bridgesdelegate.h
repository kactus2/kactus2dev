/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgesdelegate.h
 *		Project: Kactus 2
 */

#ifndef BRIDGESDELEGATE_H
#define BRIDGESDELEGATE_H

#include <QStyledItemDelegate>
#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
// Data.
//-----------------------------------------------------------------------------
class BridgesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The owner of this delegate.
	 *
	*/
	BridgesDelegate(QSharedPointer<Component> component, QObject *parent);
	
	//! The destructor.
	virtual ~BridgesDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent Owner for the editor.
	 *      @param [in] option Contains options for the editor.
	 *      @param [in] index Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor The editor where the data is to be set.
	 *      @param [in] index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *      @param [in] editor The editor that contains the data to store.
	 *      @param [in] model Model that contains the data structure where data is to be saved to.
	 *      @param [in] index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

private:
	
	//! No copying
	BridgesDelegate(const BridgesDelegate& other);

	//! No assignment
	BridgesDelegate& operator=(const BridgesDelegate& other);

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Boolean for ad-hoc group modify.
	bool opaqueGroupModify_;

	//! The new state for the group modify.
	Qt::CheckState opaque;
};

#endif // BRIDGESDELEGATE_H
