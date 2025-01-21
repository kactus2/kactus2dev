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

#ifndef CPUWRITER_H
#define CPUWRITER_H

#include "Cpu.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT CPU element.
//-----------------------------------------------------------------------------
namespace CPUWriter
{
    /*!
     *  Write a CPU to an XML file.
     *
     *    @param [in] writer  The used xml writer.
     *    @param [in] cpu		The cpu to be written.
     */
    IPXACTMODELS_EXPORT void writeCPU(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu, Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Writes the address space references.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] cpu		The cpu to be written.
         */
        void writeAddressSpaceRefs(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu);

        void writeBlockSize(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu);

        void writeRegions(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu);

        void writeExecutableImages(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu);

        void writeLanguageTools(QXmlStreamWriter& writer, QSharedPointer<LanguageTools> languageTools);

        void writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu);

        void writeMemoryMapRef(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu);
    }
};

#endif // CPUWRITER_H
