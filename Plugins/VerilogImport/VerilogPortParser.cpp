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
    const QString COMMENT("//[ \\t]*([^\\r\\n]*)(?=\\r?\\n|$)");

    const QRegExp PORT_EXP("(" + DIRECTION + ")\\s+(" + TYPE + ")?\\s*(?:signed)?\\s*(" + RANGE + ")?\\s*"
        "(" + NAMES + ")\\s*(?:[,;][ \\t]*(?:"+ COMMENT + ")?|[ \\t]*(?:"+ COMMENT + ")?\\s*$)");

    const QRegExp PORT_1995("(" + DIRECTION + ")\\s+(" + RANGE + ")?\\s*(" + NAMES + ")\\s*[;]"
        "[ \\t]*(?:"+ COMMENT + ")?");

    const QRegExp MODULE_DECLARATION("module\\s+\\w+\\s*(#\\s*[(][^)]*[)])?\\s*[(]");
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
    QRegExp moduleEnd("endmodule", Qt::CaseSensitive);

    bool noValidModule = (MODULE_DECLARATION.indexIn(input) == -1 || moduleEnd.indexIn(input) == -1);

    if (noValidModule)
    {
        return QString();
    }

    QString portSection;
    if (hasVerilog1995Ports(input))
    {
        portSection = findVerilog1995PortsSectionInModule(input);
    }
    else
    {
        portSection = findVerilog2001PortsSection(input);
    }
    return portSection;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::hasVerilog1995Ports()
//-----------------------------------------------------------------------------
bool VerilogPortParser::hasVerilog1995Ports(QString const& input) const
{
    int endOfModuleDeclaration = MODULE_DECLARATION.indexIn(input) + MODULE_DECLARATION.matchedLength();

    bool hasPortsAfterModuleDeclaration = (PORT_1995.indexIn(input, endOfModuleDeclaration) != -1);

    return hasPortsAfterModuleDeclaration;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findVerilog1995PortsSectionInModule()
//-----------------------------------------------------------------------------
QString VerilogPortParser::findVerilog1995PortsSectionInModule(QString const& input) const
{    
    QRegExp moduleEnd("endmodule", Qt::CaseSensitive);    

    int endOfModuleDeclaration = MODULE_DECLARATION.indexIn(input) + MODULE_DECLARATION.matchedLength();    
    int endOfModule = moduleEnd.indexIn(input, endOfModuleDeclaration);

    QString section = input.mid(endOfModuleDeclaration, endOfModule - endOfModuleDeclaration);
    section = removeCommentLines(section);

    int firstPort = PORT_1995.indexIn(section);
    int lastPort = PORT_1995.lastIndexIn(section);
    int endOfPorts = lastPort += PORT_1995.matchedLength();

    bool noPorts = (firstPort == -1 || lastPort == -1);
    if (noPorts)
    {
        return QString();
    }


    int portSectionLength = endOfPorts - firstPort;

    return section.mid(firstPort, portSectionLength);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findVerilog2001PortsSection()
//-----------------------------------------------------------------------------
QString VerilogPortParser::findVerilog2001PortsSection(QString const& input) const
{
    QRegExp portsEnd("[)];");

    int portSectionBegin = MODULE_DECLARATION.indexIn(input) + MODULE_DECLARATION.matchedLength();
    int portSectionEnd = portsEnd.indexIn(input, portSectionBegin);

    int portSectionLength = portSectionEnd - portSectionBegin;

    return input.mid(portSectionBegin, portSectionLength);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VerilogPortParser::removeCommentLines(QString portSection) const
{
    QRegExp commentLines("(^|\\r?\\n)[ \\t]*" + COMMENT);
    QRegExp multilineComments("/\\*.*\\*/");
    multilineComments.setMinimal(true);

    return portSection.remove(commentLines).remove(multilineComments);
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
    QString description = PORT_EXP.cap(5).trimmed();

    if (description.isEmpty())
    {
        description = PORT_EXP.cap(6).trimmed();
    }

    return description;
}
