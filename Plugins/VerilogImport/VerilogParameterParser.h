//-----------------------------------------------------------------------------
// File: VerilogParameterParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 25.09.2014
//
// Description:
// Parser for Verilog parameters.
//-----------------------------------------------------------------------------

#ifndef VERILOGPARAMETERPARSER_H
#define VERILOGPARAMETERPARSER_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QSharedPointer>
#include <QString>
#include <IPXACTmodels/common/ModuleParameter.h>

#include <KactusAPI/include/HighlightSource.h>

#include "IPXACTmodels/Component/ComponentInstantiation.h"

class Component;

//-----------------------------------------------------------------------------
//! Parser for Verilog parameters.
//-----------------------------------------------------------------------------
class VerilogParameterParser : public HighlightSource
{
public:

    //! The constructor.
    VerilogParameterParser();

    //! The destructor.
    virtual ~VerilogParameterParser();

    // Disable copying.
    VerilogParameterParser(VerilogParameterParser const& rhs) = delete;
    VerilogParameterParser& operator=(VerilogParameterParser const& rhs) = delete;

    /*!
     *  Runs the port parsing for the given input and adds the parsed ports to the given component.
     *
     *    @param [in] componentDeclaration            The selected component declaration from the input text.
     *    @param [in] targetComponent                 The component to add all the imported parameters to.
     *    @param [in] targetComponentInstantiation    The instantiation to add all the imported parameters to.
     */
    virtual void import(QString const& componentDeclaration, QSharedPointer<Component> targetComponent,
        QSharedPointer<ComponentInstantiation> targetComponentInstantiation);

    /*!
     *  Sets the given highlighter to be used by the generic parser.
     *
     *    @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);

    /*!
     *  Finds parameter declarations in the given input.
     *
     *    @param [in] input   The input text to parse.
     *
     *    @return   The resulting list of parameter declarations.
     */
    QStringList findDeclarations(QString const& input);

    /*!
     *  Parses parameters out of declaration.
     *
     *    @param [in] input   The input text containing a declaration of parameters.
     *
     *    @return The parsed model parameters.
     */
    QList<QSharedPointer<ModuleParameter> > parseParameters(QString const &input);

private:

    /*!
     * Find the section in the input containing the parameter declarations.
     *
     *    @param [in] input  The input text to search.
     *
     *    @return The section of the text containing parameter declarations.
     */
     QString findParameterSection(QString const &input);

    /*!
     *  Finds parameter declarations from the given string.
     *
     *    @param [in] componentDeclaration    The component declaration containing the parameters.
     *    @param [in] parameterArea           The input text containing the declarations.     
     *
     *    @return   The resulting list of declarations.
     */
     QStringList findParameterDeclarations(QString const& componentDeclaration, QString const& parameterArea);

    /*!
     *  Create an IP-XACT type for the parameter according to the given data type.
     *
     *    @param [in] dataType    The selected data type.
     *
     *    @return The type matching the data type, or none if data type does not match any IP-XACT type.
     */
    QString createTypeFromDataType(QString const& dataType);

    /*!
     *  Tries to parse the type of the declared parameters.
     *
     *    @param [in] input   The input text containing a declaration of parameters.
     *
     *    @return   The parsed type.
     */
     QString parseType(QString const& input);
     
     /*!
      *  Parses the left value of bit width of the parameter.
      *
      *    @param [in] declaration    The parameter declaration to parse.
      *
      *    @return The left value of the parameters bit width vector.
      */
     QString parseBitWidthLeft(QString const& declaration);

     /*!
      *  Parses the right value of bit width of the parameter.
      *
      *    @param [in] declaration    The parameter declaration to parse.
      *
      *    @return The right value of the parameters bit width vector.
      */
     QString parseBitWidthRight(QString const& declaration);

     /*!
      *  Parses the left bound for the parameter array.
      *
      *    @param [in] declaration   The parameter declaration to parse.
      *
      *    @return The left array bound.
      */
     QString parseArrayLeft(QString const& declaration);
     
     /*!
      *  Parses the right bound for the parameter array.
      *
      *    @param [in] declaration   The parameter declaration to parse.
      *
      *    @return The right array bound.
      */
     QString parseArrayRight(QString const& declaration);

    /*!
     *  Tries to parse the descriptions of the declared parameters.
     *
     *    @param [in] input   The input text containing a declaration of parameters.
     *
     *    @return   The parsed description.
     */
     QString parseDescription(QString const& input);

     /*!
      *  Copies the model parameter ids from the component to the parsed model parameters.
      *
      *    @param [in] parsedParameters   The parsed model parameters.
      *    @param [in] targetComponent    The component to import to.
      */
     void copyIdsFromOldModelParameters(QList<QSharedPointer<ModuleParameter> > parsedParameters,
         QSharedPointer<ComponentInstantiation> targetComponentInstantiation);

     /*!
      *  Replaces the referenced model parameter names with their ids in model parameter values.
      *
      *    @param [in] targetComponent   The component whose model parameter values to replace.
      */
     void replaceNamesReferencesWithIds(QList<QSharedPointer<ModuleParameter> > parsedParameters,
         QSharedPointer<Component> targetComponent,
         QSharedPointer<ComponentInstantiation> targetComponentInstantiation);

     /*!
      *  Replaces macro uses in parameter with corresponding id references.
      *
      *    @param [in] parameter          The parameter whose macro uses to replace.
      *    @param [in] targetComponent    The containing component.
      */
     void replaceNameReferencesWithParameterIds(QSharedPointer<Parameter> parameter, 
         QSharedPointer<Component> targetComponent) const;

     /*!
      *  Replaces a name reference in an expression with id.
      *  Increases reference count of the referenced parameter accordingly.
      *
      *    @param [in] expression   The expression to replace the references in.
      *    @param [in] namePattern  The pattern for name reference search.
      *    @param [in] referenced   The parameter that matches the name pattern.
      *
      *    @return The expression where names have been replaced with the id of the referenced parameter.
      */
     QString replaceNameWithId(QString const& expression, QRegularExpression& namePattern, 
         QSharedPointer<Parameter> referenced) const;

     //-----------------------------------------------------------------------------
     // Data.
     //-----------------------------------------------------------------------------

    //! The highlighter to use.
    Highlighter* highlighter_;

};

#endif // VERILOGPARAMETERPARSER_H
