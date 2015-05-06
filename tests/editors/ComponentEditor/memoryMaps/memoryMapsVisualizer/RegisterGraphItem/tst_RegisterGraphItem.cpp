//-----------------------------------------------------------------------------
// File: tst_RegisterGraphItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.04.2015
//
// Description:
// Unit test for class RegisterGraphItem.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/addressblockgraphitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/fieldgraphitem.h>
#include <editors/ComponentEditor/visualization/memorygapitem.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>
#include <editors/ComponentEditor/common/NullParser.h>

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/field.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/registermodel.h>

class tst_RegisterGraphItem : public QObject
{
    Q_OBJECT

public:
    tst_RegisterGraphItem();
    
private slots:

    void testConstructor();

    void testRegisterWithField();

    void testRegisterWithConsecutiveFields();

    void testFieldInSecondAddress();

    void testEmptyAfterLastField();
  
    void testFieldOutsideRegisterIsNotValid();

    void testFieldEndingOutsideRegisterIsNotValid();

    void testEmptyBetweenTwoFields();

    void testPartiallyOverlappingFields();
     
    void testFullyOverlappingFields();
    
    void testMultipleFieldsOverlapping();
     
    void testTwoFieldsCompletelyOverlappingThird();
     
    void testZeroWidthFields();

    void testTwoDimensional();

    void testLastDimensionExceedsAddressBlockRange();

    void testExpressions();

    void testNonPresentField();

private:
    void expandItem(RegisterGraphItem* registerItem);

    RegisterGraphItem* createRegisterItem();

    FieldGraphItem* createFieldItem(QString name, unsigned int offset, unsigned int bitWidth, RegisterGraphItem* parentRegister);

