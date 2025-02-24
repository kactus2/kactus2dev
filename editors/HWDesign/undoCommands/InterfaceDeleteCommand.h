//-----------------------------------------------------------------------------
// File: InterfaceDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing top interface item in design diagram.
//-----------------------------------------------------------------------------

#ifndef INTERFACEDELETECOMMANDS_H
#define INTERFACEDELETECOMMANDS_H

#include <QObject>
#include <QUndoCommand>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/VLNV.h>

class AbstractionType;
class BusInterface;
class HierarchicalBusInterfaceItem;
class DesignDiagram;
class GraphicsColumn;
class Port;
class PortMap;

//-----------------------------------------------------------------------------
//! Undo command for removing top interface item in design diagram.
//-----------------------------------------------------------------------------
class InterfaceDeleteCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *    @param [in] interface    The interface to delete.
     *    @param [in] removePorts  If true, the ports that are part of the interface are also removed.
     */
    InterfaceDeleteCommand(DesignDiagram* diagram, HierarchicalBusInterfaceItem* interface, bool removePorts,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~InterfaceDeleteCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:

    //! Emitted when the interface is deleted.
    void interfaceDeleted();

private:
    //! Disable copying.
    InterfaceDeleteCommand(InterfaceDeleteCommand const& rhs);
    InterfaceDeleteCommand& operator=(InterfaceDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram interface.
    HierarchicalBusInterfaceItem* interface_;

    //! The bus interface.
    QSharedPointer<BusInterface> busIf_;

    //! The port's parent.
    GraphicsColumn* parent_;

    //! The graphics scene.
    DesignDiagram* diagram_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

#endif // INTERFACEDELETECOMMANDS_H
