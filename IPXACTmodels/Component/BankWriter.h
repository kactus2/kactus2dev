//-----------------------------------------------------------------------------
// File: BankWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 05.06.2026
//
// Description:
// Writer for ipxact:bank elements.
//-----------------------------------------------------------------------------

#pragma once

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>

class Bank;

namespace BankWriter
{
    IPXACTMODELS_EXPORT void writeBank(QXmlStreamWriter& writer, QSharedPointer<Bank> bank, Document::Revision docRevision);

    namespace Details
    {
        void writeAlignment(QXmlStreamWriter& writer, QSharedPointer<Bank> bank);

        void writeBankDefinitionRef(QXmlStreamWriter& writer, QSharedPointer<Bank> bank);

        void writeAddressBlocks(QXmlStreamWriter& writer, QSharedPointer<Bank> bank, Document::Revision docRevision);
        void writeBanks(QXmlStreamWriter& writer, QSharedPointer<Bank> bank, Document::Revision docRevision);
        void writeSubspaces(QXmlStreamWriter& writer, QSharedPointer<Bank> bank, Document::Revision docRevision);
    }
}