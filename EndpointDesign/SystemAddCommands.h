//-----------------------------------------------------------------------------
// File: SystemAddCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.10.2011
//
// Description:
// Undo add commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMADDCOMMANDS_H
#define SYSTEMADDCOMMANDS_H

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
class ComponentItem;
class ApplicationItem;
class ProgramEntityItem;
class IComponentStack;
class SWConnection;

//-----------------------------------------------------------------------------
//! SystemColumnAddCommand class.
//-----------------------------------------------------------------------------
class SystemColumnAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout        The column layout.
     *      @param [in] name          The name of the column.
     *      @param [in] parent        The parent command.
     */
    SystemColumnAddCommand(SystemColumnLayout* layout, QString const& name,
                           QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemColumnAddCommand();

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
    SystemColumnAddCommand(SystemColumnAddCommand const& rhs);
    SystemColumnAddCommand& operator=(SystemColumnAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    SystemColumnLayout* layout_;

    //! The name of the column.
    QString name_;

    //! Created diagram column.
    SystemColumn* column_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! SWConnectionAddCommand class.
//-----------------------------------------------------------------------------
class SWConnectionAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] scene  The scene.
     *      @param [in] conn   The connection to add.
     *      @param [in] parent The parent command.
     */
    SWConnectionAddCommand(QGraphicsScene* scene, SWConnection* conn,
                           QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWConnectionAddCommand();

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
    SWConnectionAddCommand(SWConnectionAddCommand const& rhs);
    SWConnectionAddCommand& operator=(SWConnectionAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint connection.
    SWConnection* conn_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! EndpointConnectionAddCommand class.
//-----------------------------------------------------------------------------
class EndpointConnectionAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] scene  The scene.
     *      @param [in] conn   The connection to add.
     *      @param [in] parent The parent command.
     */
    EndpointConnectionAddCommand(QGraphicsScene* scene, EndpointConnection* conn,
                                 QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointConnectionAddCommand();

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
    EndpointConnectionAddCommand(EndpointConnectionAddCommand const& rhs);
    EndpointConnectionAddCommand& operator=(EndpointConnectionAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint connection.
    EndpointConnection* conn_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! PlatformCompAddCommand class.
//-----------------------------------------------------------------------------
class PlatformCompAddCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] mappingComp  The parent mapping component.
     *      @param [in] item         The platform component item to add.
     *      @param [in] parent       The parent undo command.
     */
    PlatformCompAddCommand(MappingComponentItem* mappingComp, PlatformComponentItem* item,
                           QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~PlatformCompAddCommand();

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
    void componentInstantiated(ComponentItem* item);

    //! Signaled when the component instance is removed from the design.
    void componentInstanceRemoved(ComponentItem* item);

private:
    // Disable copying.
    PlatformCompAddCommand(PlatformCompAddCommand const& rhs);
    PlatformCompAddCommand& operator=(PlatformCompAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The platform component item.
    PlatformComponentItem* item_;

    //! The platform component's parent mapping component.
    MappingComponentItem* mappingComp_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ProgramEntityAddCommand class.
//-----------------------------------------------------------------------------
class ProgramEntityAddCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] mappingComp  The parent mapping component.
     *      @param [in] item         The program entity item to add.
     *      @param [in] parent       The parent undo command.
     */
    ProgramEntityAddCommand(MappingComponentItem* mappingComp, ProgramEntityItem* item,
                            QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ProgramEntityAddCommand();

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
    void componentInstantiated(ComponentItem* item);

    //! Signaled when the component instance is removed from the design.
    void componentInstanceRemoved(ComponentItem* item);

private:
    // Disable copying.
    ProgramEntityAddCommand(ProgramEntityAddCommand const& rhs);
    ProgramEntityAddCommand& operator=(ProgramEntityAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The program entity item.
    ProgramEntityItem* item_;

    //! The parent mapping component.
    MappingComponentItem* mappingComp_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ApplicationAddCommand class.
//-----------------------------------------------------------------------------
class ApplicationAddCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] progEntity  The parent program entity.
     *      @param [in] item        The application item to add.
     *      @param [in] parent      The parent undo command.
     */
    ApplicationAddCommand(ProgramEntityItem* progEntity, ApplicationItem* item,
                          QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ApplicationAddCommand();

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
    void componentInstantiated(ComponentItem* item);

    //! Signaled when the component instance is removed from the design.
    void componentInstanceRemoved(ComponentItem* item);

private:
    // Disable copying.
    ApplicationAddCommand(ApplicationAddCommand const& rhs);
    ApplicationAddCommand& operator=(ApplicationAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The application item.
    ApplicationItem* item_;

    //! The parent program entity item.
    ProgramEntityItem* progEntity_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! SystemItemAddCommand class.
//-----------------------------------------------------------------------------
class SystemItemAddCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] stack   The component stack to which to add the componetn.
     *      @param [in] item    The mapping component item to add.
     *      @param [in] parent  The parent command.
     */
    SystemItemAddCommand(IComponentStack* stack, ComponentItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemItemAddCommand();

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
	void componentInstantiated(ComponentItem* comp);

	//! Signaled when the component instance is removed from the design.
	void componentInstanceRemoved(ComponentItem* comp);

private:
    // Disable copying.
    SystemItemAddCommand(SystemItemAddCommand const& rhs);
    SystemItemAddCommand& operator=(SystemItemAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component item.
    ComponentItem* item_;

    //! The item's parent component stack.
    IComponentStack* stack_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! EndpointAddCommand class.
//-----------------------------------------------------------------------------
class EndpointAddCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] stack   The parent endpoint stack.
     *      @param [in] item    The endpoint item to add.
     *      @param [in] parent  The parent undo command.
     */
    EndpointAddCommand(EndpointStack* stack, EndpointItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointAddCommand();

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
    EndpointAddCommand(EndpointAddCommand const& rhs);
    EndpointAddCommand& operator=(EndpointAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint item.
    EndpointItem* item_;

    //! The parent endpoint stack.
    EndpointStack* stack_;

    //! Boolean flag for indicating if the endpoint should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMADDCOMMANDS_H
