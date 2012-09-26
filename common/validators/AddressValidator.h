//-----------------------------------------------------------------------------
// File: AddressValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.9.2012
//
// Description:
// Declares the address validator class.
//-----------------------------------------------------------------------------

#ifndef ADDRESSVALIDATOR_H
#define ADDRESSVALIDATOR_H

#include <QValidator>

//-----------------------------------------------------------------------------
//! Hexadecimal address validator.
//-----------------------------------------------------------------------------
class AddressValidator : public QValidator
{
public:
    /*!
     *  Constructor.
     *  
     *      @param [in] parent  The parent.
     */
    AddressValidator(QObject* parent = 0);

    /*!
     *  Destructor.
     */
    ~AddressValidator();

    /*!
     *  Sets the minimum allowed address.
     *  
     *      @param [in] address The address.
     */
    void setMinAddress(unsigned int address);

    /*!
     *  Sets the maximum allowed address.
     *  
     *      @param [in] address The address.
     */
    void setMaxAddress(unsigned int address);

    /*!
     *  Tries to fix the given input.
     *  
     *      @param [in,out] input The input.
     */
    virtual void fixup(QString& input) const;

    /*!
     *  Validates.
     *  
     *      @param [in,out] input   The input.
     *      @param [in,out] pos     The position.
     *  
     *      @return The validation result.
     */
    virtual State validate(QString& input, int& pos) const;

private:
    // Disable copying.
    AddressValidator(AddressValidator const& rhs);
    AddressValidator& operator=(AddressValidator const& rhs);

    /*!
     *  Validates the address input against the allowed address bounds.
     *  
     *      @param [in] input   The input.
     *  
     *      @return True if valid, false if out of bounds.
     */
    bool validateAddressBounds(QString const& input) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The minimum address.
    unsigned int minAddress_;

    //! The maximum address.
    unsigned int maxAddress_;
};

#endif // ADDRESSVALIDATOR_H
