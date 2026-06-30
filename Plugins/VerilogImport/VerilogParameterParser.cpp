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
#include <IPXACTmodels/Component/Model.h>

#include <IPXACTmodels/utilities/Search.h>

#include <KactusAPI/include/KactusColors.h>

#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <common/expressions/utilities.h>

#include <QList>
#include <QString>
#include <QRegularExpression>
#include <QStringBuilder>

namespace
{
    const QRegularExpression TYPE_RULE("(?:localparam|parameter)?\\s*((?:\\w|:)+\\s+)?((?:\\w|:)+)?\\s*(?:(" + VerilogSyntax::RANGE + ")?\\s*(" + 
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
void VerilogParameterParser::import(QString const& componentDeclaration, QSharedPointer<Component> targetComponent,
    QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    auto declarations = findDeclarations(componentDeclaration);

    QList<QPair<QSharedPointer<ModuleParameter>, QString> > parsedParameters; // For mapping parameter type (localparam/parameter) to parsed parameter
    for (auto const& declaration : declarations)
    {
        auto params = parseParameters(declaration.value_);

        // In most cases size of params should be 1
        std::for_each(params.begin(), params.end(),
            [&declaration, &parsedParameters](QSharedPointer<ModuleParameter> moduleParam)
            {
                parsedParameters.append(std::make_pair(moduleParam, declaration.type_));
            });
    }

    if (targetComponentInstantiation.isNull() == false)
    {
        copyIdsFromOldModelParameters(parsedParameters, targetComponentInstantiation);

        QList<QSharedPointer<ModuleParameter> > existingParameterList =
            *targetComponentInstantiation->getModuleParameters();
        for (int i = 0; i < existingParameterList.size(); ++i)
        {
            auto existingParameter = existingParameterList.at(i);
            if (existingParameter->getAttribute("imported").isEmpty())
            {
                targetComponentInstantiation->getModuleParameters()->removeAll(existingParameter);
            }
            else
            {
                existingParameter->setAttribute("imported", "");
            }
        }

        std::for_each(parsedParameters.cbegin(), parsedParameters.cend(),
            [&targetComponentInstantiation](auto const& pair)
            {
                targetComponentInstantiation->getModuleParameters()->append(pair.first);
            });
    }

    replaceNamesReferencesWithIds(parsedParameters, targetComponent, targetComponentInstantiation);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findDeclarations()
//-----------------------------------------------------------------------------
// QStringList VerilogParameterParser::findDeclarations(QString const& input)
QList<VerilogParameterParser::ParameterDeclaration> VerilogParameterParser::findDeclarations(QString const& input)
{    
    return findParameterDeclarations(input, findParameterSection(input));
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findParameterSection()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::findParameterSection(QString const &input)
{
    QString inspect = VerilogSyntax::cullStrayComments(input);

    // Priority on parameters declared in ANSI-style at module header.
    // Otherwise, search the whole input.
    QRegularExpression parameterStart(QStringLiteral("#\\s*[(]"));
    QRegularExpressionMatch match = parameterStart.match(inspect);

    int beginIndex = match.capturedEnd();
    int endIndex = -1;
    if (beginIndex != -1)
    {
        endIndex =
            ExpressionUtilities::findMatchingEndParenthesis(inspect, beginIndex - 1, VerilogSyntax::COMMENT);
    }

    int length = input.length();
    if (beginIndex == -1)
    {
        ++length;
    }
    else if (endIndex != -1)
    {
        length = endIndex - beginIndex;
    }

    return inspect.mid(beginIndex, length);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ModuleParameter> > VerilogParameterParser::parseParameters(QString const &input)
{
    QList<QSharedPointer<ModuleParameter> > parameters;

    // Find the type and the declaration. Only one per declaration is supported.
    QString type = parseType(input).trimmed();
    QString typeWithSign = type;
    QString signModifier = TYPE_RULE.match(input).captured(2);

    // Create complete type if there is a sign modifier
    if (QRegularExpression("(signed|unsigned)").match(signModifier.toLower()).hasMatch())
    {
        typeWithSign.append(QStringLiteral(" ") % signModifier); // e.g. int unsigned
    }
    else
    {
        signModifier.clear();
    }

    QString bitWidthLeft = parseBitWidthLeft(input);
    QString bitWidthRight = parseBitWidthRight(input);
    QString arrayLeft = parseArrayLeft(input);
    QString arrayRight = parseArrayRight(input);
    QString description = parseDescription(input);

    QString inputWithoutComments = input;
    inputWithoutComments.remove(QRegularExpression(VerilogSyntax::COMMENT));

    QString parameterDefinition =  "(" + VerilogSyntax::NAMES + ")\\s*=((\\s*(" + 
        VerilogSyntax::PARAMETER_VALUE + ")+\\s*)+)";

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
        moduleParameter->setDataType(typeWithSign);
        moduleParameter->setType(createTypeFromDataType(type));
        moduleParameter->setSign(signModifier);
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
    return knownTypes.match(dataType).captured();
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findDeclarations()
//-----------------------------------------------------------------------------
// QStringList VerilogParameterParser::findParameterDeclarations(QString const& componentDeclaration,
QList<VerilogParameterParser::ParameterDeclaration > VerilogParameterParser::findParameterDeclarations(QString const& componentDeclaration,
    QString const& parameterArea)
{
    // List of detected parameter declarations.
    QList<ParameterDeclaration> declarations;

    // Rule used to detect parameter declarations.
    QRegularExpression declarationRule("\\b(localparam|parameter)\\s+.*(?:"
        "(;[ \\t]*" + VerilogSyntax::COMMENT + ")|"
        "(;|$)|"
        "(,([ \\t]*" + VerilogSyntax::COMMENT +")?(?=\\s*\\b(?:localparam|parameter)\\b)))",
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::InvertedGreedinessOption |
        QRegularExpression::DotMatchesEverythingOption);
    
    QRegularExpression commentBegin(QStringLiteral("//"));
    QRegularExpression lineBegin(QStringLiteral("^|\\r?\\n"));

    QRegularExpressionMatchIterator iter = declarationRule.globalMatch(parameterArea);
    while (iter.hasNext())
    {        
        QRegularExpressionMatch match = iter.next();
        auto fullDeclaration = match.captured();
        auto paramType = match.captured(1); // get the param type: localparam or parameter
        int declarationBegin = match.capturedStart();

        // Check keyword parameter is not inside a comment.
        if (parameterArea.lastIndexOf(lineBegin, declarationBegin) > parameterArea.lastIndexOf(commentBegin, declarationBegin))
        {
            if (highlighter_)
            {
                highlighter_->applyHighlight(fullDeclaration, KactusColors::Importer::MODELPARAMETER, componentDeclaration);
            }

            declarations.append(ParameterDeclaration{paramType, fullDeclaration});
        }
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
    if (type == QLatin1String("parameter"))
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
    QString bitRange = TYPE_RULE.match(declaration).captured(4);

    QRegularExpressionMatch rangeMatch = VerilogSyntax::CAPTURING_RANGE.match(bitRange);
    QString left = rangeMatch.captured(1);

    return left;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseBitWidthRight()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseBitWidthRight(QString const& declaration)
{
    QString bitRange = TYPE_RULE.match(declaration).captured(4);

    QRegularExpressionMatch rangeMatch = VerilogSyntax::CAPTURING_RANGE.match(bitRange);
    QString right = rangeMatch.captured(2);

    return right;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseArrayLeft()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseArrayLeft(QString const& declaration)
{
    QString bitRange = TYPE_RULE.match(declaration).captured(3);

    return VerilogSyntax::CAPTURING_RANGE.match(bitRange).captured(1);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseArrayRight()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseArrayRight(QString const& declaration)
{
    QString bitRange = TYPE_RULE.match(declaration).captured(3);

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
void VerilogParameterParser::copyIdsFromOldModelParameters(QList<QPair<QSharedPointer<ModuleParameter>, QString> > parsedParameters,
    QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    for (auto const& [parameter, type] : parsedParameters)
    {
		for (QSharedPointer<ModuleParameter> existingParameter :
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
void VerilogParameterParser::replaceNamesReferencesWithIds(
    QList<QPair<QSharedPointer<ModuleParameter>, QString> > const& parsedParameters,
    QSharedPointer<Component> targetComponent,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    for (auto const& [moduleParameter, type] : parsedParameters)
    {
        QSharedPointer<Parameter> targetParameter = 
            Search::findByName(moduleParameter->name(), *targetComponent->getParameters());

        if (targetParameter.isNull())
        {
            targetParameter = QSharedPointer<Parameter>(new Parameter());
            targetParameter->setName(moduleParameter->name());
            targetComponent->getParameters()->append(targetParameter);
        }
        
        targetParameter->setValue(moduleParameter->getValue());
        targetParameter->setType(moduleParameter->getType());
        targetParameter->setSign(moduleParameter->getSign());
        targetParameter->setArrayLeft(moduleParameter->getArrayLeft());
        targetParameter->setArrayRight(moduleParameter->getArrayRight());
        targetParameter->setVectorLeft(moduleParameter->getVectorLeft());
        targetParameter->setVectorRight(moduleParameter->getVectorRight());
        targetParameter->setDescription(moduleParameter->description());
        targetParameter->increaseUsageCount();

        if (type.compare(QStringLiteral("localparam")) == 0)
        {
            targetParameter->setValueResolve(QStringLiteral("immediate"));
            moduleParameter->setValueResolve(QStringLiteral("immediate"));
        }

        moduleParameter->setValue(targetParameter->getValueId());
    }
    
    for (QSharedPointer<Parameter> parameter : *targetComponent->getParameters())
    {
        replaceNameReferencesWithParameterIds(parameter, targetComponent);
    }

    if (targetComponentInstantiation.isNull() == false)
    {
        foreach(QSharedPointer<ModuleParameter> parameter, *targetComponentInstantiation->getModuleParameters())
        {
            replaceNameReferencesWithParameterIds(parameter, targetComponent);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::replaceParameterNamesWithIds()
//-----------------------------------------------------------------------------
void VerilogParameterParser::replaceNameReferencesWithParameterIds(QSharedPointer<Parameter> parameter,
    QSharedPointer<Component> targetComponent) const
{
    foreach (QSharedPointer<Parameter> define, *targetComponent->getParameters())
    {
        QRegularExpression macroUsage("`?" + define->name() + "(?![\\._])"); // don't match systemverilog struct field accesses, e.g MyStruct.field, and other things that can cause partial match

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
