//-----------------------------------------------------------------------------
// File: SWCompItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing SW components.
//-----------------------------------------------------------------------------

#ifndef SWCOMPITEM_H
#define SWCOMPITEM_H

#include "SWComponentItem.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

class SystemColumn;
class IComponentStack;

//-----------------------------------------------------------------------------
//! Graphics item for visualizing SW components.
//-----------------------------------------------------------------------------
class SWCompItem : public SWComponentItem
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SW_COMPONENT_ITEM };

    /*!
     *  Constructor.
     */
    SWCompItem(LibraryInterface* libInterface,
               QSharedPointer<Component> component,
               QString const& instanceName,
               QString const& displayName = QString(),
               QString const& description = QString(),
               QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
               unsigned int id = 0);

    /*!
     *  Destructor.
     */
    ~SWCompItem();

    /*!
     *  Returns the graphics item type.
     */
    int type() const { return Type; }

    virtual void updateComponent();

public slots:
    void openSource();

signals:
    void openSource(ComponentItem* compItem);

protected:
    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    //! Opens up the context menu when right mouse button is pressed.
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

private:
    // Disable copying.
    SWCompItem(SWCompItem const& rhs);
    SWCompItem& operator=(SWCompItem const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // Constants.
    enum
    {
        WIDTH = 160,
        MIN_HEIGHT = 60
    };

    //! The node id.
    unsigned int id_;

    //! The old stack from where the mouse drag event began.
    IComponentStack* oldStack_;

    //! The mapping component's old position before mouse move.
    QPointF oldPos_;
    QPointF oldStackPos_;

    QGraphicsPixmapItem* hierIcon_;
};

//-----------------------------------------------------------------------------

#endif // SWCOMPITEM_H
