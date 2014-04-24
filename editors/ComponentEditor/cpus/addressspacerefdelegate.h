/* 
 *  	Created on: 27.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacerefdelegate.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEREFDELEGATE_H
#define ADDRESSSPACEREFDELEGATE_H

#include <common/delegates/ComboDelegate/combodelegate.h>

#include <models/component.h>

#include <QSharedPointer>

/*! \brief Delegate that manages the editors to select an address space in component.
 *
 */
class AddressSpaceRefDelegate : public ComboDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this delegate.
	 * \param component Pointer to the component being edited.
	 *
	*/
	AddressSpaceRefDelegate(QObject *parent,
		QSharedPointer<Component> component);
	
	//! \brief The destructor
	virtual ~AddressSpaceRefDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

private:
	//! \brief No copying
	AddressSpaceRefDelegate(const AddressSpaceRefDelegate& other);

	//! \brief No assignment
	AddressSpaceRefDelegate& operator=(const AddressSpaceRefDelegate& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // ADDRESSSPACEREFDELEGATE_H
