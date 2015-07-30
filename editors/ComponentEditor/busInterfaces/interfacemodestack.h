/* 
 *  	Created on: 20.6.2012
 *      Author: Antti Kamppi
 * 		filename: interfacemodestack.h
 *		Project: Kactus 2
 */

#ifndef INTERFACEMODESTACK_H
#define INTERFACEMODESTACK_H

#include "busifinterfacemaster.h"
#include "busifinterfacemonitor.h"
#include "busifinterfacemslave.h"
#include "busifinterfaceslave.h"
#include "busifinterfacesystem.h"
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QStackedWidget>
#include <QSharedPointer>

class LibraryInterface;
class BusIfGeneralTab;
class ExpressionParser;

/*! \brief This widget stack contains the different editors to edit interface mode specific details of bus interface.
 *
 */
class InterfaceModeStack : public QStackedWidget
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] busif               Pointer to the bus interface being edited.
	 *      @param [in] component           Pointer to the component being edited.
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
	 *      @param [in] handler             Pointer to the instance that manages the library.
	 *      @param [in] expressionParser    Pointer to the expression parser.
	 *      @param [in] parent              Pointer to the owner of this stack.
	 */
	InterfaceModeStack(QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
		LibraryInterface* handler,
        QSharedPointer<ExpressionParser> expressionParser,
		BusIfGeneralTab* parent);
	
	//! \brief The destructor
	virtual ~InterfaceModeStack();

	//! \brief Refresh the contents of the stack's editors.
	void refresh();

public slots:

	/*! \brief Select the correct editor for the given interface mode.
	 *
	 * \param mode The mode for the bus interface.
	 *
	*/
	void setMode(General::InterfaceMode mode);

signals:

	//! \brief Emitted when the state of one of the editors change.
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
	
	//! \brief No copying
	InterfaceModeStack(const InterfaceModeStack& other);

	//! \brief No assignment
	InterfaceModeStack& operator=(const InterfaceModeStack& other);

	//! \brief Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! \brief Pointer to the owner of this stack.
	BusIfGeneralTab* parent_;

	//! \brief The current mode of the interface.
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
