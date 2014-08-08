//-----------------------------------------------------------------------------
// File: PortSorter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2014
//
// Description:
// Interface for component port name sorter.
//-----------------------------------------------------------------------------

#ifndef PORTSORTER_H
#define PORTSORTER_H

#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! Interface for component port name sorter.
//-----------------------------------------------------------------------------
class PortSorter 
{
public:

	//! The destructor.
    virtual ~PortSorter() {};

    /*!
     *  Gets the sorted port names of a component.
     *
     *      @param [in] component   The component whose port names to find.
     *
     *      @return The sorted port names of a given component.
     */
    virtual QStringList sortedPortNames(QSharedPointer<const Component> component) const = 0;

};

#endif // PORTSORTER_H
