//-----------------------------------------------------------------------------
// File: tst_VerilogParameterParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 25.09.2014
//
// Description:
// Unit test for class VerilogParameterParser.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <Plugins/VerilogImport/VerilogParameterParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>

class tst_VerilogParameterParser : public QObject
{
    Q_OBJECT

public:
    tst_VerilogParameterParser();

private slots:
    void noParameters();
    void noPorts();

    void singleLineHeader();
    void multiLineHeader();
    void multiLineComments();
    void evilMultiLineComment();
    void commentLines();
    void multiDeclaration();
    void multiLineDeclarations();

    void oneParameter();
    void threeParameter();
    void spacedParameters();

    void describedParameters();
    void oneUnDescribed();
    void multiLineDescribed();
    void describedParentheses();
    void evilDescription();
	void testStringValue();

    void otherParameterAsParameterValue();
    void testAnsiArrayValue();
    void testAnsiArrayValue_data();

    void oldParameter();
    void multipleOldParameters();
    void variantOldParameters();
    void multiOldParameters();
    void oldWithEndModule();
    void doubleMultiOldParameters();       
    void testOldArrayValue();
    void testOldArrayValue_data();

    void parametersToComponent();
    void oldParametersToComponent();

    void operations();
    void oldOperations();
    void bases();
    void braces();
    void twoDimensional();
    void closerOnLine();
    void closerOnLineDescribedparentheses();
    void closerOnLineDescribedparentheses2();
    void parenthesesInPorts();
    void customCase();

private:
	void verifyParameter( QSharedPointer<Parameter> parameter,
		QString name, QString value, QString description );

};

void tst_VerilogParameterParser::verifyParameter( QSharedPointer<Parameter> parameter,
	QString name, QString value, QString description )
{
	QCOMPARE(parameter->name(), name);
	QCOMPARE(parameter->getValue(), value);
	QCOMPARE(parameter->description(), description);
}

tst_VerilogParameterParser::tst_VerilogParameterParser()
{
}

void tst_VerilogParameterParser::noParameters()
{
    QString input = "module shifter();\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE( declarations.size(), 0 );
}

void tst_VerilogParameterParser::noPorts()
{
    QString input = "module shifter #(parameter ok joku=12);\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE( declarations[0], QString( "parameter ok joku=12") );
}

void tst_VerilogParameterParser::singleLineHeader()
{
    QString input = "module shifter #(parameter ok joku=12) (\n"
        "input reset_n, //reset async active low\n"
        "input clk, //input clock\n"
        "input data_ena, //serial data enable\n"
        "input serial_data, //serial data input\n"
        "output reg [width-1:0] parallel_data //parallel data out\n"
        ");\n"
        "always @ (posedge clk, negedge reset_n)\n"
        "if(!reset_n) parallel_data <= �0; //could not do \"width�d0\"\n"
        "else if (data_ena)\n"
        "parallel_data <= {serial_data, parallel_data[width-1:1]};\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE( declarations.size(), 1 );
    QCOMPARE( declarations[0], QString( "parameter ok joku=12") );
}

void tst_VerilogParameterParser::multiLineHeader()
{
    QString input = "module shifter #(\n"
        "parameter ok width=8, count=6\n"                
        ") (\n"
        "input reset_n, //reset async active low\n"
        "input clk, //input clock\n"
        "input data_ena, //serial data enable\n"
        "input serial_data, //serial data input\n"
        "output reg [width-1:0] parallel_data //parallel data out\n"
        ");\n"
        "always @ (posedge clk, negedge reset_n)\n"
        "if(!reset_n) parallel_data <= �0; //could not do \"width�d0\"\n"
        "else if (data_ena)\n"
        "parallel_data <= {serial_data, parallel_data[width-1:1]};\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE( declarations.size(), 1 );
    QCOMPARE(declarations[0], QString("parameter ok width=8, count=6") );
}

