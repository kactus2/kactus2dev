//-----------------------------------------------------------------------------
// File: ReplaceComponentCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.04.2016
//
// Description:
// Undo command for replacing a HW component instance with another one.
//-----------------------------------------------------------------------------

#ifndef REPLACECOMPONENTCOMMAND_H
#define REPLACECOMPONENTCOMMAND_H

#include <QUndoCommand>

class ComponentItem;
class DesignDiagram;
class HWComponentItem;
class HWConnectionEndpoint;
class ConnectionEndpoint;
class GraphicsConnection;

class ComponentInstance;

//-----------------------------------------------------------------------------
//! Undo command for replacing a HW component instance with another one.
//-----------------------------------------------------------------------------
class ReplaceComponentCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] diagram     The containing design diagram.
	 *      @param [in] oldComp     The replaced component.
	 *      @param [in] newComp     The new component.
	 *      @param [in] existing    Flag for existence checking.
	 *      @param [in] keepOld     Flag for keeping the old item.
	 *      @param [in] parent      Owner of this command.
	 */
	ReplaceComponentCommand(DesignDiagram* diagram, HWComponentItem* oldComp, HWComponentItem* newComp,
                            bool existing, bool keepOld, QUndoCommand* parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~ReplaceComponentCommand();

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
    ReplaceComponentCommand(ReplaceComponentCommand const& rhs);
    ReplaceComponentCommand& operator=(ReplaceComponentCommand const& rhs);

    /*!
     *  Change connections.
     *
     *      @param [in] keepExistingComponent   Flag for existence checking.
     *      @param [in] connectionNames         Names of the changed connections.
     *      @param [in] oldComponentItem        The replaced component.
     *      @param [in] newComponentItem        The new component.
     *      @param [in] diagram                 The containing design diagram.
     */
    void changeConnections(bool keepExistingComponent, QStringList& connectionNames,
        HWComponentItem* oldComponentItem, HWComponentItem* newComponentItem, DesignDiagram* diagram);

    /*!
     *  Create exchange commands for connections between the end points.
     *
     *      @param [in] connectionNames     Names of the changed connections.
     *      @param [in] oldEndpoint         The replaced end point.
     *      @param [in] newEndpoint         The new end point.
     */
    void createConnectionExchangeCommands(QStringList& connectionNames, ConnectionEndpoint* oldEndpoint,
        HWConnectionEndpoint* newEndpoint);

    /*!
     *  Create delete commands for connections in the selected end point.
     *
     *      @param [in] connectionNames     Names of the changed connections.
     *      @param [in] deletedEndPoint     The selected end point.
     *      @param [in] diagram             The containing design diagram.
     */
    void createConnectionDeleteCommands(QStringList& connectionNames, ConnectionEndpoint* deletedEndPoint,
        DesignDiagram* diagram);

    /*!
     *  Create a single delete command for the selected connection.
     *
     *      @param [in] connectionNames     Names of the deleted connections.
     *      @param [in] connection          The selected connection.
     *      @param [in] diagram             The containing design diagram.
     */
    void createSingleConnectionDeleteCommand(QStringList& connectionNames, GraphicsConnection* connection,
        DesignDiagram* diagram);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The old component to replace.
    HWComponentItem* oldComp_;

    //! The new component that replaces the old one.
    HWComponentItem* newComp_;
};

//-----------------------------------------------------------------------------

#endif // REPLACECOMPONENTCOMMAND_H
