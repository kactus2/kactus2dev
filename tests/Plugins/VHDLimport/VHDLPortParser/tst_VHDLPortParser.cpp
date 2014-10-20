//-----------------------------------------------------------------------------
// File: tst_VHDLPortParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 22.09.2014
//
// Description:
// Unit test for class VHDLPortParser.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QObject>
#include <QSharedPointer>

#include <IPXACTmodels/component.h>

#include <Plugins/VHDLimport/VHDLPortParser.h>

Q_DECLARE_METATYPE(General::Direction)

class tst_VHDLPortParser : public QObject
{
    Q_OBJECT

public:
    tst_VHDLPortParser();

private slots:
    void init();

    //! Test cases:
    void testPortIsParsed();
    void testPortIsParsed_data();
    
    void testMultiplePortsAreParsed();
    void testMultiplePortsAreParsed_data();

    void testCommentedPortIsNotParsed();

    void testPortsAreNotParsedOutsideEntity();

private:

    void runParser(QString const& fileContent);
   
    //! The parser to test.
    VHDLPortParser parser_;

    //! Test component to add parsed ports to.
    QSharedPointer<Component> importComponent_;
};

//-----------------------------------------------------------------------------
// Function: tst_VHDLPortParser::tst_VHDLPortParser()
//-----------------------------------------------------------------------------
tst_VHDLPortParser::tst_VHDLPortParser() : parser_(this), importComponent_(new Component())
{

}

//-----------------------------------------------------------------------------
// Function: tst_VHDLPortParser::init()
//-----------------------------------------------------------------------------
void tst_VHDLPortParser::init()
{
    importComponent_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlPortParser::testPortIsParsed()
//-----------------------------------------------------------------------------
void tst_VHDLPortParser::testPortIsParsed()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, expectedName);
    QFETCH(General::Direction, expectedDirection);    
    QFETCH(int, expectedSize);
    QFETCH(QString, expectedType);    
    QFETCH(QString, expectedDefaultValue);
    QFETCH(QString, expectedDescription);

    runParser(fileContent);

    QCOMPARE(importComponent_->getPorts().count(), 1);

    QSharedPointer<Port> createdPort = importComponent_->getPorts().first();
    QCOMPARE(createdPort->getName(), expectedName);
    QCOMPARE(createdPort->getDirection(), expectedDirection);
    QCOMPARE(createdPort->getPortSize(), expectedSize);
    QCOMPARE(createdPort->getTypeName(), expectedType);    
    QCOMPARE(createdPort->getDefaultValue(), expectedDefaultValue);
    QCOMPARE(createdPort->getDescription(), expectedDescription);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlPortParser::testPortIsParsed_data()
