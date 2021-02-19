//-----------------------------------------------------------------------------
// File: ModuleParameterInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.09.2020
//
// Description:
// Interface for editing module parameters.
//-----------------------------------------------------------------------------

#ifndef MODULEPARAMETERINTERFACE_H
#define MODULEPARAMETERINTERFACE_H

#include <editors/ComponentEditor/parameters/AbstractParameterInterface.h>

class ComponentInstantiationParameterFinder;
class ComponentInstantiation;
class ModuleParameter;

//-----------------------------------------------------------------------------
//! Interface for editing module parameters.
//-----------------------------------------------------------------------------
class ModuleParameterInterface : public AbstractParameterInterface
{
public:

    /*!
     *  The constructor.
     *
     *      @param [ni] validator               Validator for parameters.
     *      @param [ni] expressionParser        Parser for expressions.
     *      @param [ni] expressionFormatter     Formatter for expressions.
     *      @param [ni] parameterFinder         Parameter finder for component instantiation parameters.
     */
    ModuleParameterInterface(QSharedPointer<ParameterValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ComponentInstantiationParameterFinder> parameterFinder);

    /*!
     *  The destructor.
     */
    virtual ~ModuleParameterInterface() = default;

    /*!
     *  Set available module parameters.
     *
     *      @param [in] instantiation   Component instantiation containing the new module parameters.
     */
    void setModuleParameters(QSharedPointer<ComponentInstantiation> instantiation);

    /*!
     *  Get index of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Index of the selected item.
     */
    virtual int getItemIndex(std::string const& itemName) const override;

    /*!
     *  Get name of the indexed item.
     *
     *      @param [in] itemIndex   Index of the selected item.
     *
     *      @return Name of the selected item.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const override;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const override;

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const override;

    /*!
     *  Get the data type of the selected module parameter.
     *
     *      @param [in] parameterName   Name of the selected module parameter.
     *
     *      @return Data type of the selected module parameter.
     */
    std::string getDataType(std::string const& parameterName) const;
    
    /*!
     *  Set a new data type for the selected module parameter.
     *
     *      @param [in] parameterName   Name of the selected module parameter.
     *      @param [in] newDataType     New data type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDataType(std::string const& parameterName, std::string const& newDataType);

    /*!
     *  Get the usage type of the selected module parameter.
     *
     *      @param [in] parameterName   Name of the selected module parameter.
     *
     *      @return Usage type of the selected module parameter.
     */
    std::string getUsageType(std::string const& parameterName) const;

    /*!
     *  Set a new usage type for the selected module parameter.
     *
     *       @param [in] parameterName   Name of the selected module parameter.
     *       @param [in] newUsageType    New usage type.
     *
     *       @return True, if successful, false otherwise.
     */
    bool setUsageType(std::string const& parameterName, std::string const& newUsageType);

    /*!
     *  Add a new module parameter.
     *
     *      @param [in] row                 Index of the new parameter.
     *      @param [in] newParameterName    New of the new parameter.
     */
    void addModuleParameter(int const& row, std::string const& newParameterName = std::string(""));

    /*!
     *  Remove the selected module parameter.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeModuleParameter(std::string const& parameterName);

    //! No copying. No assignment.
    ModuleParameterInterface(const ModuleParameterInterface& other) = delete;
    ModuleParameterInterface& operator=(const ModuleParameterInterface& other) = delete;

private:

    /*!
     *  Get the selected parameter.
     *
     *      @param [in] parameterName   Name of the selected parameter.
     *
     *      @return The selected parameter.
     */
    virtual QSharedPointer<Parameter> getParameter(std::string const& parameterName) const override final;

    /*!
     *  Get the selected module parameter.
     *
     *      @param [in] parameterName   Name of the selected module parameter.
     *
     *      @return The selected module parameter.
     */
    QSharedPointer<ModuleParameter> getModuleParameter(std::string const& parameterName) const;
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of available module parameters.
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters_;

    //! Parameter finder for component instantiation parameters.
    QSharedPointer<ComponentInstantiationParameterFinder> parameterFinder_;
};

#endif // MODULEPARAMETERINTERFACE_H
