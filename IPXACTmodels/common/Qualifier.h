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

    bool operator==(Qualifier const& other) const
    {
        return
            isAddress == other.isAddress && isData == other.isData &&
            isClock == other.isClock && isReset == other.isReset &&
            resetLevel == other.resetLevel && isValid == other.isValid &&
            isInterrupt == other.isInterrupt && isClockEn == other.isClockEn &&
            clockEnLevel == other.clockEnLevel && isPowerEn == other.isPowerEn &&
            powerEnLevel == other.powerEnLevel && powerDomainRef == other.powerDomainRef &&
            isOpcode == other.isOpcode && isProtection == other.isProtection &&
            isFlowControl == other.isFlowControl && flowType == other.flowType &&
            userFlowType == other.userFlowType && isUser == other.isUser &&
            userDefinedInformation == other.userDefinedInformation &&
            isRequest == other.isRequest && isResponse == other.isResponse;
    }

    bool operator!=(Qualifier const& other) const
    {
        return !operator==(other);
    }
};

#endif // QUALIFIER_H
