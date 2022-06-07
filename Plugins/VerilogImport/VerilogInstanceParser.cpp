//-----------------------------------------------------------------------------
// File: VerilogInstanceParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.04.2019
//
// Description:
// Parser for Verilog instances.
//-----------------------------------------------------------------------------

#include "VerilogInstanceParser.h"
#include "VerilogSyntax.h"

#include <common/expressions/utilities.h>

#include <KactusAPI/include//ImportColors.h>

#include <wizards/ComponentWizard/InstanceData.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/utilities/Search.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: VerilogInstanceParser::VerilogInstanceParser()
//-----------------------------------------------------------------------------
VerilogInstanceParser::VerilogInstanceParser():
highlighter_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogInstanceParser::setHighlighter()
//-----------------------------------------------------------------------------
void VerilogInstanceParser::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
}

//-----------------------------------------------------------------------------
// Function: VerilogInstanceParser::import()
//-----------------------------------------------------------------------------
void VerilogInstanceParser::import(QString const& input, QString const& componentDeclaration,
    QSharedPointer<Component> targetComponent)
{
    QVector<QRegularExpressionMatch> instanceDeclarations = findInstances(input, componentDeclaration);

    if (!instanceDeclarations.isEmpty())
    {
        QSharedPointer<Kactus2Group> instancesGroup(new Kactus2Group(InstanceData::VERILOGINSTANCES));

        for (auto& instanceMatch : instanceDeclarations)
        {
            QString instanceModuleName = instanceMatch.captured(1);
            QString parameterDeclarations = instanceMatch.captured(2);
            QString instanceName = instanceMatch.captured(3);
            QString connectionDeclarations = instanceMatch.captured(4);

            QSharedPointer<Kactus2Group> instanceExtension(new Kactus2Group(InstanceData::SINGLEINSTANCE));

            QSharedPointer<Kactus2Value> instanceNameExtension(new Kactus2Value(
                InstanceData::INSTANCENAME, instanceName));
            QSharedPointer<Kactus2Value> moduleExtension(new Kactus2Value(
                InstanceData::MODULENAME, instanceModuleName));

            instanceExtension->addToGroup(instanceNameExtension);
            instanceExtension->addToGroup(moduleExtension);

            if (!parameterDeclarations.isEmpty())
            {
                QVector<QPair<QString, QString> > instanceParameters = getSeparatedSubItems(parameterDeclarations);
                QSharedPointer<Kactus2Group> parameterGroup = createSubItemGroup(
                    InstanceData::PARAMETERS, InstanceData::SINGLEPARAMETER, instanceParameters);
                if (parameterGroup)
                {
                    instanceExtension->addToGroup(parameterGroup);
                }
            }

            if (!connectionDeclarations.isEmpty())
            {
                QVector<QPair<QString, QString> > instanceConnections = getSeparatedSubItems(connectionDeclarations);
                QSharedPointer<Kactus2Group> connectionGroup = createSubItemGroup(
                    InstanceData::CONNECTIONS, InstanceData::SINGLECONNECTION, instanceConnections);
                if (connectionGroup)
                {
                    instanceExtension->addToGroup(connectionGroup);
                }
            }

            instancesGroup->addToGroup(instanceExtension);
        }

        targetComponent->getVendorExtensions()->append(instancesGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogInstanceParser::findInstances()
//-----------------------------------------------------------------------------
QVector<QRegularExpressionMatch> VerilogInstanceParser::findInstances(QString const& input,
    QString const& componentDeclaration)
{
    QVector<QRegularExpressionMatch> instances;
    QString inspect = componentDeclaration;

    QRegularExpression multilineComment(VerilogSyntax::MULTILINE_COMMENT);
    QRegularExpression strayComment(VerilogSyntax::COMMENT);
    inspect = inspect.remove(VerilogSyntax::COMMENTLINE).remove(multilineComment).remove(strayComment);

    QString expressionString =
        "\\b([a-zA-Z_][\\w$]*)(\\s+#(?:(?:.|\\n)(?!(?:[)]\\s*[a-zA-Z_]|;)))*(?:.|\\n)\\))?\\s+([a-zA-Z_][\\w$]*)\\s*(\\((?:(?:.|\\n)(?!\\);))*\\s*\\)+;)";
    QRegularExpression instanceExpression(expressionString);
    QRegularExpressionMatchIterator instanceMatchIterator = instanceExpression.globalMatch(inspect);

    QRegularExpressionMatchIterator multilineCommentIterator = multilineComment.globalMatch(input);

    while (instanceMatchIterator.hasNext())
    {
        QRegularExpressionMatch instanceMatch = instanceMatchIterator.next();

        QString instanceModuleName = instanceMatch.captured(1);
        if (instanceMatch.captured(1).compare(QStringLiteral("module"), Qt::CaseInsensitive) != 0)
        {
            instances.append(instanceMatch);
            highlightInstance(input, componentDeclaration, instanceMatch, multilineCommentIterator);
        }
    }

    return instances;
}

//-----------------------------------------------------------------------------
// Function: VerilogInstanceParser::highlightInstance()
//-----------------------------------------------------------------------------
void VerilogInstanceParser::highlightInstance(QString const& input, QString const& moduleDeclaration,
    QRegularExpressionMatch const& instanceMatch, QRegularExpressionMatchIterator const& multilineCommentIterator)
{
    QString instanceModuleName = instanceMatch.captured(1);
    QString instanceName = instanceMatch.captured(3);

    QRegularExpression inputInstanceExpression("\\b(" + instanceModuleName +
        "\\s+(?:[^;])*?" +
        instanceName + ")\\s*(\\((?:(?:.|\\n)(?!\\);))*\\s*\\)+;)", QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator instanceIntroductionIterator = inputInstanceExpression.globalMatch(input);
    while (instanceIntroductionIterator.hasNext())
    {
        QRegularExpressionMatch instanceIntroductionMatch = instanceIntroductionIterator.next();
        if (!matchIsWithinComments(instanceIntroductionMatch, multilineCommentIterator))
        {
            QString matchedIntroduction = instanceIntroductionMatch.captured(1);
            int instanceModuleBeginIndex =
                instanceIntroductionMatch.capturedStart() + matchedIntroduction.indexOf(instanceModuleName);
            int instanceModuleEndIndex = instanceModuleBeginIndex + instanceModuleName.length();

            int instanceNameBeginIndex =
                instanceIntroductionMatch.capturedStart() + matchedIntroduction.lastIndexOf(instanceName);
            int instanceNameEndIndex = instanceNameBeginIndex + instanceName.length();

            highlighter_->applyHighlight(
                instanceModuleBeginIndex, instanceModuleEndIndex, ImportColors::INSTANCECOLOR);
            highlighter_->applyHighlight(
                instanceNameBeginIndex, instanceNameEndIndex, ImportColors::INSTANCECOLOR);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogInstanceParser::matchIsWithinComments()
//-----------------------------------------------------------------------------
bool VerilogInstanceParser::matchIsWithinComments(QRegularExpressionMatch const& expressionMatch,
    QRegularExpressionMatchIterator commentMatchIterator) const
{
    int expressionStart = expressionMatch.capturedStart();

    while (commentMatchIterator.hasNext())
    {
        QRegularExpressionMatch commentMatch = commentMatchIterator.next();
        int commentStart = commentMatch.capturedStart();
        int commentEnd = commentMatch.capturedEnd();

        if (expressionStart > commentStart && expressionStart < commentEnd)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: VerilogInstanceParser::getSeparatedSubItems()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > VerilogInstanceParser::getSeparatedSubItems(QString& subItemDeclarations) const
{
    QVector<QPair<QString, QString> > subItemReDeclarations;

    QRegularExpression subItemSeparatorExpression(QStringLiteral("((?:.(?!\\())*.)(.*)"));

    QRegularExpression subItemExpression(QStringLiteral("\\.(?:(.|\\s)(?!(,|(\\)\\s*\\s))))*.*\\)"));
    QRegularExpressionMatchIterator subItemMatchIterator = subItemExpression.globalMatch(subItemDeclarations);

    while (subItemMatchIterator.hasNext())
    {
        QRegularExpressionMatch subItemMatch = subItemMatchIterator.next();
        QString subItemText = subItemMatch.captured().remove(0, 1);

        QRegularExpressionMatch separatedSubItemMatch = subItemSeparatorExpression.match(subItemText);
        QString subItemName = separatedSubItemMatch.captured(1).simplified();
        QString subItemValue = separatedSubItemMatch.captured(2).remove(0, 1);
        subItemValue.chop(1);
        subItemValue = subItemValue.simplified();

        QPair<QString, QString> subItem;
        subItem.first = subItemName;
        subItem.second = subItemValue;

        subItemReDeclarations.append(subItem);
    }

    return subItemReDeclarations;
}

//-----------------------------------------------------------------------------
// Function: VerilogInstanceParser::createSubItemGroup()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Group> VerilogInstanceParser::createSubItemGroup(QString const& groupName,
    QString const& itemType, QVector<QPair<QString, QString>> const& subItems) const
{
    if (subItems.isEmpty())
    {
        return QSharedPointer<Kactus2Group>();
    }
    
    QSharedPointer<Kactus2Group> subItemGroup(new Kactus2Group(groupName));
    for (auto const& item : subItems)
    {
        QSharedPointer<Kactus2Group> nameValueGroup(new Kactus2Group(itemType));

        QSharedPointer<Kactus2Value> itemName(new Kactus2Value(InstanceData::SUBITEMNAME, item.first));
        QSharedPointer<Kactus2Value> itemValue(new Kactus2Value(InstanceData::SUBITEMVALUE, item.second));

        nameValueGroup->addToGroup(itemName);
        nameValueGroup->addToGroup(itemValue);

        subItemGroup->addToGroup(nameValueGroup);
    }

    return subItemGroup;
}
