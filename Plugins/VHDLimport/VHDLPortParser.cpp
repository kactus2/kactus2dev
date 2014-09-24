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

#include "VhdlSyntax.h"

namespace
{
    //! VHDL port direction.
    const QString DIRECTION = "in|out|inout|buffer|linkage";

    //! VHDL port type definition is <typename>[(<left> downto <right>)].
    const QString PORT_TYPE = "(?:\\w+)(?:\\s*[(]\\s*(?:" + VHDLSyntax::MATH_EXP +
        ")\\s+\\w+\\s+(?:" + VHDLSyntax::MATH_EXP + ")\\s*[)])?";

    //! Vector bounds are defined as (<left> downto <right>).
    const QRegExp VECTOR_BOUNDS_EXP("[(]\\s*(" + VHDLSyntax::MATH_EXP + ")\\s+\\w+\\s+" + 
        "(" + VHDLSyntax::MATH_EXP + ")\\s*[)]");

    /*! Port declaration is <port_names> : <direction> <type> [<default>] [pragma] ; [description]    
     *  A pragma e.g. synthesis translate_off may be inserted in the declaration before the ending
     *  semicolon or string's end.
     */
    const QRegExp PORT_EXP("(" + VHDLSyntax::NAMES + ")+\\s*[:]\\s*(" + DIRECTION + ")\\s+(" + PORT_TYPE + ")" +
                                     "(?:\\s*" + VHDLSyntax::DEFAULT + ")?(?:\\s*" + VHDLSyntax::PRAGMA + ")?"
                                     "(?:" + VHDLSyntax::DECLARATION_END + ")", Qt::CaseInsensitive);

    //! Ports are declared inside entity by PORT ( <port_declarations> );
    const QRegExp PORTS_BEGIN_EXP("(PORT)\\s*[(]", Qt::CaseInsensitive);
    const QRegExp PORTS_END_EXP("[)]\\s*[;](?=\\s*(?:" + VHDLSyntax::COMMENT + "\\s*)*(END|BEGIN|PORT)\\s+)", 
        Qt::CaseInsensitive);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::()
//-----------------------------------------------------------------------------
VHDLPortParser::VHDLPortParser(QObject* parent): QObject(parent), ports_(), highlighter_(0), portVisualizer_(0)
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
        if (highlighter_)
        {
            highlighter_->applyHighlight(portDeclaration, KactusColors::SW_COMPONENT);
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
// Function: VHDLPortParser::setPortVisualizer()
//-----------------------------------------------------------------------------
void VHDLPortParser::setPortVisualizer(PortVisualizer* visualizer)
{
    portVisualizer_ = visualizer;
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
// Function: VHDLPortParser::clear()
//-----------------------------------------------------------------------------
void VHDLPortParser::removePreviousPorts()
{
    if (portVisualizer_)
    {
        foreach (QSharedPointer<Port> port, ports_)
        {
            portVisualizer_->removePort(port);
        }
    }

    ports_.clear();
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
    int entityBegin = VHDLSyntax::ENTITY_BEGIN_EXP.indexIn(input);
    int entityEnd = VHDLSyntax::ENTITY_END_EXP.indexIn(input, entityBegin);

    int portsBeginIndex = PORTS_BEGIN_EXP.indexIn(input, entityBegin);
    portsBeginIndex += PORTS_BEGIN_EXP.matchedLength();

    if (portsBeginIndex > entityEnd)
    {
        return QString();
    }

    int portsEndIndex = PORTS_END_EXP.indexIn(input, portsBeginIndex);

    return input.mid(portsBeginIndex, portsEndIndex - portsBeginIndex);
}

//-----------------------------------------------------------------------------
// Function: VHDLPortParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VHDLPortParser::removeCommentLines(QString input) const
{
    QRegExp commentLine("^" + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP.pattern() + "|" +
        VHDLSyntax::ENDLINE + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP.pattern());

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
        
        ports_.append(port);
        emit add(port, declaration);

        if (portVisualizer_)
        {
            portVisualizer_->addPort(port);
        }
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

    if(!vectorBounds.isEmpty() && VECTOR_BOUNDS_EXP.indexIn(vectorBounds) != -1 )
    {
        QString leftEquation = VECTOR_BOUNDS_EXP.cap(1);

        VHDLEquationParser equationParser(ownerComponent->getModelParameters());
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

    if(!vectorBounds.isEmpty() && VECTOR_BOUNDS_EXP.indexIn(vectorBounds) != -1 )
    {
        QString rightEquation = VECTOR_BOUNDS_EXP.cap(2);

        VHDLEquationParser equationParser(ownerComponent->getModelParameters());
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

    QString typePattern = PORT_TYPE;
    typePattern.replace("(?:","(");

    QRegExp typeExpression(typePattern, Qt::CaseInsensitive);
    typeExpression.indexIn(fullType);
    
    return typeExpression.cap(2);
}