//-----------------------------------------------------------------------------
// File: DiagramChangeCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.8.2011
//
// Description:
// Undo change commands for the block diagram.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMCHANGECOMMANDS_H
#define DIAGRAMCHANGECOMMANDS_H

#include <models/generaldeclarations.h>

#include <common/ColumnTypes.h>

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QMap>

class DiagramInterconnection;
class DiagramPort;
class DiagramComponent;
class ComponentItem;
class DiagramInterface;
class DiagramColumn;
class DiagramColumnLayout;
class DiagramConnectionEndPoint;
class ActiveViewModel;
class DiagramAdHocPort;
class AdHocEnabled;

//-----------------------------------------------------------------------------
//! ColumnChangeCommand class.
//-----------------------------------------------------------------------------
class ColumnChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] column           The column.
     *      @param [in] newName          The column's new name.
     *      @param [in] newContentType   The new content type.
     *      @param [in] newAllowedItems  The new allowed items.
     *      @param [in] parent           The parent command.
     */
    ColumnChangeCommand(DiagramColumn* column, QString const& newName,
                        ColumnContentType newContentType,
                        unsigned int newAllowedItems,
                        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ColumnChangeCommand();

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
    ColumnChangeCommand(ColumnChangeCommand const& rhs);
    ColumnChangeCommand& operator=(ColumnChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    DiagramColumnLayout* layout_;

    //! The diagram column.
    DiagramColumn* column_;

    //! The column's old name.
    QString oldName_;

    //! The column's old content type.
    ColumnContentType oldContentType_;

    //! The column's old allowed items.
    unsigned int oldAllowedItems_;

    //! The column's new name.
    QString newName_;

    //! The column's new content type.
    ColumnContentType newContentType_;

    //! The column's new allowed items.
    unsigned int newAllowedItems_;
};

//-----------------------------------------------------------------------------
//! ComponentChangeNameCommand class.
//-----------------------------------------------------------------------------
class ComponentChangeNameCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component      The component.
     *      @param [in] newName        The component's new name.
     *      @param [in] parent         The parent command.
     */
    ComponentChangeNameCommand(ComponentItem* component, QString const& newName,
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

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram component.
    ComponentItem* component_;

    //! The component's old name.
    QString oldName_;

    //! The component's new name.
    QString newName_;
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

class ComponentConfElementChangeCommand: public QUndoCommand {

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component instance that is being edited.
	 * \param newConfElements The new configurable elements for the instance.
	 * \param parent Pointer to the owner of this command.
	 *
	*/
	ComponentConfElementChangeCommand(ComponentItem* component,
		const QMap<QString, QString>& newConfElements, 
		QUndoCommand* parent = 0);

	//! \brief The destructor
	virtual ~ComponentConfElementChangeCommand();

	//! \brief Undoes the command.
	virtual void undo();

	//! \brief Redoes the command.
	virtual void redo();

private:
	//! \brief No copying
	ComponentConfElementChangeCommand(const ComponentConfElementChangeCommand& other);

	//! \brief No assignment
	ComponentConfElementChangeCommand& operator=(const ComponentConfElementChangeCommand& other);

	//! \brief Pointer to the component instance that's configurable elements are changed.
	ComponentItem* component_;

	//! \brief The old configurable element values.
	QMap<QString, QString> oldConfElements_;

	//! \brief The new configurable element values.
	QMap<QString, QString> newConfElements_;
};

class ComponentActiveViewChangeCommand : public QUndoCommand
{
public:

    /*! \brief The constructor
     *
     * \param instanceName The name of the component instance
     * \param oldActiveView The name of the previous active view.
     * \param newActiveView The name of the new active view.
     * \param activeViewModel Pointer to the model that manages the active views.
     * \param parent Pointer to the parent command.
     *
    */
    ComponentActiveViewChangeCommand(const QString& instanceName,
		QString const& oldActiveView, 
		QString const& newActiveView,
		ActiveViewModel* activeViewModel,
		QUndoCommand* parent = 0);

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

    //! \brief The component's old active view.
    QString instanceName_;

    //! \brief The component's new name.
    QString newViewName_;

    //! \brief The component's new active view.
    QString oldViewName_;

	//! \brief Pointer to the model that manages the active views.
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
     *      @param [in] component  The component.
     *      @param [in] vlnv       The VLNV of the packetized component.
     *      @param [in] parent     The parent command.
     */
    ComponentPacketizeCommand(ComponentItem* component, VLNV const& vlnv, QUndoCommand* parent = 0);

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

    //! The diagram component.
    ComponentItem* component_;

    //! The component VLNV.
    VLNV vlnv_;
};

//-----------------------------------------------------------------------------
//! EndPointChangeCommand class.
//-----------------------------------------------------------------------------
class EndPointChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endPoint          The diagram connection end point.
     *      @param [in] newName           The end point's new name.
     *      @param [in] newInterfaceMode  The end point's new interface mode.
     *      @param [in] newDescription	  The end point's new description.
     *      @param [in] parent            The parent command.
     */
	EndPointChangeCommand(DiagramConnectionEndPoint* endPoint,
		QString const& newName,
		General::InterfaceMode newMode,
		QString const& newDescription,
		QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndPointChangeCommand();

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
    EndPointChangeCommand(EndPointChangeCommand const& rhs);
    EndPointChangeCommand& operator=(EndPointChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram connection end point.
    DiagramConnectionEndPoint* endPoint_;

    //! The end point's old name.
    QString oldName_;

    //! The end point's old interface mode.
    General::InterfaceMode oldMode_;

	//! \brief The end point's old description.
	QString oldDescription_;

    //! The end point's new name.
    QString newName_;

    //! The end point's new interface mode.
    General::InterfaceMode newMode_;

	//! \brief The end point's new description.
	QString newDescription_;
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
     *      @param [in] endPoint          The diagram connection end point.
     *      @param [in] oldBusType        The end point's old bus type.
     *      @param [in] oldAbsType        The end point's old abs type.
     *      @param [in] oldInterfaceMode  The end point's old interface mode.
     *      @param [in] oldName           The end point's old name.
     *      @param [in] parent            The parent command.
     */
    EndPointTypesCommand(DiagramConnectionEndPoint* endPoint, VLNV const& oldBusType,
                         VLNV const& oldAbsType, General::InterfaceMode oldMode,
                         QString const& oldName, QUndoCommand* parent = 0);

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

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram connection end point.
    DiagramConnectionEndPoint* endPoint_;

    //! The end point's old bus type.
    VLNV oldBusType_;

    //! The end point's old abs type.
    VLNV oldAbsType_;

    //! The end point's old interface mode.
    General::InterfaceMode oldMode_;

    //! The end point's old name.
    QString oldName_;

    //! The end point's new bus type.
    VLNV newBusType_;

    //! The end point's new abs type.
    VLNV newAbsType_;

    //! The end point's new interface mode.
    General::InterfaceMode newMode_;

    //! The end point's new name.
    QString newName_;

    //! The interface modes for the other end points of the connections.
    QMap<DiagramConnectionEndPoint*, General::InterfaceMode> connModes_;
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
     *      @param [in] endPoint          The diagram connection end point.
     *      @param [in] newPortMaps       The new port maps for the end point.
     *      @param [in] parent            The parent command.
     */
    EndPointPortMapCommand(DiagramConnectionEndPoint* endPoint,
                           QList< QSharedPointer<General::PortMap> > newPortMaps,
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
    DiagramConnectionEndPoint* endPoint_;

    //! The end point's old port maps.
    QList< QSharedPointer<General::PortMap> > oldPortMaps_;

    //! The end point's new port maps.
    QList< QSharedPointer<General::PortMap> > newPortMaps_;
};

//-----------------------------------------------------------------------------
//! Undo command for changing the ad-hoc port visibility.
//-----------------------------------------------------------------------------
class AdHocVisibilityChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component     The ad-hoc enabled data source.
     *      @param [in] portName      The name of the port.
     *      @param [in] newVisiblity  The new ad-hoc visibility of the port.
     *      @param [in] parent        The parent undo command.
     */
    AdHocVisibilityChangeCommand(AdHocEnabled* dataSource, QString const& portName,
                                 bool newVisibility, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~AdHocVisibilityChangeCommand();

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
    AdHocVisibilityChangeCommand(AdHocVisibilityChangeCommand const& rhs);
    AdHocVisibilityChangeCommand& operator=(AdHocVisibilityChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component containing the port.
    AdHocEnabled* dataSource_;

    //! The name of the port.
    QString portName_;

    //! The saved port position.
    QPointF pos_;

    //! The new ad-hoc visibility for the port.
    bool newVisibility_;
};

//-----------------------------------------------------------------------------
//! Undo command for changes made to a DiagramInterConnection.
//-----------------------------------------------------------------------------
class ConnectionChangeCommand : public QUndoCommand {

public:

	/*! \brief The constructor
	 *
	 * \param connection Pointer to the connection that was changed.
	 * \param newName The new name of the connection.
	 * \param newDescription The new description of the connection.
	 * \param parent Pointer to the parent of this undo command.
	 *
	*/
	ConnectionChangeCommand(DiagramInterconnection* connection,
		const QString& newName,
		const QString& newDescription,
		QUndoCommand* parent = 0);

	//! \brief The destructor
	virtual ~ConnectionChangeCommand();

	/*! \brief Undoes the command.
	 *
	*/
	virtual void undo();

	/*! \brief Redoes the command.
	 *
	 * Redo is automatically executed when this command is added to the undo stack.
	*/
	virtual void redo();

private:
	//! \brief No copying
	ConnectionChangeCommand(const ConnectionChangeCommand& other);

	//! \brief No assignment
	ConnectionChangeCommand& operator=(const ConnectionChangeCommand& other);

	//! \brief Pointer to the connection to change.
	DiagramInterconnection* connection_;

	//! \brief The new name for the connection.
	QString newName_;

	//! \brief The new description for the connection.
	QString newDescription_;

	//! \brief The old name of the connection.
	QString oldName_;

	//! \brief The old description of the connection.
	QString oldDescription_;
};

//-----------------------------------------------------------------------------
//! Undo command for changing the ad-hoc bounds of an ad-hoc connection.
//-----------------------------------------------------------------------------
class AdHocBoundsChangeCommand : public QUndoCommand
{
public:

	/*!
     *  Constructor.
	 */
	AdHocBoundsChangeCommand(DiagramInterconnection* connection, bool right, int endpointIndex,
                             int oldValue, int newValue, QUndoCommand* parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~AdHocBoundsChangeCommand();

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
    AdHocBoundsChangeCommand(AdHocBoundsChangeCommand const& rhs);
    AdHocBoundsChangeCommand& operator=(AdHocBoundsChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the connection to change.
	DiagramInterconnection* connection_;

    //! If true, the change concerns the right bound. Otherwise it concerns the left bound.
    bool right_;

    //! The end point index.
    int endpointIndex_;

    //! The old bound value.
    int oldValue_;

    //! The new bound value.
    int newValue_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMCHANGECOMMANDS_H
