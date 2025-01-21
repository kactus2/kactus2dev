//-----------------------------------------------------------------------------
// File: NMTokenValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.12.2014
//
// Description:
// Validator for IP-Xact's NMtoken-type
//-----------------------------------------------------------------------------

#ifndef NMTOKENVALIDATOR_H
#define NMTOKENVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QRegularExpressionValidator>

//-----------------------------------------------------------------------------
//! NMTokenValidator class.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT NMTokenValidator : public QRegularExpressionValidator
{
    Q_OBJECT

public:

    /*!
     *  Constructor for the validator.
     *
     *    @param [in] parent   Parent of the validator.
     */
    explicit NMTokenValidator(QObject* parent);

    //! No copying
    NMTokenValidator(const NMTokenValidator& other) = delete;
    
    //! No assignment
    NMTokenValidator& operator=(const NMTokenValidator& other) = delete;

    /*!
     *  Destructor of the validator.
     */
    virtual ~NMTokenValidator() = default;

};

#endif // NMTOKENVALIDATOR_H
