//-----------------------------------------------------------------------------
// File: PortList.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.01.2019
//
// Description:
// Lists component ports as name, direction pairs.
//-----------------------------------------------------------------------------

#ifndef PORTLIST_H
#define PORTLIST_H

#include <QListWidget>
#include <QSharedPointer>

class Component;
class ComponentItem;

//-----------------------------------------------------------------------------
//! Lists component ports as name, direction pairs.
//-----------------------------------------------------------------------------
class PortList : public QListWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] componentItem   Component item containing the ports.
     *      @param [in] parent          The parent item.
     */
    PortList(ComponentItem* componentItem, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    ~PortList();

    /*!
     *  Get the component item containing the ports.
     *
     *      @return The containing component item.
     */
    ComponentItem* getContainingItem() const;

protected:

    /*!
     *  Handles the mouse press events. Used for creating the drag mime data.
     *
     *      @param [in] event   The mouse event.
     */
    virtual void mousePressEvent(QMouseEvent *event);

private:
    // No copying. No assignments.
    PortList(PortList const& rhs);
    PortList& operator=(PortList const& rhs);

    /*!
     *  Setup the port list.
     *
     *      @param [in] containingComponent     The selected component.
     */
    void setupPortList(QSharedPointer<Component> containingComponent);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component item containing the displayed ports.
    ComponentItem* componentItem_;
};

#endif // PORTLIST_H
