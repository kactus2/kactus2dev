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


#include <common/KactusColors.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/modelparameter.h>

#include <common/widgets/vhdlParser/VhdlSyntax.h>

namespace
{
    //! VHDL port direction.
    const QString DIRECTION = "in|out|inout|buffer|linkage";

    //! VHDL port type definition is <typename>[(<left> downto <right>)].
    const QString PORT_TYPE = "(?:\\w+)(?:\\s*[(]\\s*(?:" + VhdlSyntax::MATH_EXP +
        ")\\s+\\w+\\s+(?:" + VhdlSyntax::MATH_EXP + ")\\s*[)])?";

    //! Type definition in port declaration is <typename>[(<left> downto <right>)].
    const QRegExp TYPE_EXP = QRegExp(PORT_TYPE);

        /*! Port declaration is <port_names> : <direction> <type> [<default>] [pragma] ; [description]    
     *  A pragma e.g. synthesis translate_off may be inserted in the declaration before the ending
     *  semicolon or string's end.
     */
    const QRegExp PORT_EXP = QRegExp("(" + VhdlSyntax::NAMES + ")+\\s*[:]\\s*(" + DIRECTION + ")\\s+(" + PORT_TYPE + ")" +
                                     "(?:\\s*" + VhdlSyntax::DEFAULT + ")?(?:\\s*" + VhdlSyntax::PRAGMA + ")?(?:" + VhdlSyntax::DECLARATION_END + ")",
                                     Qt::CaseInsensitive);

    //! Ports are declared inside entity by PORT ( <port_declarations> );
    const QRegExp PORTS_BEGIN_EXP = QRegExp("(PORT)\\s*[(]", Qt::CaseInsensitive);
    const QRegExp PORTS_END_EXP = QRegExp("[)]\\s*[;](?=\\s*(?:" + VhdlSyntax::COMMENT + "\\s*)*(END|BEGIN|GENERIC|PORT))", 
        Qt::CaseInsensitive);

