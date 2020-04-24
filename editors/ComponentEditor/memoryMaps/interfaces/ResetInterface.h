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

#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>
#include <editors/ComponentEditor/common/interfaces/CommonInterface.h>

class Field;
class FieldReset;
class FieldValidator;

//-----------------------------------------------------------------------------
//! Interface for editing resets.
//-----------------------------------------------------------------------------
class ResetInterface : public ParameterizableInterface, public CommonInterface
{

public:

    /*!
     *  The constructor.
     */
    ResetInterface();

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
     *  Set field validator.
     *
     *      @param [in] validator   Validator for fields.
     */
    void setValidator(QSharedPointer<FieldValidator> validator);

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
     *      @param [in] currentResetType    Index of the selected reset.
     *      @param [in] newResetType        The new reset type reference.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setResetTypeReference(std::string const& currentResetType, std::string const& newResetType);
    
    /*!
     *  Get the calculated reset value of the selected reset type.
     *
     *      @param [in] resetType   The selected reset.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated reset value of the selected port.
     */
    std::string getResetValue(std::string const& resetType, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted reset value expression of the selected reset type.
     *
     *      @param [in] resetType   The selected reset.
     *
     *      @return Formatted reset value expression of the selected reset.
     */
    std::string getResetValueFormattedExpression(std::string const& resetType) const;

    /*!
     *  Get the reset value expression of the selected reset type.
     *
     *      @param [in] resetType   The selected reset.
     *
     *      @return Reset value expression of the selected reset.
     */
    std::string getResetValueExpression(std::string const& resetType) const;

    /*!
     *  Set a new reset value for the selected reset type.
     *
     *      @param [in] resetType       The selected reset.
     *      @param [in] newResetValue   New reset value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setResetValue(std::string const& resetType, std::string const& newResetValue);

    /*!
     *  Get the calculated reset mask value of the selected reset type.
     *
     *      @param [in] resetType   The selected reset.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated reset mask value of the selected port.
     */
    std::string getResetMaskValue(std::string const& resetType, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted reset mask expression of the selected reset type.
     *
     *      @param [in] resetType   The selected reset.
     *
     *      @return Formatted reset mask expression of the selected reset.
     */
    std::string getResetMaskFormattedExpression(std::string const& resetType) const;

    /*!
     *  Get the reset mask expression of the selected reset type.
     *
     *      @param [in] resetType   The selected reset.
     *
     *      @return Reset mask expression of the selected reset.
     */
    std::string getResetMaskExpression(std::string const& resetType) const;

    /*!
     *  Set a new reset mask for the selected reset type.
     *
     *      @param [in] resetType       The selected reset.
     *      @param [in] newResetMask    New reset mask.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setResetMask(std::string const& resetType, std::string const& newResetMask);

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the reset type reference of the selected reset is valid.
     *
     *      @param [in] resetType   Reset type reference of the selected reset.
     *
     *      @return True, if the selected reset type reference is valid, false otherwise.
     */
    bool hasValidResetType(std::string const& resetType) const;

    /*!
     *  Check if the reset value of the selected reset is valid.
     *
     *      @param [in] resetType   Reset type reference of the selected reset.
     *
     *      @return True, if the selected reset value is valid, false otherwise.
     */
    bool hasValidResetValue(std::string const& resetType) const;

    /*!
     *  Check if the reset mask of the selected reset is valid.
     *
     *      @param [in] resetType   Reset type reference of the selected reset.
     *
     *      @return True, if the selected reset mask is valid, false otherwise.
     */
    bool hasValidResetMask(std::string const& resetType) const;
    
    /*!
     *  Add a new reset.
     *
     *      @param [in] row             Row of the new reset.
     *      @param [in] newResetName    Name of the new reset.
     */
    void addReset(int const& row, std::string const& newResetName = std::string(""));

    /*!
     *  Remove the selected reset.
     *
     *      @param [in] resetName   Name of the selected reset.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeReset(std::string const& resetName);

    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const
        override final;

private:

    /*!
     *  Get the selected reset.
     *
     *      @param [in] resetName   Name of the selected reset.
     *
     *      @return The selected reset.
     */
    QSharedPointer<FieldReset> getReset(std::string const& resetType) const;

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