void tst_VerilogParameterParser::multiLineComments()
{
    QString input = "module shifter #(\n"
        "parameter ok joku=8,\n"
        "/*uliuli uli*/\n"
        "esa=6,\n"
        "parameter joku=22,\n"
        "/*ULINA\n"
        "KALINA*/\n"
        "jaa joku=1234\n"
        ") (\n"
        "input reset_n, //reset async active low\n"
        "input clk, //input clock\n"
        "input data_ena, //serial data enable\n"
        "input serial_data, //serial data input\n"
        "/*APINOITA\n"
        "PITKIN OITA\n"
        "usrujsjjrr*/\n"
        "output reg [width-1:0] parallel_data //parallel data out \n"
        ");\n"
        "always @ (posedge clk, negedge reset_n)\n"
        "if(!reset_n) parallel_data <= �0; //could not do \"width�d0\"\n"
        "else if (data_ena)\n"
        "parallel_data <= {serial_data, parallel_data[width-1:1]};\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE( declarations.size(), 2 );
    QCOMPARE(declarations[0], QString("parameter ok joku=8,\n\nesa=6,") );
    QCOMPARE(declarations[1], QString("parameter joku=22,\n\njaa joku=1234"));
}

void tst_VerilogParameterParser::evilMultiLineComment()
{
    QString input = "module shifter #(\n"
        "parameter uus,\n"
        "parameter x\n"
        ") (\n"
        "input reset_n, //reset async active low\n"
        "input clk, //input clock\n"
        "output reg [width-1:0] parallel_data //parallel data out \n"
        ");/*APINOITA\n"
        "PITKIN OITA\n"
        "usrujsjjrr*/\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE( declarations.size(), 2 );
    QCOMPARE(declarations[0], QString("parameter uus,") );
    QCOMPARE(declarations[1], QString("parameter x"));
}

void tst_VerilogParameterParser::commentLines()
{
    QString input = "module shifter #(\n"
        "parameter ok joku=8, //herpaderp\n"
        "//uliuli uli\n"
        "esa=6,\n"
        "parameter joku=22,\n"
        "//ULINA\n"
        "//KALINA\n"
        "jaa joku=1234\n"
        ") (\n"
        "input reset_n, //reset async active low\n"
        "input clk, //input clock\n"
        "input data_ena, //serial data enable\n"
        "input serial_data, //serial data input\n"
        "//APINOITA\n"
        "//PITKIN OITA\n"
        "//usrujsjjrr\n"
        "output reg [width-1:0] parallel_data //parallel data out \n"
        ");\n"
        "always @ (posedge clk, negedge reset_n)\n"
        "if(!reset_n) parallel_data <= �0; //could not do \"width�d0\"\n"
        "else if (data_ena)\n"
        "parallel_data <= {serial_data, parallel_data[width-1:1]};\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE( declarations.size(), 2 );
    QCOMPARE(declarations[0], QString("parameter ok joku=8, //herpaderp\nesa=6,") );
    QCOMPARE(declarations[1], QString("parameter joku=22,\njaa joku=1234"));
}

void tst_VerilogParameterParser::multiDeclaration()
{
    QString input = "module shifter #(parameter ok joku=12, esa = 3, jari = 123);\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE(declarations.size(), 1);
    QCOMPARE(declarations[0], QString( "parameter ok joku=12, esa = 3, jari = 123"));
}

void tst_VerilogParameterParser::multiLineDeclarations()
{
    QString input = "module shifter #(parameter ok joku=12,\n"
        "esa = 3,\n"
        "jari = 123,\n"
        "parameter singed def[1:4] jees=1234, horspo = 567);\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE(declarations.size(), 2);
    QCOMPARE( declarations[0], QString( "parameter ok joku=12,\nesa = 3,\njari = 123,") );
    QCOMPARE( declarations[1], QString( "parameter singed def[1:4] jees=1234, horspo = 567") );
}

void tst_VerilogParameterParser::oneParameter()
{
    QString input = "module shifter #(\n"
        "parameter ok joku=8\n"
        ") (\n"
        "); endmodule\n";

    VerilogParameterParser parser;
    QString description;
    QString type;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE(declarations.size(), 1);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    QCOMPARE(parameters.size(), 1);
    verifyParameter( parameters[0], "joku", "8", "");
}

void tst_VerilogParameterParser::threeParameter()
{
    QString input = "module shifter #(\n"
        "parameter ok joku=8,\n"
        "parameter int esa=6,\n"
        "parameter joku=22\n"
        ") (\n"
        "); endmodule\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE(declarations.size(), 3);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6", "");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "joku", "22", "");
}

