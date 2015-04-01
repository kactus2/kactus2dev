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

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/NullParser.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>

#include <QString>
#include <QRegularExpression>

namespace
{
    //! Verilog port directions.
    const QString PORT_DIRECTION("input|output|inout");

    //! Verilog port types.
    const QString PORT_TYPE("\\w+");

    //! Subset of Verilog preprocessor commands.
    const QString PREPROCESSOR_COMMAND("`(?:ifdef|ifndef|endif)[^\\r\\n]*");

    //! Port declaration ends if preprocessor command is found. Preceding comment will be included.
    const QString END_BEFORE_PREPROCESSOR("(?:[ \\t]*" + VerilogSyntax::COMMENT + ")?"
        "(?=\\s*" + PREPROCESSOR_COMMAND + ")");

    //! Port declaration ends if next port declaration is found.
    const QString END_BEFORE_NEXT_ITEM("\\s*[,;][ \\t]*(?:"+ VerilogSyntax::COMMENT + ")?");

    //! Port declaration ends if no more ports follow the declaration.
    const QString LAST_PORT("(?:[ \\t]*" + VerilogSyntax::COMMENT + ")?(?=\\s*$)");

    //! Port declaration must end with one of the conditions above.
    const QString PORT_DECLARATION_END("(?:" + END_BEFORE_PREPROCESSOR + ")|"
        "(?:" + END_BEFORE_NEXT_ITEM + ")|"
        "(?:" + LAST_PORT +")");

    //! Verilog ports in both ANSI-C and Verilog-1995 style.
    const QRegularExpression PORT_EXP("(" + PORT_DIRECTION + ")\\s+(?:(" + PORT_TYPE + ")\\s+)?(?:signed)?\\s*"
        "(" + VerilogSyntax::RANGE + ")?\\s*(" + VerilogSyntax::NAMES + ")(?:" + PORT_DECLARATION_END + ")");

    //! Verilog ports in Verilog-1995 style.
    const QRegularExpression PORT_1995("(" + PORT_DIRECTION + ")\\s+(" + VerilogSyntax::RANGE + ")?\\s*"
        "(" + VerilogSyntax::NAMES + ")\\s*[;](?:[ \\t]*"+ VerilogSyntax::COMMENT + ")?");
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::VerilogPortParser()
//-----------------------------------------------------------------------------
VerilogPortParser::VerilogPortParser(): highlighter_(0), parser_(new NullParser)
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
void VerilogPortParser::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    foreach (QSharedPointer<Port> existingPort, targetComponent->getPorts())
    {
        existingPort->setDirection(General::DIRECTION_PHANTOM);
    }

