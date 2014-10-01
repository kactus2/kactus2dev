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

    void oldParameter();
    void multipleOldParameters();
    void variantOldParameters();
    void multiOldParameters();
    void doubleMultiOldParameters();

    void parametersToComponent();
    void oldParametersToComponent();

private:
    void verifyParameter( QSharedPointer<ModelParameter> parameter,
        QString name, QString value, QString type, QString description );
};

tst_VerilogParameterParser::tst_VerilogParameterParser()
{
}

void tst_VerilogParameterParser::verifyParameter( QSharedPointer<ModelParameter> parameter,
    QString name, QString value, QString type, QString description )
{
    QCOMPARE(parameter->getName(), name);
    QCOMPARE(parameter->getValue(), value);
    QCOMPARE(parameter->getDataType(), type);
    QCOMPARE(parameter->getDescription(), description);
}

void tst_VerilogParameterParser::singleLineHeader()
{
    QString input = "module shifter #(ok joku=12) (\n"
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
    QStringList declarations; parser.findANSIDeclarations(input, declarations);
    QCOMPARE( declarations[0], QString( "ok joku=12") );
}

void tst_VerilogParameterParser::multiLineHeader()
{
    QString input = "module shifter #(\n"
        "ok joku=8,\n"
        "parameter esa=6,\n"
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
    QStringList declarations; parser.findANSIDeclarations(input, declarations);
    QCOMPARE(declarations[0], QString("ok joku=8,") );
    QCOMPARE(declarations[1], QString("parameter esa=6,"));
    QCOMPARE(declarations[2], QString("parameter joku=22,"));
    QCOMPARE(declarations[3], QString("jaa joku=1234"));
}

void tst_VerilogParameterParser::multiLineComments()
{
    QString input = "module shifter #(\n"
        "ok joku=8,\n"
        "/*uliuli uli*/\n"
        "parameter esa=6,\n"
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
    QStringList declarations; parser.findANSIDeclarations(input, declarations);
    QCOMPARE(declarations[0], QString("ok joku=8,") );
    QCOMPARE(declarations[1], QString("parameter esa=6,"));
    QCOMPARE(declarations[2], QString("parameter joku=22,"));
    QCOMPARE(declarations[3], QString("jaa joku=1234"));
}

void tst_VerilogParameterParser::commentLines()
{
    QString input = "module shifter #(\n"
        "ok joku=8 //herpaderp\n"
        "//uliuli uli\n"
        "parameter esa=6\n"
        "parameter joku=22\n"
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
    QStringList declarations; parser.findANSIDeclarations(input, declarations);
    QCOMPARE(declarations[0], QString("ok joku=8 //herpaderp") );
    QCOMPARE(declarations[1], QString("parameter esa=6"));
    QCOMPARE(declarations[2], QString("parameter joku=22"));
    QCOMPARE(declarations[3], QString("jaa joku=1234"));
}

void tst_VerilogParameterParser::multiDeclaration()
{
    QString input = "module shifter #(ok joku=12, esa = 3, jari = 123) (\n";

    VerilogParameterParser parser;
    QStringList declarations; parser.findANSIDeclarations(input, declarations);
    QCOMPARE( declarations[0], QString( "ok joku=12, esa = 3, jari = 123") );
}

void tst_VerilogParameterParser::multiLineDeclarations()
{
    QString input = "module shifter #(parameter ok joku=12,\n"
        "esa = 3,\n"
        "jari = 123\n"
        "parameter singed def[1:4] jees=1234, horspo = 567 ) (\n";

    VerilogParameterParser parser;
    QStringList declarations; parser.findANSIDeclarations(input, declarations);
    QCOMPARE( declarations[0], QString( "parameter ok joku=12,\nesa = 3,\njari = 123") );
    QCOMPARE( declarations[1], QString( "def[1:4] jees=1234, horspo = 567") );
}

void tst_VerilogParameterParser::oneParameter()
{
    QString input = "module shifter #(\n"
        "ok joku=8\n"
        ") (\n";

    VerilogParameterParser parser;
    QString description;
    QString type;
    QStringList declarations; parser.findANSIDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "ok", "" );
}

void tst_VerilogParameterParser::threeParameter()
{
    QString input = "module shifter #(\n"
        "parameter ok joku=8;\n"
        "esa=6;\n"
        "parameter joku=22\n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations; parser.findANSIDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "ok", "" );
    parser.parseParameters(declarations[1], parameters);
    verifyParameter( parameters[1], "esa", "6", "", "" );
    parser.parseParameters(declarations[2], parameters);
    verifyParameter( parameters[2], "joku", "22", "", "" );
}

void tst_VerilogParameterParser::spacedParameters()
{
    QString input = "module shifter #(\n"
        "ok joku =8;\n"
        "parameter esa= 6;\n"
        "hopo = 22\n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations; parser.findANSIDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "ok", "" );
    parser.parseParameters(declarations[1], parameters);
    verifyParameter( parameters[1], "esa", "6", "", "" );
    parser.parseParameters(declarations[2], parameters);
    verifyParameter( parameters[2], "hopo", "22", "", "" );
}

