//-----------------------------------------------------------------------------
// File: busportsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.6.2011
//
// Description:
// The delegate that provides editors for logical port properties in Abstraction Definition.
//-----------------------------------------------------------------------------

#ifndef BUSPORTSDELEGATE_H
#define BUSPORTSDELEGATE_H

#include <QStyledItemDelegate>

class BusDefinition;
//-----------------------------------------------------------------------------
//! The delegate that provides editors for logical port properties in Abstraction Definition.
//-----------------------------------------------------------------------------
class BusPortsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent The owner of this instance
	*/
	BusPortsDelegate(QObject *parent);
	
	//! The destructor
	virtual ~BusPortsDelegate();

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
        
	/*! Set the bus definition referenced by the edited abstraction definition.
	 *
	 *      @param [in] busDefinition  The referenced bus definition.
	*/
    void setBusDef(QSharedPointer<BusDefinition> busDefinition);

private slots:

	/*! Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	//! No copying
	BusPortsDelegate(const BusPortsDelegate& other);

	//! No assignment
	BusPortsDelegate& operator=(const BusPortsDelegate& other);

    //! The bus definition referenced by the edited abstraction definition.
    QSharedPointer<BusDefinition> busDefinition_;
};

#endif // BUSPORTSDELEGATE_H