    foreach (QString portDeclaration, findPortDeclarations(input))
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
// Function: VerilogPortParser::setExpressionParser()
//-----------------------------------------------------------------------------
void VerilogPortParser::setExpressionParser(QSharedPointer<ExpressionParser> parser)
{
    parser_ = parser;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findPortDeclarations()
//-----------------------------------------------------------------------------
QStringList VerilogPortParser::findPortDeclarations(QString const& input) const
{
    QString portSection = findPortsSection(removeIgnoredLines(input));

    return portDeclarationsIn(portSection);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findPortsSection()
//-----------------------------------------------------------------------------
QString VerilogPortParser::findPortsSection(QString const& input) const
{
    bool noValidModule = (input.indexOf(VerilogSyntax::MODULE_BEGIN) == -1 || 
        input.indexOf(VerilogSyntax::MODULE_END) == -1);

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
    bool hasPortsAfterModuleDeclaration = (input.indexOf(PORT_1995, findStartOfPortList(input)) != -1);
    return hasPortsAfterModuleDeclaration;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findStartOfPortList()
//-----------------------------------------------------------------------------
int VerilogPortParser::findStartOfPortList(QString const& input) const
{
    return input.lastIndexOf('(', input.indexOf(VerilogSyntax::MODULE_BEGIN)) + 1;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::findVerilog1995PortsSectionInModule()
//-----------------------------------------------------------------------------
QString VerilogPortParser::findVerilog1995PortsSectionInModule(QString const& input) const
{    
    int startOfPortList = findStartOfPortList(input);    
    int endOfModule = input.indexOf(VerilogSyntax::MODULE_END, startOfPortList);

    QString section = input.mid(startOfPortList, endOfModule - startOfPortList);
    section = removeIgnoredLines(section);

    int firstPort = section.indexOf(PORT_1995);
    int lastPort = section.lastIndexOf(PORT_1995);
    int endOfPorts = lastPort += PORT_1995.match(section, lastPort).capturedLength();

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
    QRegularExpression portsEnd("[)];");

    int portSectionBegin = findStartOfPortList(input);
    int portSectionEnd = input.indexOf(portsEnd, portSectionBegin);

    bool noPortSection = (portSectionBegin == -1 || portSectionEnd == -1);

    if (noPortSection)
    {
        return QString();
    }

    int portSectionLength = portSectionEnd - portSectionBegin;

    return input.mid(portSectionBegin, portSectionLength);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::removeIgnoredLines()
//-----------------------------------------------------------------------------
QString VerilogPortParser::removeIgnoredLines(QString portSection) const
{    
    QRegularExpression multilineComment(VerilogSyntax::MULTILINE_COMMENT);
  
    return portSection.remove(VerilogSyntax::COMMENTLINE).remove(multilineComment);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::portDeclarationsIn()
//-----------------------------------------------------------------------------
QStringList VerilogPortParser::portDeclarationsIn(QString const& portSection) const
{
    QStringList portDeclarations;

    int index = portSection.indexOf(PORT_EXP, 0);
    while(index != -1)
    {
        QRegularExpressionMatch portMatch = PORT_EXP.match(portSection, index);
        portDeclarations.append(portMatch.captured());
        index = portSection.indexOf(PORT_EXP, index + portMatch.capturedLength());
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

    QString type = PORT_EXP.match(portDeclaration).captured(2);
    QString typeDefinition;

    QString leftBound = parseLeftBound(portDeclaration, targetComponent);
    QString lowerBound = parseRightBound(portDeclaration, targetComponent);

    QStringList portNames = parsePortNames(portDeclaration);

    QString description = parseDescription(portDeclaration);

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
            targetComponent->addPort(port);
        }

        port->setName(name);
        port->setDirection(direction);
        port->setLeftBound(parser_->parseExpression(leftBound).toInt());
        port->setRightBound(parser_->parseExpression(lowerBound).toInt());
        port->setLeftBoundExpression(leftBound);
        port->setRightBoundExpression(lowerBound);
        port->setTypeName(type);
        port->setTypeDefinition(type, typeDefinition);
        port->setDescription(description);
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
    QString directionString = PORT_EXP.match(portDeclaration).captured(1);

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
QString VerilogPortParser::parseLeftBound(QString const& portDeclaration, 
    QSharedPointer<Component> targetComponent) const
{
    QString bounds = PORT_EXP.match(portDeclaration).captured(3);

    QRegularExpression boundedExp("\\[(" + VerilogSyntax::TERM + ")\\s*[:]\\s*(" + VerilogSyntax::TERM + ")\\]");

    QString leftBound = 0;
    if (!bounds.isEmpty())
    {
        leftBound = boundedExp.match(bounds).captured(1);

        if (!parser_->isValidExpression(leftBound))
        {
            leftBound = replaceModelParameterNamesWithIds(leftBound, targetComponent);
        }
    }     

     return leftBound;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseRightBound()
//-----------------------------------------------------------------------------
QString VerilogPortParser::parseRightBound(QString const& portDeclaration, 
    QSharedPointer<Component> targetComponent) const
{
    QString bounds = PORT_EXP.match(portDeclaration).captured(3);

    QRegularExpression boundedExp("\\[(" + VerilogSyntax::TERM + ")\\s*[:]\\s*(" + VerilogSyntax::TERM + ")\\]");

    QString rightBound = 0;
    if (!bounds.isEmpty())
    {
        rightBound = boundedExp.match(bounds).captured(2);

        if (!parser_->isValidExpression(rightBound))
        {
            rightBound = replaceModelParameterNamesWithIds(rightBound, targetComponent);
        }
    }

    return rightBound;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::replaceModelParameterNamesWithIds()
//-----------------------------------------------------------------------------
QString VerilogPortParser::replaceModelParameterNamesWithIds(QString const& expression, 
    QSharedPointer<Component> targetComponent) const
{
    QString result = expression;
    if (expression.contains('`'))
    {
        foreach (QSharedPointer<Parameter> define, *targetComponent->getParameters())
        {
            QRegularExpression macroUsage("`" + define->getName());
            if (macroUsage.match(result).hasMatch())
            {
                result.replace(macroUsage, define->getValueId());

                for(int i = 0; i < expression.count(macroUsage); i++)
                {
                    define->increaseUsageCount();
                }
            }
        }
    }
    
    foreach (QSharedPointer<ModelParameter> modelParameter, *targetComponent->getModelParameters())
    {
        QRegularExpression nameReference("\\b" + modelParameter->getName() + "\\b");
        if (nameReference.match(result).hasMatch())
        {
            result.replace(nameReference, modelParameter->getValueId());

            for(int i = 0; i < expression.count(nameReference); i++)
            {
                modelParameter->increaseUsageCount();
            }
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parsePortNames()
//-----------------------------------------------------------------------------
QStringList VerilogPortParser::parsePortNames(QString const& portDeclaration) const
{
    QString names = PORT_EXP.match(portDeclaration).captured(4);
    
    return names.split(QRegularExpression("\\s*[,]\\s*"), QString::SkipEmptyParts);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseDescription()
//-----------------------------------------------------------------------------
QString VerilogPortParser::parseDescription(QString const& portDeclaration) const
{
    QRegularExpression commentExp(VerilogSyntax::COMMENT);
    return commentExp.match(portDeclaration).captured(1).trimmed();
}
