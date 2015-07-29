//-----------------------------------------------------------------------------
// File: businterfacesdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// The delegate that provides editors for bus interfaces.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACESDELEGATE_H
#define BUSINTERFACESDELEGATE_H

#include <editors/ComponentEditor/common/MultilineDescriptionDelegate.h>

#include "BusInterfaceColumns.h"

//-----------------------------------------------------------------------------
//! The delegate that provides editors for bus interfaces.
//-----------------------------------------------------------------------------
class BusInterfacesDelegate : public MultilineDescriptionDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent The owner of the delegate.
	*/
	BusInterfacesDelegate(QObject *parent);
	
	//! The destructor
	virtual ~BusInterfacesDelegate();

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

protected:

    virtual int descriptionColumn() const;

private slots:

	/*! Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	
	//! No copying
	BusInterfacesDelegate(const BusInterfacesDelegate& other);
	BusInterfacesDelegate& operator=(const BusInterfacesDelegate& other);


};

#endif // BUSINTERFACESDELEGATE_H
