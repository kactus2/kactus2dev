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
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::VerilogParameterParser()
//-----------------------------------------------------------------------------
VerilogParameterParser::VerilogParameterParser() : highlighter_(0)
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
// Function: VerilogParameterParser::import()
//-----------------------------------------------------------------------------
void VerilogParameterParser::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    // Find parameter declarations. Try both formats, as we cannot know which one is used.
    QStringList declarations;
    findANSIDeclarations(input, declarations);
    findOldDeclarations(input, declarations);

    // Parse model parameters out of declarations.
    QList<QSharedPointer<ModelParameter> > parameters;

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

    // And that is why only the module header is inspected in the parsing.
    QString inspect = input.mid( 0, endIndex );

    // We shall further crop until the start of the ports.
    QRegExp portsBegin(QString("([)](\\s*|(\\s*" + VerilogSyntax::COMMENT + "\\s*))[(])"), Qt::CaseInsensitive);
    QRegExp beginEnd(QString("([)])"), Qt::CaseInsensitive);
    int portLoc = portsBegin.lastIndexIn(inspect);
    int loc = portLoc;

    if ( loc == -1 )
    {
        // If not encountered, crop until the end of the module begin.
        int endLoc = beginEnd.lastIndexIn(inspect);
        loc = endLoc;
    }
    else
    {
        // If encountered, rip off the port declarations.
        QRegExp portsProper(QString("([(].*[)])"), Qt::CaseInsensitive);
        loc = portsProper.indexIn(inspect,loc);
        inspect = inspect.left(loc);
        loc = beginEnd.lastIndexIn(inspect);
    }

    // If the last location is contained within a comment, rip off the commend and find the another last.
    QRegExp lastComment(VerilogSyntax::COMMENT, Qt::CaseInsensitive);
    int commentLoc = lastComment.lastIndexIn(inspect);

    if ( commentLoc != -1 && commentLoc < loc && loc < commentLoc + lastComment.matchedLength() )
    {
        inspect = inspect.left(commentLoc);

        loc = beginEnd.lastIndexIn(inspect);
    }

    inspect = inspect.left(loc);

    // Cull the stray comments to avoid distractions to parsing.
    cullStrayComments(inspect);

    QRegExp declarRule(QString("parameter\\s+"), Qt::CaseInsensitive);

    findDeclarations(declarRule, inspect, declarations);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findOldDeclarations()
//-----------------------------------------------------------------------------
void VerilogParameterParser::findOldDeclarations(QString const &input, QStringList& declarations)
{
    // In the OLD style parameter declarations the parameters are between module header and footer.
    int startIndex = VerilogSyntax::MODULE_BEGIN.indexIn(input) + VerilogSyntax::MODULE_BEGIN.matchedLength();
    int length = VerilogSyntax::MODULE_END.indexIn(input) - startIndex;

    // And that is why the inspected the region between the header and footer are included to the parsing.
    QString joku = input;
    QString inspect = joku.mid( startIndex, length );

    cullStrayComments(inspect);

    QRegExp declarRule(QString("parameter\\s+"), Qt::CaseInsensitive);

    findDeclarations(declarRule, inspect, declarations);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseParameters()
//-----------------------------------------------------------------------------
void VerilogParameterParser::parseParameters(QString const &input,
    QList<QSharedPointer<ModelParameter> >& parameters)
{
    // Find the type and the declaration. Only one per declaration is supported.
    QString type = parseType(input);
    QString description = parseDescription(input);

    // Must have name-value pair, possibly multiple times separated by comma. May have comments between.
    QRegExp parameterRule("(" + VerilogSyntax::NAME_VALUE + "(\\s*,\\s*(" + VerilogSyntax::COMMENT +
        ")?\\s*" + VerilogSyntax::NAME_VALUE + ")*)", Qt::CaseInsensitive);

    parameterRule.indexIn( input );
    QString parametersString = parameterRule.cap(0);

    // We know for sure that each name value pair is separated by comma, and as such we get a list of them.
    QStringList parametersList = parametersString.split(",");

    foreach ( QString parameter, parametersList )
    {
        // After acquiring a name value pair, we separate the name and the value from each other.
        QRegExp splitRule("(\\w+)\\s*=((\\s*(" + VerilogSyntax::OPERATION_OR_ALPHANUMERIC + "))+)", Qt::CaseInsensitive);
        splitRule.indexIn(parameter);

        QString name = splitRule.cap(1);
        QString value = splitRule.cap(2);

        QRegExp cullRule("//", Qt::CaseInsensitive);
        int cullIndex = cullRule.indexIn(value);
        value = value.left(cullIndex);

        value = value.trimmed();

        // Each name value pair produces a new model parameter, but the type and the description is recycled.
        QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
        modelParameter->setName(name.trimmed());
        modelParameter->setDataType(type);
        modelParameter->setValue(value);
        modelParameter->setUsageType("nontyped");
        modelParameter->setDescription(description);

        // Add to the list, as there may be more than one.
        parameters.append(modelParameter);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findDeclarations()
//-----------------------------------------------------------------------------
void VerilogParameterParser::findDeclarations(QRegExp &declarRule, QString &inspect, QStringList &declarations)
{
    int prevIndex = 0;
    int declarIndex = declarRule.indexIn(inspect);
    declarRule.setMinimal(true);

    // Repeat the parsing until no more matches are found.
    while ( declarIndex != - 1)  
    {
        int declarLength = declarRule.matchedLength();

        // Seek for the next match beginning from the end of the previous match.
        prevIndex = declarIndex;
        declarIndex = declarRule.indexIn(inspect, declarIndex  + declarLength + 1);

        // Take the matching part and append to the list.
        QString declaration = inspect.mid( prevIndex, declarIndex - prevIndex );

        declaration = declaration.trimmed();
        declarations.append(declaration);

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
