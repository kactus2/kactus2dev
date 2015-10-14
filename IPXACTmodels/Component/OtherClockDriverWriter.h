//-----------------------------------------------------------------------------
// File: OtherClockDriverWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.10.2015
//
// Description:
// Writer class for IP-XACT OtherClockDriver element.
//-----------------------------------------------------------------------------

#ifndef OtherClockDriverWriter_H
#define OtherClockDriverWriter_H

#include "OtherClockDriver.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

class ClockUnit;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT OtherClockDriver element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT OtherClockDriverWriter : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    OtherClockDriverWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~OtherClockDriverWriter();

    /*!
     *  Write a OtherClockDriver to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] OtherClockDriver		The OtherClockDriver to be written.
     */
    void writeOtherClockDriver(QXmlStreamWriter& writer, QSharedPointer<OtherClockDriver> otherClockDriver) const;

private:

    //! No copying allowed.
    OtherClockDriverWriter(OtherClockDriverWriter const& rhs);
    OtherClockDriverWriter& operator=(OtherClockDriverWriter const& rhs);

    /*!
     *  Write the other clock driver attributes.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] otherClockDriver    The new other clock driver.
     */
    void writeClockDriverAttributes(QXmlStreamWriter& writer, QSharedPointer<OtherClockDriver> otherClockDriver) const;

    /*!
     *  Write a clock unit.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] clockUnit       The new other clock driver.
     *      @param [in] elementName     The name of the element being written.
     */
    void writeClockUnit(QXmlStreamWriter& writer, QSharedPointer<ClockUnit> clockUnit, QString const& elementName)
        const;
};

#endif // OtherClockDriverWriter_H