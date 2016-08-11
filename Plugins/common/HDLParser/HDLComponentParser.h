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

//-----------------------------------------------------------------------------
//! Class for writing a component as a Verilog module.
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

    /*!
     *  Writes a single parameter declaration.
     *
     *      @param [in] outputStream    The output to write to.
     *      @param [in] parameter       The parameter to write.
     *      @param [in] isLast          Indicates if the parameter is the last to write.
     */
    void writeParameter(QTextStream& outputStream, QSharedPointer<Parameter> parameter, bool isLast) const;

    /*!
     *  Writes the module port declaration.
     *
     *      @param [in] outputStream   The output to write to.
     */
    void writePortDeclarations(QTextStream& outputStream) const;
    
    /*!
     *  Writes a single port declaration.
     *
     *      @param [in] outputStream    The output to write to.
     *      @param [in] port            The port to write.
     *      @param [in] isLast          Indicates if the port is the last to write.
     */
    void writePort(QTextStream& outputStream, QSharedPointer<Port> port, bool isLast) const;

    /*!
     *  Writes a bus interface introduction.
     *
     *      @param [in] interfaceName           The name of the bus interface to introduce.
     *      @param [in] previousInterfaceName   The name of the previously introduced bus interface.
     *      @param [in] outputStream            The output to write to.
     */
    void writeInterfaceIntroduction(QString const& interfaceName, QString& previousInterfaceName,
        QTextStream& outputStream ) const;

    /*!
     *  Writes the inserted wires and component instances of the top-level component.
     *
     *      @param [in] outputStream   The output to write to.
     */
    void writeInternalWiresAndComponentInstances(QTextStream& outputStream) const;

    /*!
     *  Writes the ending clause for the module.
     *
     *      @param [in] outputStream   The output to write to.
     */
    void writeModuleEnd(QTextStream& outputStream) const;

    /*!
     *  Gets the indentation to use.
     *
     *      @return The indentation to use.
     */
    QString indentation() const;

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
