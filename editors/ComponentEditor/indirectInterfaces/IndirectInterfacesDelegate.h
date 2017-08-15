//-----------------------------------------------------------------------------
// File: IndirectInterfacesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// Delegate class for Indirect interfaces table of a component.
//-----------------------------------------------------------------------------

#ifndef INDIRECT_INTERFACES_DELEGATE_H
#define INDIRECT_INTERFACES_DELEGATE_H

#include <editors/ComponentEditor/common/MultilineDescriptionDelegate.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
//! The delegate that provides editors for bus interfaces.
//-----------------------------------------------------------------------------
class IndirectInterfacesDelegate : public MultilineDescriptionDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
     *      @param [in] component   The component containing the edited indirect interfaces.
	 *      @param [in] parent      The owner of the delegate.
	*/
	IndirectInterfacesDelegate(QSharedPointer<Component> component, QObject *parent);
	
	//! The destructor
	virtual ~IndirectInterfacesDelegate();

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
	IndirectInterfacesDelegate(const IndirectInterfacesDelegate& other);
	IndirectInterfacesDelegate& operator=(const IndirectInterfacesDelegate& other);

    QStringList findAvailableReferences() const;

    //! The component containing the edited indirect interfaces.
    QSharedPointer<Component> component_;
};

#endif // INDIRECT_INTERFACES_DELEGATE_H
