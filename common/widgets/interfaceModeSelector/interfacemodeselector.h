/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: interfacemodeselector.h
 *		Project: Kactus 2
 */

#ifndef INTERFACEMODESELECTOR_H
#define INTERFACEMODESELECTOR_H

#include <models/businterface.h>
#include <models/generaldeclarations.h>

#include <QComboBox>
#include <QSharedPointer>

/*! \brief The editor to select an interface mode for bus interface within a component.
 *
 */
class InterfaceModeSelector : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this editor,
	 * \param busif Pointer to the bus interface that sets the selected interface at start.
	 * \param showMonitor If this is true then also "monitor" option is shown.
	 *
	*/
	InterfaceModeSelector(QWidget *parent, 
		QSharedPointer<BusInterface> busif = QSharedPointer<BusInterface>(),
		bool showMonitor = true);

	/*! \brief The constructor
	 *
	 * \param mode The interface mode that is selected at start.
	 * \param parent Pointer to the owner of the editor.
	 * \param showMonitor If this is true then also "monitor" option is shown.
	 *
	*/
	InterfaceModeSelector(General::InterfaceMode mode, 
		QWidget* parent, 
		bool showMonitor = true);

	//! \brief The destructor
	virtual ~InterfaceModeSelector();

	/*! \brief Get the currently selected mode.
	 *
	 * \return The selected interface mode.
	*/
	General::InterfaceMode selected() const;

public slots:

	/*! \brief Set the selected mode.
	 * 
	 * This slot does not emit any signals when it is called. 
	 *
	 * \param mode The mode to set selected.
	 *
	*/
	void setMode(const General::InterfaceMode mode);

	/*! \brief Set the selected mode.
	 *
	 * Calling this slot emits the modeSelected(mode) signal.
	 * 
	 * \param modeName The name of the mode. This should be one of the defined in General::INTERFACE_MODE_NAMES
	 *
	*/
	void setMode(const QString& modeName);

signals:

	//! \brief Emitted when the selected interface mode changes.
	void modeSelected(General::InterfaceMode mode);

private:

	//! \brief No copying
	InterfaceModeSelector(const InterfaceModeSelector& other);

	//! \brief No assignment
	InterfaceModeSelector& operator=(const InterfaceModeSelector& other);

	//! \brief Set up the editor.
	void initialize();

	//! \brief If this is true then the "monitor" option is shown in drop down list.
	bool showMonitor_;
};

#endif // INTERFACEMODESELECTOR_H
