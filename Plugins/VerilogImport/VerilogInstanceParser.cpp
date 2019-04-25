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

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

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
void VerilogInstanceParser::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    QVector<QRegularExpressionMatch> instanceDeclarations = findInstances(input);

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
QVector<QRegularExpressionMatch> VerilogInstanceParser::findInstances(QString const &input)
{
    QVector<QRegularExpressionMatch> instances;
    QString inspect = VerilogSyntax::cullStrayComments(input);
    
    QRegularExpression commentExpression(VerilogSyntax::COMMENT);
    inspect.remove(commentExpression);

    QString expressionString =
        "\\b([a-zA-Z_][\\w$]*)(\\s+#(?:(?:.|\\n)(?!(?:[)]\\s*[a-zA-Z_]|;)))*(?:.|\\n)\\))?\\s+([a-zA-Z_][\\w$]*)\\s*(\\((?:(?:.|\\n)(?!\\);))*\\s*\\)+;)";
    QRegularExpression instanceExpression(expressionString);
    QRegularExpressionMatchIterator instanceMatchIterator = instanceExpression.globalMatch(inspect);

    while (instanceMatchIterator.hasNext())
    {
        QRegularExpressionMatch instanceMatch = instanceMatchIterator.next();

        QString instanceModuleName = instanceMatch.captured(1);
        if (instanceMatch.captured(1).compare(QStringLiteral("module"), Qt::CaseInsensitive) != 0)
        {
            instances.append(instanceMatch);
        }
    }

    return instances;
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
