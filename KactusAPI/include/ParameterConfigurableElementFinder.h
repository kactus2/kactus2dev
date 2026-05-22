//-----------------------------------------------------------------------------
// File: ParameterConfigurableElementFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 18.02.2026
//
// Description:
// The implementation for finding configurable elements from a list with the correct ID.
//-----------------------------------------------------------------------------

#ifndef PARAMETERCONFIGURABLEELEMENTFINDER_H
#define PARAMETERCONFIGURABLEELEMENTFINDER_H

#include "ListParameterFinder.h"

#include "KactusAPI/KactusAPIGlobal.h"

#include <QSharedPointer>
#include <QList>

class ConfigurableElementValue;

//-----------------------------------------------------------------------------
//! The implementation for finding configurable elements from a list with the correct ID.
//-----------------------------------------------------------------------------
class KACTUS2_API ParameterConfigurableElementFinder : public ListParameterFinder
{

public:

    /*!
     *  The constructor.
     */
    ParameterConfigurableElementFinder();

    /*!
     *  The Destructor.
     */
    ~ParameterConfigurableElementFinder() override = default;

	//! No copying. No assignment. No regret.
    ParameterConfigurableElementFinder(const ParameterConfigurableElementFinder& other) = delete;
    ParameterConfigurableElementFinder& operator=(const ParameterConfigurableElementFinder& other) = delete;

    /*!
     *  Finds the value of the parameter with the given id.
     *
     *    @param [in] id      The id of the parameter to search for.
     *
     *    @return The value of the parameter.
     */
    QString valueForId(QStringView id) const override;

    /*!
     *  Set the parameter list.
     *
     *    @param [in] elementList   List of the available configurable element values.
     */
	void setCEVList(QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > elementList) noexcept;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The configurable element values are searched from this list.
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevList_ =
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > >(new QList<QSharedPointer<ConfigurableElementValue> >());
};

#endif // PARAMETERCONFIGURABLEELEMENTFINDER_H
