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

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>

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
    void commentLines();
    void multiDeclaration();
    void multiLineDeclarations();

    void oneParameter();
    void threeParameter();
    void spacedParameters();
    void doubleMultiParameters();

    void describedParameters();
    void oneUnDescribed();
    void multiLineDescribed();
    void describedparentheses();
    
    void otherParameterAsParameterValue();

    void oldParameter();
    void multipleOldParameters();
    void variantOldParameters();
    void multiOldParameters();
    void oldWithEndModule();
    void doubleMultiOldParameters();

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
    void verifyParameter( QSharedPointer<ModelParameter> parameter,
        QString name, QString value, QString type, QString description );
   
};

void tst_VerilogParameterParser::verifyParameter( QSharedPointer<ModelParameter> parameter,
    QString name, QString value, QString type, QString description )
{
    QCOMPARE(parameter->getName(), name);
    QCOMPARE(parameter->getValue(), value);
    QCOMPARE(parameter->getDataType(), type);
    QCOMPARE(parameter->getDescription(), description);
}

tst_VerilogParameterParser::tst_VerilogParameterParser()
{
}

void tst_VerilogParameterParser::noParameters()
{
    QString input = "module shifter();\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);
    QCOMPARE( declarations.size(), 0 );
}

void tst_VerilogParameterParser::noPorts()
{
    QString input = "module shifter #(parameter ok joku=12);\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);
    QCOMPARE( declarations[0], QString( "parameter ok joku=12") );
}

void tst_VerilogParameterParser::singleLineHeader()
{
    QString input = "module shifter #(parameter ok joku=12) (\n"
        "input reset_n, //reset async active low\n"
        "input clk, //input clock\n"
        "input data_ena, //serial data enable\n"
        "input serial_data, //serial data input\n"
        "output reg [width-1:0] parallel_data //parallel data out );\n"
        "always @ (posedge clk, negedge reset_n)\n"
        "if(!reset_n) parallel_data <= ’0; //could not do \"width’d0\"\n"
        "else if (data_ena)\n"
        "parallel_data <= {serial_data, parallel_data[width-1:1]};\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);
    QCOMPARE( declarations[0], QString( "parameter ok joku=12") );
}

void tst_VerilogParameterParser::multiLineHeader()
{
    QString input = "module shifter #(\n"
        "parameter ok joku=8,\n"
        "esa=6,\n"
        "parameter joku=22,\n"
        "jaa joku=1234\n"
        ") (\n"
        "input reset_n, //reset async active low\n"
        "input clk, //input clock\n"
        "input data_ena, //serial data enable\n"
        "input serial_data, //serial data input\n"
        "output reg [width-1:0] parallel_data //parallel data out );\n"
        "always @ (posedge clk, negedge reset_n)\n"
        "if(!reset_n) parallel_data <= ’0; //could not do \"width’d0\"\n"
        "else if (data_ena)\n"
        "parallel_data <= {serial_data, parallel_data[width-1:1]};\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);
    QCOMPARE(declarations[0], QString("parameter ok joku=8,\nesa=6,") );
    QCOMPARE(declarations[1], QString("parameter joku=22,\njaa joku=1234"));
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
        "output reg [width-1:0] parallel_data //parallel data out );\n"
        "always @ (posedge clk, negedge reset_n)\n"
        "if(!reset_n) parallel_data <= ’0; //could not do \"width’d0\"\n"
        "else if (data_ena)\n"
        "parallel_data <= {serial_data, parallel_data[width-1:1]};\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);
    QCOMPARE(declarations[0], QString("parameter ok joku=8,\n\nesa=6,") );
    QCOMPARE(declarations[1], QString("parameter joku=22,\n\njaa joku=1234"));
}

void tst_VerilogParameterParser::commentLines()
{
    QString input = "module shifter #(\n"
        "parameter ok joku=8, //herpaderp\n"
        "//uliuli uli\n"
        "esa=6\n"
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
        "output reg [width-1:0] parallel_data //parallel data out );\n"
        "always @ (posedge clk, negedge reset_n)\n"
        "if(!reset_n) parallel_data <= ’0; //could not do \"width’d0\"\n"
        "else if (data_ena)\n"
        "parallel_data <= {serial_data, parallel_data[width-1:1]};\n"
        "endmodule";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);
    QCOMPARE(declarations[0], QString("parameter ok joku=8, //herpaderp\nesa=6") );
    QCOMPARE(declarations[1], QString("parameter joku=22,\njaa joku=1234"));
}

void tst_VerilogParameterParser::multiDeclaration()
{
    QString input = "module shifter #(parameter ok joku=12, esa = 3, jari = 123);\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);
    QCOMPARE( declarations[0], QString( "parameter ok joku=12, esa = 3, jari = 123") );
}

