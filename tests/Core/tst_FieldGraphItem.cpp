//-----------------------------------------------------------------------------
// File: tst_FieldGraphItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.04.2015
//
// Description:
// Unit test for class FieldGraphItem.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/fieldgraphitem.h>
#include <editors/ComponentEditor/visualization/memorygapitem.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>
#include <editors/ComponentEditor/common/NullParser.h>

#include <IPXACTmodels/Component/Field.h>

class tst_FieldGraphItem : public QObject
{
    Q_OBJECT

public:
    tst_FieldGraphItem();
    
private slots:

    void testConstructor();

    void testExpressions();
};

//-----------------------------------------------------------------------------
// Function: tst_FieldGraphItem::tst_FieldGraphItem()
//-----------------------------------------------------------------------------
tst_FieldGraphItem::tst_FieldGraphItem()
{
}

//-----------------------------------------------------------------------------
// Function: tst_FieldGraphItem::testConstructor()
//-----------------------------------------------------------------------------
void tst_FieldGraphItem::testConstructor()
{
    QSharedPointer<Field> testField(new Field());
    testField->setName("testField");
    testField->setBitOffset("0");
    testField->setBitWidth("2");

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    FieldGraphItem* FieldItem = new FieldGraphItem(testField, noParser, 0);    

    QCOMPARE(FieldItem->name(), QString("testField"));
    QCOMPARE(FieldItem->getOffset(), quint64(0));
    QCOMPARE(FieldItem->getLastAddress(), quint64(1));
    QCOMPARE(FieldItem->getDisplayOffset(), quint64(1));
    QCOMPARE(FieldItem->getDisplayLastAddress(), quint64(0));

    QCOMPARE(FieldItem->pos().y(), qreal(0));

    delete FieldItem;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldGraphItem::testExpressions()
//-----------------------------------------------------------------------------
void tst_FieldGraphItem::testExpressions()
{
    QSharedPointer<Field> testField(new Field());
    testField->setBitOffset("1+1");
    testField->setBitWidth("2*2");

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    FieldGraphItem* FieldItem = new FieldGraphItem(testField, parser, 0);    

    QCOMPARE(FieldItem->getBitWidth(), 4);
    QCOMPARE(FieldItem->getOffset(), quint64(2));
    QCOMPARE(FieldItem->getLastAddress(), quint64(5));
    QCOMPARE(FieldItem->getDisplayOffset(), quint64(5));
    QCOMPARE(FieldItem->getDisplayLastAddress(), quint64(2));

    QCOMPARE(FieldItem->pos().y(), qreal(0));

    delete FieldItem;
}

QTEST_MAIN(tst_FieldGraphItem)

#include "tst_FieldGraphItem.moc"
