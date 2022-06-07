//-----------------------------------------------------------------------------
// File: VHDLPortParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.09.2014
//
// Description:
// Parser for VHDL ports.
//-----------------------------------------------------------------------------

#include "VHDLPortParser.h"

#include "VHDLSyntax.h"

#include <KactusAPI/include/ImportColors.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/common/ModuleParameter.h>

#include <QRegularExpression>

namespace
{
    //! VHDL port direction.
    const QString DIRECTION = "in|out|inout|buffer|linkage";

    //! VHDL vector port boundary.
    const QString BOUNDARY = "[\\w\\d )(+*/-]+";

    //! VHDL port type definition is <typename>[(<left> to/downto <right>)].
    const QString PORT_TYPE = "(?:\\w+)"
        "(?:\\s*[(]\\s*(?:" + BOUNDARY + ")\\s+(?:down)?to\\s+(?:" + BOUNDARY + ")\\s*[)])?";

    /*! Port declaration is <port_names> : <direction> <type> [<default>] [pragma] ; [description]    
     *  A pragma e.g. synthesis translate_off may be inserted in the declaration before the ending
     *  semicolon or string's end.
     */
    const QRegularExpression PORT_EXP(
        "(" + VHDLSyntax::NAMES + ")+\\s*[:]\\s*(" + DIRECTION + ")\\s+(" + PORT_TYPE + ")" +
        "(?:\\s*" + VHDLSyntax::DEFAULT + ")?(?:\\s*" + VHDLSyntax::PRAGMA + ")?"
        "(?:" + VHDLSyntax::DECLARATION_END + ")", QRegularExpression::CaseInsensitiveOption);

