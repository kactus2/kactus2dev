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

#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/Model.h>

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QString>
#include <QRegularExpression>

namespace
{
    const QRegularExpression TYPE_RULE("(\\w+)\\s+(?:(" + VerilogSyntax::RANGE + ")?\\s*(" + 
        VerilogSyntax::RANGE + "))?\\s*" + VerilogSyntax::NAME_VALUE, QRegularExpression::CaseInsensitiveOption);
}

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
void VerilogParameterParser::import(QString const& input, QSharedPointer<Component> targetComponent,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    // Find parameter declarations. Try both formats, as we cannot know which one is used.
    QStringList declarations;
    declarations.append(findANSIDeclarations(input));
    declarations.append(findOldDeclarations(input));

    QList<QSharedPointer<ModuleParameter> > parsedParameters;
    foreach (QString const& declaration, declarations)
    {
        parsedParameters.append(parseParameters(declaration));
    }

    copyIdsFromOldModelParameters(parsedParameters, targetComponentInstantiation);

    foreach (QSharedPointer<ModuleParameter> existingParameter, *targetComponentInstantiation->getModuleParameters())
    {
        if (existingParameter->getAttribute("imported").isEmpty())
        {
            targetComponentInstantiation->getModuleParameters()->removeAll(existingParameter);
        }
        else
        {
            existingParameter->setAttribute("imported", "");
        }        
    }

    targetComponentInstantiation->getModuleParameters()->append(parsedParameters);

    replaceNamesReferencesWithIds(targetComponent, targetComponentInstantiation);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findANSIDeclarations()
//-----------------------------------------------------------------------------
QStringList VerilogParameterParser::findANSIDeclarations(QString const& input)
{
    // Find the module declaration first.
    QPair<int,int> declarationPosition = VerilogSyntax::findModuleDeclaration(input);

    // If cannot find, return empty list.
    if (declarationPosition.first == -1 || declarationPosition.second == -1)
    {
        return QStringList();
    }

    // In the ANSI style parameter declarations the parameters are contained WITHIN the module header
    // and thus only the module header is inspected in the parsing.
    QString inspect = input.mid(declarationPosition.first, declarationPosition.second);

    // Cull the stray comments to avoid distractions to parsing.
    inspect = VerilogSyntax::cullStrayComments(inspect);

    // We shall further crop until the start of the ports.
    QRegularExpression portsBegin("([)](\\s*|(\\s*" + VerilogSyntax::COMMENT + "\\s*))[(])", 
        QRegularExpression::CaseInsensitiveOption);

    QRegularExpression beginEnd("([)])");

    int loc = inspect.lastIndexOf(portsBegin);

    if ( loc != -1 )
    {
        // If encountered, rip off the port declarations.
        QRegularExpression portsProper("([(].*[)])", QRegularExpression::DotMatchesEverythingOption);
        loc = inspect.indexOf(portsProper,loc);
        inspect = inspect.left(loc);
        loc = inspect.lastIndexOf(beginEnd);

        inspect = inspect.left(loc);
    }

    return findDeclarations(inspect, false);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findOldDeclarations()
//-----------------------------------------------------------------------------
QStringList VerilogParameterParser::findOldDeclarations(QString const& input)
{
    // In the OLD style parameter declarations the parameters are between module header and footer.
    int startIndex = VerilogSyntax::MODULE_BEGIN.match(input).capturedEnd();
    int length = input.indexOf(VerilogSyntax::MODULE_END) - startIndex;

    // And that is why the inspected the region between the header and footer are included to the parsing.
    QString inspect = input.mid(startIndex, length);
    inspect = VerilogSyntax::cullStrayComments(inspect);

    return findDeclarations(inspect, true);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ModuleParameter> > VerilogParameterParser::parseParameters(QString const &input)
{
    QList<QSharedPointer<ModuleParameter> > parameters;

    // Find the type and the declaration. Only one per declaration is supported.
    QString type = parseType(input);
    QString bitWidthLeft = parseBitWidthLeft(input);
    QString bitWidthRight = parseBitWidthRight(input);
    QString arrayLeft = parseArrayLeft(input);
    QString arrayRight = parseArrayRight(input);
    QString description = parseDescription(input);

    QString inputWithoutComments = input;
    inputWithoutComments.remove(QRegularExpression(VerilogSyntax::COMMENT));

    QString parameterDefinition =  "(" + VerilogSyntax::NAMES + ")\\s*=((\\s*(" + 
        VerilogSyntax::OPERATION_OR_ALPHANUMERIC + ")+\\s*)+)";

    QRegularExpression parameterRule(parameterDefinition + "(\\s*,\\s*" + parameterDefinition + ")*", 
        QRegularExpression::CaseInsensitiveOption);    

    QRegularExpression splitRule(parameterDefinition, QRegularExpression::CaseInsensitiveOption);

    QString parametersString = parameterRule.match(inputWithoutComments).captured();

    // We know for sure that each name value pair is separated by comma, and as such we get a list of them.
    // Names are defined before the = operator.
    foreach (QString const& parameter, 
        parametersString.split(QRegularExpression(",(?=.*=)", QRegularExpression::DotMatchesEverythingOption)))
    {
        // After acquiring a name value pair, we separate the name and the value from each other.
        QString name = splitRule.match(parameter).captured(1).trimmed();
        QString value = splitRule.match(parameter).captured(2).simplified();

        // Each name value pair produces a new module parameter, but the type and the description is recycled.
        QSharedPointer<ModuleParameter> moduleParameter =  QSharedPointer<ModuleParameter>(new ModuleParameter());      
        moduleParameter->setName(name);
        moduleParameter->setDataType(type);
        moduleParameter->setType(createTypeFromDataType(type));
        moduleParameter->setValue(value);
        moduleParameter->setUsageType("nontyped");
        moduleParameter->setVectorLeft(bitWidthLeft);
        moduleParameter->setVectorRight(bitWidthRight);
        moduleParameter->setArrayLeft(arrayLeft);
        moduleParameter->setArrayRight(arrayRight);
        moduleParameter->setDescription(description);

        // No type plus has vectors, means that we set type as bit for compatibility.
        if (!moduleParameter->getVectors()->isEmpty() && moduleParameter->getType().isEmpty())
        {
            moduleParameter->setType(QStringLiteral("bit"));
        }

        parameters.append(moduleParameter);
    }

    return parameters;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::createTypeFromDataType()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::createTypeFromDataType(QString const& dataType)
{
    QRegularExpression knownTypes(QStringLiteral("bit|byte|shortint|int|longint|shortreal|real|string"));

    if (knownTypes.match(dataType).hasMatch())
    {
        return dataType;
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findDeclarations()
//-----------------------------------------------------------------------------
QStringList VerilogParameterParser::findDeclarations(QString const& inspect, bool findTerminator)
{
    // Rule used to detect parameter declarations.
    QRegularExpression declarationRule("\\bparameter\\s+", QRegularExpression::CaseInsensitiveOption);

    // List of detected parameter declarations.
    QStringList declarations;

    // The initial current is zero, since it will immediately replaced with the first next.
    int currentStart = 0;
    // The initial next is the first match, since it will become the first current.
    int nextStart = inspect.indexOf(declarationRule);

    // Repeat the parsing until no more matches are found.
    while (nextStart != -1)  
    {
        // The next of the previous iteration is now the current.
        currentStart = nextStart;
        // Find the location of the next parameter for this iteration.
        nextStart = inspect.indexOf(declarationRule, currentStart + 1);

        // Must find where the declaration ends: By default, its the next declaration.
        int splitLocation = nextStart;

        if (splitLocation < 0 && findTerminator)
        {
            // If next declaration is not found, find the next terminator.
            splitLocation = inspect.indexOf(";", currentStart + 1);

            // Then take the rest of the line, so that we get the comments as well.
            splitLocation = inspect.indexOf("\n", splitLocation);
        }

        QString declaration;

        if (splitLocation >= 0)
        {
            // Everything between current and split location are the declaration.
            int length = splitLocation - currentStart;

            declaration = inspect.mid(currentStart, length);
        }
        else
        {
            // No split location found means that take the rest of the sting.
            declaration = inspect.mid(currentStart);
        }

        // Also remove extra white space.
        declaration = declaration.trimmed();

        // Highlight the selection if applicable.
        if (highlighter_)
        {
            highlighter_->applyHighlight(declaration, ImportColors::MODELPARAMETER);
        }

		// Append the part to the list.
        declarations.append(declaration);
    }

    return declarations;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseType()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseType(QString const& input)
{
    // The type is assumed to be the first word in the declaration.
    QString type = TYPE_RULE.match(input).captured(1);
    if (type == "parameter")
    {
        type.clear();
    }

    return type;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseBitWidthLeft()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseBitWidthLeft(QString const& declaration)
{
    QString bitRange = TYPE_RULE.match(declaration).captured(3);

    QRegularExpressionMatch rangeMatch = VerilogSyntax::CAPTURING_RANGE.match(bitRange);
    QString left = rangeMatch.captured(1);

    return left;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseBitWidthRight()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseBitWidthRight(QString const& declaration)
{
    QString bitRange = TYPE_RULE.match(declaration).captured(3);

    QRegularExpressionMatch rangeMatch = VerilogSyntax::CAPTURING_RANGE.match(bitRange);
    QString right = rangeMatch.captured(2);

    return right;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseArrayLeft()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseArrayLeft(QString const& declaration)
{
    QString bitRange = TYPE_RULE.match(declaration).captured(2);

    return VerilogSyntax::CAPTURING_RANGE.match(bitRange).captured(1);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseArrayRight()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseArrayRight(QString const& declaration)
{
    QString bitRange = TYPE_RULE.match(declaration).captured(2);

    return VerilogSyntax::CAPTURING_RANGE.match(bitRange).captured(2);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseDescription()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseDescription(QString const& input)
{
    QString description;

    // If exist, the description is the last comment in the declaration.
    QRegularExpression commentRule(VerilogSyntax::COMMENT, QRegularExpression::CaseInsensitiveOption);
    int lastCommentIndex = input.lastIndexOf(commentRule);

    if (lastCommentIndex != -1)
    {
        QRegularExpressionMatch commmentMatch = commentRule.match(input, lastCommentIndex);

        // Found the index. The description is starting index + length.
        description = input.mid(commmentMatch.capturedStart(), commmentMatch.capturedLength());

        // Some times the expression leaves the comment tag to the description.
        if (description.startsWith("//"))
        {
            description = description.remove(0, 2);
        }

        // No need for extra white spaces.
        description = description.trimmed();
    }

    return description;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::copyIdsFromOldModelParameters()
//-----------------------------------------------------------------------------
void VerilogParameterParser::copyIdsFromOldModelParameters(QList<QSharedPointer<ModuleParameter> > parsedParameters,
    QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    foreach (QSharedPointer<ModuleParameter> parameter, parsedParameters)
    {
		foreach (QSharedPointer<ModuleParameter> existingParameter,
            *targetComponentInstantiation->getModuleParameters())
		{
			if (existingParameter->name().compare(parameter->name()) == 0)
			{
				parameter->setValueId(existingParameter->getValueId());
				break;
			}
		}
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::replaceReferenceNamesWithIds()
//-----------------------------------------------------------------------------
void VerilogParameterParser::replaceNamesReferencesWithIds(QSharedPointer<Component> targetComponent,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    foreach (QSharedPointer<ModuleParameter> moduleParameter, *targetComponentInstantiation->getModuleParameters())
    {
        QSharedPointer<Parameter> targetParameter = findParameter(moduleParameter->name(), targetComponent);
        if (targetParameter.isNull())
        {
            targetParameter = QSharedPointer<Parameter>(new Parameter());
            targetParameter->setName(moduleParameter->name());
            targetComponent->getParameters()->append(targetParameter);
        }
        
        targetParameter->setValue(moduleParameter->getValue());
        targetParameter->setArrayLeft(moduleParameter->getArrayLeft());
        targetParameter->setArrayRight(moduleParameter->getArrayRight());
        targetParameter->setVectorLeft(moduleParameter->getVectorLeft());
        targetParameter->setVectorRight(moduleParameter->getVectorRight());
        targetParameter->setDescription(moduleParameter->description());
        targetParameter->increaseUsageCount();

        moduleParameter->setValue(targetParameter->getValueId());
    }
    
    foreach (QSharedPointer<Parameter> parameter, *targetComponent->getParameters())
    {
        replaceNameReferencesWithParameterIds(parameter, targetComponent);
    }

    foreach (QSharedPointer<ModuleParameter> parameter, *targetComponentInstantiation->getModuleParameters())
    {
        replaceNameReferencesWithParameterIds(parameter, targetComponent);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> VerilogParameterParser::findParameter(QString const& name, 
    QSharedPointer<Component> component) const
{
    foreach (QSharedPointer<Parameter> parameter, *component->getParameters())
    {
        if (parameter->name().compare(name) == 0)
        {
            return parameter;
        }
    }

    return QSharedPointer<Parameter>();
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::replaceParameterNamesWithIds()
//-----------------------------------------------------------------------------
void VerilogParameterParser::replaceNameReferencesWithParameterIds(QSharedPointer<Parameter> parameter,
    QSharedPointer<Component> targetComponent) const
{
    foreach (QSharedPointer<Parameter> define, *targetComponent->getParameters())
    {
        QRegularExpression macroUsage("`?" + define->name() + "\\b");

        QString parameterValue = replaceNameWithId(parameter->getValue(), macroUsage, define);
        parameter->setValue(parameterValue);

        QString bitWidthLeft = replaceNameWithId(parameter->getVectorLeft(), macroUsage, define);
        parameter->setVectorLeft(bitWidthLeft);

        QString bitWidthRight = replaceNameWithId(parameter->getVectorRight(), macroUsage, define);
        parameter->setVectorRight(bitWidthRight);

        QString arrayLeft = replaceNameWithId(parameter->getArrayLeft(), macroUsage, define);
        parameter->setArrayLeft(arrayLeft);

        QString arrayRight = replaceNameWithId(parameter->getArrayRight(), macroUsage, define);
        parameter->setArrayRight(arrayRight);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::replaceNameWithId()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::replaceNameWithId(QString const& expression, QRegularExpression& namePattern, 
    QSharedPointer<Parameter> referenced) const
{
    QString replaced = expression;
    QRegularExpressionMatch match = namePattern.match(expression);
    if (match.hasMatch())
    {
        replaced.replace(namePattern, referenced->getValueId());

        int count = match.capturedTexts().count();
        for (int i = 0; i < count; i++)
        {
            referenced->increaseUsageCount();
        }
    }

    return replaced;
}
