//-----------------------------------------------------------------------------
// File: FieldInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.04.2020
//
// Description:
// Interface for editing registers.
//-----------------------------------------------------------------------------

#ifndef FIELDINTERFACE_H
#define FIELDINTERFACE_H

#include <editors/ComponentEditor/common/ParameterizableInterface.h>

class Field;
class FieldValidator;

//-----------------------------------------------------------------------------
//! Interface for editing registers.
//-----------------------------------------------------------------------------
class FieldInterface : public ParameterizableInterface
{

public:

	/*!
	 *  The constructor.
	 */
	FieldInterface();
	
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
     *  Set field validator.
     *
     *      @param [in] validator   Validator for fields.
     */
    void setValidator(QSharedPointer<FieldValidator> validator);

    /*!
     *  Get index of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Index of the selected item.
     */
    virtual int getItemIndex(string const& itemName) const override final;

    /*!
     *  Get name of the indexed item.
     *
     *      @param [in] itemIndex   Index of the selected item.
     *
     *      @return Name of the selected item.
     */
    virtual string getIndexedItemName(int const& itemIndex) const override final;

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
    virtual vector<string> getItemNames() const override final;

    /*!
     *  Set a new name for the selected item.
     *
     *      @param [in] currentName     Name of the selected item.
     *      @param [in] newName         New name for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(string const& currentName, string const& newName) override final;

    /*!
     *  Get the description of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Description of the selected item.
     */
    virtual string getDescription(string const& itemName) const override final;

    /*!
     *  Set a new description for the selected item.
     *
     *      @param [in] itemName        Name of the selected item.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(string const& itemName, string const& newDescription) override final;

    /*!
     *  Get the calculated offset value of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated offset value of the selected field.
     */
    string getOffsetValue(string const& fieldName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted offset expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Formatted offset expression of the selected field.
     */
    string getOffsetFormattedExpression(string const& fieldName) const;

    /*!
     *  Get the offset expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Offset expression of the selected field.
     */
    string getOffsetExpression(string const& fieldName) const;

    /*!
     *  Set a new offset value for the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] newOffset   New offset value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setOffset(string const& fieldName, string const& newOffset);

    /*!
     *  Get the calculated bit width value of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated bit width value of the selected parameter.
     */
    string getWidthValue(string const& fieldName, int const& baseNumber = 0) const;
    
    /*!
     *  Get the formatted bit width expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Formatted bit width expression of the selected field.
     */
    string getWidthFormattedExpression(string const& fieldName) const;

    /*!
     *  Get the bit width expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Bit width expression of the selected field.
     */
    string getWidthExpression(string const& fieldName) const;

    /*!
     *  Set a new bit width value for the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] newWidth    New bit width value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setWidth(string const& fieldName, string const& newWidth);

    /*!
     *  Get the reset values of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Reset values of the selected field.
     */
    string getResets(string const& fieldName) const;

    /*!
     *  Get the tooltip of the reset values of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Tooltip constructed from the reset values of the selected field.
     */
    string getResetsToolTip(string const& fieldName) const;

    /*!
     *  Get the volatile status of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Volatile status of the selected field.
     */
    string getVolatile(string const& fieldName) const;

    /*!
     *  Set the volatile status of the selected field.
     *
     *      @param [in] fieldName       Name of the selected field.
     *      @param [in] newVolatile     The new volatile status of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setVolatile(string const& fieldName, string const& newVolatile);

    /*!
     *  Get the access of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Access of the selected field.
     */
    string getAccess(string const& fieldName) const;

    /*!
     *  Set the access of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] newAccess   The new access of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setAccess(string const& fieldName, string const& newAccess);

    /*!
     *  Get the modified write value of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Modified write value of the selected field.
     */
    string getModifiedWrite(string const& fieldName) const;

    /*!
     *  Set the modified write value of the selected field.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] newModifiedWrite    The new modified write value of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setModifiedWrite(string const& fieldName, string const& newModifiedWrite);

    /*!
     *  Get the read action of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Read action of the selected field.
     */
    string getReadAction(string const& fieldName) const;

    /*!
     *  Set the read action of the selected field.
     *
     *      @param [in] fieldName       Name of the selected field.
     *      @param [in] newReadAction   The new read action of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setReadAction(string const& fieldName, string const& newReadAction);

    /*!
     *  Get the testable string of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Testable string of the selected field.
     */
    string getTestableValue(string const& fieldName) const;

    /*!
     *  Get the testable bool of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Testable bool of the selected field.
     */
    bool getTestableBool(string const& fieldName) const;

    /*!
     *  Set the testable value of the selected field.
     *
     *      @param [in] fieldName       Name of the selected field.
     *      @param [in] newTestable     The new testable value of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setTestable(string const& fieldName, string const& newTestable);

    /*!
     *  Get the test constraint of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Test constraint of the selected field.
     */
    string getTestConstraint(string const& fieldName) const;

    /*!
     *  Set the test constraint of the selected field.
     *
     *      @param [in] fieldName           Name of the selected field.
     *      @param [in] newTestConstraint   The new test constraint of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setTestConstraint(string const& fieldName, string const& newTestConstraint);

    /*!
     *  Get the calculated is present value of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated is present value of the selected field.
     */
    string getIsPresentValue(string const& fieldName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted is present expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Formatted is present expression of the selected field.
     */
    string getIsPresentFormattedExpression(string const& fieldName) const;

    /*!
     *  Get the is present expression of the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return Is present expression of the selected parameter.
     */
    string getIsPresentExpression(string const& fieldName) const;

    /*!
     *  Set a new is present value for the selected field.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newIsPresent    New is present value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setIsPresent(string const& fieldName, string const& newIsPresent);

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const string& itemName, string const&  valueID) const override final;

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
    virtual bool itemHasValidName(string const& itemName) const override final;

    /*!
     *  Check if the selected field has a valid offset.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return True, if the offset is valid, false otherwise.
     */
    bool hasValidOffset(string const& fieldName) const;

    /*!
     *  Check if the selected field has a valid bit width.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return True, if the bit width is valid, false otherwise.
     */
    bool hasValidWidth(string const& fieldName) const;

    /*!
     *  Check if the selected field has valid reset values.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return True, if the reset values are valid, false otherwise.
     */
    bool hasValidResets(string const& fieldName) const;

    /*!
     *  Check if the selected field has a valid is present.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return True, if the is present is valid, false otherwise.
     */
    bool hasValidIsPresent(string const& fieldName) const;

    /*!
     *  Check if the selected field has a valid access.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return True, if the access is valid, false otherwise.
     */
    bool hasValidAccess(string const& fieldName) const;

    /*!
     *  Add a new field.
     *
     *      @param [in] row             Row of the new field.
     *      @param [in] newFieldName    Name of the new field.
     */
    void addField(int const& row, string const& newFieldName = string(""));

    /*!
     *  Remove the selected field.
     *
     *      @param [in] FieldName   Name of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeField(string const& fieldName);

    /*!
     *  Copy the selected fields.
     *
     *      @param [in] selectedRows    Indexes of the selected fields.
     */
    void copyRows(std::vector<int> selectedRows);

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
    std::vector<std::string> getExpressionsInSelectedFields(std::vector<std::string> fieldNames) const;

private:

    /*!
     *  Get the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return The selected field.
     */
    QSharedPointer<Field> getField(string const& fieldName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained fields.
    QSharedPointer<QList<QSharedPointer<Field> > > fields_;

    //! Validator for fields.
    QSharedPointer<FieldValidator> validator_;
};

#endif // REGISTERTABLEMODEL_H