    const QRegExp RANGE_EXP = QRegExp("[(]\\s*(" + VhdlSyntax::MATH_EXP + ")\\s+\\w+\\s+" + 
        "(" + VhdlSyntax::MATH_EXP + ")\\s*[)]");
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::()
//-----------------------------------------------------------------------------
VHDLPortParser::VHDLPortParser(QObject* parent): QObject(parent)
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
void VHDLPortParser::runParser(QString const& input, QSharedPointer<Component> targetComponent)
{
    foreach (QString portDeclaration, findPortDeclarations(input))
    {
        createPortFromDeclaration(portDeclaration, targetComponent);
        emit highlight(portDeclaration, KactusColors::SW_COMPONENT);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseLeftBound()
//-----------------------------------------------------------------------------
int VHDLPortParser::parseLeftBound(QString const& input, QSharedPointer<Component> ownerComponent) const
{
    QString vectorBounds = parseVectorBounds(input);

    int leftValue = parseLeftValue(vectorBounds, ownerComponent);

    return leftValue;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseRightBound()
//-----------------------------------------------------------------------------
int VHDLPortParser::parseRightBound(QString const& input, QSharedPointer<Component> ownerComponent) const
{
    QString vectorBounds = parseVectorBounds(input);

    int rightValue = parseRightValue(vectorBounds, ownerComponent);

    return rightValue;
}    

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseDefaultValue()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseDefaultValue(QString const& input) const
{
    PORT_EXP.indexIn(input);
    return PORT_EXP.cap(4).trimmed();
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
    int beginIndex = PORTS_BEGIN_EXP.indexIn(input);
    beginIndex += PORTS_BEGIN_EXP.matchedLength();

    int endIndex = PORTS_END_EXP.indexIn(input, beginIndex);

    return input.mid(beginIndex, endIndex - beginIndex);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VHDLPortParser::removeCommentLines(QString input) const
{
    QRegExp commentLine("^" + VhdlSyntax::SPACE + VhdlSyntax::COMMENT_LINE_EXP.pattern() + "|" +
        VhdlSyntax::ENDLINE + VhdlSyntax::SPACE + VhdlSyntax::COMMENT_LINE_EXP.pattern());

    return input.remove(commentLine);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortDeclarations()
//-----------------------------------------------------------------------------
QStringList VHDLPortParser::portDeclarationsIn(QString const& portSectionWithoutCommentLines) const
{
    QStringList portDeclarations;

    int nextPort = PORT_EXP.indexIn(portSectionWithoutCommentLines, 0);
    while (nextPort != -1)
    {
        portDeclarations.append(PORT_EXP.cap());

        nextPort = PORT_EXP.indexIn(portSectionWithoutCommentLines, nextPort + PORT_EXP.matchedLength());
    }

    return portDeclarations;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::createPort()
//-----------------------------------------------------------------------------
void VHDLPortParser::createPortFromDeclaration(QString const& declaration, QSharedPointer<Component> targetComponent)
{
    QStringList portNames = parsePortNames(declaration);
    General::Direction direction =  parsePortDirection(declaration); 
    QString type = parsePortType(declaration);
    QString defaultValue = parseDefaultValue(declaration);
    QString description = parseDescription(declaration);

    int leftBound = parseLeftBound(declaration, targetComponent);
    int rightBound = parseRightBound(declaration, targetComponent);;

    foreach(QString name, portNames)
    {   
        QSharedPointer<Port> port(new Port(name, direction, leftBound, rightBound, type, "", defaultValue, 
            description));
        
        emit add(port, declaration);
        targetComponent->addPort(port);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortNames()
//-----------------------------------------------------------------------------
QStringList VHDLPortParser::parsePortNames(QString const& declaration) const
{
    PORT_EXP.indexIn(declaration);

    QString portNames = PORT_EXP.cap(1);
    return portNames.split(QRegExp("\\s*[,]\\s*"), QString::SkipEmptyParts);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortDirection()
//-----------------------------------------------------------------------------
General::Direction VHDLPortParser::parsePortDirection(QString const& declaration) const
{
    PORT_EXP.indexIn(declaration);
    QString direction = PORT_EXP.cap(2).toLower();
    
    return General::str2Direction(direction, General::DIRECTION_INVALID);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortType()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parsePortType(QString const& declaration) const
{
    PORT_EXP.indexIn(declaration);
    QString fullType = PORT_EXP.cap(3);

    QRegExp typeExpression = QRegExp(TYPE_EXP.pattern().replace("(?:","("), Qt::CaseInsensitive);
    typeExpression.indexIn(fullType);
    
    return typeExpression.cap(1); 
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseDescription()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseDescription(QString const& declaration)
{
    PORT_EXP.indexIn(declaration);
    
    QString description = PORT_EXP.cap(5).trimmed();

    if (description.isEmpty())
    {
        description = PORT_EXP.cap(6).trimmed();
    }

    return description;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseLeftValue()
//-----------------------------------------------------------------------------
int VHDLPortParser::parseLeftValue(QString const& vectorBounds, QSharedPointer<Component> ownerComponent) const
{
    int value = 0;

    if(!vectorBounds.isEmpty() && RANGE_EXP.indexIn(vectorBounds) != -1 )
    {
        QString leftEquation = RANGE_EXP.cap(1);

        EquationParser equationParser(ownerComponent->getModelParameters());
        value = equationParser.parse(leftEquation);
    }	

    return value;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parseRightValue()
//-----------------------------------------------------------------------------
int VHDLPortParser::parseRightValue(QString const& vectorBounds, QSharedPointer<Component> ownerComponent) const
{
    int value = 0;

    if(!vectorBounds.isEmpty() && RANGE_EXP.indexIn(vectorBounds) != -1 )
    {
        QString rightEquation = RANGE_EXP.cap(2);

        EquationParser equationParser(ownerComponent->getModelParameters());
        value = equationParser.parse(rightEquation);
    }	

    return value;
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::parsePortBounds()
//-----------------------------------------------------------------------------
QString VHDLPortParser::parseVectorBounds(QString const& declaration) const
{
    PORT_EXP.indexIn(declaration);
    QString fullType = PORT_EXP.cap(3);

    QRegExp typeExpression = QRegExp(TYPE_EXP.pattern().replace("(?:","("), Qt::CaseInsensitive);
    typeExpression.indexIn(fullType);
    
    return typeExpression.cap(2);
}
