//-----------------------------------------------------------------------------
// File: tst_AddressBlockGraphItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.04.2015
//
// Description:
// Unit test for class AddressBlockGraphItem.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/addressblockgraphitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>
#include <editors/ComponentEditor/visualization/memorygapitem.h>

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/registermodel.h>

class tst_AddressBlockGraphItem : public QObject
{
    Q_OBJECT

public:
    tst_AddressBlockGraphItem();
    
private slots:

    void testConstructor();

    void testAddressBlockWithRegister();

    void testRegisterInSecondAddress();

    void testEmptyAfterLastRegister();

private:
    void expandItem(AddressBlockGraphItem* memoryMapItem);

    QList<MemoryGapItem*> findMemoryGaps(AddressBlockGraphItem* memoryMapItem);
};

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockGraphItem::tst_AddressBlockGraphItem()
//-----------------------------------------------------------------------------
tst_AddressBlockGraphItem::tst_AddressBlockGraphItem()
{
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockGraphItem::testConstructor()
//-----------------------------------------------------------------------------
void tst_AddressBlockGraphItem::testConstructor()
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName("testBlock");
    addressBlock->setBaseAddress(0);
    addressBlock->setRange("1");

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, 0);

    QCOMPARE(addressBlockItem->getName(), QString("testBlock"));
    QCOMPARE(addressBlockItem->getOffset(), quint64(0));
    QCOMPARE(addressBlockItem->getLastAddress(), quint64(0));
    QCOMPARE(addressBlockItem->getDisplayOffset(), quint64(0));
    QCOMPARE(addressBlockItem->getDisplayLastAddress(), quint64(0));

    QCOMPARE(addressBlockItem->pos().y(), qreal(0));

    delete addressBlockItem;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockGraphItem::testMemoryMapWithAddressBlock()
