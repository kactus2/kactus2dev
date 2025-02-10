//-----------------------------------------------------------------------------
// File: ModeConditionParserFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.09.2024
//
// Description:
// Interface for creating and handling expression parsers for mode conditions. The base interface is independent of 
// KactusAPI and should be used in IPXACTmodels validators.
//-----------------------------------------------------------------------------

#ifndef MODECONDITIONPARSERINTERFACE_H
#define MODECONDITIONPARSERINTERFACE_H

#include "ExpressionParserInterface.h"
#include <KactusAPI/KactusAPIGlobal.h>

#include <IPXACTmodels/Component/FieldSlice.h>
#include <IPXACTmodels/Component/PortSlice.h>
#include <IPXACTmodels/Component/Mode.h>

class ExpressionParser;
class ParameterFinder;

class ModeConditionParserBaseInterface : public ExpressionParserInterface
{
public:

	ModeConditionParserBaseInterface() = default;
	~ModeConditionParserBaseInterface() override = default;

    //! No copying, no assignment.
    ModeConditionParserBaseInterface(ModeConditionParserBaseInterface const& other) = delete;
    ModeConditionParserBaseInterface& operator=(ModeConditionParserBaseInterface const& other) = delete;

	/*!
	 *	Set the field slices of the selected mode condition parser.
	 *  
	 *      @param [in] expressionParser     The mode condition parser to set the field slices to.
	 *      @param [in] fieldSlices          The field slices to set.
	 */
	virtual void setFieldSlices(QSharedPointer<ExpressionParser> expressionParser, FieldSlice::List fieldSlices) = 0;
	
	/*!
	 *	Set the port slices of the selected mode condition parser.
	 *
	 *      @param [in] expressionParser     The mode condition parser to set the port slices to.
	 *      @param [in] portSlices           The port slices to set.
	 */
    virtual void setPortSlices(QSharedPointer<ExpressionParser> expressionParser, PortSlice::List portSlices) = 0;

    /*!
     *	Set the available modes to the selected mode condition parser.
     *  
     *      @param [in] expressionParser     The mode condition parser to update.
     *      @param [in] modes                The modes to set.
     */
    virtual void setModes(QSharedPointer<ExpressionParser> expressionParser, Mode::List modes) = 0;
};

class KACTUS2_API ModeConditionParserInterface : public ModeConditionParserBaseInterface
{
public:
    explicit ModeConditionParserInterface(QSharedPointer<ParameterFinder> parameterFinder);
    ~ModeConditionParserInterface() final = default;

    ModeConditionParserInterface(ModeConditionParserInterface const& other) = delete;
    ModeConditionParserInterface& operator=(ModeConditionParserInterface const& other) = delete;

    /*!
     *	Create the expression parser.
     *
     * 	    @return The created expression parser.
     */
    [[nodiscard]] QSharedPointer<ExpressionParser> createParser() final;

    /*!
     *	Set the mode-specific field slices to use while parsing expressions.
     *  
     *      @param [in] expressionParser        The parser whose field slices to set.
     *      @param [in] fieldSlices             The field slices to set.
     */
    void setFieldSlices(QSharedPointer<ExpressionParser> expressionParser, FieldSlice::List fieldSlices) final;

    /*!
     *	Set the mode-specific port slices to use while parsing expressions.
     *
     *      @param [in] expressionParser        The parser whose port slices to set.
     *      @param [in] portSlices              The port slices to set.
     */
    void setPortSlices(QSharedPointer<ExpressionParser> expressionParser, PortSlice::List portSlices) final;

    /*!
     *	Set the available modes to use while parsing expressions.
     *
     *      @param [in] expressionParser        The parser whose modes to set.
     *      @param [in] modes                   The modes to set.
     */
    void setModes(QSharedPointer<ExpressionParser> expressionParser, Mode::List modes) final;

private:

    //! Parameter finder to use in the expression parser.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // MODECONDITIONPARSERINTERFACE_H
