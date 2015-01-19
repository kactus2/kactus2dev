//-----------------------------------------------------------------------------
// File: ParameterReferenceCounter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.01.2015
//
// Description:
// Calculates the amount of references made to parameters and gives this data to the parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERREFERENCECOUNTER_H
#define PARAMETERREFERENCECOUNTER_H

#include "ReferenceCounter.h"

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

//-----------------------------------------------------------------------------
//! ParameterReferenceCounter class. 
//-----------------------------------------------------------------------------
class ParameterReferenceCounter : public ReferenceCounter
{
    Q_OBJECT

public:

    /*!
     *  Constructor.
     */
    ParameterReferenceCounter(QSharedPointer<ParameterFinder> parameterFinder);

    /*!
     *  Destructor.
     */
    ~ParameterReferenceCounter();

public slots:

    /*!
     *  Increase the reference count of the parameter matching the id.
     *
     *      @param [in] id      The id of the parameter which is being referenced.
     */
    virtual void increaseReferenceCount(QString id);
    
    /*!
     *  Decrease the reference count of the parameter matching the id.
     *
     *      @param [in] id      The id of the parameter which was referenced.
     */
    virtual void decreaseReferenceCount(QString id);

private:

	//! No copying
    ParameterReferenceCounter(const ParameterReferenceCounter& other);
	//! No assignment
    ParameterReferenceCounter& operator=(const ParameterReferenceCounter& other);

    //! Object for finding parameters with the correct id.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // PARAMETERREFERENCECOUNTER_H
