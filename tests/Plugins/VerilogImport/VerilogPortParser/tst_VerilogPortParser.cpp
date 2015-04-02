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

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <IPXACTmodels/component.h>

Q_DECLARE_METATYPE(General::Direction)

class tst_VerilogPortParser : public QObject
{
    Q_OBJECT

public:
    tst_VerilogPortParser();

    private slots:
        void init();

        // Test cases:
        void testNothingIsParsedFromMalformedInput();
        void testNothingIsParsedFromMalformedInput_data();

        void testVerilog2001PortIsParsed();
        void testVerilog2001PortIsParsed_data();

        void testVerilog1995PortIsParsed();
        void testVerilog1995PortIsParsed_data();

        void testMultiplePortsAreParsed();
        void testMultiplePortsAreParsed_data();

        void testMultipleVerilog1995PortsAreParsed();
        void testMultipleVerilog1995PortsAreParsed_data();

        void testPortsInSubmodulesAreNotParsed();
        void testPortsInSubmodulesAreNotParsed_data();

        void testEquationAsPortBound();
        void testEquationAsPortBound_data();

        void testArrayPort();
        void testArrayPort_data();

        void testMacroInBoundsIsParsed();
        void testMacroInBoundsIsParsed_data();
private:

    void runParser(QString const& input);   

    QSharedPointer<Component> importComponent_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::tst_VerilogPortParser()
//-----------------------------------------------------------------------------
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
    QCOMPARE(importComponent_->getModelParameters()->count(), 0);
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

    QTest::newRow("Unknown direction") <<
        "module test (\n"
        "    in data\n"
        ");\n"
        "endmodule";


    QTest::newRow("Illegal characters in port name") <<
        "module test (\n"
        "    input data%for\n"
        ");\n"
        "endmodule";

    QTest::newRow("Typed 1995-style port") <<
        "module test (data);\n"
        "    output reg data;\n"
        "\n"
        "endmodule";

    QTest::newRow("Signed 1995-style port.") <<
        "module test (data);\n"
        "    output wire signed [2:0] data; // 2's complement data.\n"
        "\n"
        "endmodule";

