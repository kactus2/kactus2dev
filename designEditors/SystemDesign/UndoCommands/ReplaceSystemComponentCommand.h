//-----------------------------------------------------------------------------
// File: ReplaceSystemComponentCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.03.2016
//
// Description:
// Undo change command for replacing system component item.
//-----------------------------------------------------------------------------

#ifndef REPLACESYSTEMCOMPONENTCOMMAND_H
#define REPLACESYSTEMCOMPONENTCOMMAND_H

#include <IPXACTmodels/common/VLNV.h>

#include <QUndoCommand>

class ComponentItem;
class SystemComponentItem;
class Design;
class ComInterconnection;
class ApiInterconnection;
class GraphicsConnection;
class ConnectionEndpoint;

//-----------------------------------------------------------------------------
//! Undo command for replacing a system component instance with another one.
//-----------------------------------------------------------------------------
class ReplaceSystemComponentCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] oldComp             The old system component item.
	 *      @param [in] newComp             The new system component item.
	 *      @param [in] existing            Flag for existence checking.
	 *      @param [in] keepOld             Flag for keeping the old item.
     *      @param [in] containingDesign    The design containing the system component.
	 *      @param [in] parent              Owner of the object.
	 */
	ReplaceSystemComponentCommand(SystemComponentItem* oldComp, SystemComponentItem* newComp, bool existing,
        bool keepOld, QSharedPointer<Design> containingDesign, QUndoCommand* parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~ReplaceSystemComponentCommand();

	/*!
	 *  Undoes the command.
	 */
	virtual void undo();

	/*! 
	 *  Redoes the command.
	 */
	virtual void redo();

signals:
    //! Emitted when a new component is instantiated to the design.
    void componentInstantiated(ComponentItem*);

    //! Emitted when a component instance is removed from the design.
    void componentInstanceRemoved(ComponentItem*);

private:
    // Disable copying.
    ReplaceSystemComponentCommand(ReplaceSystemComponentCommand const& rhs);
    ReplaceSystemComponentCommand& operator=(ReplaceSystemComponentCommand const& rhs);

    /*!
     *  Change the connections of the replaced component item.
     *
     *      @param [in] oldComponent    The component item being changed.
     *      @param [in] newComponent    The new component item.
     *      @param [in] design          The design containing the connections.
     */
    void changeConnections(SystemComponentItem* oldComponent, SystemComponentItem* newComponent,
        QSharedPointer<Design> design);

    /*!
     *  Create exchange command for the replaced connection.
     *
     *      @param [in] connection      The selected graphics connection.
     *      @param [in] oldEndpoint     The old end point.
     *      @param [in] newEndpoint     The new end point.
     */
    void createConnectionExchangeCommand(GraphicsConnection* connection, ConnectionEndpoint* oldEndpoint,
        ConnectionEndpoint* newEndpoint);

    /*!
     *  Create delete commands for a selected connection.
     *
     *      @param [in] connection  The selected connection.
     *      @param [in] design      Design containing the selected connection.
     */
    void createConnectionDeleteCommand(GraphicsConnection* connection, QSharedPointer<Design> design);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The old component to replace.
    SystemComponentItem* oldComp_;

    //! The new component that replaces the old one.
    SystemComponentItem* newComp_;

    //! Replaced component exists?
    bool existing_;
};

//-----------------------------------------------------------------------------

#endif // REPLACESYSTEMCOMPONENTCOMMAND_H
