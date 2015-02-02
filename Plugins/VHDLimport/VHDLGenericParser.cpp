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

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>

#include <QRegularExpression>

namespace
{
    //! Generics are declared inside entity by GENERIC ( <generic_declarations> );
    const QRegularExpression GENERICS_BEGIN_EXP = QRegularExpression("(GENERIC)\\s*[(]", 
        QRegularExpression::CaseInsensitiveOption);    

    const QRegularExpression GENERICS_END_EXP = QRegularExpression("[)]\\s*[;](?=(?:\\s*(" + VHDLSyntax::COMMENT +
        ")\\s*)*(END|BEGIN|PORT))*", QRegularExpression::CaseInsensitiveOption);   

    //! Generic declaration is <generic_names> : <type> [<default>] [pragma]; [description]
    const QRegularExpression GENERIC_EXP = QRegularExpression("(" + VHDLSyntax::NAMES + ")\\s*[:]\\s*(\\w+)(?:\\s*"
        + VHDLSyntax::DEFAULT + ")?" + "(?:\\s*" + VHDLSyntax::PRAGMA + ")?(?:" + VHDLSyntax::DECLARATION_END + ")", 
        QRegularExpression::CaseInsensitiveOption);
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::VHDLGenericParser()
//-----------------------------------------------------------------------------
VHDLGenericParser::VHDLGenericParser(QObject* parent): QObject(parent), highlighter_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::VHDLGenericParser()
//-----------------------------------------------------------------------------
VHDLGenericParser::~VHDLGenericParser()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::import()
//-----------------------------------------------------------------------------
void VHDLGenericParser::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    foreach (QSharedPointer<ModelParameter> modelParameter, targetComponent->getModelParameters())
    {
        modelParameter->setAttribute("kactus2:import", "no");
    }

    foreach(QString declaration, findGenericDeclarations(input))
    {
        createModelParameterFromDeclaration(declaration, targetComponent);
        if (highlighter_)
        {
            highlighter_->applyHighlight(declaration, ImportColors::MODELPARAMETER);
        }        
    }

    foreach (QSharedPointer<ModelParameter> modelParameter, targetComponent->getModelParameters())
    {
        if (modelParameter->getAttribute("kactus2:import") == "no")
        {
            targetComponent->getModelParameters().removeAll(modelParameter);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::setHighlighter()
//-----------------------------------------------------------------------------
void VHDLGenericParser::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::createModelParameterFromDeclaration()
//-----------------------------------------------------------------------------
void VHDLGenericParser::createModelParameterFromDeclaration(QString const& declaration, 
    QSharedPointer<Component> targetComponent)
{
    QRegularExpressionMatch matchedDeclaration = GENERIC_EXP.match(declaration);

    QString cap = matchedDeclaration.captured();

    QStringList genericNames = matchedDeclaration.captured(1).split(QRegularExpression("\\s*[,]\\s*"), QString::SkipEmptyParts);
    QString type = matchedDeclaration.captured(2);
    QString defaultValue = matchedDeclaration.captured(3);

    QString description = matchedDeclaration.captured(5).trimmed();
    if (description.isEmpty())
    {
        description = matchedDeclaration.captured(6).trimmed();
    }

    foreach(QString name, genericNames)
    {   
        QSharedPointer<ModelParameter> parameter = targetComponent->getModel()->getModelParameter(name.trimmed());
        if (parameter.isNull())
        {
            parameter = QSharedPointer<ModelParameter>(new ModelParameter());
            targetComponent->getModel()->addModelParameter(parameter);
        }

        parameter->setName(name.trimmed());
        parameter->setDataType(type);
        parameter->setDescription(description);
        parameter->setValue(defaultValue);
        parameter->setUsageType("nontyped");
        parameter->setAttribute("kactus2:import", "");
    } 
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VHDLGenericParser::removeCommentLines(QString section) const
{
    QRegExp commentLine("^" + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP + "|" +
        VHDLSyntax::ENDLINE + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP);

    return section.remove(commentLine);
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
// Function: VHDLGenericParser::genericDeclarationsIn()
//-----------------------------------------------------------------------------
QStringList VHDLGenericParser::genericDeclarationsIn(QString const& sectionWithoutCommentLines) const
{
    QStringList genericDeclarations;

    int nextGeneric = sectionWithoutCommentLines.indexOf(GENERIC_EXP, 0);
    while (nextGeneric != -1)
    {
        QRegularExpressionMatch match = GENERIC_EXP.match(sectionWithoutCommentLines, nextGeneric);
        genericDeclarations.append(match.captured());
        nextGeneric = sectionWithoutCommentLines.indexOf(GENERIC_EXP, nextGeneric + match.capturedLength());
    }

    return genericDeclarations;
}
