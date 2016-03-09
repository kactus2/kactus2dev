//-----------------------------------------------------------------------------
// File: CPUWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Writer class for IP-XACT CPU element.
//-----------------------------------------------------------------------------

#ifndef CPUWriter_H
#define CPUWriter_H

#include "Cpu.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>
#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT CPU element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT CPUWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    CPUWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~CPUWriter();

    /*!
     *  Write a CPU to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] cpu		The cpu to be written.
     */
    void writeCPU(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu) const;

private:

    //! No copying allowed.
    CPUWriter(CPUWriter const& rhs);
    CPUWriter& operator=(CPUWriter const& rhs);

    /*!
     *  Writes the name group to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] cpu		The cpu to be written.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu) const;

    /*!
     *  Writes the presence to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] cpu		The cpu to be written.
     */
    void writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu) const;

    /*!
     *  Writes the address space references.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] cpu		The cpu to be written.
     */
    void writeAddressSpaceRefs(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu) const;

};

#endif // CPUWriter_H