void tst_VerilogParameterParser::spacedParameters()
{
    QString input = "module shifter #(\n"
        "parameter ok joku =8,\n"
        "parameter esa= 6,\n"
        "parameter hopo = 22\n"
        ") (\n"
        "); endmodule\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE(declarations.size(), 3);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6", "");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "hopo", "22", "");
}

void tst_VerilogParameterParser::describedParameters()
{
    QString input = "module shifter #(\n"
        "parameter ok joku =8,//seli\n"
        "parameter int juu =98, //evo\n"
        "parameter esa= 6, // epeli\n"
        "parameter hopo = 22 // o olololo ollllooo \n"
        ") (\n"
        "); endmodule\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE(declarations.size(), 4);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "seli");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "juu", "98", "evo");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "esa", "6", "epeli");
    parameters.append(parser.parseParameters(declarations[3]));
    verifyParameter( parameters[3], "hopo", "22", "o olololo ollllooo");
}

void tst_VerilogParameterParser::oneUnDescribed()
{
    QString input = "module shifter #(\n"
        "parameter ok joku =8,//joku\n"
        "parameter int juu =98,\n"
        "parameter esa= 6 // epeli\n"
        ") (\n"
        "); endmodule\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "joku");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "juu", "98", "");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "esa", "6", "epeli");
}

void tst_VerilogParameterParser::multiLineDescribed()
{
    QString input = "module shifter #(\n"
        "parameter char joku =8, juu =98,\n"
        "esa= 6 // epeli\n"
        ") (\n"
        "); endmodule\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "epeli");
    verifyParameter( parameters[1], "juu", "98", "epeli");
    verifyParameter( parameters[2], "esa", "6", "epeli");
}

void tst_VerilogParameterParser::describedParentheses()
{
    QString input = "module shifter #(\n"
        "parameter esa= 6 , // epeli\n"
        "parameter hopo = 22 // o (olololo ollllooo) \n"
        ") (\n"
        "); endmodule\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "esa", "6", "epeli");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "hopo", "22", "o (olololo ollllooo)");
}

void tst_VerilogParameterParser::evilDescription()
{
    QString input = "module shifter #(\n"
        "parameter DATAWIDTH= 32 // module declaration ends here );\n"
        "parameter AUB = DATAWIDTH/4 // (oon); \n"
        ") (\n"
        "); endmodule\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "DATAWIDTH", "32", "module declaration ends here );");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "AUB", "DATAWIDTH/4", "(oon);");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogParameterParser::testStringValue()
