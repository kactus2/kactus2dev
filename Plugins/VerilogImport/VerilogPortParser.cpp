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

#include <KactusAPI/include//ImportColors.h>

#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/NullParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

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
    const QRegularExpression PORT_EXP("(" + PORT_DIRECTION + ")\\s+(?:wire\\s+)?(?:(" + PORT_TYPE + ")\\s+)?"
        "(?:signed)?\\s*(?:(" + VerilogSyntax::RANGE +")?\\s*(" + VerilogSyntax::RANGE + "))?\\s*"
        "(" + VerilogSyntax::NAMES + ")(?:" + PORT_DECLARATION_END + ")");

    //! Verilog ports in Verilog-1995 style.
    const QRegularExpression PORT_1995("(" + PORT_DIRECTION + ")\\s+"
        "(?:(" + PORT_TYPE + ")\\s+)?(" + VerilogSyntax::RANGE + ")?\\s*"
        "(" + VerilogSyntax::NAMES + ")\\s*[;](?:[ \\t]*"+ VerilogSyntax::COMMENT + ")?");
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::VerilogPortParser()
//-----------------------------------------------------------------------------
VerilogPortParser::VerilogPortParser(): highlighter_(0), parser_(new NullParser)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::runParser()
//-----------------------------------------------------------------------------
void VerilogPortParser::import(QString const& input, QSharedPointer<Component> targetComponent,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    foreach (QSharedPointer<Port> existingPort, *targetComponent->getPorts())
    {
        existingPort->setDirection(DirectionTypes::DIRECTION_PHANTOM);
    }

    foreach (QString portDeclaration, findPortDeclarations(input))
    {
        createPortFromDeclaration(portDeclaration, targetComponent, targetComponentInstantiation);
        highlight(portDeclaration, input);
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
    bool noValidModule = input.indexOf(VerilogSyntax::MODULE_BEGIN) == -1 || 
        input.indexOf(VerilogSyntax::MODULE_END) == -1;

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
    QString commentsRemoved = input;
    commentsRemoved.remove(QRegularExpression(VerilogSyntax::COMMENT));

    bool hasPortsAfterModuleDeclaration = (commentsRemoved.indexOf(PORT_1995, findStartOfPortList(commentsRemoved)) != -1);
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
    QString section = input;

    int startOfPortList = section.indexOf(QRegularExpression("[)]\\s*;"), findStartOfPortList(input)); 
    int endOfModule = section.indexOf(VerilogSyntax::MODULE_END, startOfPortList);
    
    QRegularExpression lastPort(PORT_1995.pattern() + "(?!\\s*(" + VerilogSyntax::COMMENT + ")?\\s*" + PORT_1995.pattern() + ")");

    int endOfPortList = qMin(lastPort.match(section, startOfPortList).capturedEnd(), endOfModule);

    return section.mid(startOfPortList, endOfPortList - startOfPortList);
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
QString VerilogPortParser::removeIgnoredLines(QString const& portSection) const
{
    QString portSectionWithoutComments = portSection;
    QRegularExpression multilineComment(VerilogSyntax::MULTILINE_COMMENT);

    return  portSectionWithoutComments.remove(VerilogSyntax::COMMENTLINE).remove(multilineComment);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::portDeclarationsIn()
//-----------------------------------------------------------------------------
QStringList VerilogPortParser::portDeclarationsIn(QString const& portSection) const
{
    QStringList portDeclarations;

    QRegularExpressionMatchIterator matches = PORT_EXP.globalMatch(portSection);
    while (matches.hasNext())
    {
        portDeclarations.append(matches.next().captured());
    }

    return portDeclarations;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::createPortFromDeclaration()
//-----------------------------------------------------------------------------
void VerilogPortParser::createPortFromDeclaration(QString const& portDeclaration,
    QSharedPointer<Component> targetComponent,
    QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const
{
    DirectionTypes::Direction direction = parseDirection(portDeclaration);

    QString type = PORT_EXP.match(portDeclaration).captured(2);
    if (type.compare(QLatin1String("signed")) == 0)
    {
        type.clear();
    }

    QString typeDefinition;

    QPair<QString, QString> vectorBounds = parseVectorBounds(portDeclaration, targetComponent);
    QPair<QString, QString> arrayBounds = parseArrayBounds(portDeclaration, targetComponent);

    QStringList portNames = parsePortNames(portDeclaration);

    QString description = parseDescription(portDeclaration);

    for (QString const& name : portNames)
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
            targetComponent->getPorts()->append(port);
        }

        port->setName(name);
        port->setDirection(direction);
        port->setLeftBound(vectorBounds.first);
        port->setRightBound(vectorBounds.second);
        port->setTypeName(type);
        port->setTypeDefinition(type, typeDefinition);
        port->setArrayLeft(arrayBounds.first);
        port->setArrayRight(arrayBounds.second);
        port->setDescription(description);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::highlight()
//-----------------------------------------------------------------------------
void VerilogPortParser::highlight(QString const& portDeclaration, QString const& componentDeclaration)
{
    if (highlighter_)
    {
        highlighter_->applyHighlight(portDeclaration, ImportColors::PORT, componentDeclaration);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction VerilogPortParser::parseDirection(QString const& portDeclaration) const
{
    QString directionString = PORT_EXP.match(portDeclaration).captured(1);

    DirectionTypes::Direction portDirection = DirectionTypes::DIRECTION_INVALID;
    if (directionString == QLatin1String("input"))
    {
        portDirection = DirectionTypes::IN;
    }
    else if (directionString == QLatin1String("output"))
    {
        portDirection = DirectionTypes::OUT;
    }
    else if(directionString == QLatin1String("inout"))
    {
        portDirection = DirectionTypes::INOUT;
    }

    return portDirection;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseArrayBounds()
//-----------------------------------------------------------------------------
QPair<QString, QString> VerilogPortParser::parseArrayBounds(QString const& portDeclaration,
	QSharedPointer<Component> targetComponent ) const
{
    QString vectorBounds = PORT_EXP.match(portDeclaration).captured(3);

    return parseLeftAndRight(vectorBounds, targetComponent);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseLeftAndRight()
//-----------------------------------------------------------------------------
QPair<QString, QString> VerilogPortParser::parseLeftAndRight(QString const& bounds,
	QSharedPointer<Component> targetComponent) const
{
    auto leftBound = QString();
    auto rightBound = QString();

    if (!bounds.isEmpty())
    {
        // If there is a ternary operator, special handling is required.
        QRegularExpressionMatch matchTernary = VerilogSyntax::TERNARY_LEFT.match(bounds);
        if (matchTernary.hasMatch())
        {
            QString tempBounds(bounds);
            QString placeholder("");
            for (int i = 0; i <= matchTernary.lastCapturedIndex(); ++i)
            {
                QString captured = matchTernary.captured(i);
                // Removed captured text.
                placeholder.resize(captured.length(), ' ');
                tempBounds.replace(captured, placeholder);
            }
            auto boundsPos = tempBounds.lastIndexOf(QLatin1String(":"));
            leftBound = bounds.left(boundsPos).remove('[');
            rightBound = bounds.right(bounds.length() - boundsPos - 1).remove(']');
        }
        else
        {
            leftBound = VerilogSyntax::CAPTURING_RANGE.match(bounds).captured(1);
            rightBound = VerilogSyntax::CAPTURING_RANGE.match(bounds).captured(2);
        }

        bool leftValid = false;
        parser_->parseExpression(leftBound, &leftValid);
        if (leftValid == false)
        {
            leftBound = replaceNameReferencesWithIds(leftBound, targetComponent);
        }

        bool rightValid = false;
        parser_->parseExpression(rightBound, &rightValid);
        if (rightValid == false)
        {
            rightBound = replaceNameReferencesWithIds(rightBound, targetComponent);
        }
    }

    return qMakePair(leftBound, rightBound);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseVectorBounds()
//-----------------------------------------------------------------------------
QPair<QString, QString> VerilogPortParser::parseVectorBounds(QString const& portDeclaration,
	QSharedPointer<Component> targetComponent) const
{
    QString vectorBounds = PORT_EXP.match(portDeclaration).captured(4);

    return parseLeftAndRight(vectorBounds, targetComponent);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::replaceNameReferencesWithIds()
//-----------------------------------------------------------------------------
QString VerilogPortParser::replaceNameReferencesWithIds(QString const& expression, 
    QSharedPointer<Component> targetComponent) const
{
    QString result = expression;

        foreach (QSharedPointer<Parameter> define, *targetComponent->getParameters())
        {
            QRegularExpression macroUsage("`?" + define->name() + "\\b");
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
// Function: VerilogPortParser::parsePortNames()
//-----------------------------------------------------------------------------
QStringList VerilogPortParser::parsePortNames(QString const& portDeclaration) const
{
    QString names = PORT_EXP.match(portDeclaration).captured(5);
    
    return names.split(QRegularExpression(QStringLiteral("\\s*[,]\\s*")), Qt::SkipEmptyParts);
}

//-----------------------------------------------------------------------------
// Function: VerilogPortParser::parseDescription()
//-----------------------------------------------------------------------------
QString VerilogPortParser::parseDescription(QString const& portDeclaration) const
{
    QRegularExpression commentExp(VerilogSyntax::COMMENT);
    return commentExp.match(portDeclaration).captured(1).trimmed();
}
