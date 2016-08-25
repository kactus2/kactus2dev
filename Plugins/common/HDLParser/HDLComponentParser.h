//-----------------------------------------------------------------------------
// File: HDLComponentParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 11.08.2016
//
// Description:
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------

#ifndef HDLCOMPONENTPARSER_H
#define HDLCOMPONENTPARSER_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Port.h>

#include <Plugins/VerilogGenerator/PortSorter/PortSorter.h>
#include <Plugins/VerilogGenerator/ModelParameterVerilogWriter/ModelParameterVerilogWriter.h>
#include <Plugins/VerilogGenerator/common/Writer.h>
#include <Plugins/VerilogGenerator/common/WriterGroup.h>

#include <Plugins/VerilogGenerator/TextBodyWriter/TextBodyWriter.h>
#include <Plugins/common/HDLParser/HDLParserCommon.h>

#include <QSharedPointer>
#include <QTextStream>
#include <QList>

class ExpressionParser;
class ExpressionFormatter;
class AddressBlock;

//-----------------------------------------------------------------------------
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------
class HDLComponentParser
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component to write to Verilog.
     *      @param [in] activeView              The active view name for the component.
	 *      @param [in] sorter                  Sorter for the ports in the component.
     *      @param [in] expressionParser		Parser for writing expressions.
     *      @param [in] expressionFormatter     Formatter for writing expressions.
	 */
	HDLComponentParser(QSharedPointer<Component> component, QSharedPointer<View> activeView,
		QSharedPointer<const PortSorter> sorter, QSharedPointer<ExpressionFormatter> expressionFormatter);

	//! The destructor.
	~HDLComponentParser();

    QSharedPointer<GenerationComponent> parseComponent() const;

private:
	// Disable copying.
	HDLComponentParser(HDLComponentParser const& rhs);
	HDLComponentParser& operator=(HDLComponentParser const& rhs);
    
	void parseRegisters(QSharedPointer<GenerationComponent> target) const;

    void parseAddressBlock(QSharedPointer<AddressBlock> ab, QSharedPointer<GenerationRemap> target) const;

     /*!
      *  Culls parameter declarations for the module.
      */
    void parseParameterDeclarations(QSharedPointer<GenerationComponent> target) const;
	
    /*!
     *  Sorts list of module parameters based on their interdependencies.
     *
     *      @param [in] currentInsta			The component instantiation, which module parameters are referred.
     *      @param [out] parametersToWrite      The list containing parameters, that will be sorted.
     */
	void sortParameters(QList<QSharedPointer<Parameter> >& parameters,
		QList<QSharedPointer<Parameter> >& parametersToWrite) const;

    void parseRemapStates(QSharedPointer<GenerationComponent> target) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component to write to Verilog module.
    QSharedPointer<Component> component_;

    //! The component active view.
    QSharedPointer<View> activeView_;

    //! Sorter for the ports of the component.
    QSharedPointer<const PortSorter> sorter_;

    //! Writers for the inner elements e.g. wires and subcomponent instances.
    QList<QSharedPointer<Writer> > childWriters_;

    //! The formatter for expressions.
	QSharedPointer<ExpressionFormatter> formatter_;

	//! The implementation, which is essentially the user-written things in the module.
	QSharedPointer<TextBodyWriter> implementation_;

	//! The extra stuff that comes after the written module.
    QSharedPointer<TextBodyWriter> postModule_;
};

#endif // HDLCOMPONENTPARSER_H
