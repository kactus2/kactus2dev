//-----------------------------------------------------------------------------
// File: OtherClockDriverReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.10.2015
//
// Description:
// Reader class for IP-XACT OtherClockDriver element.
//-----------------------------------------------------------------------------

#ifndef OtherClockDriverReader_H
#define OtherClockDriverReader_H

#include "OtherClockDriver.h"

#include <IPXACTmodels/common/ClockUnit.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT OtherClockDriver element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT OtherClockDriverReader : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    OtherClockDriverReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~OtherClockDriverReader();

    /*!
     *  Creates a new OtherClockDriver from a given OtherClockDriver node.
     *
     *      @param [in] OtherClockDriverNode    XML description of the OtherClockDriver.
     */
    QSharedPointer<OtherClockDriver> createOtherClockDriverFrom(QDomNode const& otherClockDriverNode) const;

private:

    //! No copying allowed.
    OtherClockDriverReader(OtherClockDriverReader const& rhs);
	OtherClockDriverReader& operator=(OtherClockDriverReader const& rhs);

    /*!
     *  Parse the clock driver attributes.
     *
     *      @param [in] otherClockDriverNode    XML description of the other clock driver.
     *      @param [in] newOtherClockDriver     The new other clock driver.
     */
    void parseClockDriverAttributes(QDomNode const& otherClockDriverNode,
        QSharedPointer<OtherClockDriver> newOtherClockDriver) const;

    /*!
     *  Parse a clock unit.
     *
     *      @param [in] otherClockDriverNode    XML description of the other clock driver.
     *      @param [in] elementName             The name of the clock unit element.
     */
    QSharedPointer<ClockUnit> parseClockUnit(QDomNode const& otherClockDriverNode, QString const& elementName) const;

    /*!
     *  Parse the clock pulse value.
     *
     *      @param [in] otherClockDriverNode    XML description of the other clock driver.
     *      @param [in] newOtherClockDriver     The new other clock driver.
     */
    void parseClockPulseValue(QDomNode const& otherClockDriverNode,
        QSharedPointer<OtherClockDriver> newOtherClockDriver) const;
};

#endif // OtherClockDriverReader_H