//-----------------------------------------------------------------------------
void tst_VerilogParameterParser::testStringValue()
{
	QString input = "module shifter #(\n"
		"parameter type = \"barrel\", // \"barrel\" or \"arithmetic\"\n"
		"parameter DATAWIDTH = 8 \n"
		") (\n"
		"); endmodule\n";

	VerilogParameterParser parser;
	QStringList declarations = parser.findDeclarations(input);

	QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
	verifyParameter(parameters[0], "type", "\"barrel\"", "\"barrel\" or \"arithmetic\"");
	parameters.append(parser.parseParameters(declarations[1]));
	verifyParameter(parameters[1], "DATAWIDTH", "8", "");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogParameterParser::otherParameterAsParameterValue()
//-----------------------------------------------------------------------------
void tst_VerilogParameterParser::otherParameterAsParameterValue()
{
    QSharedPointer<Component> targetComponent(new Component(VLNV(), Document::Revision::Std14));

    QString input = "module test #(\n"
        "parameter first = 1,\n"
        "parameter second = first\n"
        ") ();\n"
		"endmodule";

	QSharedPointer<ComponentInstantiation> instance( new ComponentInstantiation );
	instance->setName("module_parameter_instantiation");
	targetComponent->getComponentInstantiations()->append(instance);

    VerilogParameterParser parser;
	parser.import(input, targetComponent, instance);

    verifyParameter(targetComponent->getParameters()->at(1), "second", targetComponent->getParameters()->at(0)->getValueId(), "");

    QCOMPARE(instance->getModuleParameters()->at(0)->getValue(), targetComponent->getParameters()->at(0)->getValueId());
    QCOMPARE(instance->getModuleParameters()->at(1)->getValue(), targetComponent->getParameters()->at(1)->getValueId());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogParameterParser::testArrayValue()
//-----------------------------------------------------------------------------
void tst_VerilogParameterParser::testAnsiArrayValue()
{
    QFETCH(QString, input);
    QFETCH(QString, expectedValue);

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter(parameters[0], "bits", expectedValue, "Array of bit vectors.");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogParameterParser::testAnsiArrayValue_data()
//-----------------------------------------------------------------------------
void tst_VerilogParameterParser::testAnsiArrayValue_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedValue");

    QTest::newRow("Array on a single rows")
        << "module arrayTest #(\n"
        "parameter [15:0] bits = {4'h0,4'h4,4'h8,4'hF} // Array of bit vectors.\n"
        ") ();\n"
        "endmodule"
        << "{4'h0,4'h4,4'h8,4'hF}";

    QTest::newRow("Array on multiple rows")
        << "module arrayTest #(\n"
        "parameter [15:0] bits = {4'h0,\n"
        "4'h4,\n"
        "4'h8,\n"
        "4'hF} // Array of bit vectors.\n"
        ") ();\n"
        "endmodule"
        << "{4'h0, 4'h4, 4'h8, 4'hF}";

    QTest::newRow("Array on multiple rows with trailing comments")
        << "module arrayTest #(\n"
        "parameter [15:0] bits = {4'h0, // Comment #2\n"
        "4'h4, // Comment #2.\n"
        "4'h8,\n"
        "4'hF} // Array of bit vectors.\n"
        ") ();\n"
        "endmodule"
        << "{4'h0, 4'h4, 4'h8, 4'hF}";

    QTest::newRow("Empty array")
        << "module arrayTest #(\n"
        "parameter [15:0] bits = {} // Array of bit vectors.\n"
        ") ();\n"
        "endmodule"
        << "{}";
}

void tst_VerilogParameterParser::oldParameter()
{
    QString input = "module shifter ("
    ");\n"
    "parameter evo joku=9; // ulina\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "9", "ulina");
}

void tst_VerilogParameterParser::multipleOldParameters()
{
    QString input = "module shifter (\n"
        "ok tttt=8\n"
        ");\n"
        "parameter joku=8; \n"
        "parameter jou olololoo= 1234; //hopotus\n"
        "parameter juuuuu = 22;//hapatus";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "olololoo", "1234", "hopotus");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juuuuu", "22", "hapatus");
}

void tst_VerilogParameterParser::variantOldParameters()
{
    QString input = "module shifter (\n"
        "ok tttt=8\n"
        ");\n"
        "parameter signed	range[12:99] joku=8; \n"
        "parameter range[8:9] olololoo= 1234; //hopotus\n"
        "parameter horspo juuuuu = 22//hapatus";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "olololoo", "1234", "hopotus");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juuuuu", "22", "hapatus");
}

void tst_VerilogParameterParser::multiOldParameters()
{
    QString input = "module shifter (\n"
        "ok tttt=8\n"
        ");\n"
        "parameter joku=8, \n"
        "//vali spiikki \n"
        "olololoo= 1234, //hopotus\n"
        "juuuuu = 22;//hapatus";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);

    verifyParameter( parameters[0], "joku", "8", "hapatus");
    verifyParameter( parameters[1], "olololoo", "1234", "hapatus");
    verifyParameter( parameters[2], "juuuuu", "22", "hapatus");
}

void tst_VerilogParameterParser::oldWithEndModule()
{
    QString input = "module shifter (\n"
        "ok tttt=8\n"
        ");\n"
        "parameter joku=8, \n"
        "//vali spiikki \n"
        "olololoo= 1234, //hopotus\n"
        "juuuuu = 22;//hapatus\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);

    verifyParameter( parameters[0], "joku", "8", "hapatus");
    verifyParameter( parameters[1], "olololoo", "1234", "hapatus");
    verifyParameter( parameters[2], "juuuuu", "22", "hapatus");
}

