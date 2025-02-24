//-----------------------------------------------------------------------------
// File: apiinterfacesdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 27.06.2012
//
// Description:
// The delegate that provides editors to edit the API definitions.
//-----------------------------------------------------------------------------

#ifndef APIINTERFACESDELEGATE_H
#define APIINTERFACESDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit the API definitions.
//-----------------------------------------------------------------------------
class ApiInterfacesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *    @param [in] parent The owner of the delegate.
	 */
	ApiInterfacesDelegate(QObject *parent);
	
	//! The destructor
	virtual ~ApiInterfacesDelegate() = default;

    //! No copying
    ApiInterfacesDelegate(const ApiInterfacesDelegate& other) = delete;

    //! No assignment
    ApiInterfacesDelegate& operator=(const ApiInterfacesDelegate& other) = delete;

	/*! Create a new editor for the given item
	 *
	 *    @param [in] parent  Owner for the editor.
	 *    @param [in] option  Contains options for the editor.
	 *    @param [in] index   Model index identifying the item.
	 *
	 *    @return The editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *    @param [in] editor The editor where the data is to be set.
	 *    @param [in] index Model index identifying the item that's data is to be set.
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *    @param [in] editor  The editor that contains the data to store.
	 *    @param [in] model   Model that contains the data structure where data is to be saved to.
	 *    @param [in] index   Model index identifying the item that's data is to be saved.
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		QModelIndex const& index) const;

private slots:

	//! Commit the data from the sending editor and close the editor.	 
	void commitAndCloseEditor();

};

#endif // APIINTERFACESDELEGATE_H
