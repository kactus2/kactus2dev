//-----------------------------------------------------------------------------
// File: ReferenceCounter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.01.2015
//
// Description:
// The interface for reference counters.
//-----------------------------------------------------------------------------

#ifndef REFERENCECOUNTER_H
#define REFERENCECOUNTER_H

#include <QObject>

//-----------------------------------------------------------------------------
//! ParameterCounter interface. 
//-----------------------------------------------------------------------------
class ReferenceCounter : public QObject
{
    Q_OBJECT

public:

    virtual ~ReferenceCounter() {};

public slots:

    /*!
     *  Increase the reference count of the parameter matching the id.
     *
     *      @param [in] id      The id of the parameter which is being referenced.
     */
    virtual void increaseReferenceCount(QString id) = 0;

    /*!
     *  Decrease the reference count of the parameter matching the id.
     *
     *      @param [in] id      The id of the parameter which was referenced.
     */
    virtual void decreaseReferenceCount(QString id) = 0;
};

#endif // REFERENCECOUNTER_H
