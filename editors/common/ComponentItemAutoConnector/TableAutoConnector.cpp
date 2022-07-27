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
    int currentRowCount = selectedTable->rowCount();

    QVector<QPair<QString, QString> > combinations =
        removeExistingCombinations(getCombinations(firstComponent, secondComponent), selectedTable);

    selectedTable->setRowCount(currentRowCount + combinations.size());

    for (int i = 0; i < combinations.size(); ++i)
    {
        QTableWidgetItem* firstItem = createTableWidgetItem(combinations.at(i).first, firstComponent);
        QTableWidgetItem* secondItem = createTableWidgetItem(combinations.at(i).second, secondComponent);

        selectedTable->setItem(currentRowCount + i, 0, firstItem);
        selectedTable->setItem(currentRowCount + i, 1, secondItem);
    }
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

            QMultiMap<double, QString> possibleConnecedItems;
            for (auto comparisonItem : possibleCombinations.at(i).second)
            {
                double jaroDistance =
                    JaroWinklerAlgorithm::calculateJaroWinklerDistance(itemToBeConnected, comparisonItem);
                if (jaroDistance >= JAROWINKLERTRESHOLD)
                {
                    possibleConnecedItems.insert(jaroDistance, comparisonItem);
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
// Function: TableAutoConnector::removeExistingCombinations()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > TableAutoConnector::removeExistingCombinations(
    QVector<QPair<QString, QString> > const& combinations, QTableWidget* targetTable) const
{
    QVector<QPair<QString, QString> >  existingCombinations;

    for (int i = 0; i < targetTable->rowCount(); ++i)
    {
        QPair<QString, QString> singleCombination;
        singleCombination.first = getTextFromTableItem(i, 0, targetTable);
        singleCombination.second = getTextFromTableItem(i, 1, targetTable);

        existingCombinations.append(singleCombination);
    }

    QVector<QPair<QString, QString> > newCombinations;

    for (int i = 0; i < combinations.size(); ++i)
    {
        QString combinationFirst = combinations.at(i).first;
        QString combinationSecond = combinations.at(i).second;

        if (!combinationExistsInTable(combinationFirst, combinationSecond, existingCombinations))
        {
            QPair<QString, QString> singleCombination;
            singleCombination.first = combinationFirst;
            singleCombination.second = combinationSecond;

            newCombinations.append(singleCombination);
        }
    }

    return newCombinations;
}

//-----------------------------------------------------------------------------
// Function: TableAutoConnector::getTextFromTableItem()
//-----------------------------------------------------------------------------
QString TableAutoConnector::getTextFromTableItem(int const& itemRow, int const& itemColumn,
    QTableWidget* targetTable) const
{
    QTableWidgetItem* tableItem = targetTable->item(itemRow, itemColumn);
    if (tableItem)
    {
        return tableItem->text();
    }
    else
    {
        return QStringLiteral("");
    }
}

//-----------------------------------------------------------------------------
// Function: TableAutoConnector::combinationExistsInTable()
//-----------------------------------------------------------------------------
bool TableAutoConnector::combinationExistsInTable(QString const& combinationFirst,
    QString const& combinationSecond, QVector<QPair<QString, QString> > const& existingCombinations) const
{
    for (auto const& singleCombination : existingCombinations)
    {
        if (singleCombination.first == combinationFirst || singleCombination.second == combinationSecond)
        {
            return true;
        }
    }

    return false;
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

//-----------------------------------------------------------------------------
// Function: TableAutoConnector::clearTable()
//-----------------------------------------------------------------------------
void TableAutoConnector::clearTable(QTableWidget* selectedTable) const
{
    selectedTable->setRowCount(0);
}

//-----------------------------------------------------------------------------
// Function: TableAutoConnector::connectSelectedFromLists()
//-----------------------------------------------------------------------------
void TableAutoConnector::connectSelectedFromLists(QListView* firstList, QListView* secondList,
    QTableWidget* targetTable) const
{
    QModelIndex firstItemIndex = firstList->currentIndex();
    QModelIndex secondItemIndex = secondList->currentIndex();

    if (firstItemIndex.isValid() && secondItemIndex.isValid())
    {
        QString firstItemName = firstItemIndex.data(Qt::DisplayRole).toString();
        QIcon firstItemIcon = firstItemIndex.data(Qt::DecorationRole).value<QIcon>();

        QString secondItemName = secondItemIndex.data(Qt::DisplayRole).toString();
        QIcon secondItemIcon = secondItemIndex.data(Qt::DecorationRole).value<QIcon>();

        QTableWidgetItem* firstItem = new QTableWidgetItem(firstItemIcon, firstItemName);
        QTableWidgetItem* secondItem = new QTableWidgetItem(secondItemIcon, secondItemName);

        int newRow = targetTable->rowCount();

        targetTable->setRowCount(newRow + 1);

        targetTable->setItem(newRow, 0, firstItem);
        targetTable->setItem(newRow, 1, secondItem);
    }
}
