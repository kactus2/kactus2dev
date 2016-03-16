//-----------------------------------------------------------------------------
// File: SystemComponentDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.02.2016
//
// Description:
// Delete command for component instances in system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCOMPONENTDELETECOMMAND_H
#define SYSTEMCOMPONENTDELETECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QObject>

class ComponentItem;
class IGraphicsItemStack;

class Design;
class GraphicsConnection;

//-----------------------------------------------------------------------------
//! Delete command for component instances in system designs.
//-----------------------------------------------------------------------------
class SystemComponentDeleteCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  The constructor.
     *
     *      @param [in] item                The component item to delete.
     *      @param [in] containingDesign    The design containing the system component item.
     *      @param [in] parent              The owner of this command.
     */
    SystemComponentDeleteCommand(ComponentItem* item, QSharedPointer<Design> containingDesign,
        QUndoCommand* parent = 0);

    /*!
     *  The destructor.
     */
    ~SystemComponentDeleteCommand();

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
	void componentInstantiated(ComponentItem* comp);

	//! Emitted when a component instance is removed from the design.
	void componentInstanceRemoved(ComponentItem* comp);

private:
    // Disable copying.
    SystemComponentDeleteCommand(SystemComponentDeleteCommand const& rhs);
    SystemComponentDeleteCommand& operator=(SystemComponentDeleteCommand const& rhs);

    /*!
     *  Add delete command for the contained interconnection.
     *
     *      @param [in] connection  The selected connection.
     */
    void addConnectionDeleteCommand(GraphicsConnection* connection);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component item.
    ComponentItem* item_;

    //! The component's parent column.
    IGraphicsItemStack* stack_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;

    //! If true, the command has not been run previously.
    bool firstRun_;

    //! The design containing the system component item.
    QSharedPointer<Design> containingDesign_;
};

#endif // SYSTEMCOMPONENTDELETECOMMAND_H
