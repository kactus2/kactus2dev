//-----------------------------------------------------------------------------
// File: EndpointStack.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.5.2011
//
// Description:
// Endpoint stack graphics item for managing endpoints.
//-----------------------------------------------------------------------------

#ifndef ENDPOINTSTACK_H
#define ENDPOINTSTACK_H

#include <MCAPI/MCAPIDesignerTypes.h>

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QObject>

class EndpointItem;
class GraphicsRectButton;
class ProgramEntityItem;

//-----------------------------------------------------------------------------
//! EndpointStack class.
//-----------------------------------------------------------------------------
class EndpointStack : public QObject, public QGraphicsRectItem
{
    Q_OBJECT 

public:
    enum { Type = UserType + 10 };

    /*!
     *  Constructor.
     */
    EndpointStack(ProgramEntityItem* parentNode);

    /*!
     *  Destructor.
     */
    ~EndpointStack();

    /*!
     *  Sets the stack editable/non-editable.
     */
    void setEditable(bool editable);

    /*!
     *  Sets the ordering of the endpoints based on the given map.
     *
     *      @param [in] positions The endpoint positions.
     */
    void setEndpointOrder(QMap<QString, QPointF> const& positions);

    /*!
     *  Expands/collapses the endpoint stack.
     *
     *      @param [in] expanded True for expand; false for collapse.
     */
    void setExpanded(bool expanded);

    /*!
     *  Adds an endpoint to the stack.
     *
     *      @param [in] endpoint The endpoint to add.
     */
    void addEndpoint(EndpointItem* endpoint);

    /*!
     *  Removes an endpoint from the stack.
     *
     *      @param [in] endpoint The endpoint to remove.
     */
    void removeEndpoint(EndpointItem* endpoint);

    /*!
     *  Called when an endpoint is moved within the node. The endpoint's position is corrected so that
     *  it is valid according to the node.
     *
     *      @param [in]     endpoint  The endpoint that is being moved.
     */
    void onMoveEndpoint(EndpointItem* endpoint);

    /*!
     *  Corrects the endpoint's position after mouse release.
     */
    void onReleaseEndpoint(EndpointItem* endpoint);

    /*!
     *  Returns the height of the visible part of the stack.
     */
    int getVisibleHeight() const;

    /*!
     *  Returns the list of endpoints.
     */
    QList<EndpointItem*> const& getEndpoints() const;

    /*!
     *  Returns true if the endpoints in the stack can be edited.
     */
    bool isEditable() const;

    /*!
     *  Returns true if the endpoint stack is expanded.
     */
    bool isExpanded() const;

    int type() const { return Type; }

public slots:
    //! Adds an endpoint to the stack (requests information from the user with a dialog).
    void addEndpoint();

    /*!
     *  Adds an endpoint to the stack.
     *
     *      @param [in] name  The name of the endpoint.
     *      @param [in] dir   The endpoint direction.
     *      @param [in] type  The endpoint data type.
     */
    void addEndpoint(QString const& name, MCAPIEndpointDirection dir, MCAPIDataType type);

signals:
    void visibleHeightChanged(int height);

    //! Signaled when the endpoint stack has changed.
    void contentChanged();

protected:
    //! Called when the users presses the mouse over the stack header.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    // Disable copying.
    EndpointStack(EndpointStack const& rhs);
    EndpointStack& operator=(EndpointStack const& rhs);

    /*!
     *  Updates the endpoint positions based on the expand/collapse state.
     */
    void updateEndpointPositions();

    //! EndpointList type.
    typedef QList<EndpointItem*> EndpointList;

    enum
    {
        HEIGHT = 20,
        MIN_Y_PLACEMENT = HEIGHT + 0,
        SPACING = 0
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Parent program entity.
    ProgramEntityItem* parentProgEntity_;

    //! Header label.
    QGraphicsTextItem* headerLabel_;

    //! Show/hide button pixmap.
    QGraphicsPixmapItem* btnShowHide_;

    //! None rectangle (used if there are no endpoints in the stack).
    QGraphicsRectItem* noneRect_;

    //! None label.
    QGraphicsTextItem* noneLabel_;

    //! Add button.
    GraphicsRectButton* btnAdd_;

    //! The list of endpoints in the stack.
    EndpointList endpoints_;

    //! Boolean for the current stack state (expanded/collapsed).
    bool expanded_;

    //! Boolean for current edit state (editable/non-editable).
    bool editable_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTSTACK_H
