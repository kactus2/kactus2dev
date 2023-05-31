//-----------------------------------------------------------------------------
// File: ComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 6.11.2011
//
// Description:
// Base class for all graphics items that represent an IP-XACT component.
//-----------------------------------------------------------------------------

#ifndef COMPONENTITEM_H
#define COMPONENTITEM_H

#include "ConnectionEndpoint.h"

#include <editors/common/diagramgrid.h>
#include <editors/common/Association/Associable.h>

#include <common/layouts/IVGraphicsLayout.h>
#include <common/layouts/VCollisionLayout.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>

class Component;
class ComponentInstance;
class LibraryInterface;
class IGraphicsItemStack;

//-----------------------------------------------------------------------------
//! ComponentItem class.
//-----------------------------------------------------------------------------
class ComponentItem : public QObject, public QGraphicsRectItem, public Associable
{
    Q_OBJECT 

public:
    /*!
     *  Constructor.
     *
     *      @param [in] size                      The initial rectangle size.
     *      @param [in] libInterface              The library interface.
     *      @param [in] component                 The actual component.
     *      @param [in] instanceName              The name of the component instance.
     *      @param [in] parent                    The parent graphics item.
     */


    ComponentItem(QRectF const& size,
        LibraryInterface* libInterface,
        QSharedPointer<ComponentInstance> instance,
        QSharedPointer<Component> component,
        QGraphicsItem* parent);

	/*!
     *  Destructor.
     */
	~ComponentItem() override;

    // Disable copying.
    ComponentItem(ComponentItem const& rhs) = delete;
    ComponentItem& operator=(ComponentItem const& rhs) = delete;

    /*!
     *  Updates the component item to reflect the current state of the component model.
     */
    virtual void updateComponent();

    /*!
     *  Updates the size of the component box.
     */
    void updateSize();

    /*!
	 *  Returns the height of the component box.
	 */
    virtual qreal getHeight();
    
    /*!
	 *  Returns the width of the component box.
	 */
    virtual qreal getWidth();

	/*!
     *  Sets the instance name.
     *
     *      @param [in] newName The instance name to set.
     */
    void setName(QString const& newName);

    /*!
     *  Sets the display name for the component instance.
     *
     *      @param [in] displayName The display name to set.
	 */
	void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the component instance.
     *
     *      @param [in] description The description to set.
     */
	void setDescription(const QString& description);

    /*!
     *  Returns the instance name.
     */
    std::string name() const;

	/*!
     *  Returns the display name of the component instance.
     */
	QString displayName() const;

	/*!
     *  Returns the description of the component instance.
     */
	QString description() const;

    /*!
     *  Returns the IP-XACT component model.
     */
    QSharedPointer<Component> componentModel() const;

    /*!
     *  Returns the IP-XACT component instance.
     */
    QSharedPointer<ComponentInstance> getComponentInstance() const;

	/*! \brief Get list of views the component has.
	 *
	 * \return QStringList containing the names of the views for the component.
	*/
	QStringList getViews() const;

    /*!
     *  Returns the list of endpoints in the component.
     */
    QList<ConnectionEndpoint*> getEndpoints() const;

    /*!
     *  Returns the parent graphics item stack.
     */
    IGraphicsItemStack* getParentStack();

	/*! \brief Get the uuid of the instance.
     *
	 * \return QString containing uuid.
	 */
    std::string getUuid() const;

    /*!
     *  Defines the connection point for associations in scene coordinates.
     *     
     *      @param [in] otherEnd   The position of the other end connection point.
     *
     *      @return The connection point of the item.
     */
    virtual QPointF connectionPoint(QPointF const& otherEnd = QPointF()) const;

    /*!
     *  Marks the component as a packetized component.
     */
    virtual void setPackaged() = 0;

    /*!
     *  Marks the component as a draft component.
     */
    virtual void setDraft() = 0;
	
    /*!
     *  Handle the movement of a connection end point item.
     *
     *      @param [in] port    The selected connection end point item.
     */
    virtual void onMovePort(ConnectionEndpoint* port) = 0;

    // The basic width of the components.
    static const int COMPONENTWIDTH = 220;

signals:
    //! Emitted when an endpoint has been moved.
    void endpointMoved(ConnectionEndpoint* endpoint);

    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage) const;

	//! \brief Emitted when the name of the component item changes.
	void nameChanged(QString const& newName, QString const& oldName);

	//! \brief Emitted when the display name changes.
	void displayNameChanged(QString const& newName);

	//! \brief Emitted when the description changes.
	void descriptionChanged(QString const& newDescription);

	//! \brief Emitted when the configurable elements change.
	void confElementsChanged(QMap<QString, QString> const& confElements);

	//! \brief Emitted right before this component is destroyed.
	void destroyed(ComponentItem* comp);

protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface();

    /*!
     *  Updates the name label with the given text.
     *
     *      @param [in] text The text to display in the label.
     */
    virtual void updateNameLabel();


    /*!
     *  Adds an interface item to component side determined by the item position.
     *
     *      @param [in] port   The interface item to add.
     */
    void addPortToSideByPosition(ConnectionEndpoint* port);

    /*!
     *  Adds an interface item to the component side with less ports.
     *
     *      @param [in] port   The interface item to add.
     */
    void addPortToSideWithLessPorts(ConnectionEndpoint* port);

    /*!
     *  Adds a bus interface on the left side of the component item.
     *
     *      @param [in] port   The port to add.
     */
    void addPortToLeft(ConnectionEndpoint* port);

    /*!
     *  Adds a bus interface on the right side of the component item.
     *
     *      @param [in] port   The port to add.
     */
    void addPortToRight(ConnectionEndpoint* port);


    /*!
     *  Check and resize the port labels to better match with the component width.
     *
     *      @param [in] port       The port that is compared to the other stack.
     *      @param [in] otherSide  The stack containing the ports of the other side.
     */
    void checkPortLabelSize(ConnectionEndpoint* port, QList<ConnectionEndpoint*> const& otherSide);

    const int SPACING = GridSize;
    const int MIN_Y_PLACEMENT = 3 * GridSize;
    const int BOTTOM_MARGIN = 2 * GridSize;


    //! The layout for ports.
    QSharedPointer< IVGraphicsLayout<ConnectionEndpoint> > portLayout_ =
        QSharedPointer< IVGraphicsLayout<ConnectionEndpoint> >(new VCollisionLayout<ConnectionEndpoint>(SPACING));

    //! The left and right port stacks.
    QList<ConnectionEndpoint*> leftPorts_;
    QList<ConnectionEndpoint*> rightPorts_;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_ = nullptr;

    //! The component model.
    QSharedPointer<Component> component_ = nullptr;

    //! The component instance.
    QSharedPointer<ComponentInstance> componentInstance_ = nullptr;

    //! The name label.
    QGraphicsTextItem* nameLabel_= new QGraphicsTextItem(this);

};

//-----------------------------------------------------------------------------

#endif // COMPONENTITEM_H
