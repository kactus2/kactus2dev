//-----------------------------------------------------------------------------
// File: SWComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.11.2011
//
// Description:
// Base class for all SW components visualized in a system design.
//-----------------------------------------------------------------------------

#ifndef SWCOMPONENTITEM_H
#define SWCOMPONENTITEM_H

#include "SWPortItem.h"

#include <common/graphicsItems/ComponentItem.h>

//-----------------------------------------------------------------------------
//! SWComponentItem class.
//-----------------------------------------------------------------------------
class SWComponentItem : public ComponentItem
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] size                      The initial rectangle size.
     *      @param [in] instanceName              The name of the component instance.
     *      @param [in] displayName               The component instance's display name.
     *      @param [in] description               The component instance's description.
     *      @param [in] configurableElementValue  The component instance's configurable element values.
     *      @param [in] parent                    The parent graphics item.
     */
    SWComponentItem(QRectF const& size,
                    QSharedPointer<Component> component,
                    QString const& instanceName = QString("unnamed"),
                    QString const& displayName = QString(),
                    QString const& description = QString(),
                    QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
                    QGraphicsItem *parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~SWComponentItem();

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    virtual void updateComponent();

    /*!
     *  Sets the flag whether the component has been imported or not.
     *
     *      @param [in] imported If true, the component is marked as imported.
     */
    void setImported(bool imported);

    /*!
     *  Returns true if the component has been marked as imported.
     */
    bool isImported() const;

    /*!
     *  Called when a port is being moved.
     *
     *      @param [in] port The port that is being moved.
     */
    void onMovePort(SWPortItem* port);
    /*!
     *  Updates the size of the component box.
     */
    void updateSize();

protected:
    /*!
     *  Returns the height for the component box.
     */
    virtual qreal getHeight() const;

private:
    // Disable copying.
    SWComponentItem(SWComponentItem const& rhs);
    SWComponentItem& operator=(SWComponentItem const& rhs);

    /*!
     *  Adds the given port to the component.
     *
     *      @param [in] port   The port to add.
     *      @param [in] right  If true, the port is added on the right side. Otherwise it
     *                         is added to the left side.
     */
    void onAddPort(SWPortItem* port, bool right);

    enum
    {
        SPACING = 8,
        MIN_Y_PLACEMENT = 3 * GridSize,
        BOTTOM_MARGIN = 2 * GridSize
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The boolean flag for imported property.
    bool imported_;

    //! The left and right port stacks.
    QList<SWPortItem*> leftPorts_;
    QList<SWPortItem*> rightPorts_;
};

//-----------------------------------------------------------------------------

#endif // SWCOMPONENTITEM_H
