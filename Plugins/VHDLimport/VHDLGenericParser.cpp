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

#include "VhdlSyntax.h"

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>

namespace
{
    //! Generics are declared inside entity by GENERIC ( <generic_declarations> );
    const QRegExp GENERICS_BEGIN_EXP = QRegExp("(GENERIC)\\s*[(]", Qt::CaseInsensitive);    

    const QRegExp GENERICS_END_EXP = QRegExp("[)]\\s*[;](?=(?:\\s*(" + VHDLSyntax::COMMENT + ")\\s*)*(END|BEGIN|PORT))*");   

    //! Generic declaration is <generic_names> : <type> [<default>] [pragma]; [description]    
    const QRegExp GENERIC_EXP = QRegExp("(" + VHDLSyntax::NAMES + ")\\s*[:]\\s*(\\w+)(?:\\s*" + VHDLSyntax::DEFAULT + ")?" +
        "(?:\\s*" + VHDLSyntax::PRAGMA + ")?(?:" + VHDLSyntax::DECLARATION_END + ")", 
        Qt::CaseInsensitive);
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::VHDLGenericParser()
//-----------------------------------------------------------------------------
VHDLGenericParser::VHDLGenericParser(QObject* parent): QObject(parent), generics_(), 
    highlighter_(0), genericVisualizer_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::VHDLGenericParser()
//-----------------------------------------------------------------------------
VHDLGenericParser::~VHDLGenericParser()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::parse()
//-----------------------------------------------------------------------------
void VHDLGenericParser::runParser(QString const& input, QSharedPointer<Component> targetComponent)
{
    foreach(QString declaration, findGenericDeclarations(input))
    {
        createModelParameterFromDeclaration(declaration, targetComponent);
        if (highlighter_)
        {
            highlighter_->applyHighlight(declaration, ImportColors::MODELPARAMETER);
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
// Function: VHDLGenericParser::setModelParameterVisualizer()
//-----------------------------------------------------------------------------
void VHDLGenericParser::setModelParameterVisualizer(ModelParameterVisualizer* visualizer)
{
    genericVisualizer_ = visualizer;
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::removePreviousGenerics()
//-----------------------------------------------------------------------------
void VHDLGenericParser::removePreviousGenerics()
{
    if (genericVisualizer_)
    {
        foreach (QSharedPointer<ModelParameter> parsedGeneric, generics_)
        {
            genericVisualizer_->removeModelParameter(parsedGeneric);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::createModelParameterFromDeclaration()
//-----------------------------------------------------------------------------
void VHDLGenericParser::createModelParameterFromDeclaration(QString const& declaration, 
    QSharedPointer<Component> targetComponent)
{
    GENERIC_EXP.indexIn(declaration);
    QStringList genericNames = GENERIC_EXP.cap(1).split(QRegExp("\\s*[,]\\s*"), QString::SkipEmptyParts);
    QString type = GENERIC_EXP.cap(2);
    QString defaultValue = GENERIC_EXP.cap(3);

    QString description = GENERIC_EXP.cap(4).trimmed();
    if (description.isEmpty())
    {
        description = GENERIC_EXP.cap(5).trimmed();
    }

    foreach(QString name, genericNames)
    {   
        QSharedPointer<ModelParameter> parameter(new ModelParameter());
        parameter->setName(name.trimmed());
        parameter->setDataType(type);
        parameter->setDescription(description);
        parameter->setValue(defaultValue);
        parameter->setUsageType("nontyped");

        if (genericVisualizer_)
        {
            genericVisualizer_->addModelParameter(parameter);
        }

        targetComponent->getModel()->addModelParameter(parameter);
        generics_.append(parameter);
    } 
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VHDLGenericParser::removeCommentLines(QString section) const
{
    QRegExp commentLine("^" + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP.pattern() + "|" +
        VHDLSyntax::ENDLINE + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP.pattern());

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
    int entityBegin = VHDLSyntax::ENTITY_BEGIN_EXP.indexIn(input);
    int entityEnd = VHDLSyntax::ENTITY_END_EXP.indexIn(input, entityBegin);

    int genericsBeginIndex = GENERICS_BEGIN_EXP.indexIn(input);
    genericsBeginIndex += GENERICS_BEGIN_EXP.matchedLength();

    if (genericsBeginIndex > entityEnd)
    {
        return QString();
    }

    int genericsEndIndex = GENERICS_END_EXP.indexIn(input, genericsBeginIndex);

    return input.mid(genericsBeginIndex, genericsEndIndex - genericsBeginIndex);
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::genericDeclarationsIn()
//-----------------------------------------------------------------------------
QStringList VHDLGenericParser::genericDeclarationsIn(QString const& sectionWithoutCommentLines) const
{
    QStringList genericDeclarations;

    int nextGeneric = GENERIC_EXP.indexIn(sectionWithoutCommentLines, 0);
    while (nextGeneric != -1)
    {
        genericDeclarations.append(GENERIC_EXP.cap());
        nextGeneric = GENERIC_EXP.indexIn(sectionWithoutCommentLines, nextGeneric + GENERIC_EXP.matchedLength());
    }

    return genericDeclarations;
}