    QTest::newRow("Port after endmodule") <<
        "module test (data);\n"
        "endmodule\n"
        "    input data;";

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::runParser()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::runParser(QString const& input)
{
    QSharedPointer<ParameterFinder> finder(new ComponentParameterFinder(importComponent_));
    QSharedPointer<ExpressionParser> expressionParser(new IPXactSystemVerilogParser(finder));

    VerilogPortParser parser;
    parser.setExpressionParser(expressionParser);
    parser.import(input, importComponent_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testPortIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testVerilog2001PortIsParsed()
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
void tst_VerilogPortParser::testVerilog2001PortIsParsed_data()
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
// Function: tst_VerilogPortParser::testVerilog1995PortIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testVerilog1995PortIsParsed()
{
    QFETCH(QString, input);
    QFETCH(QString, expectedPortName);
    QFETCH(General::Direction, expectedDirection);
    QFETCH(int, expectedLeft);
    QFETCH(int, expectedRight);
    QFETCH(QString, expectedDescription);

    runParser(input);

    QCOMPARE(importComponent_->getPorts().count(), 1);

    QSharedPointer<Port> createdPort = importComponent_->getPorts().first();

    QCOMPARE(createdPort->getName(), expectedPortName);
    QCOMPARE(createdPort->getTypeName(), QString(""));
    QCOMPARE(createdPort->getDirection(), expectedDirection);
    QCOMPARE(createdPort->getLeftBound(), expectedLeft);
    QCOMPARE(createdPort->getRightBound(), expectedRight);
    QCOMPARE(createdPort->getDescription(), expectedDescription);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testVerilog1995PortIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testVerilog1995PortIsParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedPortName");
    QTest::addColumn<General::Direction>("expectedDirection");
    QTest::addColumn<int>("expectedLeft");
    QTest::addColumn<int>("expectedRight");
    QTest::addColumn<QString>("expectedDescription");

    QTest::newRow("Simplest possible port") <<
        "module test (clk);\n"
        "    input clk;\n"
        "\n"
        "endmodule"
        << "clk" << General::IN << 0 << 0 << "";

    QTest::newRow("Little-endian port") <<
    "module test (data);\n"
    "    input [7:0] data;\n"
    "\n"
    "endmodule"
    << "data" <<  General::IN << 7 << 0 << "";

    QTest::newRow("Big-endian port") <<
    "module test (data);\n"
    "    input [0:7] data;\n"
    "\n"
    "endmodule"
    << "data" <<  General::IN << 0 << 7 << "";
    
    QTest::newRow("Comment as a description.") <<
    "module test (rst_n);\n"
    "    input rst_n; // Active low reset.\n"
    "\n"
    "endmodule"
    << "rst_n" <<  General::IN << 0 << 0 << "Active low reset.";

    QTest::newRow("Additional whitespaces in port declaration") <<
    "module test (cs);output \n"
    "\n"
    "    [1:0] \n"
    "    cs  ;  //Chip select\n"
    "\n"
    "endmodule"
    << "cs" <<  General::OUT << 1 << 0 << "Chip select";

    QTest::newRow("Module with parameters.") <<
    "module test (clk);\n"
    "    parameter size = 8;"
    "    input clk;\n"
    "\n"
    "endmodule"
    << "clk" <<  General::IN << 0 << 0 << "";
    
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultiplePortsAreParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultiplePortsAreParsed()
{
    QFETCH(QString, input);
    QFETCH(QStringList, expectedPorts);

    runParser(input);

    QCOMPARE(importComponent_->getPortNames(), expectedPorts);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultiplePortsAreParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultiplePortsAreParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QStringList>("expectedPorts");

    QTest::newRow("Three basic ports") <<
        "module test (\n"
        "    input clk,\n"
        "    input rst_n,\n"
        "    output reg [7:0] data\n"
        ");\n"
        "endmodule"
        << QString("clk, rst_n, data").split(", ");

    QTest::newRow("Comments among ports") <<
        "module test ( // Ports are declared here.\n"
        "    input clk,\n"  
        "    // Comment between ports.\n"
        "    input rst_n\n"
        "    // End ports.\n"
        ");\n"
        "endmodule" 
        << QString("clk, rst_n").split(", ");

    QTest::newRow("Multiple declarations at once") <<
        "module test (\n"
        "    input clk, rst_n, enable,\n" 
        "    output full, ack\n" 
        ");\n"
        "endmodule"
        << QString("clk, rst_n, enable, full, ack").split(", ");

    // TODO: Syntactical error. What is the correct behavior?
    QTest::newRow("last port with tailing colon") << 
        "module test(\n"
        "    input clk,\n"
        "    input rst,\n"
        "    input enable,\n"
        ");\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");

    QTest::newRow("Commented out ports") << 
        "module test(\n"
        "    //input clk,\n"
        "    //input rst,\n"
        "    input enable\n"
        ");\n"
        "endmodule"
        << QString("enable").split(", ");

    QTest::newRow("Commented out ports using multiline comment") << 
        "module test(\n"
        "    input clk,\n"        
        "    input rst,\n"
        "/* COMMENT OUT FOR SYNTHESIS\n"
        "    output sim_value,\n"
        "    output sim_debug,\n"
        "*/\n"
        "    input enable \n"
        ");\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");

    QTest::newRow("Misplaced comma") << 
        "module test(\n"
        "    input clk,\n" 
        "    input rst\n"
        "    ,input enable \n"
        ");\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");

        QTest::newRow("Conditional ports") << 
        "module test(\n"
        "    `ifdef SYNCHRONOUS\n"
        "    input clk,\n" 
        "    `endif\n"
        "    input rst\n"
        "    `ifdef ENABLE_PIN\n"
        "    ,input enable \n"
        "    `endif\n"
        ");\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");

