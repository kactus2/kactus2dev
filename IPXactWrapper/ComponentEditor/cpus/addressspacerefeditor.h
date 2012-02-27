/* 
 *  	Created on: 27.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacerefeditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEREFEDITOR_H
#define ADDRESSSPACEREFEDITOR_H

#include <common/widgets/listManager/listmanager.h>
#include <models/component.h>

#include <QSharedPointer>

/*! \brief Editor to edit a list of references to address spaces.
 *
 */ 
class AddressSpaceRefEditor : public ListManager {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param title The title for the group box.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	AddressSpaceRefEditor(QSharedPointer<Component> component,
		const QString title = tr("Address space references"), 
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~AddressSpaceRefEditor();

	/*! \brief Initialize the address space reference editor.
	 *
	 * This function must be called after creating the address space reference 
	 * editor and before using it.
	 * 
	 * \param items QStringList that contains the items to add to the widget.
	 *
	*/
	virtual void initialize(const QStringList& items = QStringList());

private:
	//! \brief No copying
	AddressSpaceRefEditor(const AddressSpaceRefEditor& other);

	//! \brief No assignment
	AddressSpaceRefEditor& operator=(const AddressSpaceRefEditor& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // ADDRESSSPACEREFEDITOR_H
