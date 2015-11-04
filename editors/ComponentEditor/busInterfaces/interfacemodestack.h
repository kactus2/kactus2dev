//-----------------------------------------------------------------------------
// File: interfacemodestack.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.06.2012
//
// Description:
// Container for editor on the general tab of a bus interface editor.
//-----------------------------------------------------------------------------

#ifndef INTERFACEMODESTACK_H
#define INTERFACEMODESTACK_H

#include "busifinterfacemaster.h"
#include "busifinterfacemonitor.h"
#include "busifinterfacemslave.h"
#include "busifinterfaceslave.h"
#include "busifinterfacesystem.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QStackedWidget>
#include <QSharedPointer>

class BusInterface;
class Component;

class BusIfGeneralTab;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! This widget stack contains the different editors to edit interface mode specific details of bus interface.
//-----------------------------------------------------------------------------
class InterfaceModeStack : public QStackedWidget
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] busif               The bus interface being edited.
	 *      @param [in] component           The component being edited.
	 *      @param [in] parameterFinder     The parameter finder.
	 *      @param [in] handler             The instance that manages the library.
	 *      @param [in] expressionParser    The expression parser.
	 *      @param [in] parent              The owner of this stack.
	 */
	InterfaceModeStack(QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
		LibraryInterface* handler,
        QSharedPointer<ExpressionParser> expressionParser,
		QWidget* parent);
	
	//! The destructor
	virtual ~InterfaceModeStack();

	//! Refresh the contents of the stack's editors.
	void refresh();

public slots:

	/*! Select the correct editor for the given interface mode.
	 *
	 *      @param [in] mode The mode for the bus interface.
	 *
	*/
	void setMode(General::InterfaceMode mode);

signals:

	//! Emitted when the state of one of the editors change.
	void contentChanged();

    /*!
     *  Increase the number of references made to the selected parameter.
     *
     *      @param [in] id      The id of the selected parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references made to the selected parameter.
     *
     *      @param [in] id      The id of the selected parameter.
     */
    void decreaseReferences(QString const& id);

private:
	
	//! No copying
	InterfaceModeStack(const InterfaceModeStack& other);

	//! No assignment
	InterfaceModeStack& operator=(const InterfaceModeStack& other);

	//! The bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! The current mode of the interface.
	General::InterfaceMode mode_;

    //! The master interface.
	BusIfInterfaceMaster master_;

    //! The slave interface.
	BusIfInterfaceSlave slave_;

    //! The system interface.
	BusIfInterfaceSystem system_;

    //! The mirrored master interface.
	BusIfInterfaceMaster mirroredMaster_;

    //! The mirrored slave interface.
	BusIfInterfaceMSlave mirroredSlave_;

    //! The mirrored system interface.
	BusIfInterfaceSystem mirroredSystem_;

    //! The monitor interface.
	BusIfInterfaceMonitor monitor_;
};

#endif // INTERFACEMODESTACK_H
