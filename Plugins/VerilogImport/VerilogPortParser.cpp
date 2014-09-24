//-----------------------------------------------------------------------------
// File: VerilogPortParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.09.2014
//
// Description:
// Parser for Verilog ports.
//-----------------------------------------------------------------------------

#include "VerilogPortParser.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>

#include <QString>
#include <QRegExp>

namespace
{
    const QString DIRECTION("input|output|inout");
    const QString TYPE("wire|reg|integer|time|tri|tri0|tri1|triand|trior|trireg|supply0|supply1|wand|wor|logic");
    const QString RANGE("[\\[]\\w+\\s*[:]\\s*\\w+[\\]]");
    const QString NAMES("\\w+(?:\\s*[,]\\s*\\w+)*");
    const QString COMMENT("//[ \\t]*([^\\r?\\n]*)(?=\\r?\\n)");

    const QRegExp PORT_EXP("(" + DIRECTION + ")\\s+(" + TYPE + ")?\\s*(?:signed)?\\s*(" + RANGE + ")?\\s*"
        "(" + NAMES + ")\\s*(?:[;]\\s*(?:"+ COMMENT + ")?|(?:"+ COMMENT + ")?\\s*$)");
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::VerilogPortParser()
//-----------------------------------------------------------------------------
VerilogPortParser::VerilogPortParser()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::~VerilogPortParser()
//-----------------------------------------------------------------------------
VerilogPortParser::~VerilogPortParser()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::runParser()
//-----------------------------------------------------------------------------
void VerilogPortParser::runParser(QString const& input, QSharedPointer<Component> targetComponent)
{
    foreach(QString portDeclaration, findPortDeclarations(input))
    {
        createPortFromDeclaration(portDeclaration, targetComponent);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findPortDeclarations()
//-----------------------------------------------------------------------------
QStringList VerilogPortParser::findPortDeclarations(QString const& input) const
{
    QString portSection = findPortsSection(input);
    portSection = removeCommentLines(portSection);

    return portDeclarationsIn(portSection);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findPortsSection()
//-----------------------------------------------------------------------------
QString VerilogPortParser::findPortsSection(QString const& input) const
{
    QRegExp moduleBegin("module\\s+\\w+\\s*(#\\s*[(][^)]*[)])?\\s*[(]\\s*", Qt::CaseInsensitive);
    QRegExp moduleEnd("endmodule", Qt::CaseInsensitive);
    QRegExp portsEnd("[)];");

    int beginIndex = moduleBegin.indexIn(input);
    int endOfPorts = portsEnd.indexIn(input, beginIndex);
    int moduleEnding = moduleEnd.indexIn(input, endOfPorts);

    if (moduleEnding == -1 || beginIndex > endOfPorts)
    {
        return QString();
    }

    return input.mid(beginIndex, endOfPorts - beginIndex);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VerilogPortParser::removeCommentLines(QString portSection) const
{
    QRegExp commentExp("(^|\\r?\\n)[ \\t]*//([^\\r?\\n]*)(?=\\r?\\n)");
    return portSection.remove(commentExp);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::portDeclarationsIn()
//-----------------------------------------------------------------------------
QStringList VerilogPortParser::portDeclarationsIn(QString const& portSection) const
{
    QStringList portDeclarations;

    int index = PORT_EXP.indexIn(portSection, 0);
    while(index != -1)
    {
        portDeclarations.append(PORT_EXP.cap(0));
        index = PORT_EXP.indexIn(portSection, index + PORT_EXP.matchedLength());
    }

    if (!portDeclarations.isEmpty() && portDeclarations.last().contains(';'))
    {
        portDeclarations.removeLast();
    }

    return portDeclarations;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::createPortFromDeclaration()
//-----------------------------------------------------------------------------
void VerilogPortParser::createPortFromDeclaration(QString const& portDeclaration,
    QSharedPointer<Component> targetComponent) const
{
    General::Direction portDirection = parseDirection(portDeclaration);

    QString typeName = PORT_EXP.cap(2);

    int leftBound = parseLeftBound(portDeclaration);
    int lowerBound = parseRightBound(portDeclaration);

    QStringList portNames = parsePortNames(portDeclaration);

    QString description = parseDescription(portDeclaration);

    foreach(QString portName, portNames)
    {
        QSharedPointer<Port> port(new Port(portName, portDirection, leftBound, lowerBound, typeName, 
            "", "", description));
        targetComponent->addPort(port);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseDirection()
//-----------------------------------------------------------------------------
General::Direction VerilogPortParser::parseDirection(QString const& portDeclaration) const
{
    PORT_EXP.indexIn(portDeclaration);
    QString directionString = PORT_EXP.cap(1);

    General::Direction portDirection = General::DIRECTION_INVALID;
    if (directionString == "input")
    {
        portDirection = General::IN;
    }
    else if (directionString == "output")
    {
        portDirection = General::OUT;
    }
    else if(directionString == "inout")
    {
        portDirection = General::INOUT;
    }

    return portDirection;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseLeftBound()
//-----------------------------------------------------------------------------
int VerilogPortParser::parseLeftBound(QString const& portDeclaration) const
{
    PORT_EXP.indexIn(portDeclaration);
    QString bounds = PORT_EXP.cap(3);

    QRegExp boundedExp("[\\[](\\w+)\\s*[:]\\s*(\\w+)[\\]]");
    boundedExp.indexIn(bounds);
    return boundedExp.cap(1).toInt();
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseRightBound()
//-----------------------------------------------------------------------------
int VerilogPortParser::parseRightBound(QString const& portDeclaration) const
{
    PORT_EXP.indexIn(portDeclaration);
    QString bounds = PORT_EXP.cap(3);

    QRegExp boundedExp("[\\[](\\w+)\\s*[:]\\s*(\\w+)[\\]]");
        boundedExp.indexIn(bounds);
    return boundedExp.cap(2).toInt();
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parsePortNames()
//-----------------------------------------------------------------------------
QStringList VerilogPortParser::parsePortNames(QString const& portDeclaration) const
{
    PORT_EXP.indexIn(portDeclaration);
    QString names = PORT_EXP.cap(4);
    
    return names.split(QRegExp("\\s*[,]\\s*"), QString::SkipEmptyParts);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseDescription()
//-----------------------------------------------------------------------------
QString VerilogPortParser::parseDescription(QString const& portDeclaration) const
{
    PORT_EXP.indexIn(portDeclaration);
    QString description = PORT_EXP.cap(5);

    if (description.isEmpty())
    {
        description = PORT_EXP.cap(6).trimmed();
    }

    return description;
}