//-----------------------------------------------------------------------------
void tst_AddressBlockGraphItem::testAddressBlockWithRegister()
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName("testBlock");
    addressBlock->setBaseAddress(0);
    addressBlock->setRange("1");

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, 0);
    addressBlockItem->setAddressableUnitBits(8);

    QList<QSharedPointer<RegisterModel> > registers;
    QSharedPointer<Register> reg(new Register());
    reg->setAddressOffset("0");
    reg->setSize(8);
    registers.append(reg);
    addressBlock->getRegisterData().append(registers);

    RegisterGraphItem* registerItem = new RegisterGraphItem(reg, addressBlockItem);
    addressBlockItem->addChild(registerItem);

    QCOMPARE(addressBlockItem->getName(), QString("testBlock"));
    QCOMPARE(addressBlockItem->getOffset(), quint64(0));
    QCOMPARE(addressBlockItem->getLastAddress(), quint64(0));
    QCOMPARE(addressBlockItem->getDisplayOffset(), quint64(0));
    QCOMPARE(addressBlockItem->getDisplayLastAddress(), quint64(0));

    QVERIFY(!registerItem->isVisible());

    expandItem(addressBlockItem);

    QVERIFY(registerItem->isVisible());
    QCOMPARE(registerItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(registerItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(registerItem->getOffset(), quint64(0));
    QCOMPARE(registerItem->getLastAddress(), quint64(0));
    QCOMPARE(registerItem->getDisplayOffset(), quint64(0));
    QCOMPARE(registerItem->getDisplayLastAddress(), quint64(0));

    delete addressBlockItem;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockGraphItem::testRegisterInSecondAddress()
//-----------------------------------------------------------------------------
void tst_AddressBlockGraphItem::testRegisterInSecondAddress()
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName("testBlock");
    addressBlock->setBaseAddress(0);
    addressBlock->setRange("2");

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, 0);
    addressBlockItem->setAddressableUnitBits(8);

    QList<QSharedPointer<RegisterModel> > registers;
    QSharedPointer<Register> reg(new Register());
    reg->setAddressOffset("1");
    reg->setSize(8);
    registers.append(reg);
    addressBlock->getRegisterData().append(registers);

    RegisterGraphItem* registerItem = new RegisterGraphItem(reg, addressBlockItem);
    addressBlockItem->addChild(registerItem);

    expandItem(addressBlockItem);

    QList<MemoryGapItem*> gaps = findMemoryGaps(addressBlockItem);
    QCOMPARE(gaps.count(), 1);

    MemoryGapItem* initialGap = gaps.first();
    QVERIFY(!initialGap->isConflicted());
    QCOMPARE(initialGap->getName(), QString("Reserved"));
    QCOMPARE(initialGap->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(initialGap->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(initialGap->getDisplayOffset(), quint64(0));
    QCOMPARE(initialGap->getDisplayLastAddress(), quint64(0));

    QVERIFY(registerItem->isVisible());
    QCOMPARE(registerItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(registerItem->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(registerItem->getOffset(), quint64(1));
    QCOMPARE(registerItem->getLastAddress(), quint64(1));
    QCOMPARE(registerItem->getDisplayOffset(), quint64(1));
    QCOMPARE(registerItem->getDisplayLastAddress(), quint64(1));

    delete addressBlockItem;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockGraphItem::testEmptyAfterLastRegister()
//-----------------------------------------------------------------------------
void tst_AddressBlockGraphItem::testEmptyAfterLastRegister()
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName("testBlock");
    addressBlock->setBaseAddress(0);
    addressBlock->setRange("2");

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, 0);
    addressBlockItem->setAddressableUnitBits(8);

    QList<QSharedPointer<RegisterModel> > registers;
    QSharedPointer<Register> reg(new Register());
    reg->setAddressOffset("0");
    reg->setSize(8);
    registers.append(reg);
    addressBlock->getRegisterData().append(registers);

    RegisterGraphItem* registerItem = new RegisterGraphItem(reg, addressBlockItem);
    addressBlockItem->addChild(registerItem);

    expandItem(addressBlockItem);

    QList<MemoryGapItem*> gaps = findMemoryGaps(addressBlockItem);
    QCOMPARE(gaps.count(), 1);

    QVERIFY(registerItem->isVisible());
    QCOMPARE(registerItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(registerItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(registerItem->getOffset(), quint64(0));
    QCOMPARE(registerItem->getLastAddress(), quint64(0));
    QCOMPARE(registerItem->getDisplayOffset(), quint64(0));
    QCOMPARE(registerItem->getDisplayLastAddress(), quint64(0));

    MemoryGapItem* firstGap = gaps.first();
    QVERIFY(!firstGap->isConflicted());
    QCOMPARE(firstGap->getName(), QString("Reserved"));
    QCOMPARE(firstGap->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstGap->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstGap->getDisplayOffset(), quint64(1));
    QCOMPARE(firstGap->getDisplayLastAddress(), quint64(1));
    delete addressBlockItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::expandItem()
//-----------------------------------------------------------------------------
void tst_AddressBlockGraphItem::expandItem(AddressBlockGraphItem* addressBlockItem)
{
    GraphicsExpandCollapseItem* expander(0);
    foreach (QGraphicsItem* item, addressBlockItem->childItems())
    {
        if (dynamic_cast<GraphicsExpandCollapseItem*>(item) != 0)
        {
            expander = dynamic_cast<GraphicsExpandCollapseItem*>(item);
            expander->expand();
            return;
        }
    }
}
//-----------------------------------------------------------------------------
// Function: tst_AddressBlockGraphItem::findMemoryGaps()
//-----------------------------------------------------------------------------
QList<MemoryGapItem*> tst_AddressBlockGraphItem::findMemoryGaps(AddressBlockGraphItem* memoryMapItem)
{
    QList<MemoryGapItem*> gaps;
    foreach (QGraphicsItem* child, memoryMapItem->childItems())
    {
        if (dynamic_cast<MemoryGapItem*>(child))
        {
            gaps.append(dynamic_cast<MemoryGapItem*>(child));
        }
    }

    return gaps;
}

QTEST_MAIN(tst_AddressBlockGraphItem)

#include "tst_AddressBlockGraphItem.moc"
