//-----------------------------------------------------------------------------
// File: DiagramAddCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.8.2011
//
// Description:
// Undo add commands for the design diagram.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMADDCOMMANDS_H
#define DIAGRAMADDCOMMANDS_H

#include <common/ColumnTypes.h>
#include <models/generaldeclarations.h>

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QObject>

class DiagramInterconnection;
class DiagramPort;
class DiagramComponent;
class DiagramInterface;
class DiagramColumn;
class DiagramColumnLayout;
class ComponentItem;

//-----------------------------------------------------------------------------
//! ColumnAddCommand class.
//-----------------------------------------------------------------------------
class ColumnAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout        The column layout.
     *      @param [in] name          The name of the column.
     *      @param [in] type          The content type.
     *      @param [in] allowedItems  The allowed items, if the content type is custom.
     *                                This value is discarded if the content type is something else.
     *                                See ColumnItemType for possible values.
     *      @param [in] parent        The parent command.
     */
    ColumnAddCommand(DiagramColumnLayout* layout, QString const& name,
                     ColumnContentType contentType, unsigned int allowedItems,
                     QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ColumnAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    ColumnAddCommand(ColumnAddCommand const& rhs);
    ColumnAddCommand& operator=(ColumnAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    DiagramColumnLayout* layout_;

    //! The name of the column.
    QString name_;

    //! The content type.
    ColumnContentType contentType_;

    //! The allowed items.
    unsigned int allowedItems_;

    //! Created diagram column.
    DiagramColumn* column_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ItemAddCommand class.
//-----------------------------------------------------------------------------
class ItemAddCommand : public QObject, public QUndoCommand
{

	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] column     The column to which to add the item.
     *      @param [in] item       The item to add.
     *      @param [in] parent     The parent command.
     */
    ItemAddCommand(DiagramColumn* column, QGraphicsItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ItemAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:

	//! \brief Emitted when a new component is instantiated to the design.
	void componentInstantiated(ComponentItem*);

	//! \brief Emitted when a component instance is removed from the design.
	void componentInstanceRemoved(ComponentItem*);

private:
    // Disable copying.
    ItemAddCommand(ItemAddCommand const& rhs);
    ItemAddCommand& operator=(ItemAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    QGraphicsItem* item_;

    //! The item's parent column.
    DiagramColumn* column_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! PortAddCommand class.
//-----------------------------------------------------------------------------
class PortAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component to which to add a port.
     *      @param [in] pos        The position where to add the port.
     *      @param [in] parent     The parent command.
     */
    PortAddCommand(DiagramComponent* component, QPointF const& pos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~PortAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    PortAddCommand(PortAddCommand const& rhs);
    PortAddCommand& operator=(PortAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The item.
    DiagramComponent* component_;

    //! The port position.
    QPointF pos_;

    //! The diagram port.
    DiagramPort* port_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ConnectionAddCommand class.
//-----------------------------------------------------------------------------
class ConnectionAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] scene  The scene.
     *      @param [in] conn   The interconnection to add.
     *      @param [in] parent The parent command.
     */
    ConnectionAddCommand(QGraphicsScene* scene, DiagramInterconnection* conn,
                         QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ConnectionAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    ConnectionAddCommand(ConnectionAddCommand const& rhs);
    ConnectionAddCommand& operator=(ConnectionAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interconnection.
    DiagramInterconnection* conn_;

    //! The interface modes for the endpoints.
    General::InterfaceMode mode1_;
    General::InterfaceMode mode2_;

    //! The port maps for the hierarchical end point (if any).
    QList< QSharedPointer<General::PortMap> > portMaps_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMADDCOMMANDS_H
