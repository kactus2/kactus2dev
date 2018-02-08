//-----------------------------------------------------------------------------
// File: HWChangeCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.8.2011
//
// Description:
// Undo change commands for the block diagram.
//-----------------------------------------------------------------------------

#ifndef HWCHANGECOMMANDS_H
#define HWCHANGECOMMANDS_H

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>

#include <QUndoCommand>
#include <QSharedPointer>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QMap>

class ActiveViewModel;
class AdHocEnabled;
class AdHocConnectionItem;
class AdHocPortItem;
class BusInterfaceItem;
class BusPortItem;
class ComponentItem;
class ConfigurableElementValue;
class ConnectionEndpoint;
class DesignDiagram;
class GraphicsColumn;
class GraphicsColumnLayout;
class GraphicsConnection;
class HWComponentItem;
class HWConnection;
class HWConnectionEndpoint;

class AdHocConnection;
class ComponentInstance;
class Design;
class HierInterface;
class ViewConfiguration;
class PortReference;
class AbstractionType;

//-----------------------------------------------------------------------------
//! ComponentChangeNameCommand class.
//-----------------------------------------------------------------------------
class ComponentChangeNameCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component   The component.
     *      @param [in] newName     The component's new name.
     *      @param [in] design      Design containing the component instance.
     *      @param [in] parent      The parent command.
     */
    ComponentChangeNameCommand(ComponentItem* component, QString const& newName, QSharedPointer<Design> design,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComponentChangeNameCommand();

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
    ComponentChangeNameCommand(ComponentChangeNameCommand const& rhs);
    ComponentChangeNameCommand& operator=(ComponentChangeNameCommand const& rhs);

    /*!
     *  Renames the component instance and change the component reference in connections.
     *
     *      @param [in] oldReference    The old component reference.
     *      @param [in] newReference    The new component reference.
     */
    void renameInstanceAndConnections(QString const& previousName, QString const& newReference);

    /*!
     *  Change the default name of an ad hoc connection.
     *
     *      @param [in] connection      The selected ad hoc connection.
     *      @param [in] portReference   The reference port of the containing interface.
     *      @param [in] oldReference    Old component reference.
     *      @param [in] newReference    New component reference.
     */
    void changeAdHocConnectionDefaultName(QSharedPointer<AdHocConnection> connection, QString const& portReference,
        QString const& oldReference, QString const& newReference);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram component.
    ComponentItem* component_;

    //! The component's old name.
    QString oldName_;

    //! The component's new name.
    QString newName_;

    //! Design containing the componnet instance.
    QSharedPointer<Design> containingDesign_;
};

class ComponentChangeDisplayNameCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component      The component.
     *      @param [in] newDisplayName The component's new display name.
     *      @param [in] parent         The parent command.
     */
    ComponentChangeDisplayNameCommand(ComponentItem* component, QString const& newDisplayName,
		QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComponentChangeDisplayNameCommand();

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
    ComponentChangeDisplayNameCommand(ComponentChangeDisplayNameCommand const& rhs);
    ComponentChangeDisplayNameCommand& operator=(ComponentChangeDisplayNameCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram component.
    ComponentItem* component_;

    //! The component's old display name.
    QString oldDisplayName_;

    //! The component's new display name.
    QString newDisplayName_;
};

class ComponentChangeDescriptionNameCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component      The component.
     *      @param [in] newDescription The component's new description.
     *      @param [in] parent         The parent command.
     */
    ComponentChangeDescriptionNameCommand(ComponentItem* component, QString const& newDescription,
		QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComponentChangeDescriptionNameCommand();

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
    ComponentChangeDescriptionNameCommand(ComponentChangeDescriptionNameCommand const& rhs);
    ComponentChangeDescriptionNameCommand& operator=(ComponentChangeDescriptionNameCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram component.
    ComponentItem* component_;

    //! The component's old display name.
    QString oldDescription_;

    //! The component's new display name.
    QString newDescription_;
};

class ComponentActiveViewChangeCommand : public QUndoCommand
{
public:

    /*!
     *  The constructor
     *
     *      @param [in] instanceName        The name of the component instance
     *      @param [in] oldActiveView       The name of the previous active view.
     *      @param [in] newActiveView       The name of the new active view.
     *      @param [in] activeViewModel     Pointer to the model that manages the active views.
     *      @param [in] parent              Pointer to the parent command.
     */
    ComponentActiveViewChangeCommand(const QString& instanceName, QString const& oldActiveView,
        QString const& newActiveView, ActiveViewModel* activeViewModel, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComponentActiveViewChangeCommand();

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
    ComponentActiveViewChangeCommand(ComponentChangeNameCommand const& rhs);
    ComponentActiveViewChangeCommand& operator=(ComponentChangeNameCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component's old active view.
    QString instanceName_;

    //! The component's new name.
    QString newViewName_;

    //! The component's new active view.
    QString oldViewName_;

	//! Pointer to the model that manages the active views.
	ActiveViewModel* activeViewModel_;
};

//-----------------------------------------------------------------------------
//! ComponentPacketizeCommand class.
//-----------------------------------------------------------------------------
class ComponentPacketizeCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] diagram     The design diagram containing the packaged component.
     *      @param [in] component   The component to package.
     *      @param [in] vlnv        The VLNV of the packaged component.
     *      @param [in] parent      The parent command.
     */
    ComponentPacketizeCommand(DesignDiagram* diagram, ComponentItem* component, VLNV const& vlnv, 
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComponentPacketizeCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:
    void contentChanged();

private:
    // Disable copying.
    ComponentPacketizeCommand(ComponentPacketizeCommand const& rhs);
    ComponentPacketizeCommand& operator=(ComponentPacketizeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The containing design diagram.
    DesignDiagram* diagram_;

    //! The diagram component.
    ComponentItem* componentItem_;

    //! The component VLNV.
    VLNV vlnv_;
};

//-----------------------------------------------------------------------------
//! EndpointChangeCommand class.
//-----------------------------------------------------------------------------
class EndpointChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint          The diagram connection end point.
     *      @param [in] newInterfaceMode  The end point's new interface mode.
     *      @param [in] parent            The parent command.
     */
	EndpointChangeCommand(ConnectionEndpoint* endpoint,
		General::InterfaceMode newMode,
		QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointChangeCommand();

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
    EndpointChangeCommand(EndpointChangeCommand const& rhs);
    EndpointChangeCommand& operator=(EndpointChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram connection end point.
    ConnectionEndpoint* endpoint_;

    //! The end point's old interface mode.
    General::InterfaceMode oldMode_;

    //! The end point's new interface mode.
    General::InterfaceMode newMode_;

};

//-----------------------------------------------------------------------------
//! EndpointNameChangeCommand class.
//-----------------------------------------------------------------------------
class EndpointNameChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint  The connection endpoint.
     *      @param [in] newName   The endpoint's new name.
     *      @param [in] parent    The parent command.
     */
    EndpointNameChangeCommand(ConnectionEndpoint* endpoint, 
        QString const& newName,
        QList<QSharedPointer<HierInterface> > activeIntefaces,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointNameChangeCommand();

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
    EndpointNameChangeCommand(EndpointNameChangeCommand const& rhs);
    EndpointNameChangeCommand& operator=(EndpointNameChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection endpoint.
    ConnectionEndpoint* endpoint_;

    //! The endpoint's old name.
    QString oldName_;

    //! The endpoint's new name.
    QString newName_;

    //! The interfaces affected by the name change.
    QList<QSharedPointer<HierInterface> > activeIntefaces_;
};

//-----------------------------------------------------------------------------
//! EndpointDescChangeCommand class.
//-----------------------------------------------------------------------------
class EndpointDescChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint        The connection endpoint.
     *      @param [in] newDescription  The endpoint's new description.
     *      @param [in] parent          The parent command.
     */
	EndpointDescChangeCommand(ConnectionEndpoint* endpoint, QString const& newDescription,
                              QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointDescChangeCommand();

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
    EndpointDescChangeCommand(EndpointDescChangeCommand const& rhs);
    EndpointDescChangeCommand& operator=(EndpointDescChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection endpoint.
    ConnectionEndpoint* endpoint_;

    //! The endpoint's old description.
    QString oldDescription_;

    //! The endpoint's new description.
    QString newDescription_;
};

//-----------------------------------------------------------------------------
//! EndpointDependencyDirectionChangeCommand class.
//-----------------------------------------------------------------------------
class EndpointDependencyDirectionChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint  The connection endpoint.
     *      @param [in] newDir    The endpoint's new dependency direction.
     *      @param [in] parent    The parent command.
     */
	EndpointDependencyDirectionChangeCommand(ConnectionEndpoint* endpoint, DependencyDirection newDir,
                                             QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointDependencyDirectionChangeCommand();

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
    EndpointDependencyDirectionChangeCommand(EndpointDependencyDirectionChangeCommand const& rhs);
    EndpointDependencyDirectionChangeCommand& operator=(EndpointDependencyDirectionChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection endpoint.
    ConnectionEndpoint* endpoint_;

    //! The endpoint's old dependency direction.
    DependencyDirection oldDir_;

    //! The endpoint's new dependency direction.
    DependencyDirection newDir_;
};

//-----------------------------------------------------------------------------
//! EndpointComDirectionChangeCommand class.
//-----------------------------------------------------------------------------
class EndpointComDirectionChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint  The connection endpoint.
     *      @param [in] newDir    The endpoint's new COM direction.
     *      @param [in] parent    The parent command.
     */
    EndpointComDirectionChangeCommand(ConnectionEndpoint* endpoint, DirectionTypes::Direction newDir,
                                      QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointComDirectionChangeCommand();

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
    EndpointComDirectionChangeCommand(EndpointComDirectionChangeCommand const& rhs);
    EndpointComDirectionChangeCommand& operator=(EndpointComDirectionChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection endpoint.
    ConnectionEndpoint* endpoint_;

    //! The endpoint's old COM direction.
    DirectionTypes::Direction oldDir_;

    //! The endpoint's new COM direction.
    DirectionTypes::Direction newDir_;
};

//-----------------------------------------------------------------------------
//! EndpointTransferTypeChangeCommand class.
//-----------------------------------------------------------------------------
class EndpointTransferTypeChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint  The connection endpoint.
     *      @param [in] newDir    The endpoint's new COM data type.
     *      @param [in] parent    The parent command.
     */
    EndpointTransferTypeChangeCommand(ConnectionEndpoint* endpoint, QString const& newTransferType,
                                  QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointTransferTypeChangeCommand();

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
    EndpointTransferTypeChangeCommand(EndpointTransferTypeChangeCommand const& rhs);
    EndpointTransferTypeChangeCommand& operator=(EndpointTransferTypeChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection endpoint.
    ConnectionEndpoint* endpoint_;

    //! The endpoint's old COM data type.
    QString oldTransferType_;

    //! The endpoint's new COM direction.
    QString newTransferType_;
};

//-----------------------------------------------------------------------------
//! EndpointPropertyValuesChangeCommand class.
//-----------------------------------------------------------------------------
class EndpointPropertyValuesChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint   The connection endpoint.
     *      @param [in] newValues  The endpoint's new property values.
     *      @param [in] parent     The parent command.
     */
    EndpointPropertyValuesChangeCommand(ConnectionEndpoint* endpoint,
                                        QMap<QString, QString> const& newValues,
                                        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointPropertyValuesChangeCommand();

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
    EndpointPropertyValuesChangeCommand(EndpointPropertyValuesChangeCommand const& rhs);
    EndpointPropertyValuesChangeCommand& operator=(EndpointPropertyValuesChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection endpoint.
    ConnectionEndpoint* endpoint_;

    //! The endpoint's old property values.
    QMap<QString, QString> oldValues_;

    //! The endpoint's new property values.
    QMap<QString, QString> newValues_;
};

//-----------------------------------------------------------------------------
//! EndPointTypesCommand class.
//-----------------------------------------------------------------------------
class EndPointTypesCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint    The diagram connection end point.
     *      @param [in] busType     The end point's current bus type.
     *      @param [in] absType     The end point's current abs type.
     *      @param [in] activeView  The active view of the containing item.
     *      @param [in] parent      The parent command.
     */
    EndPointTypesCommand(ConnectionEndpoint* endpoint, VLNV const& busType, VLNV const& absType,
        QString const& activeView, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndPointTypesCommand();

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
    EndPointTypesCommand(EndPointTypesCommand const& rhs);
    EndPointTypesCommand& operator=(EndPointTypesCommand const& rhs);

    void setTypes(VLNV const& busType, VLNV const& absType);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram connection end point.
    ConnectionEndpoint* endpoint_;

    //! The end point's old bus type.
    VLNV oldBusType_;

    //! The end point's old abs type.
    VLNV oldAbsType_;

    //! The end point's new bus type.
    VLNV newBusType_;

    //! The end point's new abs type.
    VLNV newAbsType_;

    //! The active view of the containing item.
    QString activeView_;
};

//-----------------------------------------------------------------------------
//! EndPointPortMapCommand class.
//-----------------------------------------------------------------------------
class EndPointPortMapCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endpoint          The diagram connection end point.
     *      @param [in] newPortMaps       The new port maps for the end point.
     *      @param [in] parent            The parent command.
     */
    EndPointPortMapCommand(ConnectionEndpoint* endpoint,
                           QList< QSharedPointer<PortMap> > newPortMaps,
                           QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndPointPortMapCommand();

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
    EndPointPortMapCommand(EndPointPortMapCommand const& rhs);
    EndPointPortMapCommand& operator=(EndPointPortMapCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram connection end point.
    ConnectionEndpoint* endpoint_;

    //! The currently active abstraction type.
    QSharedPointer<AbstractionType> abstraction_;

    //! The end point's old port maps.
    QList< QSharedPointer<PortMap> > oldPortMaps_;

    //! The end point's new port maps.
    QList< QSharedPointer<PortMap> > newPortMaps_;
};

//-----------------------------------------------------------------------------

#endif // HWCHANGECOMMANDS_H
