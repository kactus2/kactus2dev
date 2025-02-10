//-----------------------------------------------------------------------------
// File: DesignParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.04.2022
//
// Description:
// The implementation for finding parameters for designs.
//-----------------------------------------------------------------------------

#ifndef DESIGNPARAMETERFINDER_H
#define DESIGNPARAMETERFINDER_H

#include <KactusAPI/include/ListParameterFinder.h>

#include <KactusAPI/KactusAPIGlobal.h>

#include <QSharedPointer>

class ConfigurableElementValue;
class DesignInstantiation;

//-----------------------------------------------------------------------------
//! The implementation for finding parameters for designs.
//-----------------------------------------------------------------------------
class KACTUS2_API DesignParameterFinder : public ListParameterFinder
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] designInstantiation     The design instantiation whose parameters are being searched for.
     */
    DesignParameterFinder(QSharedPointer<DesignInstantiation> designInstantiation);

    /*!
     *  The destructor.
     */
    ~DesignParameterFinder() final = default;


    //! No copying. No assignment.
    DesignParameterFinder(const DesignParameterFinder& other) = delete;
    DesignParameterFinder& operator=(const DesignParameterFinder& other) = delete;

    /*!
     *  Finds the value of the parameter with the given id.
     *
     *      @param [in] id      The id of the parameter to search for.
     *
     *      @return The value of the parameter.
     */
    QString valueForId(QStringView id) const final;

private:

    //! A list of configurable element values
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElementValues_ = nullptr;
};

#endif // DESIGNPARAMETERFINDER_H
