/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraltab.h
 */

#ifndef BUSIFGENERALTAB_H
#define BUSIFGENERALTAB_H

#include <common/widgets/nameGroupBox/namegroupbox.h>
#include <common/widgets/attributeBox/attributebox.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include "busifgeneraldetails.h"

#include <QWidget>

class BusInterface;
class LibraryInterface;

/*! \brief Container for editor on the general tab of a bus interface editor.
 *
 */
class BusIfGeneralTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the library handler instance.
	 * \param dataPointer Pointer to the bus interface being edited.
	 * \param parent Pointer to the parent of this widget.
	 *
	*/
	BusIfGeneralTab(LibraryInterface* libHandler,
		void* dataPointer, 
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfGeneralTab();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void restoreChanges();

	/*! \brief Applies the changes made with the editor to the model.
	*
	* After calling this function it is no longer possible to automatically 
	* restore the previous state of the model.
	* 
	* Note: if the editor is not in valid state nothing is changed.
	*/
	virtual void applyChanges();

	/*! \brief Get the currently set bus type.
	 *
	 * \return VLNV that is set for the current bus type.
	*/
	VLNV getBusType() const;

	/*! \brief Get the currently set abstraction type.
	 *
	 * \return VLNV that is set for the current abstraction type.
	*/
	VLNV getAbsType() const;

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	/*! \brief Emitted when name of the bus interface changes
	 *
	 * \param msg QString containing the new name
	 *
	*/
	void nameChanged(const QString& msg);

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:

	//! \brief No copying
	BusIfGeneralTab(const BusIfGeneralTab& other);

	//! No assignment
	BusIfGeneralTab& operator=(const BusIfGeneralTab& other);
	
	//! \brief Pointer to the bus interface being edited.
	BusInterface* busif_;

	//! \brief Contains the name, display name and description of bus interface
	NameGroupBox nameGroup_;

	//! \brief Contains the vlnv of the bus type for this bus interface
	VLNVEditor busType_;

	//! \brief Contains the vlnv of the abstraction type for this interface
	VLNVEditor absType_;

	//! \brief Contains the details of this bus interface
	BusIfGeneralDetails details_;

	//! \brief Pointer to the instance that manages the library
	LibraryInterface* libHandler_;
};

#endif // BUSIFGENERALTAB_H
