//-----------------------------------------------------------------------------
// File: MappingComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.9.2011
//
// Description:
// Graphics item for the SW mapping component.
//-----------------------------------------------------------------------------

#ifndef MAPPINGCOMPONENTITEM_H
#define MAPPINGCOMPONENTITEM_H

#include <MCAPI/MCAPIDesignerTypes.h>
#include <common/IDFactory.h>

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QVector>
#include <QTextStream>

class EndpointDesignDiagram;
class EndpointItem;
class EndpointStack;
class AppPlaceholderItem;
class MCAPIContentMatcher;
class SystemColumn;
class Component;
class ProgramEntityItem;
class PlatformComponentItem;
class PlatformPlaceholderItem;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! MappingComponentItem class.
//-----------------------------------------------------------------------------
class MappingComponentItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 13 };

    /*!
     *  Constructor.
     *
     *      @param [in] diagram       The parent diagram.
     *      @param [in] libInterface  The library interface.
     *      @param [in] component     The underlying SW mapping component.
     *      @param [in] instanceName  The instance name.
     *      @param [in] id            The node ID.
     */
    MappingComponentItem(EndpointDesignDiagram* diagram, LibraryInterface* libInterface,
                         QSharedPointer<Component> component, QString const& instanceName, unsigned int id);

    /*!
     *  Destructor.
     */
    ~MappingComponentItem();

    /*!
     *  Saves the SW mapping component to disk, including its design.
     *
     *      @param [in] libInterface The library interface.
     *
     *      @return True, if successful. Otherwise false.
     */
    bool save(LibraryInterface* libInterface);

    /*!
     *  Sets the instance name.
     *
     *      @param [in] name The instance name.
     *
     *      @remarks The instance name acts also at the MCAPI node name, so it must be
     *               a valid C variable name.
     */
    void setName(QString const& instanceName);

    /*!
     *  Sets the node id.
     *
     *      @param [in] id The node id.
     */
    void setID(unsigned int id);

    /*!
     *  Adds a new, empty application item to the SW mapping component.
     *
     *      @param [in] name  The instance name.
     *      @param [in] pos   The position hint for the application.
     *
     *      @return The newly created application item.
     */
    ProgramEntityItem* addProgramEntity(QString const& name, QPointF const& pos);

    /*!
     *  Adds an application item to the SW mapping component.
     *
     *      @param [in] item The application item to add.
     */
    void addProgramEntity(ProgramEntityItem* item);

    /*!
     *  Removes an application item from the SW mapping component.
     *
     *      @param [in] item The application item to remove.
     */
    void removeProgramEntity(ProgramEntityItem* item);

    /*!
     *  Sets the SW platform component item.
     *
     *      @param [in] item The platform component item.
     */
    void setPlatformComponent(PlatformComponentItem* item);

    /*!
     *  Updates the size of the item based on the contents.
     */
    void updateSize();

    /*!
     *  Retrieve the endpoint with the given full name.
     *
     *      @param [in] fullName The full name of the endpoint.
     *
     *      @return The endpoint or null if not found.
     */
    EndpointItem* getEndpoint(QString const& fullName);

    /*!
     *  Returns the instance name.
     */
    QString const& getName() const;

    /*!
     *  Returns the node id.
     */
    unsigned int getID() const;

    /*!
     *  Returns the underlying HW component.
     */
    QSharedPointer<Component> getComponent() const;

    /*!
     *  Returns the parent diagram.
     */
    EndpointDesignDiagram* getDiagram() const;

    /*!
     *  Returns the port ID factory.
     */
    IDFactory& getPortIDFactory();

    /*!
     *  Returns the graphics item type.
     */
    int type() const { return Type; }

    /*!
     *  Called when an application item is being moved.
     *
     *      @param [in] item The application item that is being moved.
     */
    void onMoveItem(ProgramEntityItem* item);

    /*!
     *  Called when an item is released from being moved by mouse.
     *
     *      @param [in] item The item that has been released.
     */
    void onReleaseItem(ProgramEntityItem* item);

signals:
    //! Signaled when the node has changed.
    void contentChanged();

protected:
    //! Called when the item has changed (position etc.).
    QVariant itemChange(GraphicsItemChange change, QVariant const& value);

    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

public slots:
    void onEndpointStackChange(int height);

private:
    // Disable copying.
    MappingComponentItem(MappingComponentItem const& rhs);
    MappingComponentItem& operator=(MappingComponentItem const& rhs);

    /*!
     *  Updates the name label.
     */
    void updateNameLabel();
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // Constants.
    enum
    {
        WIDTH = 240,
        TOP_MARGIN = 40,
        BOTTOM_MARGIN = 80,
        SPACING = 20
    };

    //! Port ID factory.
    IDFactory portIDFactory_;

    //! The parent diagram.
    EndpointDesignDiagram* diagram_;

    //! The underlying SW mapping component.
    QSharedPointer<Component> component_;

    //! The instance name.
    QString name_;

    //! The node id.
    unsigned int id_;

    //! The node's name UI item.
    QGraphicsTextItem* nameLabel_;

    //! The old column from where the mouse drag event began.
    SystemColumn* oldColumn_;

    //! The program entity items.
    QList<ProgramEntityItem*> progEntitys_;

    //! The platform component placeholder.
    PlatformPlaceholderItem* platformPlaceholder_;

    //! The platform component item.
    PlatformComponentItem* platformCompItem_;
};

//-----------------------------------------------------------------------------

#endif // MAPPINGCOMPONENTITEM_H
