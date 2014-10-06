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

#include "VerilogSyntax.h"

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>
#include <Plugins/common/HDLEquationParser.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>

#include <QString>
#include <QRegExp>

namespace
{
    //! Verilog port directions.
    const QString PORT_DIRECTION("input|output|inout");

    //! Verilog port types.
    const QString PORT_TYPE("\\w+");

    //! Verilog ports in both ANSI-C and Verilog-1995 style.
    const QRegExp PORT_EXP("(" + PORT_DIRECTION + ")\\s+(" + PORT_TYPE + ")?\\s*(?:signed)?\\s*"
        "(" + VerilogSyntax::RANGE + ")?\\s*(" + VerilogSyntax::NAMES + ")"
        "(?:\\s*[,;][ \\t]*(?:"+ VerilogSyntax::COMMENT + ")?|(?:[ \\t]*"+ VerilogSyntax::COMMENT + ")?(?=\\s*$))");

    //! Verilog ports in Verilog-1995 style.
    const QRegExp PORT_1995("(" + PORT_DIRECTION + ")\\s+(" + VerilogSyntax::RANGE + ")?\\s*"
        "(" + VerilogSyntax::NAMES + ")\\s*[;](?:[ \\t]*"+ VerilogSyntax::COMMENT + ")?");
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::VerilogPortParser()
//-----------------------------------------------------------------------------
VerilogPortParser::VerilogPortParser(): highlighter_(0)
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
        highlight(portDeclaration);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::setHighlighter()
//-----------------------------------------------------------------------------
void VerilogPortParser::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
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
    bool noValidModule = (VerilogSyntax::MODULE_BEGIN.indexIn(input) == -1 || 
        VerilogSyntax::MODULE_END.indexIn(input) == -1);

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
    bool hasPortsAfterModuleDeclaration = (PORT_1995.indexIn(input, findStartOfPortList(input)) != -1);
    return hasPortsAfterModuleDeclaration;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findStartOfPortList()
//-----------------------------------------------------------------------------
int VerilogPortParser::findStartOfPortList(QString const& input) const
{
    return input.lastIndexOf('(', VerilogSyntax::MODULE_BEGIN.indexIn(input)) + 1;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findVerilog1995PortsSectionInModule()
//-----------------------------------------------------------------------------
QString VerilogPortParser::findVerilog1995PortsSectionInModule(QString const& input) const
{    
    int startOfPortList = findStartOfPortList(input);    
    int endOfModule = VerilogSyntax::MODULE_END.indexIn(input, startOfPortList);

    QString section = input.mid(startOfPortList, endOfModule - startOfPortList);
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

    int portSectionBegin = findStartOfPortList(input);
    int portSectionEnd = portsEnd.indexIn(input, portSectionBegin);

    bool noPortSection = (portSectionBegin == -1 || portSectionEnd == -1);

    if (noPortSection)
    {
        return QString();
    }

    int portSectionLength = portSectionEnd - portSectionBegin;

    return input.mid(portSectionBegin, portSectionLength);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::removeCommentLines()
//-----------------------------------------------------------------------------
QString VerilogPortParser::removeCommentLines(QString portSection) const
{    
    QRegExp multilineComment = VerilogSyntax::MULTILINE_COMMENT;
    multilineComment.setMinimal(true);

    return portSection.remove(VerilogSyntax::COMMENTLINE).remove(multilineComment);
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
    General::Direction direction = parseDirection(portDeclaration);

    QString type = PORT_EXP.cap(2);

    HDLEquationParser parser(targetComponent->getModelParameters());

    int leftBound = parseLeftBound(portDeclaration, parser);
    int lowerBound = parseRightBound(portDeclaration, parser);

    QStringList portNames = parsePortNames(portDeclaration);

    QString description = parseDescription(portDeclaration);

    foreach(QString name, portNames)
    {
        QSharedPointer<Port> port(new Port(name, direction, leftBound, lowerBound, type, "", "", description));       
        targetComponent->addPort(port);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::highlight()
//-----------------------------------------------------------------------------
void VerilogPortParser::highlight(QString const& portDeclaration)
{
    if (highlighter_)
    {
        highlighter_->applyHighlight(portDeclaration, ImportColors::PORT);
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
int VerilogPortParser::parseLeftBound(QString const& portDeclaration, HDLEquationParser const& parser) const
{
    PORT_EXP.indexIn(portDeclaration);
    QString bounds = PORT_EXP.cap(3);

    QRegExp boundedExp("\\[(" + HDLmath::TERM + ")\\s*[:]\\s*(" + HDLmath::TERM + ")\\]");
    boundedExp.indexIn(bounds);

    int value = 0;
    if (!bounds.isEmpty())
    {
        value = parser.parse(boundedExp.cap(1));
    }     

     return value;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseRightBound()
//-----------------------------------------------------------------------------
int VerilogPortParser::parseRightBound(QString const& portDeclaration, HDLEquationParser const& parser) const
{
    PORT_EXP.indexIn(portDeclaration);
    QString bounds = PORT_EXP.cap(3);

    QRegExp boundedExp("\\[(" + HDLmath::TERM + ")\\s*[:]\\s*(" + HDLmath::TERM + ")\\]");
        boundedExp.indexIn(bounds);

    int value = 0;
    if (!bounds.isEmpty())
    {
        value = parser.parse(boundedExp.cap(2));
    }

    return value;
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
