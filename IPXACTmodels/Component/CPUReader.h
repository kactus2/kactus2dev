//-----------------------------------------------------------------------------
// File: CPUReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Reader class for IP-XACT CPU element.
//-----------------------------------------------------------------------------

#ifndef CPUReader_H
#define CPUReader_H

#include "Cpu.h"

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT CPU element.
//-----------------------------------------------------------------------------
namespace CPUReader
{
    /*!
     *  Creates a new CPU from a given CPU node.
     *
     *    @param [in] cpuNode    XML description of the CPU.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Cpu> createCPUFrom(QDomNode const& cpuNode, Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Reads the presence.
         *
         *    @param [in] cpuNode    XML description of the cpu.
         *    @param [in] newCPU     The new cpu item.
         */
        void parseIsPresent(QDomNode const& cpuNode, QSharedPointer<Cpu> newCPU);

        /*!
         *  Reads the address space references.
         *
         *    @param [in] cpuNode    XML description of the cpu.
         *    @param [in] newCPU     The new cpu item.
         */
        void parseAddressSpaceRefs(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu);

        void parseBlockSize(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu);

        void parseRegions(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu);

        void parseAddressUnitBits(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu);

        void parseExecutableImages(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu);

        void parseLanguageTools(QDomElement const& languageToolsElement, QSharedPointer<ExecutableImage> image);

        void parseLinkerCommandFile(QDomElement const& linkerCommandFileElement,
            QSharedPointer<LanguageTools> languageTools);

        void parseMemoryMapReference(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu);

        /*!
         *  Reads the parameters.
         *
         *    @param [in] cpuNode    XML description of the cpu.
         *    @param [in] newCPU     The new cpu item.
         */
        void parseParameters(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu);
    }
};

#endif // CPUReader_H