        QTest::newRow("Conditional port and comma on next line") << 
            "module test(\n"
            "    input clk\n" 
            "    `ifdef EXTERNAL_RESET\n"
            "    ,input rst // reset\n"
            "    `endif\n"
            "    `ifdef ENABLE_PIN\n"
            "    ,input enable\n"
            "    `endif\n"
            ");\n"
            "endmodule"
            << QString("clk, rst, enable").split(", ");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultipleVerilog1995PortsAreParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultipleVerilog1995PortsAreParsed()
{
    testMultiplePortsAreParsed();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultipleVerilog1995PortsAreParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultipleVerilog1995PortsAreParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QStringList>("expectedPorts");

    QTest::newRow("Three basic ports") <<
        "module test (clk, rst_n, data);\n"
        "    input clk;\n"
        "    input rst_n;\n"
        "    output [7:0] data;\n"
        "\n"
        "endmodule"
        << QString("clk, rst_n, data").split(", ");
    
    QTest::newRow("Comments among ports") <<
        "module test ( // Ports are declared here.\n"
        "clk, // Only names declared here.\n"
        "rst_n);"
        "    // Declare also direction for ports.\n"
        "    input clk;\n"  
        "    // Comment between ports.\n"
        "    input rst_n;\n"
        "    // End ports.\n"
        "\n"
        "endmodule" 
        << QString("clk, rst_n").split(", ");
    
    QTest::newRow("Multiple declarations at once") <<
        "module test (clk, rst_n, enable, full, ack);\n"
        "    input clk, rst_n, enable;\n" 
        "    output full, ack;\n" 
        "\n"
        "endmodule"
        << QString("clk, rst_n, enable, full, ack").split(", ");

    QTest::newRow("last port without tailing semicolon") << 
        "module test(clk, rst, enable);\n"
        "    input clk;\n"
        "    input rst;\n"
        "    input enable\n"
        "\n"
        "endmodule"
        << QString("clk, rst").split(", ");

    QTest::newRow("Commented out ports") << 
        "module test(//clk, rst, \n"
        "            enable);\n"
        "    //input clk;\n"
        "    //input rst;\n"
        "    input enable;\n"
        "\n"
        "endmodule"
        << QString("enable").split(", ");

    QTest::newRow("Commented out ports using multiline commments") << 
        "module test(clk, rst, /* sim_value, sim_debug, */ enable);\n"
        "    input clk;\n"
        "    input rst;\n"
        "/* COMMENT OUT FOR SYNTHESIS\n"
        "    output sim_value;\n"
        "    output sim_debug;\n"
        "*/\n"
        "    input enable;\n"
        "\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");    
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testPortsInSubmodulesAreNotParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testPortsInSubmodulesAreNotParsed()
{
    testMultiplePortsAreParsed();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testPortsInSubmodulesAreNotParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testPortsInSubmodulesAreNotParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QStringList>("expectedPorts");

    QTest::newRow("Instantiation of submodule") <<
        "module tb_half_adder();\n"
        "    wire A;\n"
        "    wire B;\n"
        "    wire result_out;\n"
        "    wire carry_out;\n"
        "\n"
        "    half_adder dut(\n"
        "        .in1(A),\n"
        "        .in2(B),\n"
        "        .sum(result_out),\n"
        "        .carry(carry_out)\n"
        "    );\n"    
        "// ... \n"
        "\n"
        "endmodule"
        << QStringList();

    QTest::newRow("Instantiation of multiple submodules") <<
        "module full_adder(\n"
        "    input A, B, carry_in,\n"
        "    output sum, carry_out\n"
        ");\n"                
        "    wire ha1_ha2_sum;\n"
        "    wire c1;\n"
        "    wire c2;\n"
        "\n"
        "    half_adder ha1(\n"
        "        .in1(A),\n"
        "        .in2(B),\n"
        "        .sum(ha1_ha2_sum),\n"
        "        .carry(c1)\n"
        "    );\n"    
        "\n"
        "    half_adder ha2(\n"
        "        .in1(ha1_ha2_sum),\n"
        "        .in2(carry_in),\n"
        "        .sum(sum),\n"
        "        .carry(c2)\n"
        "    );\n"   
        "\n"
        "    or (carry_out, c1, c2);"
        "\n"
        "endmodule"
        << QString("A, B, carry_in, sum, carry_out").split(", ");

    QTest::newRow("Instantiation of multiple submodules using 1995 style module") <<
        "module full_adder(A, B, carry_in, sum, carry_out);\n"                
        "    input A, B, carry_in;\n"
        "    output sum, carry_out;\n"
        "\n"
        "    wire ha1_ha2_sum;\n"
        "    wire c1;\n"
        "    wire c2;\n"
        "\n"
        "    half_adder ha1(\n"
        "        .in1(A),\n"
        "        .in2(B),\n"
        "        .sum(ha1_ha2_sum),\n"
        "        .carry(c1)\n"
        "    );\n"    
        "\n"
        "    half_adder ha2(\n"
        "        .in1(ha1_ha2_sum),\n"
        "        .in2(carry_in),\n"
        "        .sum(sum),\n"
        "        .carry(c2)\n"
        "    );\n"   
        "\n"
        "    or (carry_out, c1, c2);"
        "\n"
        "endmodule"
        << QString("A, B, carry_in, sum, carry_out").split(", ");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testEquationAsPortBound()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testEquationAsPortBound()
{
    QFETCH(QString, fileContent);
    QFETCH(int, expectedLeft);
    QFETCH(int, expectedRight);
    QFETCH(QString, expectedLeftExpression);
    QFETCH(QString, expectedRightExpression);

    runParser(fileContent);

    QVERIFY2(importComponent_->getPorts().count() != 0, "No ports parsed from input.");

    QSharedPointer<Port> createdPort = importComponent_->getPorts().first();
    QCOMPARE(createdPort->getLeftBound(), expectedLeft);
    QCOMPARE(createdPort->getRightBound(), expectedRight);
    QCOMPARE(createdPort->getLeftBoundExpression(), expectedLeftExpression);
    QCOMPARE(createdPort->getRightBoundExpression(), expectedRightExpression);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testEquationAsPortBound_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testEquationAsPortBound_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("expectedLeft");
    QTest::addColumn<int>("expectedRight");
    QTest::addColumn<QString>("expectedLeftExpression");
    QTest::addColumn<QString>("expectedRightExpression");

    QTest::newRow("Subtraction") <<
        "module test(\n"
        "    input [2-1:0] data\n"
        ");\n"
        "endmodule\n"
        << 1 << 0 << "2-1" << "0";

    QTest::newRow("Both bounds with multiply") <<
        "module test(\n"
        "    input [2*4-1:2*1] data\n"
        ");\n"
        "endmodule\n"
        << 7 << 2 << "2*4-1" << "2*1";

    QTest::newRow("Power of two") <<
        "module test(\n"
        "    input [2**4:0] data\n"
        ");\n"
        "endmodule\n"
        << 16 << 0 << "2**4" << "0";

    QTest::newRow("Power of zero") <<
        "module test(\n"
        "    input [8**0:0] data\n"
        ");\n"
        "endmodule\n"
        << 1 << 0 << "8**0" << "0";

    QTest::newRow("Multiple operations in mixed order") <<
        "module test(\n"
        "    input [32-2**4+2*4:0] data\n"
        ");\n"
        "endmodule\n"
        << 24 << 0 << "32-2**4+2*4" << "0";

    QTest::newRow("Division") <<
        "module test(\n"
        "    input [32/2:0/2] data\n"
        ");\n"
        "endmodule\n"
        << 16 << 0 << "32/2" << "0/2";

    QTest::newRow("Division by zero results zero") <<
        "module test(\n"
        "    input [32/0:0] data\n"
        ");\n"
        "endmodule\n"
        << 0 << 0 << "32/0" << "0";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testArrayPort()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testArrayPort()
{
    QSharedPointer<Parameter> macroDefinition(new Parameter());
    macroDefinition->setName("WIDTH");
    macroDefinition->setValue("16");
    macroDefinition->setValueId("macroId");
    importComponent_->getParameters()->append(macroDefinition);

    QFETCH(QString, fileContent);
    QFETCH(QString, expectedLeftExpression);
    QFETCH(QString, expectedRightExpression);

    runParser(fileContent);

    QVERIFY2(importComponent_->getPorts().count() != 0, "No ports parsed from input.");

    QSharedPointer<Port> createdPort = importComponent_->getPorts().first();
    QCOMPARE(createdPort->getArrayLeft(), expectedLeftExpression);
    QCOMPARE(createdPort->getArrayRight(), expectedRightExpression);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testArrayPort_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testArrayPort_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("expectedLeftExpression");
    QTest::addColumn<QString>("expectedRightExpression");

    QTest::newRow("No array bounds, only vector bounds") <<
        "module test(\n"
        "    input bit [7:0] data\n"
        ");\n"
        "endmodule\n"
        << "" << "";

    QTest::newRow("Constants as array bounds") <<
        "module test(\n"
        "    input bit [7:0] [0:0] data\n"
        ");\n"
        "endmodule\n"
        << "7" << "0";

    QTest::newRow("Expression as array bounds") <<
        "module test(\n"
        "    input bit [2**4:1] [0:0] data\n"
        ");\n"
        "endmodule\n"
        << "2**4" << "1";

    QTest::newRow("Expression with parenthesis as array bounds") <<
        "module test(\n"
        "    input bit [2*(8+8-1):1] [0:0] data\n"
        ");\n"
        "endmodule\n"
        << "2*(8+8-1)" << "1";

    QTest::newRow("Macro in left array bound") <<
        "module test(\n"
        "    input [`WIDTH:0] [7:0] data\n"
        ");\n"
        "endmodule\n"
        << "macroId" << "0";

    QTest::newRow("Macro in left v expression") <<
        "module test(\n"
        "    input [8+`WIDTH:0] [7:0] data\n"
        ");\n"
        "endmodule\n"
        << "8+macroId" << "0";

    QTest::newRow("Macro in right array bound") <<
        "module test(\n"
        "    input [0:`WIDTH] [7:0] data\n"
        ");\n"
        "endmodule\n"
        << "0" << "macroId";

    QTest::newRow("Macro in right array expression") <<
        "module test(\n"
        "    input [0:2*`WIDTH] [7:0] data\n"
        ");\n"
        "endmodule\n"
        << "0" << "2*macroId";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMacroInBoundsIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMacroInBoundsIsParsed()
{
    QSharedPointer<Parameter> macroDefinition(new Parameter());
    macroDefinition->setName("WIDTH");
    macroDefinition->setValue("16");
    macroDefinition->setValueId("macroId");
    importComponent_->getParameters()->append(macroDefinition);

    QFETCH(QString, fileContent);
    QFETCH(int, expectedLeft);
    QFETCH(int, expectedRight);
    QFETCH(QString, expectedLeftExpression);
    QFETCH(QString, expectedRightExpression);

    runParser(fileContent);

    QVERIFY2(importComponent_->getPorts().count() != 0, "No ports parsed from input.");

    QSharedPointer<Port> createdPort = importComponent_->getPorts().first();
    QCOMPARE(createdPort->getLeftBound(), expectedLeft);
    QCOMPARE(createdPort->getRightBound(), expectedRight);
    QCOMPARE(createdPort->getLeftBoundExpression(), expectedLeftExpression);
    QCOMPARE(createdPort->getRightBoundExpression(), expectedRightExpression);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMacroInBoundsIsParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMacroInBoundsIsParsed_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("expectedLeft");
    QTest::addColumn<int>("expectedRight");
    QTest::addColumn<QString>("expectedLeftExpression");
    QTest::addColumn<QString>("expectedRightExpression");

    QTest::newRow("Macro in left vector bound") <<
        "module test(\n"
        "    input [`WIDTH:0] data\n"
        ");\n"
        "endmodule\n"
        << 16 << 0 << "macroId" << "0";

    QTest::newRow("Macro in left vector expression") <<
        "module test(\n"
        "    input [8+`WIDTH:0] data\n"
        ");\n"
        "endmodule\n"
        << 24 << 0 << "8+macroId" << "0";

    QTest::newRow("Macro in right vector bound") <<
        "module test(\n"
        "    input [0:`WIDTH] data\n"
        ");\n"
        "endmodule\n"
        << 0 << 16 << "0" << "macroId";

    QTest::newRow("Macro in right vector expression") <<
        "module test(\n"
        "    input [0:2*`WIDTH] data\n"
        ");\n"
        "endmodule\n"
        << 0 << 32 << "0" << "2*macroId";
}

QTEST_APPLESS_MAIN(tst_VerilogPortParser)

#include "tst_VerilogPortParser.moc"