void tst_VerilogParameterParser::multiLineDeclarations()
{
    QString input = "module shifter #(parameter ok joku=12,\n"
        "esa = 3,\n"
        "jari = 123\n"
        "parameter singed def[1:4] jees=1234, horspo = 567);\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);
    QCOMPARE( declarations[0], QString( "parameter ok joku=12,\nesa = 3,\njari = 123") );
    QCOMPARE( declarations[1], QString( "parameter singed def[1:4] jees=1234, horspo = 567") );
}

void tst_VerilogParameterParser::oneParameter()
{
    QString input = "module shifter #(\n"
        "parameter ok joku=8\n"
        ") ();\n";

    VerilogParameterParser parser;
    QString description;
    QString type;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "ok", "" );
}

void tst_VerilogParameterParser::threeParameter()
{
    QString input = "module shifter #(\n"
        "parameter ok joku=8,\n"
        "parameter int esa=6\n"
        "parameter joku=22\n"
        ") ();\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "ok", "" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6", "int", "" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "joku", "22", "", "" );
}

void tst_VerilogParameterParser::spacedParameters()
{
    QString input = "module shifter #(\n"
        "parameter ok joku =8,\n"
        "parameter esa= 6\n"
        "parameter hopo = 22\n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "ok", "" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6", "", "" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "hopo", "22", "", "" );
}

void tst_VerilogParameterParser::doubleMultiParameters()
{
    QString input = "module shifter (\n"
        "parameter joku=8, \n"
        "//vali spiikki \n"
        "olololoo= 1234, //hopotus\n"
        "juuuuu = 22 //hapatus\n"
        "parameter signed def[6:5] seppox = 5,\n"
        "jari = 1234,\n"
        "jorma = 7\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findOldDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "", "hapatus" );
    verifyParameter( parameters[1], "olololoo", "1234", "", "hapatus" );
    verifyParameter( parameters[2], "juuuuu", "22", "", "hapatus" );

    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[3], "seppox", "5", "", "" );
    verifyParameter( parameters[4], "jari", "1234", "", "" );
    verifyParameter( parameters[5], "jorma", "7", "", "" );
}

void tst_VerilogParameterParser::describedParameters()
{
    QString input = "module shifter #(\n"
        "parameter ok joku =8//seli\n"
        "parameter int juu =98 //evo\n"
        "parameter esa= 6 // epeli\n"
        "parameter hopo = 22 // o olololo ollllooo \n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "ok", "seli" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "juu", "98", "int", "evo" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "esa", "6", "", "epeli" );
    parameters.append(parser.parseParameters(declarations[3]));
    verifyParameter( parameters[3], "hopo", "22", "", "o olololo ollllooo" );
}

void tst_VerilogParameterParser::oneUnDescribed()
{
    QString input = "module shifter #(\n"
        "parameter ok joku =8//joku\n"
        "parameter int juu =98\n"
        "parameter esa= 6 // epeli\n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "ok", "joku" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "juu", "98", "int", "" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "esa", "6", "", "epeli" );
}

void tst_VerilogParameterParser::multiLineDescribed()
{
    QString input = "module shifter #(\n"
        "parameter char joku =8, juu =98,\n"
        "esa= 6 // epeli\n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "char", "epeli" );
    verifyParameter( parameters[1], "juu", "98", "char", "epeli" );
    verifyParameter( parameters[2], "esa", "6", "char", "epeli" );
}

