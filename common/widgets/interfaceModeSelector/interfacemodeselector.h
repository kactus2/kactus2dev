//-----------------------------------------------------------------------------
// File: interfacemodeselector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// The editor to select an interface mode for bus interface within a component.
//-----------------------------------------------------------------------------

#ifndef INTERFACEMODESELECTOR_H
#define INTERFACEMODESELECTOR_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QComboBox>
#include <QSharedPointer>

class BusInterface;

//-----------------------------------------------------------------------------
//! The editor to select an interface mode for bus interface within a component.
//-----------------------------------------------------------------------------
class InterfaceModeSelector : public QComboBox
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent The owner of this editor,
	 *      @param [in] busif The bus interface that sets the selected interface at start.
	 *      @param [in] showMonitor If this is true then also "monitor" option is shown.
	 *
	*/
	InterfaceModeSelector(QWidget *parent, QSharedPointer<BusInterface> busif = QSharedPointer<BusInterface>(),
		bool showMonitor = true);

	/*! The constructor
	 *
	 *      @param [in] mode The interface mode that is selected at start.
	 *      @param [in] parent The owner of the editor.
	 *      @param [in] showMonitor If this is true then also "monitor" option is shown.
	 *
	*/
	InterfaceModeSelector(General::InterfaceMode mode, 	QWidget* parent, bool showMonitor = true);

	//! The destructor
	virtual ~InterfaceModeSelector();

	/*! Get the currently selected mode.
	 *
	 *      @return The selected interface mode.
	*/
	General::InterfaceMode selected() const;

public slots:

	/*! Set the selected mode.
	 * 
	 * This slot does not emit any signals when it is called. 
	 *
	 *      @param [in] mode The mode to set selected.
	*/
	void setMode(const General::InterfaceMode mode);

	/*! Set the selected mode.
	 *
	 * Calling this slot emits the modeSelected(mode) signal.
	 * 
	 *      @param [in] modeName The name of the mode. This should be one of the defined in General::INTERFACE_MODE_NAMES
	*/
	void setMode(QString const& modeName);

signals:

	//! Emitted when the selected interface mode changes.
	void modeSelected(General::InterfaceMode mode);

private:

	//! No copying
	InterfaceModeSelector(const InterfaceModeSelector& other);

	//! No assignment
	InterfaceModeSelector& operator=(const InterfaceModeSelector& other);

	//! Set up the editor.
	void initialize();

	//! If this is true then the "monitor" option is shown in drop down list.
	bool showMonitor_;
};

#endif // INTERFACEMODESELECTOR_H
