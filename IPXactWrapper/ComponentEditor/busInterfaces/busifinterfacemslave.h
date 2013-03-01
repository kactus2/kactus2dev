/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemslave.h
 */

#ifndef BUSIFINTERFACEMSLAVE_H
#define BUSIFINTERFACEMSLAVE_H

#include "busifinterfacemodeeditor.h"
#include <models/businterface.h>
#include <models/component.h>
#include <models/mirroredslaveinterface.h>

#include <QSharedPointer>
#include <QLineEdit>

class ParameterComboBox;

/*! \brief Editor to edit mirrored slave details of a bus interface.
 *
 */
class BusIfInterfaceMSlave : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param busif Pointer to the bus interface being edited.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceMSlave(QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceMSlave();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! \brief Get the interface mode of the editor
	 * 
	 * \return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const;

	//! \brief Save the interface mode-specific details to the bus interface.
	virtual void saveModeSpecific();

private slots:

	//! \brief Handler for changes in remap address.
	void onRemapChange(const QString& newRemapAddress);

	//! \brief Handler for changes in range.
	void onRangeChange(const QString& newRange);

private:
	
	//! \brief No copying
	BusIfInterfaceMSlave(const BusIfInterfaceMSlave& other);

	//! No assignment
	BusIfInterfaceMSlave& operator=(const BusIfInterfaceMSlave& other);

	//! \brief Pointer to the mirrored slave interface mode being edited.
	QSharedPointer<MirroredSlaveInterface> mirroredSlave_;

	//! \brief Editor to set the remap address.
	ParameterComboBox* remapEdit_;

	//! \brief Editor to set the range.
	QLineEdit* rangeEdit_;
};

#endif // BUSIFINTERFACEMSLAVE_H