void tst_VerilogParameterParser::describedparentheses()
{
    QString input = "module shifter #(\n"
        "parameter esa= 6 // epeli\n"
        "parameter hopo = 22 // o (olololo ollllooo) \n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "esa", "6", "", "epeli" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "hopo", "22", "", "o (olololo ollllooo)" );
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogParameterParser::otherParameterAsParameterValue()
//-----------------------------------------------------------------------------
void tst_VerilogParameterParser::otherParameterAsParameterValue()
{
    QSharedPointer<Component> targetComponent(new Component());

    QString input = "module test #(\n"
        "parameter first = 1\n"
        "parameter second = first\n"
        ") ();\n"
        "endmodule";

    VerilogParameterParser parser;
    parser.import(input, targetComponent);

    QSharedPointer<ModelParameter> first = targetComponent->getModelParameters().first();
    QSharedPointer<ModelParameter> second = targetComponent->getModelParameters().last();
    QCOMPARE(second->getName(), QString("second"));

    QRegularExpression uuid("{[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}}");
    QVERIFY(uuid.match(second->getValue()).hasMatch());
    verifyParameter(second, "second", first->getValueId(), "", "");
}

void tst_VerilogParameterParser::oldParameter()
{
    QString input = "module shifter #(\n"
    "ok fufufuu=8//jees\n"
    ");\n"
    "parameter evo joku=9 // ulina\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findOldDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "9", "evo", "ulina" );
}

void tst_VerilogParameterParser::multipleOldParameters()
{
    QString input = "module shifter (\n"
        "ok tttt=8\n"
        ");\n"
        "parameter joku=8; \n"
        "parameter jou olololoo= 1234; //hopotus\n"
        "parameter juuuuu = 22//hapatus";

    VerilogParameterParser parser;
    QStringList declarations = parser.findOldDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "", "" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "olololoo", "1234", "jou", "hopotus" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juuuuu", "22", "", "hapatus" );
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
    QStringList declarations = parser.findOldDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "", "" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "olololoo", "1234", "", "hopotus" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juuuuu", "22", "horspo", "hapatus" );
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
    QStringList declarations = parser.findOldDeclarations(input);
    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);

    verifyParameter( parameters[0], "joku", "8", "", "hapatus" );
    verifyParameter( parameters[1], "olololoo", "1234", "", "hapatus" );
    verifyParameter( parameters[2], "juuuuu", "22", "", "hapatus" );
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
    QStringList declarations = parser.findOldDeclarations(input);
    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);

    verifyParameter( parameters[0], "joku", "8", "", "hapatus" );
    verifyParameter( parameters[1], "olololoo", "1234", "", "hapatus" );
    verifyParameter( parameters[2], "juuuuu", "22", "", "hapatus" );
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
    QStringList declarations = parser.findOldDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8", "", "hapatus" );
    verifyParameter( parameters[1], "olololoo", "1234", "", "hapatus" );
    verifyParameter( parameters[2], "juuuuu", "22", "", "hapatus" );
    
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[3], "seppox", "5", "", "" );
    verifyParameter( parameters[4], "jari", "1234", "", "" );
    verifyParameter( parameters[5], "jorma", "7", "", "" );
}

void tst_VerilogParameterParser::parametersToComponent()
{
    QString input = "module shifter #(\n"
        "parameter ok joku =8//seli\n"
        "parameter int juu =98 //evo\n"
        "parameter esa= 6, // epeli\n"
        " hopo = 22 // o olololo ollllooo \n"
        ") (\n";

    VLNV swvlvnv("","kyytto","raision kirjasto","swName","1.0");
    QSharedPointer<Component> sw = QSharedPointer<Component>(new Component(swvlvnv));

    VerilogParameterParser parser;
    parser.import(input, sw);

    verifyParameter( sw->getModel()->getModelParameter("joku"), "joku", "8", "ok", "seli" );
    verifyParameter( sw->getModel()->getModelParameter("juu"), "juu", "98", "int", "evo" );
    verifyParameter( sw->getModel()->getModelParameter("esa"), "esa", "6", "", "o olololo ollllooo" );
    verifyParameter( sw->getModel()->getModelParameter("hopo"), "hopo", "22", "", "o olololo ollllooo" );
}

void tst_VerilogParameterParser::oldParametersToComponent()
{
    QString input = "module shifter ()\n"
        "parameter ok joku =8;//seli\n"
        "parameter int juu =98; //evo\n"
        "parameter esa= 6, // epeli\n"
        " hopo = 22; // o olololo ollllooo \n";

    VLNV swvlvnv("","kyytto","raision kirjasto","swName","1.0");
    QSharedPointer<Component> sw = QSharedPointer<Component>(new Component(swvlvnv));

    VerilogParameterParser parser;
    parser.import(input, sw);

    verifyParameter( sw->getModel()->getModelParameter("joku"), "joku", "8", "ok", "seli" );
    verifyParameter( sw->getModel()->getModelParameter("juu"), "juu", "98", "int", "evo" );
    verifyParameter( sw->getModel()->getModelParameter("esa"), "esa", "6", "", "o olololo ollllooo" );
    verifyParameter( sw->getModel()->getModelParameter("hopo"), "hopo", "22", "", "o olololo ollllooo" );
}

void tst_VerilogParameterParser::operations()
{
    QString input = "module shifter #(\n"
        "parameter ok joku = 8 + 9//seli\n"
        "parameter esa= 6-5 // epeli\n"
        "parameter int juu = 98 + 7 / ( foo*bar ) //ev(o)\n"
        "parameter hopo = (22)-(((7*6))) // o olololo ollllooo \n"
        ") ();\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8 + 9", "ok", "seli" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6-5", "", "epeli" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juu", "98 + 7 / ( foo*bar )", "int", "ev(o)" );
    parameters.append(parser.parseParameters(declarations[3]));
    verifyParameter( parameters[3], "hopo", "(22)-(((7*6)))", "", "o olololo ollllooo" );
}

