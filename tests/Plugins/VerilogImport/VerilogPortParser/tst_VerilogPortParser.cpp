//-----------------------------------------------------------------------------
// File: tst_VerilogPortParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.09.2014
//
// Description:
// Unit test for class VerilogPortParser.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogImport/VerilogPortParser.h>

#include <IPXACTmodels/component.h>

Q_DECLARE_METATYPE(General::Direction)

class tst_VerilogPortParser : public QObject
{
    Q_OBJECT

public:
    tst_VerilogPortParser();

    private slots:
        void init();
        void testNothingIsParsedFromMalformedInput();
        void testNothingIsParsedFromMalformedInput_data();

        void testPortIsParsed();
        void testPortIsParsed_data();

        void testMultiplePortsAreParsed();
        void testMultiplePortsAreParsed_data();

private:

    void runParser(QString const& input);

    QSharedPointer<Component> importComponent_;
};

tst_VerilogPortParser::tst_VerilogPortParser(): importComponent_(new Component())
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::init()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::init()
{
    importComponent_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testNothingIsParsedFromMalformedInput()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testNothingIsParsedFromMalformedInput()
{
    QFETCH(QString, input);

    runParser(input);

    QCOMPARE(importComponent_->getPorts().count(), 0);
    QCOMPARE(importComponent_->getModelParameters().count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testNothingIsParsedFromMalformedInput_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testNothingIsParsedFromMalformedInput_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("empty input") << "";

    QTest::newRow("no module declared") << "// Comment line.";

    QTest::newRow("module without parathesis around port declarations") << 
        "module test\n"
        "input clk\n"
        "endmodule";

    QTest::newRow("module without closing parathesis for port declarations") << 
        "module test(\n"
        "input clk\n"
        "endmodule";

    QTest::newRow("module without endmodule") << 
        "module test(\n"
        "    input clk\n"
        ");";

    QTest::newRow("last port with tailing semicolon") << 
        "module test(\n"
        "    input clk;\n"
        ");\n"
        "endmodule";

    QTest::newRow("Unknown direction") <<
        "module test (\n"
        "    in data\n"
        ");\n"
        "endmodule";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::runParser()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::runParser(QString const& input)
{
    VerilogPortParser parser;
    parser.runParser(input, importComponent_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testPortIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testPortIsParsed()
{
    QFETCH(QString, input);
    QFETCH(QString, expectedPortName);
    QFETCH(QString, expectedType);
    QFETCH(General::Direction, expectedDirection);
    QFETCH(int, expectedLeft);
    QFETCH(int, expectedRight);
    QFETCH(QString, expectedDescription);

    runParser(input);

    QCOMPARE(importComponent_->getPorts().count(), 1);

    QSharedPointer<Port> createdPort = importComponent_->getPorts().first();

    QCOMPARE(createdPort->getName(), expectedPortName);
    QCOMPARE(createdPort->getTypeName(), expectedType);
    QCOMPARE(createdPort->getDirection(), expectedDirection);
    QCOMPARE(createdPort->getLeftBound(), expectedLeft);
    QCOMPARE(createdPort->getRightBound(), expectedRight);
    QCOMPARE(createdPort->getDescription(), expectedDescription);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testPortIsParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testPortIsParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedPortName");
    QTest::addColumn<QString>("expectedType");
    QTest::addColumn<General::Direction>("expectedDirection");
    QTest::addColumn<int>("expectedLeft");
    QTest::addColumn<int>("expectedRight");
    QTest::addColumn<QString>("expectedDescription");

    QTest::newRow("Simplest possible port") <<
        "module test (\n"
        "    input clk\n"
        ");\n"
        "endmodule"
        << "clk" << "" << General::IN << 0 << 0 << "";

    QTest::newRow("Typed port") <<
        "module test (\n"
        "    output reg data\n"
        ");\n"
        "endmodule"
        << "data" << "reg" <<  General::OUT << 0 << 0 << "";

    QTest::newRow("Little-endian port") <<
        "module test (\n"
        "    input [7:0] data\n"
        ");\n"
        "endmodule"
        << "data" << "" <<  General::IN << 7 << 0 << "";

    QTest::newRow("Big-endian port") <<
        "module test (\n"
        "    input [0:7] data\n"
        ");\n"
        "endmodule"
        << "data" << "" <<  General::IN << 0 << 7 << "";

    QTest::newRow("Typed four-bit port") <<
        "module test (\n"
        "    inout tri [3:0] data\n"
        ");\n"
        "endmodule"
        << "data" << "tri" <<  General::INOUT << 3 << 0 << "";

    QTest::newRow("Comment as a description.") <<
        "module test (\n"
        "    input rst_n // Active low reset.\n"
        ");\n"
        "endmodule"
        << "rst_n" << "" <<  General::IN << 0 << 0 << "Active low reset.";

    QTest::newRow("Signed port.") <<
        "module test (\n"
        "    output wire signed [2:0] data // 2's complement data.\n"
        ");\n"
        "endmodule"
        << "data" << "wire" <<  General::OUT << 2 << 0 << "2's complement data.";

    QTest::newRow("Additional whitespaces in port declaration") <<
        "module test (output \n"
        "reg\n"
        "\n"
        "    [1:0] \n"
        "    cs    //Chip select\n"
        ");\n"
        "endmodule"
        << "cs" << "reg" <<  General::OUT << 1 << 0 << "Chip select";

    QTest::newRow("Module with parameters.") <<
        "module test #(parameter id = 1) (\n"
        "    input clk\n"
        ");\n"
        "endmodule"
        << "clk" << "" <<  General::IN << 0 << 0 << "";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultiplePortsAreParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultiplePortsAreParsed()
{
    QFETCH(QString, input);
    QFETCH(int, expectedPortCount);

    runParser(input);

    QCOMPARE(importComponent_->getPorts().count(), expectedPortCount);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultiplePortsAreParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultiplePortsAreParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("expectedPortCount");

    QTest::newRow("Three basic ports") <<
        "module test (\n"
        "    input clk;\n"
        "    input rst_n;\n"
        "    output reg [7:0] data\n"
        ");\n"
        "endmodule"
        << 3;

    QTest::newRow("Comments among ports") <<
        "module test ( // Ports are declared here.\n"
        "    input clk;\n"  
        "    // Comment between ports.\n"
        "    input rst_n\n"
        "    // End ports.\n"
        ");\n"
        "endmodule" 
        << 2;

    QTest::newRow("Multiple declarations at once") <<
        "module test (\n"
        "    input clk, rst_n, enable;\n" 
        "    output full, ack\n" 
        ");\n"
        "endmodule"
        << 5;

}

QTEST_APPLESS_MAIN(tst_VerilogPortParser)

#include "tst_VerilogPortParser.moc"