void tst_VerilogParameterParser::doubleMultiParameters()
{
    QString input = "module shifter (\n"
        "parameter joku=8, \n"
        "//vali spiikki \n"
        "olololoo= 1234, //hopotus\n"
        "juuuuu = 22;//hapatus\n"
        "parameter signed def[6:5] seppox = 5,\n"
        "jari = 1234,\n"
        "jorma = 7\n"
        ");";

    VerilogParameterParser parser;
    QStringList declarations; parser.findOldDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "", "hapatus" );
    verifyParameter( parameters[1], "olololoo", "1234", "", "hapatus" );
    verifyParameter( parameters[2], "juuuuu", "22", "", "hapatus" );

    parser.parseParameters(declarations[1], parameters);
    verifyParameter( parameters[3], "seppox", "5", "", "" );
    verifyParameter( parameters[4], "jari", "1234", "", "" );
    verifyParameter( parameters[5], "jorma", "7", "", "" );
}

void tst_VerilogParameterParser::describedParameters()
{
    QString input = "module shifter #(\n"
        "ok joku =8//seli\n"
        "int juu =98 //evo\n"
        "parameter esa= 6 // epeli\n"
        "hopo = 22 // o olololo ollllooo \n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations; parser.findANSIDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "ok", "seli" );
    parser.parseParameters(declarations[1], parameters);
    verifyParameter( parameters[1], "juu", "98", "int", "evo" );
    parser.parseParameters(declarations[2], parameters);
    verifyParameter( parameters[2], "esa", "6", "", "epeli" );
    parser.parseParameters(declarations[3], parameters);
    verifyParameter( parameters[3], "hopo", "22", "", "o olololo ollllooo" );
}

void tst_VerilogParameterParser::oneUnDescribed()
{
    QString input = "module shifter #(\n"
        "ok joku =8//joku\n"
        "parameter int juu =98\n"
        "parameter esa= 6 // epeli\n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations; parser.findANSIDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "ok", "joku" );
    parser.parseParameters(declarations[1], parameters);
    verifyParameter( parameters[1], "juu", "98", "int", "" );
    parser.parseParameters(declarations[2], parameters);
    verifyParameter( parameters[2], "esa", "6", "", "epeli" );
}

void tst_VerilogParameterParser::multiLineDescribed()
{
    QString input = "module shifter #(\n"
        "char joku =8, juu =98,\n"
        "esa= 6 // epeli\n"
        ") (\n";

    VerilogParameterParser parser;
    QStringList declarations; parser.findANSIDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "char", "epeli" );
    verifyParameter( parameters[1], "juu", "98", "char", "epeli" );
    verifyParameter( parameters[2], "esa", "6", "char", "epeli" );
}

void tst_VerilogParameterParser::oldParameter()
{
    QString input = "module shifter #(\n"
    "ok fufufuu=8//jees\n"
    ") (\n"
    "parameter evo joku=9 // ulina\n";

    VerilogParameterParser parser;
    QStringList declarations; parser.findOldDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
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
    QStringList declarations; parser.findOldDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "", "" );
    parser.parseParameters(declarations[1], parameters);
    verifyParameter( parameters[1], "olololoo", "1234", "jou", "hopotus" );
    parser.parseParameters(declarations[2], parameters);
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
    QStringList declarations; parser.findOldDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "", "" );
    parser.parseParameters(declarations[1], parameters);
    verifyParameter( parameters[1], "olololoo", "1234", "", "hopotus" );
    parser.parseParameters(declarations[2], parameters);
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
    QStringList declarations; parser.findOldDeclarations(input, declarations);
    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);

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
    QStringList declarations; parser.findOldDeclarations(input, declarations);

    QList<QSharedPointer<ModelParameter>> parameters; parser.parseParameters(declarations[0], parameters);
    verifyParameter( parameters[0], "joku", "8", "", "hapatus" );
    verifyParameter( parameters[1], "olololoo", "1234", "", "hapatus" );
    verifyParameter( parameters[2], "juuuuu", "22", "", "hapatus" );
    
    parser.parseParameters(declarations[1], parameters);
    verifyParameter( parameters[3], "seppox", "5", "", "" );
    verifyParameter( parameters[4], "jari", "1234", "", "" );
    verifyParameter( parameters[5], "jorma", "7", "", "" );
}

void tst_VerilogParameterParser::parametersToComponent()
{
    QString input = "module shifter #(\n"
        "ok joku =8//seli\n"
        "int juu =98 //evo\n"
        "parameter esa= 6, // epeli\n"
        "hopo = 22 // o olololo ollllooo \n"
        ") (\n";

    VLNV swvlvnv("","kyytto","raision kirjasto","swName","1.0");
    QSharedPointer<Component> sw = QSharedPointer<Component>(new Component(swvlvnv));

    VerilogParameterParser parser;
    parser.runParser(input, sw);

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
        "hopo = 22; // o olololo ollllooo \n";

    VLNV swvlvnv("","kyytto","raision kirjasto","swName","1.0");
    QSharedPointer<Component> sw = QSharedPointer<Component>(new Component(swvlvnv));

    VerilogParameterParser parser;
    parser.runParser(input, sw);

    verifyParameter( sw->getModel()->getModelParameter("joku"), "joku", "8", "ok", "seli" );
    verifyParameter( sw->getModel()->getModelParameter("juu"), "juu", "98", "int", "evo" );
    verifyParameter( sw->getModel()->getModelParameter("esa"), "esa", "6", "", "o olololo ollllooo" );
    verifyParameter( sw->getModel()->getModelParameter("hopo"), "hopo", "22", "", "o olololo ollllooo" );
}

QTEST_APPLESS_MAIN(tst_VerilogParameterParser)

#include "tst_VerilogParameterParser.moc"