    QList<MemoryGapItem*> findMemoryGaps(RegisterGraphItem* memoryMapItem);
   
};

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::tst_RegisterGraphItem()
//-----------------------------------------------------------------------------
tst_RegisterGraphItem::tst_RegisterGraphItem()
{
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testConstructor()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testConstructor()
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName("testBlock");
    addressBlock->setBaseAddress(0);
    addressBlock->setRange("1");

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, noParser, 0);

    QSharedPointer<Register> testRegister(new Register());
    testRegister->setName("testRegister");
    testRegister->setAddressOffset(0);
    testRegister->setSize(1);

    RegisterGraphItem* registerItem = new RegisterGraphItem(testRegister, noParser, addressBlockItem);

    QCOMPARE(registerItem->getName(), QString("testRegister"));
    QCOMPARE(registerItem->getOffset(), quint64(0));
    QCOMPARE(registerItem->getLastAddress(), quint64(0));
    QCOMPARE(registerItem->getDisplayOffset(), quint64(0));
    QCOMPARE(registerItem->getDisplayLastAddress(), quint64(0));

    QCOMPARE(registerItem->pos().y(), qreal(0));

    delete registerItem;
    delete addressBlockItem;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testRegisterWithField()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testRegisterWithField()
{    
    RegisterGraphItem* registerItem = createRegisterItem();
    FieldGraphItem* fieldItem = createFieldItem("testField", 0, 8, registerItem);

    QCOMPARE(registerItem->getName(), QString("testRegister"));
    QCOMPARE(registerItem->getOffset(), quint64(0));
    QCOMPARE(registerItem->getLastAddress(), quint64(0));
    QCOMPARE(registerItem->getDisplayOffset(), quint64(0));
    QCOMPARE(registerItem->getDisplayLastAddress(), quint64(0));

    QVERIFY(!fieldItem->isVisible());

    expandItem(registerItem);

    QVERIFY(fieldItem->isVisible());
    QVERIFY(!fieldItem->isConflicted());
    QCOMPARE(fieldItem->getName(), QString("testField"));
    QCOMPARE(fieldItem->pos().x(), qreal(0));
    QCOMPARE(fieldItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(fieldItem->getOffset(), quint64(0));
    QCOMPARE(fieldItem->getLastAddress(), quint64(7));
    QCOMPARE(fieldItem->getDisplayOffset(), quint64(7));
    QCOMPARE(fieldItem->getDisplayLastAddress(), quint64(0));

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testRegisterWithConsecutiveField()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testRegisterWithConsecutiveFields()
{    
    RegisterGraphItem* registerItem = createRegisterItem();

    FieldGraphItem* firstField = createFieldItem("first", 0, 4, registerItem);
    FieldGraphItem* secondField = createFieldItem("second", 4, 4, registerItem);

    expandItem(registerItem);

    QVERIFY(firstField->isVisible());
    QVERIFY(!firstField->isConflicted());
    QCOMPARE(firstField->pos().x(), registerItem->boundingRect().width()/2);
    QCOMPARE(firstField->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstField->getOffset(), quint64(0));
    QCOMPARE(firstField->getLastAddress(), quint64(3));
    QCOMPARE(firstField->getDisplayOffset(), quint64(3));
    QCOMPARE(firstField->getDisplayLastAddress(), quint64(0));

    QVERIFY(secondField->isVisible());
    QVERIFY(!secondField->isConflicted());
    QCOMPARE(secondField->pos().x(), qreal(0));
    QCOMPARE(secondField->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(secondField->getOffset(), quint64(4));
    QCOMPARE(secondField->getLastAddress(), quint64(7));
    QCOMPARE(secondField->getDisplayOffset(), quint64(7));
    QCOMPARE(secondField->getDisplayLastAddress(), quint64(4));

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testFieldInSecondAddress()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testFieldInSecondAddress()
{
    RegisterGraphItem* registerItem = createRegisterItem();

    FieldGraphItem* fieldItem = createFieldItem("", 0, 7, registerItem);

    expandItem(registerItem);

    QList<MemoryGapItem*> gaps = findMemoryGaps(registerItem);
    QCOMPARE(gaps.count(), 1);

    MemoryGapItem* emptyMSB = gaps.first();
    QVERIFY(!emptyMSB->isConflicted());
    QCOMPARE(emptyMSB->getName(), QString("Reserved"));
    QCOMPARE(emptyMSB->pos().x(), qreal(0));
    QCOMPARE(emptyMSB->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(emptyMSB->getDisplayOffset(), quint64(7));
    QCOMPARE(emptyMSB->getDisplayLastAddress(), quint64(7));
    QCOMPARE(emptyMSB->boundingRect().width(), registerItem->boundingRect().width()/8);

    QVERIFY(fieldItem->isVisible());
    QCOMPARE(fieldItem->pos().x(), emptyMSB->boundingRect().right());
    QCOMPARE(fieldItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(fieldItem->getOffset(), quint64(0));
    QCOMPARE(fieldItem->getLastAddress(), quint64(6));
    QCOMPARE(fieldItem->getDisplayOffset(), quint64(6));
    QCOMPARE(fieldItem->getDisplayLastAddress(), quint64(0));
    QCOMPARE(fieldItem->boundingRect().width(), 7*registerItem->boundingRect().width()/8);

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testEmptyAfterLastField()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testEmptyAfterLastField()
{
    RegisterGraphItem* registerItem = createRegisterItem();
    FieldGraphItem* fieldItem = createFieldItem("", 1, 7, registerItem);

    expandItem(registerItem);

    QList<MemoryGapItem*> gaps = findMemoryGaps(registerItem);
    QCOMPARE(gaps.count(), 1);

    QVERIFY(fieldItem->isVisible());
    QCOMPARE(fieldItem->pos().x(), qreal(0));
    QCOMPARE(fieldItem->getOffset(), quint64(1));
    QCOMPARE(fieldItem->getLastAddress(), quint64(7));
    QCOMPARE(fieldItem->getDisplayOffset(), quint64(7));
    QCOMPARE(fieldItem->getDisplayLastAddress(), quint64(1));
    QCOMPARE(fieldItem->boundingRect().width(), 7*registerItem->boundingRect().width()/8);

    MemoryGapItem* emptyLSB = gaps.first();
    QVERIFY(!emptyLSB->isConflicted());
    QCOMPARE(emptyLSB->getName(), QString("Reserved"));
    QCOMPARE(emptyLSB->pos().x(), fieldItem->boundingRect().right());
    QCOMPARE(emptyLSB->getDisplayOffset(), quint64(0));
    QCOMPARE(emptyLSB->getDisplayLastAddress(), quint64(0));
    QCOMPARE(emptyLSB->boundingRect().width(), registerItem->boundingRect().width()/8);

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testFieldOutsideRegisterIsNotValid()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testFieldOutsideRegisterIsNotValid()
{
    RegisterGraphItem* registerItem = createRegisterItem();
    FieldGraphItem* fieldItem = createFieldItem("", 15, 1, registerItem);

    expandItem(registerItem);

    QList<MemoryGapItem*> gaps = findMemoryGaps(registerItem);
    QCOMPARE(gaps.count(), 1);

    QVERIFY(fieldItem->isConflicted());

    MemoryGapItem* placeholderForFields = gaps.first();
    QVERIFY(!placeholderForFields->isConflicted());
    QCOMPARE(placeholderForFields->getName(), QString("Reserved"));
    QCOMPARE(placeholderForFields->pos().x(), qreal(0));
    QCOMPARE(placeholderForFields->getDisplayOffset(), quint64(7));
    QCOMPARE(placeholderForFields->getDisplayLastAddress(), quint64(0));
    QCOMPARE(placeholderForFields->boundingRect().width(), registerItem->boundingRect().width());

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testFieldEndingOutsideRegisterIsNotValid()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testFieldEndingOutsideRegisterIsNotValid()
{
    RegisterGraphItem* registerItem = createRegisterItem();
    FieldGraphItem* fieldItem = createFieldItem("", 7, 8, registerItem);

    expandItem(registerItem);

    QList<MemoryGapItem*> gaps = findMemoryGaps(registerItem);
    QCOMPARE(gaps.count(), 1);

    QVERIFY(fieldItem->isConflicted());
    QCOMPARE(fieldItem->pos().x(), qreal(0));
    QCOMPARE(fieldItem->getDisplayOffset(), quint64(14));
    QCOMPARE(fieldItem->getDisplayLastAddress(), quint64(7));
    QCOMPARE(fieldItem->boundingRect().width(), registerItem->boundingRect().width()/8);

    MemoryGapItem* placeholderForFields = gaps.first();
    QVERIFY(!placeholderForFields->isConflicted());
    QCOMPARE(placeholderForFields->getName(), QString("Reserved"));
    QCOMPARE(placeholderForFields->pos().x(), registerItem->boundingRect().width()/8);
    QCOMPARE(placeholderForFields->getDisplayOffset(), quint64(6));
    QCOMPARE(placeholderForFields->getDisplayLastAddress(), quint64(0));
    QCOMPARE(placeholderForFields->boundingRect().width(), 7*registerItem->boundingRect().width()/8);

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testEmptyBetweenTwoFields()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testEmptyBetweenTwoFields()
{
    RegisterGraphItem* registerItem = createRegisterItem();

    FieldGraphItem* msbItem = createFieldItem("msb", 6, 2, registerItem);
    FieldGraphItem* lsbItem = createFieldItem("lsb", 0, 3, registerItem);

    expandItem(registerItem);

    QList<MemoryGapItem*> gaps = findMemoryGaps(registerItem);
    QCOMPARE(gaps.count(), 1);

    QVERIFY(msbItem->isVisible());
    QCOMPARE(msbItem->pos().x(), qreal(0));
    QCOMPARE(msbItem->getOffset(), quint64(6));
    QCOMPARE(msbItem->getLastAddress(), quint64(7));
    QCOMPARE(msbItem->getDisplayOffset(), quint64(7));
    QCOMPARE(msbItem->getDisplayLastAddress(), quint64(6));
    QCOMPARE(msbItem->boundingRect().width(), 2*registerItem->boundingRect().width()/8);

    MemoryGapItem* emptySpace = gaps.first();
    QVERIFY(!emptySpace->isConflicted());
    QCOMPARE(emptySpace->getName(), QString("Reserved"));
    QCOMPARE(emptySpace->pos().x(), 2*registerItem->boundingRect().width()/8);
    QCOMPARE(emptySpace->getDisplayOffset(), quint64(5));
    QCOMPARE(emptySpace->getDisplayLastAddress(), quint64(3));
    QCOMPARE(emptySpace->boundingRect().width(), 3*registerItem->boundingRect().width()/8);

    QVERIFY(lsbItem->isVisible());
    QCOMPARE(lsbItem->pos().x(), msbItem->boundingRect().width() + emptySpace->boundingRect().width());
    QCOMPARE(lsbItem->getOffset(), quint64(0));
    QCOMPARE(lsbItem->getLastAddress(), quint64(2));
    QCOMPARE(lsbItem->getDisplayOffset(), quint64(2));
    QCOMPARE(lsbItem->getDisplayLastAddress(), quint64(0));
    QCOMPARE(lsbItem->boundingRect().width(), 3*registerItem->boundingRect().width()/8);

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testPartiallyOverlappingFields()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testPartiallyOverlappingFields()
{
    RegisterGraphItem* registerItem = createRegisterItem();
    FieldGraphItem* msbItem = createFieldItem("msb", 4, 4, registerItem);
    FieldGraphItem* lsbItem = createFieldItem("lsb", 0, 5, registerItem);

    expandItem(registerItem);

    QVERIFY(msbItem->isVisible());
    QVERIFY(msbItem->isConflicted());
    QCOMPARE(msbItem->pos().x(), qreal(0));
    QCOMPARE(msbItem->getDisplayOffset(), quint64(7));
    QCOMPARE(msbItem->getDisplayLastAddress(), quint64(4));
    QCOMPARE(msbItem->boundingRect().width(), qreal(100));

    QCOMPARE(findMemoryGaps(registerItem).count(), 0);
   
    QVERIFY(lsbItem->isVisible());
    QVERIFY(msbItem->isConflicted());
    QCOMPARE(lsbItem->pos().x(), 3*registerItem->boundingRect().width()/8);
    QCOMPARE(lsbItem->getDisplayOffset(), quint64(4));
    QCOMPARE(lsbItem->getDisplayLastAddress(), quint64(0));
    QCOMPARE(lsbItem->boundingRect().width(), 5*registerItem->boundingRect().width()/8);

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testFullyOverlappingFields()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testFullyOverlappingFields()
{
    RegisterGraphItem* registerItem = createRegisterItem();
    FieldGraphItem* firstItem = createFieldItem("first", 0, 4, registerItem);
    FieldGraphItem* secondItem = createFieldItem("second", 0, 4, registerItem);

    expandItem(registerItem);

    QVERIFY(firstItem->isVisible());
    QVERIFY(firstItem->isConflicted());
    QCOMPARE(firstItem->pos().x(), qreal(100));
    QCOMPARE(firstItem->getDisplayOffset(), quint64(3));
    QCOMPARE(firstItem->getDisplayLastAddress(), quint64(0));
    QCOMPARE(firstItem->boundingRect().width(), qreal(100));

    QVERIFY(secondItem->isVisible());
    QVERIFY(secondItem->isConflicted());
    QCOMPARE(secondItem->pos().x(), qreal(100));
    QCOMPARE(secondItem->getDisplayOffset(), quint64(3));
    QCOMPARE(secondItem->getDisplayLastAddress(), quint64(0));
    QCOMPARE(secondItem->boundingRect().width(), qreal(100));

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testMultipleFieldsOverlapping()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testMultipleFieldsOverlapping()
{
    RegisterGraphItem* registerItem = createRegisterItem();

    FieldGraphItem* largeItem = createFieldItem("largeField", 0, 8, registerItem);

    FieldGraphItem* firstOverlap = createFieldItem("firstOverlap", 6, 2, registerItem);
    FieldGraphItem* secondOverlap = createFieldItem("secondOverlap", 0, 2, registerItem);

    expandItem(registerItem);

    QList<MemoryGapItem*> conflictedAreas = findMemoryGaps(registerItem);
    QCOMPARE(conflictedAreas.count(), 0);

    QVERIFY(largeItem->isVisible());
    QVERIFY(largeItem->isConflicted());
    QCOMPARE(largeItem->pos().x(), qreal(0));
    QCOMPARE(largeItem->getDisplayOffset(), quint64(7));
    QCOMPARE(largeItem->getDisplayLastAddress(), quint64(0));
    QCOMPARE(largeItem->boundingRect().width(), registerItem->boundingRect().width());

    QVERIFY(firstOverlap->isVisible());  
    QVERIFY(firstOverlap->isConflicted());
    QVERIFY(!firstOverlap->isCompletelyOverlapped());
    QCOMPARE(firstOverlap->boundingRect().width(), 2*registerItem->boundingRect().width()/8);

    QVERIFY(secondOverlap->isVisible());
    QVERIFY(secondOverlap->isConflicted());
    QVERIFY(!secondOverlap->isCompletelyOverlapped());
    QCOMPARE(secondOverlap->boundingRect().width(), 2*registerItem->boundingRect().width()/8);

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testTwoFieldsCompletelyOverlappingThird()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testTwoFieldsCompletelyOverlappingThird()
{
    RegisterGraphItem* registerItem = createRegisterItem();

    FieldGraphItem* msbItem = createFieldItem("msb", 4, 4, registerItem);
    FieldGraphItem* overlappedItem = createFieldItem("overlapped", 2, 4, registerItem);
    FieldGraphItem* lsbItem = createFieldItem("lsb", 0, 4, registerItem);

    expandItem(registerItem);

    QList<MemoryGapItem*> conflictedAreas = findMemoryGaps(registerItem);
    QCOMPARE(conflictedAreas.count(), 0);

    QVERIFY(msbItem->isConflicted());
    QCOMPARE(msbItem->pos().x(), qreal(0));
    QCOMPARE(msbItem->getDisplayOffset(), quint64(7));
    QCOMPARE(msbItem->getDisplayLastAddress(), quint64(4));
    QCOMPARE(msbItem->boundingRect().width(), registerItem->boundingRect().width()/2);

    QVERIFY(overlappedItem->isConflicted());
    QCOMPARE(overlappedItem->pos().x(), registerItem->boundingRect().width()/4);
    QCOMPARE(overlappedItem->getDisplayOffset(), quint64(5));
    QCOMPARE(overlappedItem->getDisplayLastAddress(), quint64(2));
    QCOMPARE(overlappedItem->boundingRect().width(), registerItem->boundingRect().width()/2);

    QVERIFY(lsbItem->isConflicted());
    QCOMPARE(lsbItem->pos().x(), registerItem->boundingRect().width()/2);
    QCOMPARE(lsbItem->getDisplayOffset(), quint64(3));
    QCOMPARE(lsbItem->getDisplayLastAddress(), quint64(0));
    QCOMPARE(lsbItem->boundingRect().width(), registerItem->boundingRect().width()/2);

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testZeroWidthFields()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testZeroWidthFields()
{
    RegisterGraphItem* registerItem = createRegisterItem();

    FieldGraphItem* msbItem = createFieldItem("msb", 0, 0, registerItem);
    FieldGraphItem* lsbItem = createFieldItem("lsb", 0, 0, registerItem);

    expandItem(registerItem);

    QList<MemoryGapItem*> emptySpaces = findMemoryGaps(registerItem);
    QCOMPARE(emptySpaces.count(), 1);

    QVERIFY(msbItem->isConflicted());
    QCOMPARE(msbItem->boundingRect().width(), qreal(0));

    MemoryGapItem* placeholder = emptySpaces.first();
    QVERIFY(!placeholder->isConflicted());
    QCOMPARE(placeholder->pos().x(), qreal(0));
    QCOMPARE(placeholder->getDisplayOffset(), quint64(7));
    QCOMPARE(placeholder->getDisplayLastAddress(), quint64(0));
    QCOMPARE(placeholder->boundingRect().width(), registerItem->boundingRect().width());

    QVERIFY(lsbItem->isConflicted());
    QCOMPARE(lsbItem->boundingRect().width(), qreal(0));

    delete registerItem->parent();
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testTwoDimensional()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testTwoDimensional()
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName("testBlock");
    addressBlock->setBaseAddress(0);
    addressBlock->setRange("4");

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, noParser, 0);
    addressBlockItem->setAddressableUnitBits(8);

    QSharedPointer<Register> twoDimensionalRegister(new Register());
    twoDimensionalRegister->setName("testRegister");
    twoDimensionalRegister->setAddressOffset("1");
    twoDimensionalRegister->setSize(8);
    twoDimensionalRegister->setDim(2);


    RegisterGraphItem* registerItem = new RegisterGraphItem(twoDimensionalRegister, noParser, addressBlockItem);
    registerItem->setDimensionIndex(0);
    registerItem->updateDisplay();

    RegisterGraphItem* secondDimension = new RegisterGraphItem(twoDimensionalRegister, noParser, addressBlockItem);
    secondDimension->setDimensionIndex(1);
    secondDimension->updateDisplay();

    QCOMPARE(registerItem->getName(), QString("testRegister[0]"));
    QCOMPARE(registerItem->getDisplayOffset(), quint64(1));
    QCOMPARE(registerItem->getDisplayLastAddress(), quint64(1));

    QCOMPARE(secondDimension->getName(), QString("testRegister[1]"));
    QCOMPARE(secondDimension->getDisplayOffset(), quint64(2));
    QCOMPARE(secondDimension->getDisplayLastAddress(), quint64(2));

    delete addressBlockItem;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testLastDimensionExceedsAddressBlockRange()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testLastDimensionExceedsAddressBlockRange()
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName("testBlock");
    addressBlock->setBaseAddress(0);
    addressBlock->setRange("2");

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, noParser, 0);
    addressBlockItem->setAddressableUnitBits(8);

    QSharedPointer<Register> twoDimensionalRegister(new Register());
    twoDimensionalRegister->setName("testRegister");
    twoDimensionalRegister->setAddressOffset(0);
    twoDimensionalRegister->setSize(16);
    twoDimensionalRegister->setDim(2);


    RegisterGraphItem* registerItem = new RegisterGraphItem(twoDimensionalRegister, noParser, addressBlockItem);
    registerItem->setDimensionIndex(0);
    RegisterGraphItem* secondDimension = new RegisterGraphItem(twoDimensionalRegister, noParser, addressBlockItem);
    secondDimension->setDimensionIndex(1);

    QCOMPARE(registerItem->getOffset(), quint64(0));
    QCOMPARE(registerItem->getLastAddress(), quint64(1));

    QCOMPARE(secondDimension->getOffset(), quint64(2));
    QCOMPARE(secondDimension->getLastAddress(), quint64(3));

    delete addressBlockItem;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testExpressions()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testExpressions()
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName("testBlock");
    addressBlock->setBaseAddress(0);
    addressBlock->setRange("6");

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, noParser, 0);
    addressBlockItem->setAddressableUnitBits(8);

    QSharedPointer<Register> testRegister(new Register());
    testRegister->setName("testRegister");
    testRegister->setAddressOffset("1+1");
    testRegister->setSizeExpression("8*2");
    testRegister->setDimensionExpression("3-1");

    QSharedPointer<ExpressionParser> expressionParser(new SystemVerilogExpressionParser());

    RegisterGraphItem* registerItem = new RegisterGraphItem(testRegister, expressionParser, addressBlockItem);
    
    QCOMPARE(registerItem->getName(), QString("testRegister[0]"));
    QCOMPARE(registerItem->getDisplayOffset(), quint64(2));
    QCOMPARE(registerItem->getDisplayLastAddress(), quint64(3));
    QCOMPARE(registerItem->getBitWidth(), 16);

    delete addressBlockItem;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::testNonPresentField()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::testNonPresentField()
{
    RegisterGraphItem* registerItem = createRegisterItem();

    QSharedPointer<Field> nonPresentField(new Field());
    nonPresentField->setName("invisibleField");
    nonPresentField->setBitOffset(2);
    nonPresentField->setBitWidth(2);
    nonPresentField->setIsPresentExpression("0");

    QSharedPointer<ExpressionParser> expressionParser(new SystemVerilogExpressionParser());

    FieldGraphItem* nonPresentItem = new FieldGraphItem(nonPresentField, expressionParser, registerItem);
    registerItem->addChild(nonPresentItem);

    FieldGraphItem* presentItem = createFieldItem("visibleField", 1, 3, registerItem);

    expandItem(registerItem);
    
    QCOMPARE(nonPresentItem->isVisible(), false);

    QCOMPARE(presentItem->isConflicted(), false);

    QList<MemoryGapItem*> reservedSpaces = findMemoryGaps(registerItem);

    QCOMPARE(reservedSpaces.size(), 2);
    MemoryGapItem* reservedMSBs = reservedSpaces.first();
    QCOMPARE(reservedMSBs->getDisplayOffset(), quint64(7));
    QCOMPARE(reservedMSBs->getDisplayLastAddress(), quint64(4));

    MemoryGapItem* reservedLSB = reservedSpaces.last();
    QCOMPARE(reservedLSB->getDisplayOffset(), quint64(0));
    QCOMPARE(reservedLSB->getDisplayLastAddress(), quint64(0));

    delete registerItem->parentItem();
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::expandItem()
//-----------------------------------------------------------------------------
void tst_RegisterGraphItem::expandItem(RegisterGraphItem* registerItem)
{
    GraphicsExpandCollapseItem* expander(0);
    foreach (QGraphicsItem* item, registerItem->childItems())
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
// Function: tst_RegisterGraphItem::findMemoryGaps()
//-----------------------------------------------------------------------------
QList<MemoryGapItem*> tst_RegisterGraphItem::findMemoryGaps(RegisterGraphItem* memoryMapItem)
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

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::createRegisterItem()
//-----------------------------------------------------------------------------
RegisterGraphItem* tst_RegisterGraphItem::createRegisterItem()
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName("testBlock");
    addressBlock->setBaseAddress(0);
    addressBlock->setRange("1");

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, noParser, 0);
    addressBlockItem->setAddressableUnitBits(8);

    QSharedPointer<Register> testRegister(new Register());
    testRegister->setName("testRegister");
    testRegister->setAddressOffset(0);
    testRegister->setSize(8);

    return new RegisterGraphItem(testRegister, noParser, addressBlockItem);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterGraphItem::createFieldItem()
//-----------------------------------------------------------------------------
FieldGraphItem* tst_RegisterGraphItem::createFieldItem(QString name, unsigned int offset, unsigned int bitWidth,
    RegisterGraphItem* parentRegister)
{
    QSharedPointer<Field> field(new Field());
    field->setName(name);
    field->setBitOffset(offset);
    field->setBitWidth(bitWidth);
    
    QSharedPointer<ExpressionParser> expressionParser(new SystemVerilogExpressionParser());

    FieldGraphItem* fieldItem = new FieldGraphItem(field, expressionParser, parentRegister);
    parentRegister->addChild(fieldItem);

    return fieldItem;
}

QTEST_MAIN(tst_RegisterGraphItem)

#include "tst_RegisterGraphItem.moc"
