//-----------------------------------------------------------------------------
// File: TableAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Automatically connects items of two components.
//-----------------------------------------------------------------------------

#include "TableAutoConnector.h"

#include <IPXACTmodels/Component/Component.h>

#include <editors/ComponentEditor/busInterfaces/portmaps/JaroWinklerAlgorithm.h>

//-----------------------------------------------------------------------------
// Function: TableAutoConnector::initializeTable()
//-----------------------------------------------------------------------------
void TableAutoConnector::initializeTable(QTableWidget* selectedTable, QSharedPointer<Component> firstComponent,
    QSharedPointer<Component> secondComponent) const
{
    selectedTable->setRowCount(0);

    QVector<QPair<QString, QString> > combinations = getCombinations(firstComponent, secondComponent);

    selectedTable->setRowCount(combinations.size());

    for (int i = 0; i < combinations.size(); ++i)
    {
        QTableWidgetItem* firstItem = createTableWidgetItem(combinations.at(i).first, firstComponent);
        QTableWidgetItem* secondItem = createTableWidgetItem(combinations.at(i).second, secondComponent);

        selectedTable->setItem(i, 0, firstItem);
        selectedTable->setItem(i, 1, secondItem);
    }
}

//-----------------------------------------------------------------------------
// Function: TableAutoConnector::clearTable()
//-----------------------------------------------------------------------------
void TableAutoConnector::clearTable(QTableWidget* selectedTable) const
{
    selectedTable->setRowCount(0);
}

//-----------------------------------------------------------------------------
// Function: TableAutoConnector::getCombinations()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > TableAutoConnector::getCombinations(QSharedPointer<Component> firstComponent,
    QSharedPointer<Component> secondComponent) const
{
    QVector<QPair<QString, QString> > combinations;

    QVector<QPair<QString, QVector<QString> > > possibleCombinations =
        findPossibleCombinations(firstComponent, secondComponent);
    if (!possibleCombinations.isEmpty())
    {
        const double JAROWINKLERTRESHOLD = 0.75;

        for (int i = 0; i < possibleCombinations.size(); ++i)
        {
            QString itemToBeConnected = possibleCombinations.at(i).first;

            QMap<double, QString> possibleConnecedItems;
            for (auto comparisonItem : possibleCombinations.at(i).second)
            {
                double jaroDistance =
                    JaroWinklerAlgorithm::calculateJaroWinklerDistance(itemToBeConnected, comparisonItem);
                if (jaroDistance >= JAROWINKLERTRESHOLD)
                {
                    possibleConnecedItems.insertMulti(jaroDistance, comparisonItem);
                }
            }

            if (!possibleConnecedItems.isEmpty())
            {
                QPair<QString, QString> portCombination;
                portCombination.first = itemToBeConnected;
                portCombination.second = possibleConnecedItems.last();

                combinations.append(portCombination);
            }
        }
    }

    return combinations;
}

//-----------------------------------------------------------------------------
// Function: TableAutoConnector::createTableWidgetItem()
//-----------------------------------------------------------------------------
QTableWidgetItem* TableAutoConnector::createTableWidgetItem(QString const& itemName, QSharedPointer<Component> )
    const
{
    QTableWidgetItem* newItem = new QTableWidgetItem(itemName);
    return newItem;
}
