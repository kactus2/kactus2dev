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
    virtual void import(QString const& input, QSharedPointer<Component> targetComponent);

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
     *
     *      @return   The resulting list of declarations.
     */
    QStringList findANSIDeclarations(QString const &input);

    /*!
     *  Finds parameter declarations formated in the old style.
     *
     *      @param [in] input   The input text to parse.
     *
     *      @return   The resulting list of declarations.
     */
    QStringList findOldDeclarations(QString const& input);

    /*!
     *  Parses parameters out of declaration.
     *
     *      @param [in] input   The input text containing a declaration of parameters.
     *
     *      @return The parsed model parameters.
     */
    QList<QSharedPointer<ModelParameter> > parseParameters(QString const &input);

private:

    // Disable copying.
    VerilogParameterParser(VerilogParameterParser const& rhs);
    VerilogParameterParser& operator=(VerilogParameterParser const& rhs);

    /*!
     *  Finds parameter declarations from the inspected string with provided rule.
     *
     *      @param [in] declarationRule   The rule for finding a parameter.
     *      @param [in] inspect           The input text containing the declarations.
     *
     *      @return   The resulting list of declarations.
     */
    QStringList findDeclarations(QRegularExpression const& declarationRule, QString const& inspect);

    /*!
     *  Culls multi line comments and stray single line comments out of the input text.
     *
     *      @param [in] inspect   The input text to parse.
     *
     *      @return   The input without comments.
     */
    QString cullStrayComments(QString const& inspect);

    /*!
     *  Tries to parse the type of the declared parameters.
     *
     *      @param [in] input   The input text containing a declaration of parameters.
     *
     *      @return   The parsed type.
     */
     QString parseType(QString const& input);
     
     /*!
      *  Parses the left value of bit width of the parameter.
      *
      *     @param [in] declaration    The parameter declaration to parse.
      *
      *     @return The left value of the parameters bit width vector.
      */
     QString parseBitWidthLeft(QString const& declaration);

     /*!
      *  Parses the right value of bit width of the parameter.
      *
      *      @param [in] declaration    The parameter declaration to parse.
      *
      *      @return The right value of the parameters bit width vector.
      */
     QString parseBitWidthRight(QString const& declaration);

     /*!
      *  Parses the left bound for the parameter array.
      *
      *      @param [in] declaration   The parameter declaration to parse.
      *
      *      @return The left array bound.
      */
     QString parseArrayLeft(QString const& declaration);
     
     /*!
      *  Parses the right bound for the parameter array.
      *
      *      @param [in] declaration   The parameter declaration to parse.
      *
      *      @return The right array bound.
      */
     QString parseArrayRight(QString const& declaration);

    /*!
     *  Tries to parse the descriptions of the declared parameters.
     *
     *      @param [in] input   The input text containing a declaration of parameters.
     *
     *      @return   The parsed description.
     */
     QString parseDescription(QString const& input);

     /*!
      *  Copies the model parameter ids from the component to the parsed model parameters.
      *
      *      @param [in] parsedParameters   The parsed model parameters.
      *      @param [in] targetComponent    The component to import to.
      */
     void copyIdsFromOldModelParameters(QList<QSharedPointer<ModelParameter> > parsedParameters,
         QSharedPointer<Component> targetComponent);

     /*!
      *  Replaces the referenced model parameter names with their ids in model parameter values.
      *
      *      @param [in] targetComponent   The component whose model parameter values to replace.
      */
     void replaceNamesReferencesWithIds(QSharedPointer<Component> targetComponent);

     /*!
      *  Replaces macro uses in parameter with corresponding id references.
      *
      *      @param [in] parameter          The parameter whose macro uses to replace.
      *      @param [in] targetComponent    The containing component.
      */
     void replaceMacroUsesWithParameterIds(QSharedPointer<ModelParameter> parameter, 
         QSharedPointer<Component> targetComponent) const;

     /*!
      *  Replaces a name reference in an expression with id.
      *  Increases reference count of the referenced parameter accordingly.
      *
      *      @param [in] expression   The expression to replace the references in.
      *      @param [in] namePattern  The pattern for name reference search.
      *      @param [in] referenced   The parameter that matches the name pattern.
      *
      *      @return The expression where names have been replaced with the id of the referenced parameter.
      */
     QString replaceNameWithId(QString const& expression, QRegularExpression& namePattern, 
         QSharedPointer<Parameter> referenced) const;
     
     /*!
      *  Replaces name references uses in parameter with corresponding id references.
      *
      *      @param [in] parameter          The parameter whose name references to replace.
      *      @param [in] targetComponent    The containing component.
      */
     void replaceNameReferencesWithModelParameterIds(QSharedPointer<ModelParameter> parameter, 
         QSharedPointer<Component> targetComponent) const;

     //-----------------------------------------------------------------------------
     // Data.
     //-----------------------------------------------------------------------------

    //! The highlighter to use.
    Highlighter* highlighter_;

};

#endif // VERILOGPARAMETERPARSER_H
