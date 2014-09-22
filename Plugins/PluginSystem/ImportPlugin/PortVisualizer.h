//-----------------------------------------------------------------------------
// File: Highlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.09.2014
//
// Description:
// Interface forport visualizers in component import.
//-----------------------------------------------------------------------------

#ifndef PORTVISUALIZER_H
#define PORTVISUALIZER_H

#include <QSharedPointer>

class Port;

//-----------------------------------------------------------------------------
//! Interface forport visualizers in component import.
//-----------------------------------------------------------------------------
class PortVisualizer
{
public:

	//! The destructor.
    virtual ~PortVisualizer() {};

    /*!
     *  Called when a port should be added to the visualization.
     *
     *      @param [in] portToAdd   The port to add.
     */
    virtual void addPort(QSharedPointer<Port> portToAdd) = 0;

    /*!
     *  Called when a port should be removed from the visualization.
     *
     *      @param [in] portToRemove   The port to remove.
     */
    virtual void removePort(QSharedPointer<Port> portToRemove) = 0;
};

#endif // PORTVISUALIZER_H
