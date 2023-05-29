//-----------------------------------------------------------------------------
// File: Qualifier.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Implementation for ipxact:qualifier group.
//-----------------------------------------------------------------------------

#ifndef QUALIFIER_H
#define QUALIFIER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:qualifier group.
//-----------------------------------------------------------------------------
struct IPXACTMODELS_EXPORT Qualifier 
{
    //! Flag for address qualifier.
    bool isAddress = false;

    //! Flag for data qualifier.
    bool isData = false;

    //! Flag for clock qualifier.
    bool isClock = false;

    //! Flag for reset qualifier.
    bool isReset = false;

    //! Reset qualifier level attribute. Can be low or high.
    QString resetLevel;

    //! Flag for valid qualifier.
    bool isValid = false;

    //! Flag for interrupt qualifier.
    bool isInterrupt = false;

    //! Flag for clock enable qualifier.
    bool isClockEn = false;

    //! ClockEn qualifier level attribute. Can be low or high.
    QString clockEnLevel;

    //! Flag for PowerEn qualifier. 
    bool isPowerEn = false;

    //! PowerEn qualifier level attribute. Can be low or high.
    QString powerEnLevel;

    //! PowerEn qualifier power domain ref attribute, references
    //  a power domain of a component. Must not be set for abstraction
    //  definitions.
    QString powerDomainRef;

    //! Flag for opcode qualifier.
    bool isOpcode = false;

    //! Flag for protection qualifier.
    bool isProtection = false;

    //! Flag for flow control qualifier.
    bool isFlowControl = false;

    //! FlowControl qualifier flow type attribute.
    //  Can be credit-return, ready, busy or user.
    QString flowType;

    //! User attribute of flowControl qualifier.
    //  A user defined flow type.
    QString userFlowType;

    //! Flag for user qualifier.
    //  Port contains user-defined information if true.
    bool isUser = false;
    
    //! User attribute of user qualifier.
    //  Contains the user defined information.
    QString userDefinedInformation;
    
    //! Flag for flow request qualifier.
    bool isRequest = false;

    //! Flag for response qualifier.
    bool isResponse = false;

    void clear()
    {
        isAddress = false;
        isData = false;
        isClock = false;
        isReset = false;
        resetLevel.clear();
        isValid = false;
        isInterrupt = false;
        isClockEn = false;
        clockEnLevel.clear();
        isPowerEn = false;
        powerEnLevel.clear();
        powerDomainRef.clear();
        isOpcode = false;
        isProtection = false;
        isFlowControl = false;
        flowType.clear();
        userFlowType.clear();
        isUser = false;
        userDefinedInformation.clear();
        isRequest = false;
        isResponse = false;
    }

    void setQualifier(QString const& qualifierType)
    {
        if (qualifierType == QStringLiteral("address") ||
            qualifierType == QStringLiteral("data/address"))
        {
            isAddress = true;
        }
        else if (qualifierType == QStringLiteral("data") ||
            qualifierType == QStringLiteral("data/address"))
        {
            isData = true;
        }
        else if (qualifierType == QStringLiteral("data/address"))
        {
            isAddress = true;
            isData = true;
        }
        else if (qualifierType == QStringLiteral("clock"))
        {
            isClock = true;
        }
        else if (qualifierType == QStringLiteral("reset"))
        {
            isReset = true;
        }
        else if (qualifierType == QStringLiteral("valid"))
        {
            isValid = true;
        }
        else if (qualifierType == QStringLiteral("interrupt"))
        {
            isInterrupt = true;
        }
        else if (qualifierType == QStringLiteral("clock enable"))
        {
            isClockEn = true;
        }
        else if (qualifierType == QStringLiteral("power enable"))
        {
            isPowerEn = true;
        }
        else if (qualifierType == QStringLiteral("opcode"))
        {
            isOpcode = true;
        }
        else if (qualifierType == QStringLiteral("protection"))
        {
            isProtection = true;
        }
        else if (qualifierType == QStringLiteral("flow control"))
        {
            isFlowControl = true;
        }
        else if (qualifierType == QStringLiteral("user"))
        {
            isUser = true;
        }
        else if (qualifierType == QStringLiteral("request"))
        {
            isRequest = true;
        }
        else if (qualifierType == QStringLiteral("response"))
        {
            isResponse = true;
        }
    }
};

#endif // QUALIFIER_H