//-----------------------------------------------------------------------------
void tst_VHDLPortParser::testPortIsParsed_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("expectedName");    
    QTest::addColumn<General::Direction>("expectedDirection");
    QTest::addColumn<int>("expectedSize");
    QTest::addColumn<QString>("expectedType");    
    QTest::addColumn<QString>("expectedDefaultValue");
    QTest::addColumn<QString>("expectedDescription");

    QTest::newRow("name, type and direction") << 
        "entity test is\n"
        "   port (\n"
        "       clk : in std_logic \n"
        "   );\n"
        "end test;"
        << "clk" << General::IN << 1 << "std_logic" << "" << "";

    QTest::newRow("name, type and direction on a single line") << 
        "entity test is\n"
        "   port (clk : in std_logic);\n"
        "end test;"
        << "clk" << General::IN << 1 << "std_logic"<< "" << "";

    QTest::newRow("name type, direction and default value") << 
        "entity test is\n"
        "   port (enable : in std_logic := '1');\n"
        "end test;"
        << "enable" << General::IN << 1 << "std_logic" << "'1'" << "";

    QTest::newRow("name type, direction, default value and description") << 
        "entity test is\n"
        "   port (data : out std_logic_vector(31 downto 0) := (others => '0') -- data from ip\n"
        ");\n"
        "end test;"
        << "data" << General::OUT << 32 << "std_logic_vector" << "(others => '0')" << "data from ip";

    QTest::newRow("name type, direction and default value on separate lines") << 
        "entity test is\n"
        "port (clk\n"
        ":\n"
        "in\n"
        "std_logic := '1' --       Clk from top.\n"
        ");\n"
        "end test;"
        << "clk" << General::IN << 1 << "std_logic" << "'1'" << "Clk from top.";

    QTest::newRow("port after comment.") << 
        "entity test is\n"
        "    port (\n"
        "            -- DCT signals\n"
        "        i_clk : in std_logic\n"
        ");\n"
        "end test;"
        << "i_clk" << General::IN << 1 << "std_logic" << "" << "";
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLPortParser::runParser()
//-----------------------------------------------------------------------------
void tst_VHDLPortParser::runParser(QString const& fileContent)
{
    parser_.import(fileContent, importComponent_);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlPortParser::testMultiplePortsAreParsed()
//-----------------------------------------------------------------------------
void tst_VHDLPortParser::testMultiplePortsAreParsed()
{
    QFETCH(QString, fileContent);
    QFETCH(int, expectedNumberOfPorts);

    runParser(fileContent);

    QCOMPARE(importComponent_->getPorts().count(), expectedNumberOfPorts);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlPortParser::testMultiplePortsAreParsed_data()
//-----------------------------------------------------------------------------
void tst_VHDLPortParser::testMultiplePortsAreParsed_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("expectedNumberOfPorts");

    QTest::newRow("three standard ports") << 
        "entity test is\n"
        "   port (\n"
        "       clk : in std_logic;\n"
        "       enable : in std_logic := '1';\n"
        "       data : out std_logic\n"
        "   );\n"
        "end test;"
        << 3;

    QTest::newRow("three ports on a single line") << 
        "entity test is\n"
        "   port (clk : in std_logic;enable : in std_logic := '1';data : out std_logic);\n"
        "end test;"
        << 3;

    QTest::newRow("four ports with unusual whitespaces") << 
        "entity test is\n"
        "port (clk:       in\n"
        "std_logic;addr: in\n"
        "   std_logic_vector(11 \n"
        "downto 0);      data:\n"
        "out std_logic_vector(\n"
        "31 downto 0);intr   : out std_logic)\n"
        ";\n"
        "end test;"
        << 4;

    QTest::newRow("only first ports are parsed from multiple port declarations") << 
        "entity test is\n"
        "   port (\n"
        "       clk : in std_logic;\n"        
        "       data : out std_logic_vector(31 downto 0)\n"
        "   );\n"
        "   port (\n"
        "       extra1 : in std_logic_vector(11 downto 0);\n"
        "       extra2 : in std_logic;\n"        
        "       extra3 : out std_logic_vector(31 downto 0)\n"     
        "   );\n"
        "end test;"
        << 2;

    QTest::newRow("multiple definitions") << 
        "entity test is\n"
        "   port (\n"
        "       clk, enable, serial_data : in std_logic\n"
        "   );\n"
        "end test;"
        << 3;

    QTest::newRow("empty ports") << 
        "entity test is\n"
        "   port (\n"
        "   );\n"
        "end test;"
        << 0;

    QTest::newRow("empty entity") << 
        "entity test is\n"
        "end test;"
        << 0;

    QTest::newRow("Comments between ports.") << 
        "entity test is\n"
        "   port (-- Ports are declared here.\n"        
        "-- Clock to test.\n"
        "       clk   : in  std_logic;\n"
        "    -- Reset to test.\n"
        "    -- Remember to connect.\n"
        "       rst_n : in  std_logic;\n"     
        "       data  : out std_logic_vector(31 downto 0)\n"
        "-- End of all ports.\n"
        "   );\n"
        "end test;"
        << 3;

    QTest::newRow("Vector before port whose name begins with word port.") <<
        "ENTITY test IS\n"
        "port (\n"
        "      data : in std_logic_vector(15 downto 0);\n"
        "      port_out : out std_logic\n"
        ");"
        "END test;"
        << 2;
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlPortParser::testCommentedPortIsNotParsed()
//-----------------------------------------------------------------------------
void tst_VHDLPortParser::testCommentedPortIsNotParsed()
{
    QString fileContent =
        "entity test is\n"
        "   port (\n"
        "--       clk : in std_logic\n"
        "   );\n"
        "end test;";

    runParser(fileContent);

    QCOMPARE(importComponent_->getPorts().count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLPortParser::testPortsAreNotParsedOutsideEntity()
//-----------------------------------------------------------------------------
void tst_VHDLPortParser::testPortsAreNotParsedOutsideEntity()
{
    QString fileContent(
        "entity empty is\n"
        "end empty;\n"
        "\n"
        "architecture structural of empty is\n"
        "\n"
        "component dut"
        "   port (\n"
        "       clk : in std_logic\n"
        "   );\n"
        "end dut;\n"
        "\n"
        "end structural;\n");

    runParser(fileContent);

    QCOMPARE(importComponent_->getPorts().count(), 0);
}

QTEST_APPLESS_MAIN(tst_VHDLPortParser)

#include "tst_VHDLPortParser.moc"
