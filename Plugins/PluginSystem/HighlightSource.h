//-----------------------------------------------------------------------------
// File: HighlightSource.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.09.2014
//
// Description:
// Interface for all highlighting import plugins.
//-----------------------------------------------------------------------------

#ifndef HIGHLIGHTSOURCE_H
#define HIGHLIGHTSOURCE_H

#include "Highlighter.h"

//-----------------------------------------------------------------------------
//! Interface for all highlighting import plugins.
//-----------------------------------------------------------------------------
class HighlightSource
{
public:

	//! The destructor.
    virtual ~HighlightSource() {};

    /*!
     *  Sets the given highlighter to be used by the source.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter) = 0;

};

#endif // HIGHLIGHTSOURCE_H
