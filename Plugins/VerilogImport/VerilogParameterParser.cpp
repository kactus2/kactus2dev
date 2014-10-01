//-----------------------------------------------------------------------------
// File: VerilogParameterParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 26.09.2014
//
// Description:
// Parser for Verilog parameters.
//-----------------------------------------------------------------------------

#include "VerilogParameterParser.h"
#include "VerilogSyntax.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/model.h>

#include <QString>
#include <QRegExp>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::VerilogParameterParser()
//-----------------------------------------------------------------------------
VerilogParameterParser::VerilogParameterParser()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::~VerilogParameterParser()
//-----------------------------------------------------------------------------
VerilogParameterParser::~VerilogParameterParser()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::runParser()
//-----------------------------------------------------------------------------
void VerilogParameterParser::runParser(QString const& input, QSharedPointer<Component> targetComponent)
{
    // Find parameter declarations. Try both formats, as we cannot know which one is used.
    QStringList declarations;
    findANSIDeclarations(input, declarations);
    findOldDeclarations(input, declarations);

    // Parse model parameters out of declarations.
    QList<QSharedPointer<ModelParameter>> parameters;

    foreach ( QString declaration, declarations )
    {
        parseParameters(declaration, parameters);
    }

    // Finally, add the obtained parameters to the target component.
    foreach ( QSharedPointer<ModelParameter> parameter, parameters )
    {
        targetComponent->getModel()->addModelParameter(parameter);
    }

    return;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findANSIDeclarations()
//-----------------------------------------------------------------------------
void VerilogParameterParser::findANSIDeclarations(QString const &input, QStringList& declarations)
{
    // In the ANSI style parameter declarations the parameters are contained WITHIN the module header.
    int endIndex = VerilogSyntax::MODULE_BEGIN.indexIn(input) + VerilogSyntax::MODULE_BEGIN.matchedLength();

    // And that is why only the module header is inspected int he parsing.
    QString inspect = input.mid( 0, endIndex );

    cullStrayComments(inspect);

    // May have "parameter", may have "signed + range" OR one of those OR type. Is composed of name+value pairs,
    // possibly multiple times separated by comma. May have comments between. May have comma or comment the end.
    QRegExp declarRule(QString("(parameter)?\\s*(((signed)?\\s*(\\w+" + VerilogSyntax::RANGE + ")?)|(\\w+))\\s*" +
        VerilogSyntax::NAME_VALUE + "(?:\\s*,\\s*(" + VerilogSyntax::COMMENT + ")?\\s*" + VerilogSyntax::NAME_VALUE
        + ")*((\\s*,\\s*)|(\\s*))(" + VerilogSyntax::COMMENT + ")?"), Qt::CaseInsensitive);

    findDeclarations(declarRule, inspect, declarations);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findOldDeclarations()
//-----------------------------------------------------------------------------
void VerilogParameterParser::findOldDeclarations(QString const &input, QStringList& declarations)
{
    // In the OLD style parameter declarations the parameters are after the module header.
    int endIndex = VerilogSyntax::MODULE_BEGIN.indexIn(input) + VerilogSyntax::MODULE_BEGIN.matchedLength();

    // And that is why the module header is ripped off before further parsing.
    QString joku = input;
    QString inspect = joku.remove( 0, endIndex + 1 );

    cullStrayComments(inspect);

    // MUST have "parameter", may have "signed + range" OR one of those OR type. Is composed of name+value pairs,
    // posibly multiple times separated by comma. May have comments between. May have semicolon or comment the end.
    QRegExp declarRule(QString("parameter\\s*(((signed)?\\s*(\\w+" + VerilogSyntax::RANGE + ")?)|(\\w+))\\s*" +
        VerilogSyntax::NAME_VALUE + "(?:\\s*,\\s*(" + VerilogSyntax::COMMENT + ")?\\s*" + VerilogSyntax::NAME_VALUE
        + ")*((\\s*;\\s*)|(\\s*))(" + VerilogSyntax::COMMENT + ")?"), Qt::CaseInsensitive);

    findDeclarations(declarRule, inspect, declarations);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseDeclarations()
//-----------------------------------------------------------------------------
void VerilogParameterParser::parseParameters(QString const &input, QList<QSharedPointer<ModelParameter>>& parameters)
{
    QString type = parseType(input);

    QString description = parseDescription(input);

    // Must have name-value pair, possibly multiple times separated by comma. May have comments between.
    QRegExp parameterRule("(" + VerilogSyntax::NAME_VALUE + "(\\s*,\\s*(" + VerilogSyntax::COMMENT + ")?\\s*"
        + VerilogSyntax::NAME_VALUE + ")*)", Qt::CaseInsensitive);

    parameterRule.indexIn( input );
    QString parametersString = parameterRule.cap(0);

    QStringList parametersList = parametersString.split(",");

    for ( int i = 0; i < parametersList.size(); ++i )
    {
        QString parameter = parametersList[i];
        QRegExp splitRule("(\\w+)\\s*=\\s*(\\w+)", Qt::CaseInsensitive);
        splitRule.indexIn(parameter);

        QString name = splitRule.cap(1);
        QString value = splitRule.cap(2);

        QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
        modelParameter->setName(name.trimmed());
        modelParameter->setDataType(type);
        modelParameter->setValue(value);
        modelParameter->setUsageType("nontyped");
        modelParameter->setDescription(description);

        parameters.append(modelParameter);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findDeclarations()
//-----------------------------------------------------------------------------
void VerilogParameterParser::findDeclarations(QRegExp &declarRule, QString &inspect, QStringList &declarations)
{
    int declarIndex = declarRule.indexIn(inspect);

    // Repeat the parsing until no more matches are found.
    while ( declarIndex != - 1)  
    {
        int declarLength = declarRule.matchedLength();

        // Take the matching part and append to the list.
        QString declaration = inspect.mid( declarIndex, declarLength );
        declaration = declaration.trimmed();
        declarations.append(declaration);

        // Seek for the next match beginning from the end of the previous match.
        declarIndex = declarRule.indexIn(inspect, declarIndex + declarLength);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::cullStrayComments()
//-----------------------------------------------------------------------------
void VerilogParameterParser::cullStrayComments(QString &inspect)
{
    QRegExp multiRem = QRegExp(VerilogSyntax::MULTILINE_COMMENT);
    multiRem.setMinimal(true);
    inspect = inspect.remove(multiRem);
    inspect = inspect.remove(VerilogSyntax::COMMENTLINE);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseType()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseType(QString const & input)
{
    QRegExp typeRule("(\\w+)\\s+" +  VerilogSyntax::NAME_VALUE, Qt::CaseInsensitive);
    typeRule.indexIn( input );
    QString type = typeRule.cap( 1 );

    if ( type == "parameter" )
    {
        type = "";
    }

    return type;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseDescription()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseDescription(QString const &input)
{
    QString description;
    QRegExp commentRule(VerilogSyntax::COMMENT, Qt::CaseInsensitive);
    int commentStart = commentRule.lastIndexIn(input);

    if ( commentStart != - 1 )
    {
        int commentLength = commentRule.matchedLength();

        description = input.mid(commentStart,commentLength);

        if ( description.startsWith("//") )
        {
            description = description.remove(0,2);
        }

        description = description.trimmed();
    }

    return description;
}