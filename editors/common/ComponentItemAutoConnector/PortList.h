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

//-----------------------------------------------------------------------------
//! Lists component ports as name, direction pairs.
//-----------------------------------------------------------------------------
class PortList : public QListWidget
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] containingComponent     Component containing the ports.
     *      @param [in] parent                  The parent item.
     */
    PortList(QSharedPointer<Component> containingComponent, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    ~PortList();

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
};

#endif // PORTLIST_H
