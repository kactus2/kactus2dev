//-----------------------------------------------------------------------------
// File: BankReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 05.06.2026
//
// Description:
// Reader for ipxact:bank elements.
//-----------------------------------------------------------------------------

#pragma once

#include "Bank.h"

#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamReader>
#include <QSharedPointer>
#include <QDomNode>

namespace BankReader
{
    IPXACTMODELS_EXPORT QSharedPointer<Bank> createBankFrom(QDomNode const& bankNode, Document::Revision docRevision);

    namespace Details
    {
        void parseBankAlignment(QDomNode const& bankNode, QSharedPointer<Bank> newBank);

        void parseAccessHandles(QDomNode const& bankNode, QSharedPointer<Bank> newBank, Document::Revision docRevision);

        void parseBankDefinitionRef(QDomNode const& bankNode, QSharedPointer<Bank> newBank);

        void parseAddressBlocks(QDomNode const& bankNode, QSharedPointer<Bank> newBank, Document::Revision docRevision);

        void parseBanks(QDomNode const& bankNode, QSharedPointer<Bank> newBank, Document::Revision docRevision);

        void parseSubspaces(QDomNode const& bankNode, QSharedPointer<Bank> newBank, Document::Revision docRevision);
    }
}
