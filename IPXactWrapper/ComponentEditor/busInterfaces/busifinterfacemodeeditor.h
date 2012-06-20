/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemodeeditor.h
 */

#ifndef BUSIFINTERFACEMODEEDITOR_H
#define BUSIFINTERFACEMODEEDITOR_H

#include <models/generaldeclarations.h>
#include <models/businterface.h>
#include <models/component.h>

#include <QGroupBox>
#include <QSharedPointer>

/*! \brief Base class for different interface mode editors of a bus interface.
 *
 */
class BusIfInterfaceModeEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param busif Pointer to the bus interface being edited.
	 * \param component Pointer to the component being edited.
	 * \param title The title for the group box
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceModeEditor(QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
		const QString& title,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceModeEditor();

	/*! \brief Check for the validity of the mode specific items.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const = 0;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh() = 0;

	/*! \brief Get the interface mode of the editor
	 * 
	 * \return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const = 0;

	//! \brief Save the interface mode-specific details to the bus interface.
	virtual void saveModeSpecific() = 0;

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

protected:

	//! \brief Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

private:

	//! \brief No copying
	BusIfInterfaceModeEditor(const BusIfInterfaceModeEditor& other);

	//! No assignment
	BusIfInterfaceModeEditor& operator=(const BusIfInterfaceModeEditor& other);
};

#endif // BUSIFINTERFACEMODEEDITOR_H
