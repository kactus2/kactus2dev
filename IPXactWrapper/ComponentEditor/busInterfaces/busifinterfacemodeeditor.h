/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemodeeditor.h
 */

#ifndef BUSIFINTERFACEMODEEDITOR_H
#define BUSIFINTERFACEMODEEDITOR_H

#include <models/generaldeclarations.h>

#include <QWidget>

class BusInterface;

/*! \brief Base class for different interface mode editors of a bus interface.
 *
 */
class BusIfInterfaceModeEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param busif Pointer to the bus interface being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceModeEditor(BusInterface* busif, 
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceModeEditor();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const = 0;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void restoreChanges() = 0;

	/*! \brief Applies the changes made with the editor to the model.
	*
	* After calling this function it is no longer possible to automatically 
	* restore the previous state of the model.
	* 
	* Note: if the editor is not in valid state nothing is changed.
	*/
	virtual void applyChanges() = 0;

	/*! \brief Update the display for the editor
	 *
	 * This function should be called when the editor is displayed to the user.
	 * Base class implementation does nothing
	*/
	virtual void updateDisplay();

	/*! \brief Get the interface mode of the editor
	 * 
	 * \return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const = 0;

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

protected:

	//! \brief Get pointer to the bus interface being edited.
	BusInterface* busIf();

private:

	//! \brief No copying
	BusIfInterfaceModeEditor(const BusIfInterfaceModeEditor& other);

	//! No assignment
	BusIfInterfaceModeEditor& operator=(const BusIfInterfaceModeEditor& other);

	//! \brief Pointer to the bus interface being edited
	BusInterface* busif_;
};

#endif // BUSIFINTERFACEMODEEDITOR_H
