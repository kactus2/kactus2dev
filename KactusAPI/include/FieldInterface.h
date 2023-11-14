//-----------------------------------------------------------------------------
// File: FieldInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.04.2020
//
// Description:
// Interface for editing fields.
//-----------------------------------------------------------------------------

#ifndef FIELDINTERFACE_H
#define FIELDINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <ParameterizableInterface.h>
#include <NameGroupInterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>

#include <IPXACTmodels/Component/validators/FieldAccessPolicyValidator.h>

#include <QVector>
#include <QMap>

class Field;
class FieldValidator;
class ResetInterface;
class WriteValueConstraint;
class ModeReference;
class ModeReferenceInterface;

//-----------------------------------------------------------------------------
//! Interface for editing fields.
//-----------------------------------------------------------------------------
class KACTUS2_API FieldInterface : public ParameterizableInterface, public NameGroupInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] validator               Validator for fields.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] subInterface            Interface for accessing resets.
     */
    FieldInterface(QSharedPointer<FieldValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        ResetInterface* subInterface);
	
	/*!
     *  The destructor.
     */
    virtual ~FieldInterface() = default;

    /*!
     *  Set available fields.
     *
     *      @param [in] newFields   The new fields.
     */
    void setFields(QSharedPointer<QList<QSharedPointer<Field> > > newFields);

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
    std::string getIndexedItemName(int itemIndex) const final;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const override final;

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
     *  Get the calculated offset value of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated offset value of the selected field.
     */
    std::string getOffsetValue(std::string const& fieldName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted offset expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Formatted offset expression of the selected field.
     */
    std::string getOffsetFormattedExpression(std::string const& fieldName) const;

    /*!
     *  Get the offset expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Offset expression of the selected field.
     */
    std::string getOffsetExpression(std::string const& fieldName) const;

    /*!
     *  Set a new offset value for the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] newOffset   New offset value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setOffset(std::string const& fieldName, std::string const& newOffset);

    /*!
     *  Get the calculated bit width value of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated bit width value of the selected parameter.
     */
    std::string getWidthValue(std::string const& fieldName, int const& baseNumber = 0) const;
    
    /*!
     *  Get the formatted bit width expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Formatted bit width expression of the selected field.
     */
    std::string getWidthFormattedExpression(std::string const& fieldName) const;

    /*!
     *  Get the bit width expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Bit width expression of the selected field.
     */
    std::string getWidthExpression(std::string const& fieldName) const;

    /*!
     *  Set a new bit width value for the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] newWidth    New bit width value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setWidth(std::string const& fieldName, std::string const& newWidth);

    /*!
     *  Get the reset values of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Reset values of the selected field.
     */
    std::string getResets(std::string const& fieldName) const;

    /*!
     *  Get the tooltip of the reset values of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Tooltip constructed from the reset values of the selected field.
     */
    std::string getResetsToolTip(std::string const& fieldName) const;

    /*!
     *  Get the volatile status of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Volatile status of the selected field.
     */
    std::string getVolatile(std::string const& fieldName) const;

    /*!
     *  Set the volatile status of the selected field.
     *
     *      @param [in] fieldName       Name of the selected field.
     *      @param [in] newVolatile     The new volatile status of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setVolatile(std::string const& fieldName, std::string const& newVolatile);

    /*!
     *  Get the access string of the selected field or field access policy.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] accessPolicyIndex   Index of the selected field access policy.
     *
     *      @return Access string of the selected field or field access policy.
     */
    std::string getAccessString(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Get the access of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   Index of the selected field access policy.
     *
     *      @return Access of the selected field or field access policy.
     */
    AccessTypes::Access getAccessType(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Set the access of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] newAccess           The new access of the selected field.
     *      @param [in] accessPolicyIndex   Index of the selected field access policy.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setAccess(std::string const& fieldName, std::string const& accessType, int accessPolicyIndex = -1) const;

    /*!
     *  Get the modified write value string of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   Index of the selected field access policy.
     *
     *      @return Modified write value string of the selected field or field access policy.
     */
    std::string getModifiedWriteString(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Get the modified write value of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Modified write value of the selected field.
     */
    General::ModifiedWrite getModifiedWriteValue(std::string const& fieldName) const;

    /*!
     *  Set the modified write value of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] newModifiedWrite    The new modified write value of the selected field.
     *      @param [in] accessPolicyIndex   Index of the selected field access policy.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setModifiedWrite(std::string const& fieldName, std::string const& newModifiedWrite, 
        int accessPolicyIndex = -1);

    /*!
     *  Get the read action string of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   Index of the selected field access policy.
     *
     *      @return Read action string of the selected field access policy of selected field.
     */
    std::string getReadActionString(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Get the read action of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field.
     *
     *      @return Read action of the selected field or field access policy.
     */
    General::ReadAction getReadAction(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Set the read action of the selected field or field access policy.
     *
     *      @param [in] fieldName               Name of the selected field.
     *      @param [in] newReadAction           The new read action.
     *      @param [in] accessPolicyIndex       Index of the selected field access policy.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setReadAction(std::string const& fieldName, std::string const& newReadAction, int accessPolicyIndex = -1);

    /*!
     *  Get the testable string of the selected field access policy of a selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] accessPolicyIndex   Index of the selected field access policy.
     *
     *      @return Testable string of the selected field access policy.
     */
    std::string getTestableValue(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Get the testable bool of the selected field or field access policy. Access policy index is provided
     *  only when getting the testable value of a field access policy within a 2022 std revision field.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return Testable bool of the selected field.
     */
    bool getTestableBool(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Set the testable value of either a selected field or a selected field access policy within a field.
     *  Access policy index is provided only when setting a testable value of a field access policy within a 
     *  std revision 2022 field.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] newTestable         The new testable value of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setTestable(std::string const& fieldName, std::string const& newTestable, int accessPolicyIndex = -1) const;

    /*!
     *  Get the test constraint string of the selected field.
     *  Access policy index is provided only when getting the test constraint of a field access policy within a
     *  2022 std revision field.
     * 
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return Test constraint string of the selected field.
     */
    std::string getTestConstraintString(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Get the test constraint of the selected field or field access policy. Access policy index is provided 
     *  only when getting the test constraint of a field access policy within a 2022 std revision field.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return Test constraint of the selected field.
     */
    General::TestConstraint getTestConstraint(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Set the test constraint of the selected field or field access policy. Access policy index is provided
     *  only when setting the test constraint of a field access policy within a 2022 std revision field.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] newTestConstraint   The new test constraint of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setTestConstraint(std::string const& fieldName, std::string const& newTestConstraint, int accessPolicyIndex = -1) const;

    /*!
     *  Get the calculated is present value of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated is present value of the selected field.
     */
    std::string getIsPresentValue(std::string const& fieldName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted is present expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Formatted is present expression of the selected field.
     */
    std::string getIsPresentFormattedExpression(std::string const& fieldName) const;

    /*!
     *  Get the is present expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Is present expression of the selected parameter.
     */
    std::string getIsPresentExpression(std::string const& fieldName) const;

    /*!
     *  Set a new is present value for the selected field.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newIsPresent    New is present value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setIsPresent(std::string const& fieldName, std::string const& newIsPresent);

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const override final;

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
     *  Check if the selected field has a valid offset.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return True, if the offset is valid, false otherwise.
     */
    bool hasValidOffset(std::string const& fieldName) const;

    /*!
     *  Check if the selected field has a valid bit width.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return True, if the bit width is valid, false otherwise.
     */
    bool hasValidWidth(std::string const& fieldName) const;

    /*!
     *  Check if the selected field has valid reset values.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return True, if the reset values are valid, false otherwise.
     */
    bool hasValidResets(std::string const& fieldName) const;

    /*!
     *  Check if the selected field has a valid is present.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return True, if the is present is valid, false otherwise.
     */
    bool hasValidIsPresent(std::string const& fieldName) const;

    /*!
     *  Check if the selected field or field access policy has a valid access.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   Index of the field access policy to validate.
     *
     *      @return True, if the access is valid, false otherwise.
     */
    bool hasValidAccess(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Check if the selected field access policy has a valid read response.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   Index of the field access policy to validate.
     *
     *      @return True, if the read response is valid, false otherwise.
     */
    bool hasValidReadResponse(std::string const& fieldName, int accessPolicyIndex) const;

    /*!
     *  Check if the selected field access policy has a valid reserved value.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   Index of the field access policy to validate.
     *
     *      @return True, if the reserved is valid, false otherwise.
     */
    bool hasValidReserved(std::string const& fieldName, int accessPolicyIndex) const;

    /*!
     *  Check if the selected field access policy has a valid write value constraint.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   Index of the field access policy to validate.
     *
     *      @return True, if the write value constraint is valid, false otherwise.
     */
    bool hasValidWriteValueConstraint(std::string const& fieldName, int accessPolicyIndex) const;

    /*!
     *  Add a new field.
     *
     *      @param [in] row             Row of the new field.
     *      @param [in] newFieldName    Name of the new field.
     */
    void addField(int const& row, std::string const& newFieldName = std::string(""));

    /*!
     *  Remove the selected field.
     *
     *      @param [in] FieldName   Name of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeField(std::string const& fieldName);

    /*!
     *  Copy the selected fields.
     *
     *      @param [in] selectedRows    Indexes of the selected fields.
     */
    void copyRows(std::vector<int> const& selectedRows);

    /*!
     *  Paste the selected fields.
     *
     *      @return Names of the pasted fields.
     */
    std::vector<std::string> pasteRows();

    /*!
     *  Get the number of pasted fields.
     *
     *      @return Number of the pasted fields.
     */
    int getPasteRowCount() const;

    /*!
     *  Get the expressions in the selected fields.
     *
     *      @param [in] fieldNames  Names of the selected fields.
     *
     *      @return The expressions of the selected fields.
     */
    std::vector<std::string> getExpressionsInSelectedFields(std::vector<std::string> const& fieldNames) const;

    /*!
     *  Get the sub interface.
     *
     *      @return Interface for accessing resets.
     */
    ResetInterface* getSubInterface() const;

    /*!
     *  Check if the selected field or field access policy has a write value constraint. Access policy index is 
     *  provided only when checking the existence of a write constraint of a field access policy within a 2022 
     *  std revision field.
     *
     *      @param [in] fieldName           Name of the selected field.
     *
     *      @return True, if the selected field or field access policy has write value constraint, false otherwise.
     */
    bool hasWriteConstraint(std::string const& fieldName) const;

    /*!
     *  Get the write value constraint type of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return The write value constraint type of the selected field or field access policy.
     */
    std::string getWriteConstraint(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Set a new write value constraint type for the selected field or field access policy.
     * 
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] newConstraintText   The new write value constraint type.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setWriteConstraint(std::string const& fieldName, std::string const& newConstraintText,
        int accessPolicyIndex = -1);

    /*!
     *  Get the calculated write value constraint minimum value of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *      @param [in] baseNumber          Base for displaying the value.
     *
     *      @return Calculated write value constraint minimum value of the selected field or field access policy.
     */
    std::string getWriteConstraintMinimumValue(std::string const& fieldName, int accessPolicyIndex = -1,
        int const& baseNumber = 0) const;

    /*!
     *  Get the formatted write value constraint minimum expression of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     * 
     *      @return Formatted write value constraint minimum expression of the selected field or field access policy.
     */
    std::string getWriteConstraintMinimumFormattedExpression(std::string const& fieldName,
        int accessPolicyIndex = -1) const;

    /*!
     *  Get the write value constraint minimum expression of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return Write value constraint minimum expression of the selected parameter.
     */
    std::string getWriteConstraintMinimumExpression(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Set a new write value constraint minimum value for the selected field or field access policy.
     *
     *      @param [in] parameterName               Name of the selected parameter.
     *      @param [in] newWriteConstraintMinimum   New write value constraint minimum value.
     *      @param [in] accessPolicyIndex           The index of the selected field access policy.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setWriteConstraintMinimum(std::string const& fieldName, std::string const& newWriteConstraintMinimum,
        int accessPolicyIndex = -1);

    /*!
     *  Get the calculated write value constraint maximum value of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *      @param [in] baseNumber          Base for displaying the value.
     *
     *      @return Calculated write value constraint maximum value of the selected field or field access policy.
     */
    std::string getWriteConstraintMaximumValue(std::string const& fieldName, int accessPolicyIndex = -1,
        int const& baseNumber = 0) const;

    /*!
     *  Get the formatted write value constraint maximum expression of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return Formatted write value constraint maximum expression of the selected field or field access policy.
     */
    std::string getWriteConstraintMaximumFormattedExpression(std::string const& fieldName,
        int accessPolicyIndex = -1) const;

    /*!
     *  Get the write value constraint maximum expression of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return Write value constraint maximum expression of the selected parameter.
     */
    std::string getWriteConstraintMaximumExpression(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Set a new write value constraint maximum value for the selected field or field access policy.
     *
     *      @param [in] parameterName               Name of the selected parameter.
     *      @param [in] newWriteConstraintMaximum   New write value constraint maximum value.
     *      @param [in] accessPolicyIndex           The index of the selected field access policy.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setWriteConstraintMaximum(std::string const& fieldName, std::string const& newWriteConstraintMaximum,
        int accessPolicyIndex = -1);

    /*!
     *  Get the calculated reserved value of the selected field or field access policy, which can be specified
     *  by index, for getting the reserved value from a field access policy of a 2022 standard compliant field.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The selected access policy index.
     *      @param [in] baseNumber          Base for displaying the value.
     *
     *      @return Calculated reserved value of the selected field.
     */
    std::string getReservedValue(std::string const& fieldName, int accessPolicyIndex = -1,
        int const& baseNumber = 0) const;

    /*!
     *  Get the formatted reserved expression of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return Formatted reserved expression of the selected field or field access policy.
     */
    std::string getReservedFormattedExpression(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Get the reserved expression of the selected field or field access policy.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return Reserved expression of the selected parameter.
     */
    std::string getReservedExpression(std::string const& fieldName, int accessPolicyIndex = -1) const;

    /*!
     *  Set a new reserved value for the selected field or field access policy.
     *
     *      @param [in] parameterName       Name of the selected parameter.
     *      @param [in] newReserved         New reserved value.
     *      @param [in] accessPolicyIndex   The index of the selected field access policy.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setReserved(std::string const& fieldName, std::string const& newReserved, int accessPolicyIndex = -1);

    /*!
     *  Get the ID of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return ID of the selected field.
     */
    std::string getID(std::string const& fieldName);

    /*!
     *  Set a ID for the selected field.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newID           New ID.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setID(std::string const& fieldName, std::string const& newID);

    /*!
     *	Add a new field access policy to a selected field.
     *  
     *      @param [in] fieldName     The field to add a new access policy to.
     *	    
     * 	    @return True, if successful, otherwise false.
     */
    bool addFieldAccessPolicy(std::string const& fieldName);

    /*!
     *	Remove the given field access policy from a specific field.
     *  
     *      @param [in] fieldName             The name of the field.
     *      @param [in] accessPolicyIndex     The index of the field access policy to be removed.
     *	    
     * 	    @return True, if successful, otherwise false.
     */
    bool removeFieldAccessPolicy(std::string const& fieldName, int accessPolicyIndex);

    /*!
     *	Get the mode references of a given field access policy.
     *  
     *      @param [in] fieldName               The name of the field.
     *      @param [in] accessPolicyIndex       Index of the field access policy.
     *	    
     * 	    @return A vector containing the mode references as a string-int pair.
     */
    std::vector<std::pair<std::string, unsigned int> > getModeRefs(std::string const& fieldName, int accessPolicyIndex) const;

    /*!
     *	Get all mode references of field access policies except for the one given by index.
     *  
     *      @param [in] fieldName               The field containing the field access policies.
     *      @param [in] accessPolicyIndex       The index of the access policy whose mode references are not to get.
     *	    
     * 	    @return  All mode references, except for mode references of field access policy given by index.
     */
    std::vector<std::pair<unsigned int, std::string> > getModeReferencesInUse(std::string const& fieldName, int accessPolicyIndex) const;

    /*!
     *	Get mode references of given field access policy.
     *  
     *      @param [in] fieldName               The name of the field, which contains the field access policy.
     *      @param [in] accessPolicyIndex       Index of the field access policy whose mode references to get.
     *	    
     * 	    @return  The mode references of the field access policy.
     */
    std::vector<std::pair<unsigned int, std::string> > getModeReferences(std::string const& fieldName, int accessPolicyIndex) const;

    /*!
     *	Set the mode references of a field access policy.
     *  
     *      @param [in] fieldName               The name of the field, which contains the field access policy.
     *      @param [in] accessPolicyIndex       Index of the field access policy whose mode references to set.
     *      @param [in] newModeRefs             The mode references to set.
     *	    
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool setModeReferences(std::string const& fieldName, int accessPolicyIndex, std::vector<std::pair<unsigned int, std::string> > const& newModeRefs);

    /*!
     *	Get the access policy count of the given field.
     *  
     *      @param [in] fieldName     Description
     *	    
     * 	    @return The number of field access policies in the given field.
     */
    int getAccessPolicyCount(std::string const& fieldName) const;

    /*!
     *	Get the read response of a field access policy as an expression.
     *  
     *      @param [in] fieldName               The name of the field.
     *      @param [in] accessPolicyIndex       Index of the field access policy.
     *
     * 	    @return The read response as an expression.
     */
    std::string getReadResponseExpression(std::string const& fieldName, int accessPolicyIndex) const;

    /*!
     *	Get the read response of a field access policy as a formatted expression.
     *  
     *      @param [in] fieldName               The name of the field.
     *      @param [in] accessPolicyIndex       Index of the field access policy.
     *
     * 	    @return The read response as a formatted expression.
     */
    std::string getReadResponseFormattedExpression(std::string const& fieldName, int accessPolicyIndex) const;

    /*!
     *	Get the evaluated read response value of a field access policy.
     *  
     *      @param [in] fieldName             The name of the field.
     *      @param [in] accessPolicyIndex     Index of the field access policy.
     *      @param [in] baseNumber            Base for displaying the value.
     *	    
     * 	    @return The evaluated read response.
     */
    std::string getReadResponseValue(std::string const& fieldName, int accessPolicyIndex, int baseNumber = 0) const;

    /*!
     *	Set the read response of a selected field access policy.
     *  
     *      @param [in] fieldName             The name of the field.
     *      @param [in] accessPolicyIndex     Index of the field access policy.
     *      @param [in] newReadResponse       The read response to set.
     *	    
     * 	    @return True, if successful, otherwise false.
     */
    bool setReadResponse(std::string const& fieldName, int accessPolicyIndex, std::string const& newReadResponse) const;

    /*!
     *	Get the number of references to a parameter used in the different sub-elements of the field access policy.
     *  
     *      @param [in] fieldName             The name of the field.
     *      @param [in] accessPolicyIndex     Index of the field access policy.
     *      @param [in] valueID               The parameter (ID) to check.
     *	    
     * 	    @return The number of references to a parameter used within the field access policy.
     */
    int getAllReferencesToIdInFieldAccessPolicy(std::string const& fieldName, int accessPolicyIndex, std::string const& valueID) const;

    /*!
     *	Checks if the field access policy given by index has valid mode references.
     *  
     *      @param [in] fieldName               The name of the field.
     *      @param [in] accessPolicyIndex       The index of the field access policy to check.
     *	    
     * 	    @return True, if the field access policy's mode references are valid, otherwise false.
     */
    bool hasValidAccessPolicyModeRefs(std::string const& fieldName, int accessPolicyIndex) const;

    /*!
     *	Copy selected field access policies to clipboard.
     *  
     *      @param [in] fieldName                  The field containing the access policies.
     *      @param [in] selectedAccessPolicies     List containing the indices of the selected access policies.
     */
    void copyFieldAccessPolicies(std::string const& fieldName, std::vector<int> const& selectedAccessPolicies);
    
    /*!
     *	Paste the selected field access policies.
     *  
     *      @param [in] fieldName     The name of the field to paste into.
     *	    
     * 	    @return  The number of pasted field access policies.
     */
    int pasteFieldaccessPolicies(std::string const& fieldName);

    /*!
     *	Set the mode reference interface to be used.
     *  
     *      @param [in] modeRefInterface     The mode reference interface to be used.
     */
    void setModeReferenceInterface(ModeReferenceInterface* modeRefInterface);

    /*!
     *	Get the mode reference interface in use.
     *  
     * 	    @return The mode reference interface.
     */
    ModeReferenceInterface* getModeReferenceInterface() const;

private:

    /*!
     *  Get the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return The selected field.
     */
    QSharedPointer<Field> getField(std::string const& fieldName) const;
    
    /*!
     *  Get the field with the selected name, sliced to a NameGroup.
     *
     *      @param [in] fieldName    Name of the selected field.
     *
     *      @return The sliced field with the selected name.
     */
    QSharedPointer<NameGroup> getItem(std::string const& fieldName) const override;

    /*!
     *  Get the write value constraing of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Write value constraint of the selected field.
     */
    QSharedPointer<WriteValueConstraint> getWriteValueConstraint(std::string const& fieldName, int accessPolicyIndex = -1) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained fields.
    QSharedPointer<QList<QSharedPointer<Field> > > fields_;

    //! Validator for fields.
    QSharedPointer<FieldValidator> validator_;

    //! Validator for field access policies.
    FieldAccessPolicyValidator accessPolicyValidator_;

    //! Interface for accessing resets.
    ResetInterface* subInterface_;

    //! Interface for accessing mode references.
    ModeReferenceInterface* modeReferenceInterface_;
    
};

#endif // REGISTERTABLEMODEL_H
