//-----------------------------------------------------------------------------
// File: tst_VerilogImporter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.09.2014
//
// Description:
// Unit test for class VerilogImporter.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QSharedPointer>

#include <Plugins/VerilogImport/VerilogImporter.h>

#include <IPXACTmodels/component.h>


class tst_VerilogImporter : public QObject
{
    Q_OBJECT

public:
    tst_VerilogImporter();

private slots:
    void init();
    void testNothingIsParsedFromMalformedInput();
    void testNothingIsParsedFromMalformedInput_data();


private:

    void runParser(QString const& input);

    QSharedPointer<Component> importComponent_;
};

tst_VerilogImporter::tst_VerilogImporter(): importComponent_(new Component())
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::init()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::init()
{
    importComponent_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testNothingIsParsedFromMalformedInput()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testNothingIsParsedFromMalformedInput()
{
    QFETCH(QString, input);

    runParser(input);

    QCOMPARE(importComponent_->getPorts().count(), 0);
    QCOMPARE(importComponent_->getModelParameters().count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testNothingIsParsedFromMalformedInput_data()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testNothingIsParsedFromMalformedInput_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("empty input") << "";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::runParser()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::runParser(QString const& input)
{
    VerilogImporter parser;
    parser.runParser(input, importComponent_);
}

QTEST_APPLESS_MAIN(tst_VerilogImporter)

#include "tst_VerilogImporter.moc"