    //! Ports are declared inside entity by PORT ( <port_declarations> );
    const QRegularExpression PORTS_BEGIN("(PORT)\\s*[(]", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression PORTS_END("[)]\\s*[;](?=\\s*(?:" + VHDLSyntax::COMMENT + "\\s*)*(END|BEGIN|PORT)\\s+)", 
        QRegularExpression::CaseInsensitiveOption);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::VHDLPortParser()
//-----------------------------------------------------------------------------
VHDLPortParser::VHDLPortParser(QObject* parent): QObject(parent), highlighter_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::import()
//-----------------------------------------------------------------------------
void VHDLPortParser::import(QString const& input, QSharedPointer<Component> targetComponent,
    QSharedPointer<ComponentInstantiation> /*targetComponentInstantiation*/)
{
    foreach (QSharedPointer<Port> existingPort, *targetComponent->getPorts())
    {
        existingPort->setDirection(DirectionTypes::DIRECTION_PHANTOM);
    }

    foreach (QString const& portDeclaration, findPortDeclarations(input))
    {
        createPortFromDeclaration(portDeclaration, targetComponent);
        if (highlighter_)
        {
            highlighter_->applyHighlight(portDeclaration, ImportColors::PORT, input);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::setHighlighter()
//-----------------------------------------------------------------------------
void VHDLPortParser::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortDeclarations()
//-----------------------------------------------------------------------------
QStringList VHDLPortParser::findPortDeclarations(QString const& input) const
{
    QString portSectionInEntity = findPortsSection(input);
    QString portSectionWithoutCommentLines = removeCommentLines(portSectionInEntity);

    return portDeclarationsIn(portSectionWithoutCommentLines);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::findPortsSection()
//-----------------------------------------------------------------------------
QString VHDLPortParser::findPortsSection(QString const& input) const
{
    int entityBegin = input.indexOf(VHDLSyntax::ENTITY_BEGIN_EXP);
    int entityEnd = input.indexOf(VHDLSyntax::ENTITY_END_EXP, entityBegin);

    int portsBeginIndex = PORTS_BEGIN.match(input, entityBegin).capturedEnd();

    if (portsBeginIndex > entityEnd)
    {
        return QString();
    }

    int portsEndIndex = input.indexOf(PORTS_END, portsBeginIndex);

    return input.mid(portsBeginIndex, portsEndIndex - portsBeginIndex);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VHDLPortParser::removeCommentLines(QString input) const
{
    QRegularExpression commentLine("^" + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP + "|" +
        VHDLSyntax::ENDLINE + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP);

    return input.remove(commentLine);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortDeclarations()
//-----------------------------------------------------------------------------
QStringList VHDLPortParser::portDeclarationsIn(QString const& portSectionWithoutCommentLines) const
{
    QStringList portDeclarations;

    QRegularExpressionMatchIterator i = PORT_EXP.globalMatch(portSectionWithoutCommentLines);
    while (i.hasNext())
    {
        portDeclarations.append(i.next().captured());
    }

    return portDeclarations;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::createPort()
//-----------------------------------------------------------------------------
void VHDLPortParser::createPortFromDeclaration(QString const& declaration,
    QSharedPointer<Component> targetComponent) const
{
    QStringList portNames = parsePortNames(declaration);
    DirectionTypes::Direction direction =  parsePortDirection(declaration); 
    QString type = parsePortType(declaration);
    QString typeDefinition;
    QString defaultValue = parseDefaultValue(declaration, targetComponent);
    QString description = parseDescription(declaration);

    QPair<QString, QString> bounds = parsePortBounds(declaration, targetComponent);
    QString leftBound = bounds.first;
    QString rightBound = bounds.second;

    foreach(QString const& name, portNames)
    {   
        QSharedPointer<Port> port;        
        if (targetComponent->hasPort(name))
        {
            port = targetComponent->getPort(name);
            typeDefinition = port->getTypeDefinition(type);
        }
        else
        {
            port = QSharedPointer<Port>(new Port());           
            port->setName(name);
            typeDefinition = createDefaultTypeDefinition(type);
            targetComponent->getPorts()->append(port);
        }

        port->setDirection(direction);
        port->setLeftBound(leftBound);
        port->setRightBound(rightBound);
        port->setTypeName(type);
        port->setTypeDefinition(type, typeDefinition);
        port->setDefaultValue(defaultValue);
        port->setDescription(description);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortNames()
//-----------------------------------------------------------------------------
QStringList VHDLPortParser::parsePortNames(QString const& declaration) const
{
    QString portNames = PORT_EXP.match(declaration).captured(1);
    return portNames.split(QRegExp("\\s*[,]\\s*"), QString::SkipEmptyParts);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction VHDLPortParser::parsePortDirection(QString const& declaration) const
{
    QString direction = PORT_EXP.match(declaration).captured(2).toLower();    
    return DirectionTypes::str2Direction(direction, DirectionTypes::DIRECTION_INVALID);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortType()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parsePortType(QString const& declaration) const
{
    QString typePattern = PORT_TYPE;
    typePattern.replace("(?:","(");

    QRegularExpression typeExpression(typePattern, QRegularExpression::CaseInsensitiveOption);    
   
    QString fullType = PORT_EXP.match(declaration).captured(3);
    return typeExpression.match(fullType).captured(1);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseDescription()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseDescription(QString const& declaration) const
{
    QString description = PORT_EXP.match(declaration).captured(6).trimmed();
    if (description.isEmpty())
    {
        description = PORT_EXP.match(declaration).captured(7).trimmed();
    }

    return description;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseDefaultValue()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseDefaultValue(QString const& input, QSharedPointer<Component> targetComponent) const
{
    return replaceNameReferencesWithIds(PORT_EXP.match(input).captured(4).trimmed(), targetComponent);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::replaceNameReferencesWithIds()
//-----------------------------------------------------------------------------
QString VHDLPortParser::replaceNameReferencesWithIds(QString const& expression, 
    QSharedPointer<Component> targetComponent) const
{
    QString result = expression;

    foreach (QSharedPointer<Parameter> define, *targetComponent->getParameters())
    {
        QRegularExpression macroUsage("\\b" + define->name() + "\\b");
        if (macroUsage.match(result).hasMatch())
        {
            result.replace(macroUsage, define->getValueId());

            for(int i = 0; i < expression.count(macroUsage); i++)
            {
                define->increaseUsageCount();
            }
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortBounds()
//-----------------------------------------------------------------------------
QPair<QString, QString> VHDLPortParser::parsePortBounds(QString const& declaration, 
    QSharedPointer<Component> targetComponent) const
{
    QString fullType = PORT_EXP.match(declaration).captured(3);

    QString leftValue = QString();
    QString rightValue = QString();

    int firstParenthesis = fullType.indexOf(QLatin1Char('('));
    if (firstParenthesis != -1)
    {
        int endParenthesis = findMatchingEndParenthesis(fullType, firstParenthesis);
        QString vectorBounds = fullType.mid(firstParenthesis + 1, endParenthesis - firstParenthesis - 1);

        if (!vectorBounds.isEmpty())
        {
            QRegularExpression splitter(QString("\\s+(down)?to\\s+"), QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch splitMatch = splitter.match(vectorBounds);
            QString leftEquation = vectorBounds.left(splitMatch.capturedStart());
            QString rightEquation = vectorBounds.mid(splitMatch.capturedEnd());
            
            leftValue = replaceNameReferencesWithIds(leftEquation, targetComponent);
            rightValue = replaceNameReferencesWithIds(rightEquation, targetComponent);
        }
    }

    return qMakePair(leftValue, rightValue);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::findMatchingEndParenthesis()
//-----------------------------------------------------------------------------
int VHDLPortParser::findMatchingEndParenthesis(QString const& equation, int parenthesesStart) const
{
    QRegularExpression parenthesesExpression("[()]");

    int position = parenthesesStart + 1;
    int depth = 1;

    while (depth > 0)
    {
        position = equation.indexOf(parenthesesExpression, position);
        if (parenthesesExpression.match(equation.at(position)).captured() == "(")
        {
            depth++;
        }
        else
        {
            depth--;
        }
        position++;
    }

    return position - 1;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::getDefaultTypeDefinition()
//-----------------------------------------------------------------------------
QString VHDLPortParser::createDefaultTypeDefinition(QString const& type) const
{
    QRegularExpression defaultLogicTypes("std_(u)?logic(_vector)?", 
        QRegularExpression::CaseInsensitiveOption);

    QRegularExpression defaultNumericTypes("(un)?signed", 
        QRegularExpression::CaseInsensitiveOption);

    if (defaultLogicTypes.match(type).hasMatch())
    {
        return QStringLiteral("IEEE.std_logic_1164.all");
    }
    else if (defaultNumericTypes.match(type).hasMatch())
    {
        return QStringLiteral("IEEE.numeric_std.all");
    }
    else
    {
        return QString();
    }
}
