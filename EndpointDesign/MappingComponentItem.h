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

#include "SWComponentItem.h"

#include <MCAPI/MCAPIDesignerTypes.h>
#include <common/IDFactory.h>

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QVector>
#include <QTextStream>
#include <QSet>

class EndpointDesignDiagram;
class EndpointItem;
class EndpointStack;
class EndpointConnection;
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
class MappingComponentItem : public SWComponentItem
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
                         QSharedPointer<Component> component, QString const& instanceName,
                         QString const& displayName = QString(),
                         QString const& description = QString(),
                         QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
                         unsigned int id = 0);

    /*!
     *  Destructor.
     */
    ~MappingComponentItem();

    /*!
     *  Updates the item to match the current state of the component.
     */
    virtual void updateComponent();

    /*!
     *  Saves the SW mapping component to disk, including its design.
     *
     *      @param [in] libInterface The library interface.
     *
     *      @return True, if successful. Otherwise false.
     */
    bool save(LibraryInterface* libInterface);

    /*!
     *  Sets the node id.
     *
     *      @param [in] id The node id.
     */
    void setID(unsigned int id);

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
     *  Returns the node id.
     */
    unsigned int getID() const;

    /*!
     *  Returns the parent diagram.
     */
    EndpointDesignDiagram* getDiagram() const;

    /*!
     *  Returns the port ID factory.
     */
    IDFactory& getPortIDFactory();

    /*!
     *  Returns true if the component is mapped tightly to a HW component.
     */
    bool isMapped() const;

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

    /*!
     * Returns the component's directory where the IP-XACT document is located.
     */
    QString getFileLocation() const;

protected:
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
     *  Updates the name label with the given text.
     *
     *      @param [in] text The text to display in the label.
     */
    virtual void updateNameLabel(QString const& text);

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

    //! The library interface.
    LibraryInterface* libInterface_;

    //! Icon for imported SW components.
    QGraphicsPixmapItem* importedIcon_;

    //! Port ID factory.
    IDFactory portIDFactory_;

    //! The parent diagram.
    EndpointDesignDiagram* diagram_;

    //! The node id.
    unsigned int id_;

    //! The old column from where the mouse drag event began.
    SystemColumn* oldColumn_;

    //! The program entity items.
    QList<ProgramEntityItem*> progEntitys_;

    //! The platform component placeholder.
    PlatformPlaceholderItem* platformPlaceholder_;

    //! The platform component item.
    PlatformComponentItem* platformCompItem_;

    //! The mapping component's old position before mouse move.
    QPointF oldPos_;

    //! The connections that can change during mouse move.
    QSet<EndpointConnection*> conns_;
};

//-----------------------------------------------------------------------------

#endif // MAPPINGCOMPONENTITEM_H
