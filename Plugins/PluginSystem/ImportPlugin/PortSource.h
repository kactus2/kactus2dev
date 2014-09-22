//-----------------------------------------------------------------------------
// File: PortSource.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.09.2014
//
// Description:
// Interface for import plugins creating ports.
//-----------------------------------------------------------------------------

#ifndef PORTSOURCE_H
#define PORTSOURCE_H

#include "PortVisualizer.h"

//-----------------------------------------------------------------------------
//! Interface for import plugins creating ports.
//-----------------------------------------------------------------------------
class PortSource
{
public:

	//! The destructor.
    virtual ~PortSource() {};

    /*!
     *  Sets the given visualizer to be used by the source.
     *
     *      @param [in] visualizer   The visualizer to use.          
     */
    virtual void setPortVisualizer(PortVisualizer* visualizer) = 0;

};

#endif // PORTSOURCE_H
