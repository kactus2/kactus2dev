//-----------------------------------------------------------------------------
// File: ReferenceCounter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.01.2015
//
// Description:
// The interface for reference counters.
//-----------------------------------------------------------------------------

#ifndef REFERENCECOUNTER_H
#define REFERENCECOUNTER_H

#include <QObject>
#include <QSharedPointer>

class ParametersInterface;

//-----------------------------------------------------------------------------
//! ParameterCounter interface. 
//-----------------------------------------------------------------------------
class ReferenceCounter : public QObject
{
    Q_OBJECT

public:

    virtual ~ReferenceCounter() = default;

public slots:

    /*!
     *  Increase the reference count of the parameter matching the id.
     *
     *      @param [in] id      The id of the parameter which is being referenced.
     */
    virtual void increaseReferenceCount(QString const& id) = 0;

    /*!
     *  Decrease the reference count of the parameter matching the id.
     *
     *      @param [in] id      The id of the parameter which was referenced.
     */
    virtual void decreaseReferenceCount(QString const& id) = 0;

    /*!
     *  Recalculate references made to the selected parameters.
     *
     *      @param [in] parameterList       The selected parameters.
     *      @param [in] parameterInterface  Interface for accessing parameters.
     */
    virtual void recalculateReferencesToParameters(QVector<QString> const& parameterList,
        QSharedPointer<ParametersInterface> parameterInterface) = 0;
};

#endif // REFERENCECOUNTER_H
