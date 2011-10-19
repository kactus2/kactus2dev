//-----------------------------------------------------------------------------
// File: SystemDeleteCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.10.2011
//
// Description:
// Undo delete commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDELETECOMMANDS_H
#define SYSTEMDELETECOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class SystemColumnLayout;
class SystemColumn;
class EndpointConnection;
class EndpointItem;
class EndpointStack;
class PlatformComponentItem;
class MappingComponentItem;
class SWComponentItem;
class ApplicationItem;
class ProgramEntityItem;

//-----------------------------------------------------------------------------
//! SystemColumnDeleteCommand class.
//-----------------------------------------------------------------------------
class SystemColumnDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout  The column layout.
     *      @param [in] column  The column to delete.
     */
    SystemColumnDeleteCommand(SystemColumnLayout* layout, SystemColumn* column,
                              QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemColumnDeleteCommand();

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
    SystemColumnDeleteCommand(SystemColumnDeleteCommand const& rhs);
    SystemColumnDeleteCommand& operator=(SystemColumnDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    SystemColumnLayout* layout_;

    //! The diagram column.
    SystemColumn* column_;

    //! Boolean flag for indicating if the column should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! EndpointConnectionDeleteCommand class.
//-----------------------------------------------------------------------------
class EndpointConnectionDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] conn    The connection to delete.
     *      @param [in] parent  The parent undo command.
     */
    EndpointConnectionDeleteCommand(EndpointConnection* conn, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointConnectionDeleteCommand();

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
    EndpointConnectionDeleteCommand(EndpointConnectionDeleteCommand const& rhs);
    EndpointConnectionDeleteCommand& operator=(EndpointConnectionDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interconnection.
    EndpointConnection* conn_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! EndpointDeleteCommand class.
//-----------------------------------------------------------------------------
class EndpointDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint  The endpoint to delete.
     *      @param [in] parent    The parent undo command.
     */
    EndpointDeleteCommand(EndpointItem* endpoint, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointDeleteCommand();

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
    EndpointDeleteCommand(EndpointDeleteCommand const& rhs);
    EndpointDeleteCommand& operator=(EndpointDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint.
    EndpointItem* endpoint_;

    //! The endpoint's parent stack.
    EndpointStack* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! PlatformCompDeleteCommand class.
//-----------------------------------------------------------------------------
class PlatformCompDeleteCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] item    The platform component item to delete.
     *      @param [in] parent  The parent undo command.
     */
    PlatformCompDeleteCommand(PlatformComponentItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~PlatformCompDeleteCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:
    //! Signaled when the component is instantiated to the design.
    void componentInstantiated(SWComponentItem* item);

    //! Signaled when the component instance is removed from the design.
    void componentInstanceRemoved(SWComponentItem* item);

private:
    // Disable copying.
    PlatformCompDeleteCommand(PlatformCompDeleteCommand const& rhs);
    PlatformCompDeleteCommand& operator=(PlatformCompDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The platform component item.
    PlatformComponentItem* item_;

    //! The platform component's parent mapping component.
    MappingComponentItem* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ApplicationDeleteCommand class.
//-----------------------------------------------------------------------------
class ApplicationDeleteCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] item    The application item to delete.
     *      @param [in] parent  The parent undo command.
     */
    ApplicationDeleteCommand(ApplicationItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ApplicationDeleteCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:
    //! Signaled when the component is instantiated to the design.
    void componentInstantiated(SWComponentItem* item);

    //! Signaled when the component instance is removed from the design.
    void componentInstanceRemoved(SWComponentItem* item);

private:
    // Disable copying.
    ApplicationDeleteCommand(ApplicationDeleteCommand const& rhs);
    ApplicationDeleteCommand& operator=(ApplicationDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The application item.
    ApplicationItem* item_;

    //! The endpoint's parent program entity.
    ProgramEntityItem* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ProgramEntityDeleteCommand class.
//-----------------------------------------------------------------------------
class ProgramEntityDeleteCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] item    The program entity item to delete.
     *      @param [in] parent  The parent undo command.
     */
    ProgramEntityDeleteCommand(ProgramEntityItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ProgramEntityDeleteCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:
    //! Signaled when the component is instantiated to the design.
    void componentInstantiated(SWComponentItem* item);

    //! Signaled when the component instance is removed from the design.
    void componentInstanceRemoved(SWComponentItem* item);

private:
    // Disable copying.
    ProgramEntityDeleteCommand(ProgramEntityDeleteCommand const& rhs);
    ProgramEntityDeleteCommand& operator=(ProgramEntityDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The program entity item.
    ProgramEntityItem* item_;

    //! The program entity's parent mapping component.
    MappingComponentItem* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! MappingCompDeleteCommand class.
//-----------------------------------------------------------------------------
class MappingCompDeleteCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] item The mapping component item to delete.
     */
    MappingCompDeleteCommand(MappingComponentItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~MappingCompDeleteCommand();

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
	void componentInstantiated(SWComponentItem* comp);

	//! \brief Emitted when a component instance is removed from the design.
	void componentInstanceRemoved(SWComponentItem* comp);

private:
    // Disable copying.
    MappingCompDeleteCommand(MappingCompDeleteCommand const& rhs);
    MappingCompDeleteCommand& operator=(MappingCompDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The mapping component item.
    MappingComponentItem* item_;

    //! The component's parent column.
    SystemColumn* column_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDELETECOMMANDS_H
