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

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <QString>
#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::VerilogParameterParser()
//-----------------------------------------------------------------------------
VerilogParameterParser::VerilogParameterParser() : highlighter_(0), genericVisualizer_(0)
{
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::~VerilogParameterParser()
//-----------------------------------------------------------------------------
VerilogParameterParser::~VerilogParameterParser()
{
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::setHighlighter()
//-----------------------------------------------------------------------------
void VerilogParameterParser::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::setModelParameterVisualizer()
//-----------------------------------------------------------------------------
void VerilogParameterParser::setModelParameterVisualizer(ModelParameterVisualizer* visualizer)
{
    genericVisualizer_ = visualizer;
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
void VerilogParameterParser::parseParameters(QString const &input,
    QList<QSharedPointer<ModelParameter>>& parameters)
{
    // Find the type and the declaration. Only one per declaration is supported.
    QString type = parseType(input);
    QString description = parseDescription(input);

    // Must have name-value pair, possibly multiple times separated by comma. May have comments between.
    QRegExp parameterRule("(" + VerilogSyntax::NAME_VALUE + "(\\s*,\\s*(" + VerilogSyntax::COMMENT + ")?\\s*"
        + VerilogSyntax::NAME_VALUE + ")*)", Qt::CaseInsensitive);

    parameterRule.indexIn( input );
    QString parametersString = parameterRule.cap(0);

    // We know for sure that each name value pair is separated by comma, and as such we get a list of them.
    QStringList parametersList = parametersString.split(",");

    foreach ( QString parameter, parametersList )
    {
        // After acquiring a name value pair, we separate the name and the value from each other.
        QRegExp splitRule("(\\w+)\\s*=\\s*(\\w+)", Qt::CaseInsensitive);
        splitRule.indexIn(parameter);

        QString name = splitRule.cap(1);
        QString value = splitRule.cap(2);

        // Each name value pair produces a new model parameter, but the type and the description is recycled.
        QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
        modelParameter->setName(name.trimmed());
        modelParameter->setDataType(type);
        modelParameter->setValue(value);
        modelParameter->setUsageType("nontyped");
        modelParameter->setDescription(description);

        // Add to the list, as there may be more than one.
        parameters.append(modelParameter);

        // If exists, the parameter is also passed to the visualizer.
        if (genericVisualizer_)
        {
            genericVisualizer_->addModelParameter(modelParameter);
        }
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

        // Highlight the selection if applicable.
        if (highlighter_)
        {
            highlighter_->applyHighlight(declaration, ImportColors::MODELPARAMETER);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::cullStrayComments()
//-----------------------------------------------------------------------------
void VerilogParameterParser::cullStrayComments(QString &inspect)
{
    // Removing multi line comments needs so called non-greedy matching.
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
    // The type is assumed to be the first word in the declaration.
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

    // If exist, the description is the last comment in the declaration.
    QRegExp commentRule(VerilogSyntax::COMMENT, Qt::CaseInsensitive);
    int commentStart = commentRule.lastIndexIn(input);

    if ( commentStart != - 1 )
    {
        // Found the index. The description is starting index + length.
        int commentLength = commentRule.matchedLength();
        description = input.mid(commentStart,commentLength);

        // Some times the expression leaves the comment tag to the description.
        if ( description.startsWith("//") )
        {
            description = description.remove(0,2);
        }

        // No need for extra white spaces.
        description = description.trimmed();
    }

    return description;
}