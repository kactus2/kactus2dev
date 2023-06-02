//-----------------------------------------------------------------------------
// File: ReplaceComponentCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.04.2016
//
// Description:
// Undo command for replacing a HW component instance with another one.
//-----------------------------------------------------------------------------

#ifndef REPLACECOMPONENTCOMMAND_H
#define REPLACECOMPONENTCOMMAND_H

#include <QUndoCommand>

#include <QPointF>

class Design;

class ComponentItem;
class DesignDiagram;
class HWComponentItem;
class HWConnectionEndpoint;
class ConnectionEndpoint;
class GraphicsConnection;
class ComponentInstance;
class DesignDiagram;

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
     *      @param [in] diagram     Diagram containing the scene of the component items.
	 *      @param [in] parent      Owner of this command.
	 */
    ReplaceComponentCommand(HWComponentItem* oldComp, HWComponentItem* newComp, DesignDiagram* diagram,
        QUndoCommand* parent = 0);

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
    void changeConnections(HWComponentItem* oldComponentItem, HWComponentItem* newComponentItem, 
        QVector<std::string>& connectionNames);

    /*!
     *  Create exchange commands for connections between the end points.
     *
     *      @param [in] connectionNames     Names of the changed connections.
     *      @param [in] oldEndpoint         The replaced end point.
     *      @param [in] newEndpoint         The new end point.
     */
    void createConnectionExchangeCommands(ConnectionEndpoint* oldEndpoint,
        HWConnectionEndpoint* newEndpoint, QVector<std::string>& connectionNames);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The old component to replace.
    QPointF position_;

    //! The new component that replaces the old one.
    HWComponentItem* newComp_;

    //! Diagram containing the scene of the component items.
    DesignDiagram* diagram_;
};

//-----------------------------------------------------------------------------

#endif // REPLACECOMPONENTCOMMAND_H
