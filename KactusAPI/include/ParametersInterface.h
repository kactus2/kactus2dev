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

#include <KactusAPI/KactusAPIGlobal.h>

#include <AbstractParameterInterface.h>

class Parameter;

//-----------------------------------------------------------------------------
//! Interface for editing parameters.
//-----------------------------------------------------------------------------
class KACTUS2_API ParametersInterface : public AbstractParameterInterface
{
public:

	/*!
     *  The constructor.
     *
     *      @param [in] validator               Validator for parameters.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     */
    ParametersInterface(QSharedPointer<ParameterValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter);
	
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
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const override final;
    
    /*!
     *  Add a new parameter.
     *
     *      @param [in] row                 Index of the new parameter.
     *      @param [in] newParameterName    New of the new parameter.
     */
    void addParameter(int const& row, std::string const& newParameterName = std::string(""));

    /*!
     *  Remove the selected parameter.
     *
     *      @param [in] parameterName   New of the selected parameter.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeParameter(std::string const& parameterName);

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
    virtual QSharedPointer<Parameter> getParameter(std::string const& parameterName) const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of available parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

#endif // PARAMETERSINTERFACE_H
