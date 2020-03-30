//-----------------------------------------------------------------------------
// File: ParametersInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.03.2020
//
// Description:
// Interface for editing parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERSINTERFACE_H
#define PARAMETERSINTERFACE_H

#include <editors/ComponentEditor/common/ParameterizableInterface.h>

class Choice;
class Component;
class ModuleParameter;
class Parameter;
class ParameterValidator;

//-----------------------------------------------------------------------------
//! Interface for editing parameters.
//-----------------------------------------------------------------------------
class ParametersInterface: public ParameterizableInterface
{
public:

	/*!
     *  The constructor.
     */
	ParametersInterface();
	
	/*!
     *  The destructor.
     */
    virtual ~ParametersInterface() = default;

    /*!
     *  Set available parameters.
     *
     *      @param [in] newParameters   The new parameters.
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

    /*!
     *  Set available module parameters.
     *
     *      @param [in] newParameters   The new module parameters.
     */
    void setModuleParameters(QSharedPointer<QList<QSharedPointer<ModuleParameter> > > newParameters);

    /*!
     *  Set available choices.
     *
     *      @param [in] newChoices  The new choices.
     */
    void setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > newChoices);

    /*!
     *  Set parameter validator.
     *
     *      @param [in] validator   Validator for parameters.
     */
    void setValidator(QSharedPointer<ParameterValidator> validator);

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
     *      @return True, if all the items are valid, false otherwise.
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
     *  Get the display name of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Name of the selected parameter.
     */
    string getDisplayName(string const& parameterName) const;

    /*!
     *  Set the display name of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newDisplayName  New display name for the parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDisplayName(string const& parameterName, string const& newDisplayName);

    /*!
     *  Get the type of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Type of the selected parameter.
     */
    string getType(string const& parameterName) const;

    /*!
     *  Set the type of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newType         New type for the parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setType(string const& parameterName, string const& newType);

    /*!
     *  Get the calculated bit width left value of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] baseNumber      Base for displaying the value.
     *
     *      @return Calculated bit width left value of the selected parameter.
     */
    string getBitWidthLeftValue(string const& parameterName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted bit width left expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Formatted bit width left expression of the selected parameter.
     */
    string getBitWidthLeftFormattedExpression(string const& parameterName) const;

    /*!
     *  Get the bit width left expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Bit width left expression of the selected parameter.
     */
    string getBitWidthLeftExpression(string const& parameterName) const;

    /*!
     *  Set a new bit width left value for the selected parameter.
     *
     *      @param [in] parameterName       Name of the selected parameter.
     *      @param [in] newBitWidthLeft     New bit width left value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBitWidthLeft(string const& parameterName, string const& newBitWidthLeft);

    /*!
     *  Get the calculated bit width right value of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] baseNumber      Base for displaying the value.
     *
     *      @return Calculated bit width right value of the selected port.
     */
    string getBitWidthRightValue(string const& parameterName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted bit width right expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Formatted bit width right expression of the selected parameter.
     */
    string getBitWidthRightFormattedExpression(string const& parameterName) const;

    /*!
     *  Get the bit width right expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Bit width right expression of the selected parameter.
     */
    string getBitWidthRightExpression(string const& parameterName) const;

    /*!
     *  Set a new bit width right value for the selected parameter.
     *
     *      @param [in] parameterName       Name of the selected parameter.
     *      @param [in] newBitWidthRight    New bit width right value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBitWidthRight(string const& parameterName, string const& newBitWidthRight);

    /*!
     *  Get the minimum value the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Minimum value of the selected parameter.
     */
    string getMinimum(string const& parameterName) const;

    /*!
     *  Set the minimum value for the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newMinimum      New minimum value of the parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMinimum(string const& parameterName, string const& newMinimum);

    /*!
     *  Get the maximum value the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Maximum value of the selected parameter.
     */
    string getMaximum(string const& parameterName) const;

    /*!
     *  Set the maximum value for the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newMaximum      New maximum value of the parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMaximum(string const& parameterName, string const& newMaximum);

    /*!
     *  Get the choice reference of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Choice reference of the selected parameter.
     */
    string getChoice(string const& parameterName) const;

    /*!
     *  Set the choice reference for the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newChoice       The new choice reference.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setChoice(string const& parameterName, string const& newChoice);

    /*!
     *  Get the calculated value of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] baseNumber      Base for displaying the value.
     *
     *      @return Calculated value of the selected port.
     */
    string getValue(string const& parameterName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted value expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Formatted value expression of the selected parameter.
     */
    string getValueFormattedExpression(string const& parameterName) const;

    /*!
     *  Get the value expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Value expression of the selected parameter.
     */
    string getValueExpression(string const& parameterName) const;

    /*!
     *  Set a new value for the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newValue        New value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setValue(string const& parameterName, string const& newValue);

    /*!
     *  Get the resolve of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Resolve of the selected parameter.
     */
    string getResolve(string const& parameterName) const;

    /*!
     *  Set a resolve for the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newResolve      New resolve.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setResolve(string const& parameterName, string const& newResolve);

    /*!
     *  Get the calculated array left value of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] baseNumber      Base for displaying the value.
     *
     *      @return Calculated array left value of the selected port.
     */
    string getArrayLeftValue(string const& parameterName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted array left expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Formatted array left expression of the selected parameter.
     */
    string getArrayLeftFormattedExpression(string const& parameterName) const;

    /*!
     *  Get the array left expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Array left expression of the selected parameter.
     */
    string getArrayLeftExpression(string const& parameterName) const;

    /*!
     *  Set a new array left value for the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newArrayLeft    New array left value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setArrayLeft(string const& parameterName, string const& newArrayLeft);

    /*!
     *  Get the calculated array right value of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] baseNumber      Base for displaying the value.
     *
     *      @return Calculated array right value of the selected port.
     */
    string getArrayRightValue(string const& parameterName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted array right expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Formatted array right expression of the selected parameter.
     */
    string getArrayRightFormattedExpression(string const& parameterName) const;

    /*!
     *  Get the array right expression of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Array right expression of the selected parameter.
     */
    string getArrayRightExpression(string const& parameterName) const;

    /*!
     *  Set a new array right value for the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newArrayRight   New array right value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setArrayRight(string const& parameterName, string const& newArrayRight);

    /*!
     *  Get the ID of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return ID of the selected parameter.
     */
    string getID(string const& parameterName) const;

    /*!
     *  Set a new ID for the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newID           New ID.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setID(string const& parameterName, string const& newID);

    /*!
     *  Get the usage count of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return Usage count of the selected parameter.
     */
    int getUsageCount(string const& parameterName) const;

    /*!
     *  Set a new usage count for the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *      @param [in] newUsageCount   New usage count.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setUsageCount(string const& parameterName, int const& newUsageCount);

    /*!
     *  Get the data type of the selected module parameter.
     *
     *      @param [in] parameterName   Name of the selected module parameter.
     *
     *      @return Data type of the selected module parameter.
     */
    string getDataType(string const& parameterName) const;

    /*!
     *  Set a new data type for the selected module parameter.
     *
     *      @param [in] parameterName   Name of the selected module parameter.
     *      @param [in] newDataType     New data type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDataType(string const& parameterName, string const& newDataType);

    /*!
     *  Get the usage type of the selected module parameter.
     *
     *      @param [in] parameterName   Name of the selected module parameter.
     *
     *      @return Usage type of the selected module parameter.
     */
    string getUsageType(string const& parameterName) const;

    /*!
     *  Set a new usage type for the selected module parameter.
     *
     *      @param [in] parameterName   Name of the selected module parameter.
     *      @param [in] newUsageType    New usage type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setUsageType(string const& parameterName, string const& newUsageType);

    /*!
     *  Increase the usage count of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool increaseUsageCount(string const& parameterName);

    /*!
     *  Decrease the usage count of the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool reduceUsageCount(string const& parameterName);

    /*!
     *  Add a new parameter.
     *
     *      @param [in] row                 Index of the new parameter.
     *      @param [in] newParameterName    New of the new parameter.
     */
    void addParameter(int const& row, string const& newParameterName = string(""));

    /*!
     *  Add a new module parameter.
     *
     *      @param [in] row                 Index of the new parameter.
     *      @param [in] newParameterName    New of the new parameter.
     */
    void addModuleParameter(int const& row, string const& newParameterName = string(""));

    /*!
     *  Remove the selected parameter.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeParameter(string const& parameterName);

    /*!
     *  Remove the selected module parameter.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeModuleParameter(string const& parameterName);

    /*!
     *  Check if the selected parameter has a valid type.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if the type is valid, false otherwise.
     */
    bool hasValidType(string const& parameterName) const;

    /*!
     *  Check if the selected parameter has a valid bit width.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if the bit width is valid, false otherwise.
     */
    bool hasValidBitWidth(string const& parameterName) const;

    /*!
     *  Check if the selected parameter has a valid minimum.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if the minimum is valid, false otherwise.
     */
    bool hasValidMinimum(string const& parameterName) const;

    /*!
     *  Check if the selected parameter has a valid maximum.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if the maximum is valid, false otherwise.
     */
    bool hasValidMaximum(string const& parameterName) const;

    /*!
     *  Check if the selected parameter has a valid choice.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if the choice is valid, false otherwise.
     */
    bool hasValidChoice(string const& parameterName) const;

    /*!
     *  Check if the selected parameter has a valid value.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if the value is valid, false otherwise.
     */
    bool hasValidValue(string const& parameterName) const;

    /*!
     *  Check if the selected parameter has a valid resolve.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if the resolve is valid, false otherwise.
     */
    bool hasValidResolve(string const& parameterName) const;

    /*!
     *  Check if the selected parameter has valid array values.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if the array values are valid, false otherwise.
     */
    bool hasValidArrayValues(string const& parameterName) const;

    //! No copying. No assignment.
    ParametersInterface(const ParametersInterface& other) = delete;
    ParametersInterface& operator=(const ParametersInterface& other) = delete;

private:
    
    /*!
     *  Get the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return The selected parameter.
     */
    QSharedPointer<Parameter> getParameter(string const& parameterName) const;

    /*!
     *  Get the selected module parameter.
     *
     *      @param [in] parameterName   Name of the selected module parameter.
     *
     *      @return The selected module parameter.
     */
    QSharedPointer<ModuleParameter> getModuleParameter(string const& parameterName) const;

    /*!
     *  Evaluate the value of the selected parameter.
     *
     *      @param [in] parameter   The selected parameter.
     *
     *      @return Evaluated value of the selected parameter.
     */
    QString evaluateValue(QSharedPointer<Parameter> parameter) const;

    /*!
     *  Find the selected choice.
     *
     *      @param [in] choiceName  Name of the selected choice.
     *
     *      @return The selected choice.
     */
    QSharedPointer<Choice> findChoice(QString const& choiceName) const;

    /*!
     *  Changes the array value to match the current choice.
     *
     *      @param [in] choice          The currently active choice.
     *      @param [in] arrayValue      The array value to be changed.
     *
     *      @return Array whose values have been changed to match the currently selected choice.
     */
    QString matchArrayValuesToSelectedChoice(QSharedPointer<Choice> choice, QString const& arrayValue) const;

    /*!
     *  Finds a human-readable value to display for a given enumeration.
     *
     *      @param [in] choice              The choice whose enumeration to find.
     *      @param [in] enumerationValue    The value used to search the enumeration.
     *
     *      @return A value for the enumeration to display.
     */
    QString findDisplayValueForEnumeration(QSharedPointer<Choice> choice, QString const& enumerationValue) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of available parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
    
    //! List of available module parameters.
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters_;

    //! List of available choices.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;

    //! The parameter validator.
    QSharedPointer<ParameterValidator> parameterValidator_;
};

#endif // PARAMETERSINTERFACE_H
