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

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/common/ModuleParameter.h>

#include <QRegularExpression>

namespace
{
    //! VHDL port direction.
    const QString DIRECTION = "in|out|inout|buffer|linkage";

    //! VHDL port type definition is <typename>[(<left> downto <right>)].
    const QString PORT_TYPE = "(?:\\w+)(?:\\s*[(]\\s*(?:" + HDLmath::TERM +
        ")\\s+\\w+\\s+(?:" + HDLmath::TERM + ")\\s*[)])?";

    //! Vector bounds are defined as (<left> downto <right>).
    const QRegularExpression VECTOR_BOUNDS("[(]\\s*(" + HDLmath::TERM + ")\\s+\\w+\\s+" + 
        "(" + HDLmath::TERM + ")\\s*[)]");

    /*! Port declaration is <port_names> : <direction> <type> [<default>] [pragma] ; [description]    
     *  A pragma e.g. synthesis translate_off may be inserted in the declaration before the ending
     *  semicolon or string's end.
     */
    const QRegularExpression PORT_EXP("(" + VHDLSyntax::NAMES + ")+\\s*[:]\\s*(" + DIRECTION + ")\\s+(" + PORT_TYPE + ")" +
                                     "(?:\\s*" + VHDLSyntax::DEFAULT + ")?(?:\\s*" + VHDLSyntax::PRAGMA + ")?"
                                     "(?:" + VHDLSyntax::DECLARATION_END + ")", QRegularExpression::CaseInsensitiveOption);

    //! Ports are declared inside entity by PORT ( <port_declarations> );
    const QRegularExpression PORTS_BEGIN("(PORT)\\s*[(]", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression PORTS_END("[)]\\s*[;](?=\\s*(?:" + VHDLSyntax::COMMENT + "\\s*)*(END|BEGIN|PORT)\\s+)", 
        QRegularExpression::CaseInsensitiveOption);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::()
//-----------------------------------------------------------------------------
VHDLPortParser::VHDLPortParser(QObject* parent): QObject(parent), highlighter_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::()
//-----------------------------------------------------------------------------
VHDLPortParser::~VHDLPortParser()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parse()
//-----------------------------------------------------------------------------
void VHDLPortParser::import(QString const& input, QSharedPointer<Component> targetComponent,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    foreach (QSharedPointer<Port> existingPort, *targetComponent->getPorts())
    {
        existingPort->setDirection(DirectionTypes::DIRECTION_PHANTOM);
    }

    foreach (QString portDeclaration, findPortDeclarations(input))
    {
        createPortFromDeclaration(portDeclaration, targetComponent, targetComponentInstantiation);
        if (highlighter_)
        {
            highlighter_->applyHighlight(portDeclaration, ImportColors::PORT);
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
// Function: VHDLPortParser::parseLeftBound()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseLeftBound(QString const& input,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const
{
    QString vectorBounds = parseVectorBounds(input);

    QString leftValue = parseLeftValue(vectorBounds, targetComponentInstantiation);

    return leftValue;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseRightBound()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseRightBound(QString const& input,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const
{
    QString vectorBounds = parseVectorBounds(input);

    QString rightValue = parseRightValue(vectorBounds, targetComponentInstantiation);

    return rightValue;
}    

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseDefaultValue()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseDefaultValue(QString const& input) const
{
    return PORT_EXP.match(input).captured(4).trimmed();
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

    int nextPort = portSectionWithoutCommentLines.indexOf(PORT_EXP, 0);
    while (nextPort != -1)
    {
        QRegularExpressionMatch match = PORT_EXP.match(portSectionWithoutCommentLines, nextPort);
        portDeclarations.append(match.captured());

        nextPort = portSectionWithoutCommentLines.indexOf(PORT_EXP, nextPort + match.capturedLength());
    }

    return portDeclarations;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::createPort()
//-----------------------------------------------------------------------------
void VHDLPortParser::createPortFromDeclaration(QString const& declaration, QSharedPointer<Component> targetComponent,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    QStringList portNames = parsePortNames(declaration);
    DirectionTypes::Direction direction =  parsePortDirection(declaration); 
    QString type = parsePortType(declaration);
    QString typeDefinition;
    QString defaultValue = parseDefaultValue(declaration);
    QString description = parseDescription(declaration);

    QString leftBound = parseLeftBound(declaration, targetComponentInstantiation);
    QString rightBound = parseRightBound(declaration, targetComponentInstantiation);

    foreach(QString name, portNames)
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
            targetComponent->getPorts()->append(port);
        }

        port->setDirection(direction);
        port->setLeftBound(leftBound);
        port->setRightBound(rightBound);
        port->setTypeName(type);
        port->setTypeDefinition(type, typeDefinition);
        port->setDefaultValue(defaultValue);
        port->setDescription(description);

        emit add(port, declaration);
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
    QString fullType = PORT_EXP.match(declaration).captured(3);

    QString typePattern = PORT_TYPE;
    typePattern.replace("(?:","(");

    QRegExp typeExpression(typePattern, Qt::CaseInsensitive);
    typeExpression.indexIn(fullType);
    
    return typeExpression.cap(1); 
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseDescription()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseDescription(QString const& declaration)
{
    QString description = PORT_EXP.match(declaration).captured(6).trimmed();

    if (description.isEmpty())
    {
        description = PORT_EXP.match(declaration).captured(7).trimmed();
    }

    return description;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseLeftValue()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseLeftValue(QString const& vectorBounds,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const
{
    QString value = "";

    if(!vectorBounds.isEmpty() && vectorBounds.indexOf(VECTOR_BOUNDS) != -1 )
    {
        QString leftEquation = VECTOR_BOUNDS.match(vectorBounds).captured(1);

        value = leftEquation;
    }	

    return value;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseRightValue()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseRightValue(QString const& vectorBounds,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const
{
    QString value = "";

    if(!vectorBounds.isEmpty() && vectorBounds.indexOf(VECTOR_BOUNDS) != -1 )
    {
        QString rightEquation = VECTOR_BOUNDS.match(vectorBounds).captured(2);

        value = rightEquation;
    }	

    return value;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortBounds()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseVectorBounds(QString const& declaration) const
{
    QString fullType = PORT_EXP.match(declaration).captured(3);

    QString typePattern = PORT_TYPE;
    typePattern.replace("(?:","(");

    QRegularExpression typeExpression(typePattern, QRegularExpression::CaseInsensitiveOption);
    
    return typeExpression.match(fullType).captured(2);
}
