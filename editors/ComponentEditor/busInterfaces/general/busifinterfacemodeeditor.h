/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemodeeditor.h
 */

#ifndef BUSIFINTERFACEMODEEDITOR_H
#define BUSIFINTERFACEMODEEDITOR_H

#include <IPXACTmodels/generaldeclarations.h>


#include <QGroupBox>
#include <QSharedPointer>

class BusInterface;
class Component;

//-----------------------------------------------------------------------------
//! Base class for different interface mode editors of a bus interface.
//-----------------------------------------------------------------------------
class BusIfInterfaceModeEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] busif       The bus interface being edited.
	 *      @param [in] component   The component being edited.
	 *      @param [in] title       The title for the group box
	 *      @param [in] parent      The owner of this editor.
	 *
	*/
	BusIfInterfaceModeEditor(QSharedPointer<BusInterface> busif, QSharedPointer<Component> component,
		QString const& title, QWidget *parent);
	
	//! The destructor
	virtual ~BusIfInterfaceModeEditor() = default;

	/*! Check for the validity of the mode specific items.
	*
	*      @return True if item is valid.
	*/
	virtual bool isValid() const = 0;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh() = 0;

	/*! Get the interface mode of the editor
	 * 
	 *      @return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const = 0;

	//! Save the interface mode-specific details to the bus interface.
	virtual void saveModeSpecific() = 0;

    //! No copying. No assignment.
    BusIfInterfaceModeEditor(const BusIfInterfaceModeEditor& other) = delete;
    BusIfInterfaceModeEditor& operator=(const BusIfInterfaceModeEditor& other) = delete;

signals:

	//! Emitted when contents of the model change
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(QString const& msg) const;

	//! Prints a notification to user.
	void noticeMessage(QString const& msg) const;

protected:

    /*!
     *  Get the containing bus interface.
     */
    QSharedPointer<BusInterface> getBusInterface();

    /*!
     *  Get the containing component.
     */
    QSharedPointer<Component> getComponent() const;

private:

	//! The bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! The component being edited.
	QSharedPointer<Component> component_;


};

#endif // BUSIFINTERFACEMODEEDITOR_H
