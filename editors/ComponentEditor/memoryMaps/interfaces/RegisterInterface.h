//-----------------------------------------------------------------------------
// File: RegisterInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.04.2020
//
// Description:
// Interface for editing registers.
//-----------------------------------------------------------------------------

#ifndef REGISTERINTERFACE_H
#define REGISTERINTERFACE_H

#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>
#include <editors/ComponentEditor/common/interfaces/NameGroupInterface.h>

class Register;
class RegisterBase;
class RegisterValidator;
class FieldInterface;

#include <QVector>
#include <QMap>

//-----------------------------------------------------------------------------
//! Interface for editing registers.
//-----------------------------------------------------------------------------
class RegisterInterface : public ParameterizableInterface, public NameGroupInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] validator               Validator for registers.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] subInterface            Interface for accessing fields.
     */
    RegisterInterface(QSharedPointer<RegisterValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        FieldInterface* subInterface);
	
	/*!
     *  The destructor.
     */
    virtual ~RegisterInterface() = default;

    /*!
     *  Set available registers.
     *
     *      @param [in] newRegisterData     The new register data.
     */
    void setRegisters(QSharedPointer<QList<QSharedPointer<RegisterBase> > > newRegisterData);

    /*!
     *  Get index of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Index of the selected item.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get name of the indexed item.
     *
     *      @param [in] itemIndex   Index of the selected item.
     *
     *      @return Name of the selected item.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const override final;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the number of register data.
     *
     *      @return Number of register data.
     */
    int registerDataCount() const;

    /*!
     *  Get the selected index as the index of register data.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The selected index transformed to register data index.
     */
    int indexInRegisterData(int const& index) const;

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Set a new name for the selected item.
     *
     *      @param [in] currentName     Name of the selected item.
     *      @param [in] newName         New name for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) override final;

    /*!
     *  Get the description of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Description of the selected item.
     */
    virtual std::string getDescription(std::string const& itemName) const override final;

    /*!
     *  Set a new description for the selected item.
     *
     *      @param [in] itemName        Name of the selected item.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) override final;

    /*!
     *  Get the calculated offset value of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] baseNumber      Base for displaying the value.
     *
     *      @return Calculated offset value of the selected register.
     */
    std::string getOffsetValue(std::string const& registerName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted offset expression of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return Formatted offset expression of the selected register.
     */
    std::string getOffsetFormattedExpression(std::string const& registerName) const;

    /*!
     *  Get the offset expression of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return Offset expression of the selected register.
     */
    std::string getOffsetExpression(std::string const& registerName) const;

    /*!
     *  Set a new offset value for the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] newOffset       New offset value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setOffset(std::string const& registerName, std::string const& newOffset);
    
    /*!
     *  Get the calculated dimension value of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] baseNumber      Base for displaying the value.
     *
     *      @return Calculated dimension value of the selected register.
     */
    std::string getDimensionValue(std::string const& registerName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted dimension expression of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return Formatted dimension expression of the selected register.
     */
    std::string getDimensionFormattedExpression(std::string const& registerName) const;

    /*!
     *  Get the dimension expression of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return Dimension expression of the selected register.
     */
    std::string getDimensionExpression(std::string const& registerName) const;

    /*!
     *  Set a new dimension value for the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] newDimension    New dimension value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDimension(std::string const& registerName, std::string const& newDimension);
    
    /*!
     *  Get the calculated is present value of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] baseNumber      Base for displaying the value.
     *
     *      @return Calculated is present value of the selected register.
     */
    std::string getIsPresentValue(std::string const& registerName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted is present expression of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return Formatted is present expression of the selected register.
     */
    std::string getIsPresentFormattedExpression(std::string const& registerName) const;

    /*!
     *  Get the is present expression of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return Is present expression of the selected register.
     */
    std::string getIsPresentExpression(std::string const& registerName) const;

    /*!
     *  Set a new is present value for the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] newIsPresent    New is present value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setIsPresent(std::string const& registerName, std::string const& newIsPresent);
    
    /*!
     *  Get the calculated size value of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] baseNumber      Base for displaying the value.
     *
     *      @return Calculated size value of the selected register.
     */
    std::string getSizeValue(std::string const& registerName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted size expression of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return Formatted size expression of the selected register.
     */
    std::string getSizeFormattedExpression(std::string const& registerName) const;

    /*!
     *  Get the size expression of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return Size expression of the selected register.
     */
    std::string getSizeExpression(std::string const& registerName) const;

    /*!
     *  Set a new size value for the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] newSize         New size value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setSize(std::string const& registerName, std::string const& newSize);

    /*!
     *  Get the volatile of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return The volatile value of the selected register.
     */
    std::string getVolatile(std::string const& registerName) const;

    /*!
     *  Set the volatile for the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] newVolatile     The new volatile value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setVolatile(std::string const& registerName, std::string const& newVolatile);
    
    /*!
     *  Get the access of the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return The access value of the selected register.
     */
    std::string getAccess(std::string const& registerName) const;

    /*!
     *  Set the access value for the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] newAccess       The new access value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setAccess(std::string const& registerName, std::string const& newAccess);

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const override
        final;

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected item has a valid name.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

    /*!
     *  Check if the selected register has a valid dimension.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return True, if the dimension is valid, false otherwise.
     */
    bool hasValidDimension(std::string const& registerName) const;

    /*!
     *  Check if the selected register has a valid offset.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return True, if the offset is valid, false otherwise.
     */
    bool hasValidOffset(std::string const& registerName) const;

    /*!
     *  Check if the selected register has a valid is present value.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return True, if the is present value is valid, false otherwise.
     */
    bool hasValidIsPresent(std::string const& registerName) const;

    /*!
     *  Check if the selected register has a valid size.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return True, if the size is valid, false otherwise.
     */
    bool hasValidSize(std::string const& registerName) const;

    /*!
     *  Add a new register.
     *
     *      @param [in] row                 Row of the new register.
     *      @param [in] dataIndex           Index of the new register in the register data.
     *      @param [in] newRegisterName     Name of the new register.
     */
    void addRegister(int const& row, int const& dataIndex, std::string const& newRegisterName = std::string(""));

    /*!
     *  Remove the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *      @param [in] dataIndex       Index of the removed register in the register data.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeRegister(std::string const& registerName, int const& dataIndex);

    /*!
     *  Copy the selected registers.
     *
     *      @param [in] selectedRows    Indexes of the selected registers.
     */
    void copyRows(std::vector<int> selectedRows);

    /*!
     *  Paste the selected registers.
     *
     *      @return Names of the pasted registers.
     */
    std::vector<std::string> pasteRows();

    /*!
     *  Get the number of pasted registers.
     *
     *      @return Number of the pasted registers.
     */
    int getPasteRowCount() const;

    /*!
     *  Get the expressions in the selected registers.
     *
     *      @param [in] registerNames   Names of the selected registers.
     *
     *      @return The expressions of the selected registers.
     */
    std::vector<std::string> getExpressionsInSelectedRegisters(std::vector<std::string> registerNames) const;

    /*!
     *  Change the value for address unit bits.
     *
     *      @param [in] newAddressUnitbits  The new value for address unit bits.
     */
    void setAddressUnitBits(int const& newAddressUnitbits);

    /*!
     *  Get the sub interface.
     *
     *      @return Interface for accessing fields.
     */
    FieldInterface* getSubInterface() const;

private:

    /*!
     *  Get the selected register.
     *
     *      @param [in] registerName    Name of the selected register.
     *
     *      @return The selected register.
     */
    QSharedPointer<Register> getRegister(std::string const& registerName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained register data.
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData_;

    //! List of contained registers.
    QList<QSharedPointer<Register> > registers_;

    //! Validator for registers.
    QSharedPointer<RegisterValidator> validator_;

    //! Interface for accessing fields.
    FieldInterface* subInterface_;

    //! The address unit bits of the memory map.
    unsigned int addressUnitBits_;
};

#endif // REGISTERINTERFACE_H
