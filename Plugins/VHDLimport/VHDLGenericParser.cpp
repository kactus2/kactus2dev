//-----------------------------------------------------------------------------
// File: VHDLGenericParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.09.2014
//
// Description:
// Parser for VHDL generics.
//-----------------------------------------------------------------------------

#include "VHDLGenericParser.h"

#include "VHDLSyntax.h"

#include <KactusAPI/include/ImportColors.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/common/ModuleParameter.h>

#include <QRegularExpression>

namespace
{
    //! Generics are declared inside entity by GENERIC ( <generic_declarations> );
    const QRegularExpression GENERICS_BEGIN_EXP = QRegularExpression("(GENERIC)\\s*[(]", 
        QRegularExpression::CaseInsensitiveOption);    

    const QRegularExpression GENERICS_END_EXP = QRegularExpression("[)]\\s*[;](?=(?:(\\s*" + VHDLSyntax::COMMENT +
        ")\\s*)*\\s*(END|BEGIN|PORT|GENERIC))", QRegularExpression::CaseInsensitiveOption);   

    const QString GENERIC_TYPE = "(\\w+)(?:\\s+range\\s+\\d+\\s+to\\s+\\d+)?";

    //! Generic declaration is <generic_names> : <type> [<default>] [pragma]; [description]
    const QRegularExpression GENERIC_EXP = QRegularExpression(
        "(" + VHDLSyntax::NAMES + ")\\s*[:]\\s*" + GENERIC_TYPE + "(?:\\s*" + VHDLSyntax::DEFAULT + ")?" +
        "(?:\\s*" + VHDLSyntax::PRAGMA + ")?(?:" + VHDLSyntax::DECLARATION_END + ")", 
        QRegularExpression::CaseInsensitiveOption);
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::VHDLGenericParser()
//-----------------------------------------------------------------------------
VHDLGenericParser::VHDLGenericParser(QObject* parent): QObject(parent), highlighter_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::import()
//-----------------------------------------------------------------------------
void VHDLGenericParser::import(QString const& input, QSharedPointer<Component> targetComponent,
    QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    foreach (QSharedPointer<ModuleParameter> modelParameter, *targetComponentInstantiation->getModuleParameters())
    {
        modelParameter->setAttribute("kactus2:import", "no");
    }

    foreach(QString const& declaration, findGenericDeclarations(input))
    {
        createModelParameterFromDeclaration(declaration, targetComponentInstantiation);
        if (highlighter_)
        {
            highlighter_->applyHighlight(declaration, ImportColors::MODELPARAMETER, input);
        }        
    }

    foreach (QSharedPointer<ModuleParameter> modelParameter, *targetComponentInstantiation->getModuleParameters())
    {
        if (modelParameter->getAttribute("kactus2:import").compare("no") == 0)
        {
            targetComponentInstantiation->getModuleParameters()->removeAll(modelParameter);
        }
    }

    replaceNamesReferencesWithIds(targetComponent, targetComponentInstantiation);
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::setHighlighter()
//-----------------------------------------------------------------------------
void VHDLGenericParser::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::findGenericDeclarations()
//-----------------------------------------------------------------------------
QStringList VHDLGenericParser::findGenericDeclarations(QString const& input) const
{
    QString genericSection = findGenericsSection(input);
    QString sectionWithoutCommentLines = removeCommentLines(genericSection);

    return genericDeclarationsIn(sectionWithoutCommentLines);
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::genericsSection()
//-----------------------------------------------------------------------------
QString VHDLGenericParser::findGenericsSection(QString const &input) const
{
    int entityBegin = input.indexOf(VHDLSyntax::ENTITY_BEGIN_EXP);
    int entityEnd = input.indexOf(VHDLSyntax::ENTITY_END_EXP, entityBegin);

    int genericsBeginIndex = GENERICS_BEGIN_EXP.match(input).capturedEnd(); 

    if (genericsBeginIndex > entityEnd)
    {
        return QString();
    }

    int genericsEndIndex = input.indexOf(GENERICS_END_EXP, genericsBeginIndex);

    return input.mid(genericsBeginIndex, genericsEndIndex - genericsBeginIndex);
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VHDLGenericParser::removeCommentLines(QString section) const
{
    QRegularExpression commentLine("^" + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP + "|" +
        VHDLSyntax::ENDLINE + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP);

    return section.remove(commentLine);
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::genericDeclarationsIn()
//-----------------------------------------------------------------------------
QStringList VHDLGenericParser::genericDeclarationsIn(QString const& sectionWithoutCommentLines) const
{
    QStringList genericDeclarations;

    QRegularExpressionMatchIterator i = GENERIC_EXP.globalMatch(sectionWithoutCommentLines);
    while(i.hasNext())
    {    
        genericDeclarations.append(i.next().captured());
    }

    return genericDeclarations;
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::createModelParameterFromDeclaration()
//-----------------------------------------------------------------------------
void VHDLGenericParser::createModelParameterFromDeclaration(QString const& declaration, 
    QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const
{
    QRegularExpressionMatch matchedDeclaration = GENERIC_EXP.match(declaration);

    QStringList genericNames = matchedDeclaration.captured(1).split(QRegularExpression("\\s*[,]\\s*"),
        QString::SkipEmptyParts);
    QString dataType = matchedDeclaration.captured(2);
    QString type = findMatchingType(dataType);
    QString defaultValue = matchedDeclaration.captured(3);

    QString description = matchedDeclaration.captured(5).trimmed();
    if (description.isEmpty())
    {
        description = matchedDeclaration.captured(6).trimmed();
    }

    for (QString const& name : genericNames)
    {   
        QSharedPointer<ModuleParameter> parameter = findModuleParameter(name.trimmed(), targetComponentInstantiation);
        if (parameter.isNull())
        {
            parameter = QSharedPointer<ModuleParameter>(new ModuleParameter());
            targetComponentInstantiation->getModuleParameters()->append(parameter);
        }

        parameter->setName(name.trimmed());
        parameter->setDataType(dataType);
        parameter->setType(type);
        parameter->setDescription(description);
        parameter->setValue(defaultValue);
        parameter->setUsageType("nontyped");
        parameter->setAttribute("kactus2:import", "");
    } 
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::findModuleParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModuleParameter> VHDLGenericParser::findModuleParameter(QString const& name, 
    QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const
{
    foreach (QSharedPointer<ModuleParameter> currentParameter, *targetComponentInstantiation->getModuleParameters())
    {
        if (currentParameter->name().compare(name) == 0)
        {
            return currentParameter;
        }
    }	

    return QSharedPointer<ModuleParameter>();
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::replaceReferenceNamesWithIds()
//-----------------------------------------------------------------------------
void VHDLGenericParser::replaceNamesReferencesWithIds(QSharedPointer<Component> targetComponent,
    QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const
{
    foreach (QSharedPointer<ModuleParameter> moduleParameter, *targetComponentInstantiation->getModuleParameters())
    {
        QSharedPointer<Parameter> targetParameter = findParameter(moduleParameter->name(), targetComponent);
        if (targetParameter.isNull())
        {
            targetParameter = QSharedPointer<Parameter>(new Parameter());
            targetParameter->setName(moduleParameter->name());
            targetParameter->setType(moduleParameter->getType());
            targetComponent->getParameters()->append(targetParameter);
        }
       
        targetParameter->setValue(moduleParameter->getValue());
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
// Function: VHDLGenericParser::findParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> VHDLGenericParser::findParameter(QString const& name, 
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
// Function: VHDLGenericParser::replaceParameterNamesWithIds()
//-----------------------------------------------------------------------------
void VHDLGenericParser::replaceNameReferencesWithParameterIds(QSharedPointer<Parameter> parameter,
    QSharedPointer<Component> targetComponent) const
{
    foreach (QSharedPointer<Parameter> reference, *targetComponent->getParameters())
    {
        QRegularExpression referenceUsage("\\b" + reference->name() + "\\b");

        QString parameterValue = replaceNameWithId(parameter->getValue(), referenceUsage, reference);
        parameter->setValue(parameterValue);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::replaceNameWithId()
//-----------------------------------------------------------------------------
QString VHDLGenericParser::replaceNameWithId(QString const& expression, QRegularExpression& namePattern, 
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

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::replaceNameWithId()
//-----------------------------------------------------------------------------
QString VHDLGenericParser::findMatchingType(QString const& dataType) const
{    
    QMap<QString, QString> conversionMap
    {
        { "integer", "int"},
        { "natural", "int"},
        { "positive", "int"},
        { "real", "real"},
        { "bit", "bit"},
        { "string", "string"}
    };

    return conversionMap.value(dataType);
}
