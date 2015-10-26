/* 
 *  	Created on: 26.6.2012
 *      Author: Antti Kamppi
 * 		filename: interfaceselector.h
 *		Project: Kactus 2
 */

#ifndef INTERFACESELECTOR_H
#define INTERFACESELECTOR_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QComboBox>
#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! A combo box to select a bus interface within a component.
//-----------------------------------------------------------------------------
class InterfaceSelector : public QComboBox
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] component   The component that's interfaces are being selected.
	 *      @param [in] parent      The owner of this combo box.
	 *      @param [in] mode        The mode that defines the types of interfaces to show.
	*/
	InterfaceSelector(QSharedPointer<Component> component, QWidget* parent,
		General::InterfaceMode mode = General::INTERFACE_MODE_COUNT);

	//! The destructor
	virtual	~InterfaceSelector();

	/*! Set the interface mode filter.
	 *
	 *      @param [in] mode The mode that defines the types of interfaces to show.
	 *
	 * Note: If the mode is undefined (General::INTERFACE_MODE_COUNT) then all
	 * types of interfaces are shown.
	*/
	void setInterfaceMode(General::InterfaceMode mode);

public slots:

	//! Refresh the contents of the combo box.
	void refresh();

	/*! Set the specified interface as selected.
	 *
	 *      @param [in] interfaceName The name of the interface to select.
	*/
	void selectInterface(const QString& interfaceName);

signals:

	//! Emitted when interface was selected by user.
	void interfaceSelected(const QString& interfaceName);

private slots:

	//! Handler for selected index changes.
	void onIndexChange(int newIndex);

private:
	
	//! No copying
	InterfaceSelector(const InterfaceSelector& other);

	//! No assignment
	InterfaceSelector& operator=(const InterfaceSelector& other);

	//! The filter that is used to limit the possible options.
	General::InterfaceMode mode_;

	//! The component that's interfaces are selected.
	QSharedPointer<Component> component_;
};

#endif // INTERFACESELECTOR_H
