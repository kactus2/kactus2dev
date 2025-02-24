//-----------------------------------------------------------------------------
// File: interfacemodestack.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.06.2012
//
// Description:
// Container for editor on the general tab of a bus interface editor.
//-----------------------------------------------------------------------------

#ifndef INTERFACEMODESTACK_H
#define INTERFACEMODESTACK_H

#include "MasterModeEditor.h"
#include "MirroredMasterModeEditor.h"
#include "SlaveModeEditor.h"
#include "MirroredSlaveModeEditor.h"
#include "InitiatorModeEditor.h"
#include "MirroredInitiatorModeEditor.h"
#include "TargetModeEditor.h"
#include "MirroredTargetModeEditor.h"
#include "MonitorModeEditor.h"
#include "SystemModeEditor.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <KactusAPI/include/ParameterFinder.h>

#include <QStackedWidget>
#include <QSharedPointer>

class BusInterface;
class Component;
class BusIfGeneralTab;
class ExpressionParser;
class BusInterfaceInterface;

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
	 *    @param [in] busInterface        Interface for accessing bus interfaces.
     *    @param [in] busName             Name of the edited bus interface.
	 *    @param [in] component           The component being edited.
	 *    @param [in] parameterFinder     The parameter finder.
	 *    @param [in] handler             The instance that manages the library.
	 *    @param [in] expressionParser    The expression parser.
	 *    @param [in] parent              The owner of this stack.
	 */
    InterfaceModeStack(BusInterfaceInterface* busInterface,
        std::string const& busName,
        QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        LibraryInterface* handler,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget* parent);

	/*!
     *  The destructor.
     */
	virtual ~InterfaceModeStack() = default;

	/*!
     *  Refresh the contents of the stack's editors.
     */
	void refresh();

    /*!
     *  Change the name of the edited bus interface.
     *
     *    @param [in] newName     New name of the edited bus interface.
     */
    void nameChanged(std::string const& newName);

public slots:

	/*!
     *  Select the correct editor for the given interface mode.
	 *
	 *    @param [in] mode The mode for the bus interface.
     */
	void setMode(General::InterfaceMode mode);

signals:

	/*!
     *  Emitted when the state of one of the editors change.
     */
	void contentChanged();

    /*!
     *  Increase the number of references made to the selected parameter.
     *
     *    @param [in] id      The id of the selected parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references made to the selected parameter.
     *
     *    @param [in] id      The id of the selected parameter.
     */
    void decreaseReferences(QString const& id);

private:
	
    //! No copying. No assignment.
	InterfaceModeStack(const InterfaceModeStack& other);
	InterfaceModeStack& operator=(const InterfaceModeStack& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;

    //! Name of the edited bus interface.
    std::string busName_;

    //! The master interface.
	MasterModeEditor master_;

    //! The slave interface.
	SlaveModeEditor slave_;

    //! The initiator interface.
    InitiatorModeEditor initiator_;

    //! The target interface.
    TargetModeEditor target_;

    //! The system interface.
	SystemModeEditor system_;

    //! The mirrored master interface.
    MirroredMasterModeEditor mirroredMaster_;

    //! The mirrored slave interface.
	MirroredSlaveModeEditor mirroredSlave_;

    //! The mirrored initiator interface.
    MirroredInitiatorModeEditor mirroredInitiator_;

    //! The mirrored tarGet interface.
    MirroredTargetModeEditor mirroredTarget_;

    //! The mirrored system interface.
	SystemModeEditor mirroredSystem_;

    //! The monitor interface.
	MonitorModeEditor monitor_;
};

#endif // INTERFACEMODESTACK_H