void tst_VerilogParameterParser::doubleMultiOldParameters()
{
    QString input = "module shifter (\n"
        "ok tttt=8\n"
        ");\n"
        "parameter joku=8, \n"
        "//vali spiikki \n"
        "olololoo= 1234, //hopotus\n"
        "juuuuu = 22;//hapatus\n"
        "parameter signed def[6:5] seppox = 5,\n"
        "jari = 1234,\n"
        "jorma = 7";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "hapatus");
    verifyParameter( parameters[1], "olololoo", "1234", "hapatus");
    verifyParameter( parameters[2], "juuuuu", "22", "hapatus");
    
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[3], "seppox", "5", "");
    verifyParameter( parameters[4], "jari", "1234", "");
    verifyParameter( parameters[5], "jorma", "7", "");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogParameterParser::testOldArrayValue()
//-----------------------------------------------------------------------------
void tst_VerilogParameterParser::testOldArrayValue()
{
    QFETCH(QString, input);
    QFETCH(QString, expectedValue);

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter(parameters[0], "bits", expectedValue, "Array of bit vectors.");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogParameterParser::testOldArrayValue_data()
//-----------------------------------------------------------------------------
void tst_VerilogParameterParser::testOldArrayValue_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedValue");

    QTest::newRow("Array on a single rows")
        << "module arrayTest();\n"
        "parameter [15:0] bits = {4'h0,4'h4,4'h8,4'hF}; // Array of bit vectors.\n"
        "endmodule"
        << "{4'h0,4'h4,4'h8,4'hF}";

    QTest::newRow("Array on multiple rows")
        << "module arrayTest();\n"
        "parameter [15:0] bits = {4'h0,\n"
        "4'h4,\n"
        "4'h8,\n"
        "4'hF}; // Array of bit vectors.\n"
        "endmodule"
        << "{4'h0, 4'h4, 4'h8, 4'hF}";

    QTest::newRow("Array on multiple rows with trailing comments")
        << "module arrayTest();\n"
        "parameter [15:0] bits = {4'h0, // Comment #1.\n"
        "4'h4,\n"
        "4'h8, // Comment #2\n"
        "4'hF}; // Array of bit vectors.\n"        
        << "{4'h0, 4'h4, 4'h8, 4'hF}";

    QTest::newRow("Empty array")
        << "module arrayTest();\n"
        "parameter [15:0] bits = {}; // Array of bit vectors.\n"
        "endmodule"
        << "{}";
}

void tst_VerilogParameterParser::parametersToComponent()
{
    QString input = "module shifter #(\n"
        "parameter ok joku =8,//seli\n"
        "parameter int juu =98, //evo\n"
        "parameter esa= 6, // epeli\n"
        " hopo = 22 // o olololo ollllooo \n"
        ") ();\n"
        "endmodule";

    VLNV swvlvnv(VLNV::COMPONENT,"kyytto","raision kirjasto","swName","1.0");
	QSharedPointer<Component> sw = QSharedPointer<Component>(new Component(swvlvnv, Document::Revision::Std14));

	QSharedPointer<ComponentInstantiation> instance( new ComponentInstantiation );
	instance->setName("module_parameter_instantiation");
	sw->getComponentInstantiations()->append(instance);

    VerilogParameterParser parser;
    parser.import(input, sw, instance);

    QCOMPARE(instance->getModuleParameters()->size(), 4);
    QCOMPARE(sw->getParameters()->size(), 4);
    verifyParameter( sw->getParameters()->at(0), "joku", "8", "seli");
    verifyParameter( sw->getParameters()->at(1), "juu", "98", "evo");
    verifyParameter( sw->getParameters()->at(2), "esa", "6", "o olololo ollllooo");
    verifyParameter( sw->getParameters()->at(3), "hopo", "22", "o olololo ollllooo");

    QCOMPARE(instance->getModuleParameters()->at(0)->getValue(),  sw->getParameters()->at(0)->getValueId());
    QCOMPARE(instance->getModuleParameters()->at(1)->getValue(),  sw->getParameters()->at(1)->getValueId());
    QCOMPARE(instance->getModuleParameters()->at(2)->getValue(),  sw->getParameters()->at(2)->getValueId());
    QCOMPARE(instance->getModuleParameters()->at(3)->getValue(),  sw->getParameters()->at(3)->getValueId());

}

