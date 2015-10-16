//-----------------------------------------------------------------------------
// File: cominterfacesdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.06.2012
//
// Description:
// Delegate class for component COM interfaces.
//-----------------------------------------------------------------------------

#ifndef COMINTERFACESDELEGATE_H
#define COMINTERFACESDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! Delegate class for component COM interfaces.
//-----------------------------------------------------------------------------
class ComInterfacesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] parent The owner of this delegate.
	*/
	ComInterfacesDelegate(QObject *parent);
	
	//! The destructor
	virtual ~ComInterfacesDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor  The editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *      @param [in] editor  The editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		const QModelIndex& index) const;

private slots:

	//! Commit the data from the sending editor and close the editor.
	void commitAndCloseEditor();

private:
	
	//! No copying
	ComInterfacesDelegate(const ComInterfacesDelegate& other);
	ComInterfacesDelegate& operator=(const ComInterfacesDelegate& other);
};

#endif // COMINTERFACESDELEGATE_H
