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

#include <common/KactusColors.h>
#include <common/widgets/vhdlParser/VhdlSyntax.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>

namespace
{
    //! Generics are declared inside entity by GENERIC ( <generic_declarations> );
    const QRegExp GENERICS_BEGIN_EXP = QRegExp("(GENERIC)\\s*[(]", Qt::CaseInsensitive);    

    const QRegExp GENERICS_END_EXP = QRegExp("[)]\\s*[;](?=(?:\\s*(" + VhdlSyntax::COMMENT + ")\\s*)*(END|BEGIN|PORT))*");   

    //! Generic declaration is <generic_names> : <type> [<default>] [pragma]; [description]    
    const QRegExp GENERIC_EXP = QRegExp("(" + VhdlSyntax::NAMES + ")\\s*[:]\\s*(\\w+)(?:\\s*" + VhdlSyntax::DEFAULT + ")?" +
        "(?:\\s*" + VhdlSyntax::PRAGMA + ")?(?:" + VhdlSyntax::DECLARATION_END + ")", 
        Qt::CaseInsensitive);
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::VHDLGenericParser()
//-----------------------------------------------------------------------------
VHDLGenericParser::VHDLGenericParser(QObject* parent): QObject(parent)
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
        emit highlight(declaration, KactusColors::HW_BUS_COMPONENT);
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

        emit add(parameter, declaration);
        targetComponent->getModel()->addModelParameter(parameter);
    } 
}

//-----------------------------------------------------------------------------
// Function: VHDLGenericParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VHDLGenericParser::removeCommentLines(QString section) const
{
    QRegExp commentLine("^" + VhdlSyntax::SPACE + VhdlSyntax::COMMENT_LINE_EXP.pattern() + "|" +
        VhdlSyntax::ENDLINE + VhdlSyntax::SPACE + VhdlSyntax::COMMENT_LINE_EXP.pattern());

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
    int beginIndex = GENERICS_BEGIN_EXP.indexIn(input);
    beginIndex += GENERICS_BEGIN_EXP.matchedLength();

    int endIndex = GENERICS_END_EXP.indexIn(input, beginIndex);

    return input.mid(beginIndex, endIndex - beginIndex);
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
