//-----------------------------------------------------------------------------
// File: ResetInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.04.2020
//
// Description:
// Interface for editing resets.
//-----------------------------------------------------------------------------

#ifndef RESETINTERFACE_H
#define RESETINTERFACE_H

#include <common/Global.h>

#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>
#include <editors/ComponentEditor/common/interfaces/CommonInterface.h>

class Field;
class FieldReset;
class FieldValidator;

//-----------------------------------------------------------------------------
//! Interface for editing resets.
//-----------------------------------------------------------------------------
class KACTUS2_API ResetInterface : public ParameterizableInterface, public CommonInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] validator               Validator for fields.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     */
    ResetInterface(QSharedPointer<FieldValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter);

    /*!
     *  The destructor.
     */
    virtual ~ResetInterface() = default;

    /*!
     *  Set available resets.
     *
     *      @param [in] containingField     Field containing the resets.
     */
    void setResets(QSharedPointer<Field> containingField);

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const override final;
    
    /*!
     *  Get the reset type reference of the selected reset.
     *
     *      @param [in] itemIndex   Index of the selected reset.
     *
     *      @return Reset type reference of the selected reset.
     */
    std::string getResetTypeReference(int const& itemIndex) const;

    /*!
     *  Set the reset type reference for the selected reset.
     *
     *      @param [in] resetIndex      Index of the selected reset.
     *      @param [in] newResetType    The new reset type reference.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setResetTypeReference(int const& resetIndex, std::string const& newResetType);

    /*!
     *  Get the calculated reset value of the selected reset type.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated reset value of the selected port.
     */
    std::string getResetValue(int const& resetIndex, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted reset value expression of the selected reset type.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *
     *      @return Formatted reset value expression of the selected reset.
     */
    std::string getResetValueFormattedExpression(int const& resetIndex) const;

    /*!
     *  Get the reset value expression of the selected reset type.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *
     *      @return Reset value expression of the selected reset.
     */
    std::string getResetValueExpression(int const& resetIndex) const;

    /*!
     *  Set a new reset value for the selected reset type.
     *
     *      @param [in] resetIndex      Index of the selected reset.
     *      @param [in] newResetValue   New reset value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setResetValue(int const& resetIndex, std::string const& newResetValue);

    /*!
     *  Get the calculated reset mask value of the selected reset type.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated reset mask value of the selected port.
     */
    std::string getResetMaskValue(int const& resetIndex, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted reset mask expression of the selected reset type.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *
     *      @return Formatted reset mask expression of the selected reset.
     */
    std::string getResetMaskFormattedExpression(int const& resetIndex) const;

    /*!
     *  Get the reset mask expression of the selected reset type.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *
     *      @return Reset mask expression of the selected reset.
     */
    std::string getResetMaskExpression(int const& resetIndex) const;

    /*!
     *  Set a new reset mask for the selected reset type.
     *
     *      @param [in] resetIndex      Index of the selected reset.
     *      @param [in] newResetMask    New reset mask.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setResetMask(int const& resetIndex, std::string const& newResetMask);

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the reset type reference of the selected reset is valid.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *
     *      @return True, if the selected reset type reference is valid, false otherwise.
     */
    bool hasValidResetType(int const& resetIndex) const;

    /*!
     *  Check if the reset value of the selected reset is valid.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *
     *      @return True, if the selected reset value is valid, false otherwise.
     */
    bool hasValidResetValue(int const& resetIndex) const;

    /*!
     *  Check if the reset mask of the selected reset is valid.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *
     *      @return True, if the selected reset mask is valid, false otherwise.
     */
    bool hasValidResetMask(int const& resetIndex) const;

    /*!
     *  Add a new reset.
     *
     *      @param [in] row             Row of the new reset.
     */
    void addReset(int const& row);

    /*!
     *  Remove the selected reset.
     *
     *      @param [in] resetName   Name of the selected reset.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeReset(int const& resetIndex);

    /*!
     *  Get all the references made to the selected ID in the selected reset.
     *
     *      @param [in] itemName    The reset type reference of the selected reset.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references made to the selected ID in the selected reset.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const
        override final;

    /*!
     *  Get all the references made to the selected ID in the selected reset.
     *
     *      @param [in] itemIndex   Index of the selected reset.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references made to the selected ID in the selected reset.
     */
    int getAllReferencesToIdInIndex(int const& itemIndex, std::string const& valueID) const;

private:

    /*!
     *  Get the selected reset.
     *
     *      @param [in] resetIndex  Index of the selected reset.
     *
     *      @return The selected reset.
     */
    QSharedPointer<FieldReset> getReset(int const& resetIndex) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Field containing the resets.
    QSharedPointer<Field> containingField_;

    //! List of the contained fields.
    QSharedPointer<QList<QSharedPointer<FieldReset> > > resets_;

    //! Validator for fields.
    QSharedPointer<FieldValidator> validator_;
};

#endif // RESETINTERFACE_H