void tst_VerilogParameterParser::oldParametersToComponent()
{
    QString input = "module shifter ()\n"
        "parameter ok joku =8;//seli\n"
        "parameter int juu =98; //evo\n"
        "parameter esa= 6, // epeli\n"
        " hopo = 22; // o olololo ollllooo \n";

    VLNV swvlvnv(VLNV::COMPONENT,"kyytto","raision kirjasto","swName","1.0");
    QSharedPointer<Component> sw = QSharedPointer<Component>(new Component(swvlvnv, Document::Revision::Std14));

	QSharedPointer<ComponentInstantiation> instance( new ComponentInstantiation );
	instance->setName("module_parameter_instantiation");
	sw->getComponentInstantiations()->append(instance);

    VerilogParameterParser parser;
    parser.import(input, sw, instance);

    QCOMPARE( sw->getParameters()->size(), 4 );
    QCOMPARE( instance->getModuleParameters()->size(), 4 );
    verifyParameter( sw->getParameters()->at(0), "joku", "8", "seli");
    verifyParameter( sw->getParameters()->at(1), "juu", "98", "evo");
    verifyParameter( sw->getParameters()->at(2), "esa", "6", "o olololo ollllooo");
    verifyParameter( sw->getParameters()->at(3), "hopo", "22", "o olololo ollllooo");

    QCOMPARE(instance->getModuleParameters()->at(0)->getValue(),  sw->getParameters()->at(0)->getValueId());
    QCOMPARE(instance->getModuleParameters()->at(1)->getValue(),  sw->getParameters()->at(1)->getValueId());
    QCOMPARE(instance->getModuleParameters()->at(2)->getValue(),  sw->getParameters()->at(2)->getValueId());
    QCOMPARE(instance->getModuleParameters()->at(3)->getValue(),  sw->getParameters()->at(3)->getValueId());
}

void tst_VerilogParameterParser::operations()
{
    QString input = "module shifter #(\n"
        "parameter ok joku = 8 + 9,//seli\n"
        "parameter esa= 6-5, // epeli\n"
        "parameter int juu = 98 + 7 / ( foo*bar ), //ev(o)\n"
        "parameter hopo = (22)-(((7*6))) // o olololo ollllooo \n"
        ") ();\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE( declarations.size(), 4 );

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8 + 9", "seli");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6-5", "epeli");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juu", "98 + 7 / ( foo*bar )", "ev(o)");
    parameters.append(parser.parseParameters(declarations[3]));
    verifyParameter( parameters[3], "hopo", "(22)-(((7*6)))", "o olololo ollllooo");
}

void tst_VerilogParameterParser::oldOperations()
{
    QString input = "module shifter ();\n"
        "parameter ok joku = 8 + 9;//seli\n"
        "parameter esa= 6-5 ;// epeli\n"
        "parameter int juu = 98 + 7 / ( foo*bar ); //evo\n"
        "parameter hopo = (22)-(((7*6))); // o olololo ollllooo \n"
        "endmodule\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8 + 9", "seli");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6-5", "epeli");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juu", "98 + 7 / ( foo*bar )", "evo");
    parameters.append(parser.parseParameters(declarations[3]));
    verifyParameter( parameters[3], "hopo", "(22)-(((7*6)))", "o olololo ollllooo");
}

void tst_VerilogParameterParser::bases()
{
    QString input = "module shifter #(\n"
        "parameter ok joku = 8'0009,//seli\n"
        "parameter esa= 6'000-5'h6666, // epeli\n"
        "parameter int juu = '98 / ( foo*bar ) ,//evo\n"
        "parameter hopo = '23 -'98, // o olololo ollllooo \n"
        ") ();\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8'0009", "seli");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6'000-5'h6666", "epeli");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juu", "'98 / ( foo*bar )", "evo");
    parameters.append(parser.parseParameters(declarations[3]));
    verifyParameter( parameters[3], "hopo", "'23 -'98", "o olololo ollllooo");
}

