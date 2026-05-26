//-----------------------------------------------------------------------------
// File: ListFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.05.2026
//
// Description:
// Interface for parameter lists.
//-----------------------------------------------------------------------------

#ifndef LISTFINDER_H
#define LISTFINDER_H

#include "ParameterFinder.h"

#include <IPXACTmodels/common/Parameter.h>

#include <QSharedPointer>
#include <QList>

//-----------------------------------------------------------------------------
//! Interface for parameter lists.
//-----------------------------------------------------------------------------
class ListFinder : public ParameterFinder
{

public:

    /*!
     *  The Destructor.
     */
    ~ListFinder() override = default;

    /*!
     *  Set the parameter list.
     *
     *    @param [in] parameterList   [Description].
     */
    virtual void setParameterList(QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList) = 0;
};

#endif // LISTPARAMETERFINDER_H
