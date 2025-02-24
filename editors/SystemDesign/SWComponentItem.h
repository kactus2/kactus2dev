//-----------------------------------------------------------------------------
// File: SWComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing SW components.
//-----------------------------------------------------------------------------

#ifndef SWCOMPONENTITEM_H
#define SWCOMPONENTITEM_H

#include "SystemComponentItem.h"

#include <common/graphicsItems/ComponentItem.h>

class IGraphicsItemStack;
class HWMappingItem;
class ComponentInstance;
class Component;

//-----------------------------------------------------------------------------
//! Graphics item for visualizing SW components.
//-----------------------------------------------------------------------------
class SWComponentItem : public SystemComponentItem
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SW_COMPONENT_ITEM };

    /*!
     *  The constructor.
     *
     *    @param [in] libInterface                Library interface.
     *    @param [in] component                   The instantiated component.
     *    @param [in] instanceName                The instance name.
     *    @param [in] displayName                 The instance display name.
     *    @param [in] description                 The instance description.
     *    @param [in] uuid                        The instance id.
     *    @param [in] configurableElementValues   The configurable element values.
     */
	SWComponentItem(LibraryInterface* libInterface, QSharedPointer<Component> component,
		QSharedPointer<ComponentInstance> instance);

    /*!
     *  Destructor.
     */
    ~SWComponentItem();

    /*!
     *  Sets the file set reference.
     *
     *    @param [in] fileSetName The name of the referenced file set in the top-level component.
     */
    void setFileSetRef(QString const& fileSetName);

    /*!
     *  Returns the file set reference (i.e. the name of the referenced file set).
     */
    QString const& getFileSetRef() const;

    /*!
     *  Returns the underlying HW linked with this component.
     */
    virtual HWMappingItem const* getLinkedHW() const;

    /*!
     *  Returns the graphics item type.
     */
    int type() const { return Type; }

    virtual void updateComponent();

    /*!
     *  Marks the component as a packetized component.
     */
    virtual void setPackaged();

    /*!
     *  Marks the component as a draft component.
     */
    virtual void setDraft();

    /*!
     *  Checks if the component instance is a draft.
     *
     *    @return True, if the instance is a draft, otherwise false.
     */
    bool isDraft() const;


public slots:
    void openCSource();

signals:
    //! Requests to open the C source for this component.
    void openCSource(ComponentItem* compItem);

    //! Occurs when the file set reference has been changed.
    void fileSetRefChanged(QString const& fileSetRef);

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
    SWComponentItem(SWComponentItem const& rhs);
    SWComponentItem& operator=(SWComponentItem const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // Constants.
    const int MIN_HEIGHT = 60;

    //! The old stack from where the mouse drag event began.
    IGraphicsItemStack* oldStack_ = nullptr;

    //! The mapping component's old position before mouse move.
    QPointF oldPos_;
    QPointF oldStackPos_;

    QGraphicsPixmapItem* hierIcon_ = nullptr;
    QGraphicsPixmapItem* importedIcon_ = nullptr;

    //! The file set reference. Empty string if no reference.
    QString fileSetRef_;

    //! If true, the component instance is a draft.
    bool isDraft_ = false;
};

//-----------------------------------------------------------------------------

#endif // SWCOMPONENTITEM_H