void tst_VerilogParameterParser::braces()
{
    QString input = "module shifter #(\n"
        "parameter ok joku = {8'0009},//seli\n"
        "parameter esa= {6'000-5'h6666}, // epeli\n"
        "parameter int juu = {'98} / { foo*bar } //evo\n"
        ") ();\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "{8'0009}", "seli");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "{6'000-5'h6666}", "epeli");
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juu", "{'98} / { foo*bar }", "evo");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogParameterParser::twoDimensional()
//-----------------------------------------------------------------------------
void tst_VerilogParameterParser::twoDimensional()
{
    QString input = "module shifter #(\n"
        "parameter bit          [7:0] shift_type      = '{3'b001}, //joku\n"
        "parameter bit [count:0][1:0] mask            = '{2'b00},\n"
        ") ();\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);

    verifyParameter( parameters[0], "shift_type", "'{3'b001}", "joku");
    QCOMPARE(parameters[0]->getVectorLeft(), QString("7"));
    QCOMPARE(parameters[0]->getVectorRight(), QString("0"));
    QCOMPARE(parameters[0]->getArrayLeft(),QString(""));
    QCOMPARE(parameters[0]->getArrayRight(),QString(""));

    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "mask", "'{2'b00}", "");
    QCOMPARE(parameters[1]->getArrayLeft(),QString("count"));
    QCOMPARE(parameters[1]->getArrayRight(),QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogParameterParser::closerOnLine()
//-----------------------------------------------------------------------------
void tst_VerilogParameterParser::closerOnLine()
{
    QString input = "module shifter #(\n"
        "parameter bit [count:1][2:0] shift_type      = '{3'b001}, //joku\n"
        "parameter bit [count:1][1:0] mask             = '{2'b00} ) //hopo\n"
        "();\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);
    QCOMPARE( declarations.size(), 2 );

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "shift_type", "'{3'b001}", "joku");
    QCOMPARE(parameters[0]->getVectorLeft(), QString("2"));
    QCOMPARE(parameters[0]->getVectorRight(), QString("0"));
    QCOMPARE(parameters[0]->getArrayLeft(), QString("count"));
    QCOMPARE(parameters[0]->getArrayRight(), QString("1"));

    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "mask", "'{2'b00}", "");
    QCOMPARE(parameters[1]->getVectorLeft(), QString("1"));
    QCOMPARE(parameters[1]->getVectorRight(), QString("0"));
    QCOMPARE(parameters[1]->getArrayLeft(),QString("count"));
    QCOMPARE(parameters[1]->getArrayRight(),QString("1"));

}

void tst_VerilogParameterParser::closerOnLineDescribedparentheses()
{
    QString input = "module shifter #(\n"
        "parameter esa= 6 ,// epeli\n"
        "parameter hopo = 22) // o (olololo ollllooo) \n"
        "();\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "esa", "6", "epeli");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "hopo", "22", "");
}

void tst_VerilogParameterParser::closerOnLineDescribedparentheses2()
{
    QString input = "module shifter #(\n"
        "parameter esa= 6 ,// epeli\n"
        "parameter hopo = 22) // o (olololo ollllooo) \n"
        "();\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "esa", "6", "epeli");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "hopo", "22", "");
}

void tst_VerilogParameterParser::parenthesesInPorts()
{
    QString input = "module shifter #(\n"
        "parameter esa= 6, // epeli\n"
        "parameter hopo = 22) // o (olololo ollllooo) \n"
        "(\n"
        "//joo (hkhk jkhk)\n"
        ");\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "esa", "6", "epeli");
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "hopo", "22", "");
}

void tst_VerilogParameterParser::customCase()
{
    QString input = "module verilog_bug1  # (\n"
        "PARAMETER_A    = 2,    // Number of external semaphore register in the design\n"
        "PARAMETER_B    = 0,    // Enables the KEYRAMEN (DSCON[11]) bit in the register map\n"
        "parameter PARAMETER_C    = 32)   // System data bus width (only valid values are 8, 16, and 32)\n"
        "(\n"
        "\n"
        "input        input1,                // * VDD3 Power-on-reset event\n"
        "\n"
        "output logic        input2            // * Enable the external source of the wakeup_src_mv input.\n"
        "`ifdef USE_INPUT3\n"
        ", output logic [31:0] input3            // * Signals added for testchip debug\n"
        "`endif\n"
        ");\n"
        "\n"
    "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findDeclarations(input);

    QList<QSharedPointer<ModuleParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "PARAMETER_C", "32", "");
}

QTEST_APPLESS_MAIN(tst_VerilogParameterParser)

#include "tst_VerilogParameterParser.moc"
