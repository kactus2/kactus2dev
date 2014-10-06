//-----------------------------------------------------------------------------
// File: VerilogParameterParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 25.09.2014
//
// Description:
// Parser for Verilog paramaters.
//-----------------------------------------------------------------------------

#ifndef VERILOGPARAMETERPARSER_H
#define VERILOGPARAMETERPARSER_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QSharedPointer>
#include <QString>
#include <IPXACTmodels/modelparameter.h>

#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>

class Component;

//-----------------------------------------------------------------------------
//! Parser for Verilog ports.
//-----------------------------------------------------------------------------
class VerilogParameterParser : public HighlightSource
{
public:

    //! The constructor.
    VerilogParameterParser();

    //! The destructor.
    ~VerilogParameterParser();

    /*!
     *  Runs the port parsing for the given input and adds the parsed ports to the given component.
     *
     *      @param [in] input               The input text to parse.
     *      @param [in] targetComponent     The component to add all the imported ports to.
     */
    virtual void runParser(QString const& input, QSharedPointer<Component> targetComponent);
    
    /*!
     *  Sets the given highlighter to be used by the generic parser.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);

    /*!
     *  Finds parameter declarations formated in ANSI-style.
     *
     *      @param [in] input   The input text to parse.
     *      @param [in] declarations   The resulting list of declarations.
     */
     void findANSIDeclarations(QString const &input, QStringList& declarations);

    /*!
     *  Finds parameter declarations formated in the old style.
     *
     *      @param [in] input   The input text to parse.
     *      @param [in] declarations   The resulting list of declarations.
     */
     void findOldDeclarations(QString const &input, QStringList& declarations);

    /*!
     *  Parses parameters out of declaration.
     *
     *      @param [in] input   The input text containing a declaration of parameters.
     *      @param [in] parameters   The parsed model parameters.
     */
     void parseParameters(QString const &input, QList<QSharedPointer<ModelParameter>>& parameters);

private:
    /*!
     *  Finds parameter declarations from the inspected string with provided rule.
     *
     *      @param [in] declarRule   The input text containing the declarations.
     *      @param [in] inspect   The input text containing the declarations.
     *      @param [in] declarations   The resulting list of declarations.
     */
     void findDeclarations(QRegExp &declarRule, QString &inspect, QStringList &declarations);

    /*!
     *  Culls multi line comments and stray single line comments out of the input text.
     *
     *      @param [in] inspect   The input text to parse.
     */
     void cullStrayComments(QString &inspect);

    /*!
     *  Tries to parse the type of the declared parameters.
     *
     *      @param [in] input   The input text containing a declaration of parameters.
     */
     QString parseType(QString const & input);

    /*!
     *  Tries to parse the descriptions of the declared parameters.
     *
     *      @param [in] input   The input text containing a declaration of parameters.
     */
     QString parseDescription(QString const &input);

    // Disable copying.
    VerilogParameterParser(VerilogParameterParser const& rhs);
    VerilogParameterParser& operator=(VerilogParameterParser const& rhs);

    //! The highlighter to use.
    Highlighter* highlighter_;

};

#endif // VERILOGPARAMETERPARSER_H