void tst_VerilogParameterParser::oldOperations()
{
    QString input = "module shifter ();\n"
        "parameter ok joku = 8 + 9//seli\n"
        "parameter esa= 6-5 // epeli\n"
        "parameter int juu = 98 + 7 / ( foo*bar ) //evo\n"
        "parameter hopo = (22)-(((7*6))) // o olololo ollllooo \n"
        "endmodule\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findOldDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8 + 9", "ok", "seli" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6-5", "", "epeli" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juu", "98 + 7 / ( foo*bar )", "int", "evo" );
    parameters.append(parser.parseParameters(declarations[3]));
    verifyParameter( parameters[3], "hopo", "(22)-(((7*6)))", "", "o olololo ollllooo" );
}

void tst_VerilogParameterParser::bases()
{
    QString input = "module shifter #(\n"
        "parameter ok joku = 8'0009//seli\n"
        "parameter esa= 6'000-5'h6666 // epeli\n"
        "parameter int juu = '98 / ( foo*bar ) //evo\n"
        "parameter hopo = '23 -'98 // o olololo ollllooo \n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "8'0009", "ok", "seli" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "6'000-5'h6666", "", "epeli" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juu", "'98 / ( foo*bar )", "int", "evo" );
    parameters.append(parser.parseParameters(declarations[3]));
    verifyParameter( parameters[3], "hopo", "'23 -'98", "", "o olololo ollllooo" );
}

void tst_VerilogParameterParser::braces()
{
    QString input = "module shifter #(\n"
        "parameter ok joku = {8'0009}//seli\n"
        "parameter esa= {6'000-5'h6666} // epeli\n"
        "parameter int juu = {'98} / { foo*bar } //evo\n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "joku", "{8'0009}", "ok", "seli" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "esa", "{6'000-5'h6666}", "", "epeli" );
    parameters.append(parser.parseParameters(declarations[2]));
    verifyParameter( parameters[2], "juu", "{'98} / { foo*bar }", "int", "evo" );
}

void tst_VerilogParameterParser::twoDimensional()
{
    QString input = "module shifter #(\n"
        "parameter bit [count:1][2:0] shift_type      = '{3'b001}, //joku\n"
        "parameter bit [count:1][1:0] mask             = '{2'b00},\n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "shift_type", "'{3'b001}", "bit", "joku" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "mask", "'{2'b00}", "bit", "" );
}

void tst_VerilogParameterParser::closerOnLine()
{
    QString input = "module shifter #(\n"
        "parameter bit [count:1][2:0] shift_type      = '{3'b001}, //joku\n"
        "parameter bit [count:1][1:0] mask             = '{2'b00} ) //hopo"
        "\n (";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "shift_type", "'{3'b001}", "bit", "joku" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "mask", "'{2'b00}", "bit", "" );
}

void tst_VerilogParameterParser::closerOnLineDescribedparentheses()
{
    QString input = "module shifter #(\n"
        "parameter esa= 6 // epeli\n"
        "parameter hopo = 22) // o (olololo ollllooo) \n"
        "(\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "esa", "6", "", "epeli" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "hopo", "22", "", "" );
}

void tst_VerilogParameterParser::closerOnLineDescribedparentheses2()
{
    QString input = "module shifter #(\n"
        "parameter esa= 6 // epeli\n"
        "parameter hopo = 22) // o (olololo ollllooo) \n"
        "();\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "esa", "6", "", "epeli" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "hopo", "22", "", "" );
}

void tst_VerilogParameterParser::parenthesesInPorts()
{
    QString input = "module shifter #(\n"
        "parameter esa= 6 // epeli\n"
        "parameter hopo = 22) // o (olololo ollllooo) \n"
        "(\n"
        "//joo (hkhk jkhk)\n"
        ");\n";

    VerilogParameterParser parser;
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "esa", "6", "", "epeli" );
    parameters.append(parser.parseParameters(declarations[1]));
    verifyParameter( parameters[1], "hopo", "22", "", "" );
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
    QStringList declarations = parser.findANSIDeclarations(input);

    QList<QSharedPointer<ModelParameter> > parameters = parser.parseParameters(declarations[0]);
    verifyParameter( parameters[0], "PARAMETER_C", "32", "", "" );
}

QTEST_APPLESS_MAIN(tst_VerilogParameterParser)

#include "tst_VerilogParameterParser.moc"
