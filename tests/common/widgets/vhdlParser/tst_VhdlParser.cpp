//-----------------------------------------------------------------------------
// File: tst_VhdlParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.09.2014
//
// Description:
// Unit test for class VhdlParser.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/port.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <common/KactusColors.h>
#include <common/widgets/vhdlParser/VhdlParser.h>

#include <wizards/ComponentWizard/VhdlImportEditor/SourceFileDisplayer.h>

#include <QSharedPointer>


Q_DECLARE_METATYPE(QSharedPointer<Port>)
Q_DECLARE_METATYPE(QSharedPointer<ModelParameter>)
Q_DECLARE_METATYPE(General::Direction)

class tst_VhdlParser : public QObject
{
    Q_OBJECT

public:
    tst_VhdlParser();

private slots:
    void initTestCase();
    void init();
    void cleanup();

    //! Testcases:
    void nothingParsedFromMalformedEntity();
    void nothingParsedFromMalformedEntity_data();
    void testPortIsParsed();
    void testPortIsParsed_data();
    void testMultiplePortsAreParsed();
    void testMultiplePortsAreParsed_data();

    void testPortIsHighlighted();
    void testPortIsHighlighted_data();
    void testPortUnselectedFromEditor();
    void testPortUnselectedFromEditor_data();
    void testPortUnselectedWithDoubleClick();
    void testPortUnselectedWithDoubleClick_data();

    void testGenericIsParsed();
    void testGenericIsParsed_data();
    void testMultipleGenericsAreParsed();
    void testMultipleGenericsAreParsed_data();

    void testGenericIsHighlighted();
    void testGenericIsHighlighted_data();
    void testGenerictUnselectedFromEditor();
    void testGenerictUnselectedFromEditor_data();
    void testGenericUnselectedWithDoubleClick();
    void testGenericUnselectedWithDoubleClick_data();

    void testFontInsideEntityIsBlackAndOutsideEntityGray();

private:

    SourceFileDisplayer displayEditor_;

    VhdlParser parser_;
     
    QSignalSpy* createdPorts_;
    QSignalSpy* createdGenerics_;

    void writeToInputFile(QString const& content);
    
    void verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
        const int declarationLength, QColor const& highlightColor) const;

    void verifyDeclarationIsHighlighted(const int declarationStartIndex, const int 
        declarationLength, QColor const& expectedHighlight) const;

    void verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, QColor const& highlightColor);

    void verifySectionFontColorIs(int startIndex, int endIndex, QColor const& expectedFontColor);
 
};

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::tst_VhdlParser()
//-----------------------------------------------------------------------------
tst_VhdlParser::tst_VhdlParser(): displayEditor_(), parser_(&displayEditor_, this), 
    createdPorts_(0), createdGenerics_(0)
{
    qRegisterMetaType<QSharedPointer<Port> >();
    qRegisterMetaType<QSharedPointer<ModelParameter> >();

    createdPorts_ = new QSignalSpy(&parser_, SIGNAL(addPort(QSharedPointer<Port>)));
    createdGenerics_ = new QSignalSpy(&parser_, SIGNAL(addGeneric(QSharedPointer<ModelParameter>)));

    connect(&displayEditor_, SIGNAL(doubleClicked(int)), &parser_, SLOT(toggleAt(int)));
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::initTestCase()
//-----------------------------------------------------------------------------
void tst_VhdlParser::initTestCase()
{
    QVERIFY(createdPorts_->isValid());
    QVERIFY(createdGenerics_->isValid());
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::init()
//-----------------------------------------------------------------------------
void tst_VhdlParser::init()
{
    QVERIFY(createdPorts_->isEmpty());
    QVERIFY(createdGenerics_->isEmpty());

    QVERIFY(!QFileInfo::exists(".input.vhd"));
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::cleanup()
//-----------------------------------------------------------------------------
void tst_VhdlParser::cleanup()
{
    createdPorts_->clear();
    createdGenerics_->clear();

    QFile::remove(".input.vhd");
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::nothingParsedFromMalformedEntity()
//-----------------------------------------------------------------------------
void tst_VhdlParser::nothingParsedFromMalformedEntity()
{
    QFETCH(QString, fileContent);

    writeToInputFile(fileContent);
    parser_.parseFile(".input.vhd");

    QCOMPARE(createdPorts_->count(), 0);
    QCOMPARE(createdGenerics_->count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::nothingParsedFromMalformedEntity_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::nothingParsedFromMalformedEntity_data()
{
    QTest::addColumn<QString>("fileContent");

    QTest::newRow("empty file") << "";

    QTest::newRow("no entity defined") << 
        "architecture rtl of empty is\n"
        "begin\n"
        "end;";

    QTest::newRow("no closing bracket for ports") << 
        "entity noPortBracket is"
        "port (\n"
        "   clk: in std_logic\n"
        "-- no closing bracket after ports."
        "end noPortBracket;";

    QTest::newRow("no closing bracket for generics") << 
        "entity noGenericBracket is"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        "-- no closing bracket after generics."
        "end noGenericBracket;";

    QTest::newRow("no end for entity") << 
        "entity noBracket is"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        ");"
        "port (\n"
        "   clk: in std_logic\n"
        ");";

    QTest::newRow("multiple entities") << 
        "entity firstEntity is\n"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        ");\n"
        "port (\n"
        "   clk: in std_logic\n"
        ");\n"
        "end firstEntity;\n"
        "\n"
        "entity secondEntity is\n"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        ");\n"
        "port (\n"
        "   clk: in std_logic\n"
        ");\n"
        "end secondEntity;\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::writeToInputFile()
//-----------------------------------------------------------------------------
void tst_VhdlParser::writeToInputFile(QString const& content)
{
    QFile input(".input.vhd");
    input.open(QIODevice::WriteOnly);
    input.write(content.toLocal8Bit());
    input.close();
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testWellFormedPortIsParsed()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortIsParsed()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, expectedName);
    QFETCH(General::Direction, expectedDirection);    
    QFETCH(int, expectedSize);
    QFETCH(QString, expectedType);    
    QFETCH(QString, expectedDefaultValue);
    QFETCH(QString, expectedDescription);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    QCOMPARE(createdPorts_->count(), 1);

    QSharedPointer<Port> createdPort = createdPorts_->first().first().value<QSharedPointer<Port> >();
    QCOMPARE(createdPort->getName(), expectedName);
    QCOMPARE(createdPort->getDirection(), expectedDirection);
    QCOMPARE(createdPort->getPortSize(), expectedSize);
    QCOMPARE(createdPort->getTypeName(), expectedType);    
    QCOMPARE(createdPort->getDefaultValue(), expectedDefaultValue);
    QCOMPARE(createdPort->getDescription(), expectedDescription);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testWellFormedPortIsParsed_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortIsParsed_data()
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
        "std_logic := '1' -- Clk from top.\n"
        ");\n"
        "end test;"
        << "clk" << General::IN << 1 << "std_logic" << "'1'" << "Clk from top.";
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testMultiplePortsAreParsed()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testMultiplePortsAreParsed()
{
    QFETCH(QString, fileContent);
    QFETCH(int, expectedNumberOfPorts);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    QCOMPARE(createdPorts_->count(), expectedNumberOfPorts);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testMultiplePortsAreParsed_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testMultiplePortsAreParsed_data()
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
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(int, declarationStartIndex);
    QFETCH(int, declarationLength);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    QColor selectedColor = KactusColors::SW_COMPONENT;
    verifyNotHighlightedBeforeDeclaration(declarationStartIndex, selectedColor);
    verifyDeclarationIsHighlighted(declarationStartIndex, declarationLength, selectedColor);
    verifyNotHighlightedAfterDeclartion(declarationStartIndex, declarationLength, selectedColor);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortIsHighlighted_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("declarationStartIndex");
    QTest::addColumn<int>("declarationLength");
    
    QTest::newRow("simplest possible port declaration") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic\n"
        "   );\n"
        "end test;"
        << 34 << 18;

    QTest::newRow("port declaration with default value") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic := '1'\n"
        "   );\n"
        "end test;"
        << 34 << 25;

    QTest::newRow("port declaration with description") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic -- Clk from top\n"
        "   );\n"
        "end test;"
        << 34 << 34;

    QTest::newRow("port declaration followed by another port") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic; -- Clk from top\n"
        "        enable : in std_logic"
        "   );\n"
        "end test;"
        << 34 << 35;

    QTest::newRow("port declaration with multiple ports") <<
        "entity test is\n"
        "    port (\n"
        "        clk, enable : in std_logic -- One bit ports\n"
        "   );\n"
        "end test;"
        << 34 << 43;

    QTest::newRow("name type, direction and default value on separate lines") << 
        "entity test is\n"
        "port (clk\n"
        ":\n"
        "in\n"
        "std_logic := '1' -- Clk from top.\n"
        ");\n"
        "end test;"
        << 21 << 42;
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::verifyNotHighlightedBeforeDeclaration()
//-----------------------------------------------------------------------------
void tst_VhdlParser::verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, QColor const& highlightColor)
{
    QTextCursor cursor = displayEditor_.textCursor();
    cursor.setPosition(declarationStartIndex);

    QVERIFY2(cursor.charFormat().background().color() != highlightColor, 
        "Highlight for imported declaration should not begin before declaration.");
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::verifyDeclarationIsHighlighed()
//-----------------------------------------------------------------------------
void tst_VhdlParser::verifyDeclarationIsHighlighted(const int declarationStartIndex, 
    const int declarationLength, QColor const& expectedHighlight) const
{
    QTextCursor cursor = displayEditor_.textCursor();

    for (int i = 1; i <= declarationLength; i++)
    {
        cursor.setPosition(declarationStartIndex + i);
        QVERIFY2(cursor.charFormat().background().color() == expectedHighlight,
            "Highlight is not applied to declaration.");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::verifyNotHighlightedAfterDeclartion()
//-----------------------------------------------------------------------------
void tst_VhdlParser::verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
    const int declarationLength, QColor const& highlightColor) const
{
    QTextCursor cursor = displayEditor_.textCursor();

    cursor.setPosition(declarationStartIndex + declarationLength + 1);
    QVERIFY2(cursor.charFormat().background().color() != highlightColor,
        "Highlight for imported declaration should end after declaration.");
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortUnselectedFromEditor()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortUnselectedFromEditor()
{
    QFETCH(QString, fileContent);
    QFETCH(int, declarationStartIndex);
    QFETCH(int, declarationLength);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    QSharedPointer<Port> createdPort = createdPorts_->first().first().value<QSharedPointer<Port> >();
    parser_.editorRemovedPort(createdPort);    

    QColor unselectedColor = KactusColors::MISSING_COMPONENT;
    verifyNotHighlightedBeforeDeclaration(declarationStartIndex, unselectedColor);
    verifyDeclarationIsHighlighted(declarationStartIndex, declarationLength, unselectedColor);
    verifyNotHighlightedAfterDeclartion(declarationStartIndex, declarationLength, unselectedColor);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortUnselectedFromEditor_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortUnselectedFromEditor_data()
{
    testPortIsHighlighted_data();
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortUnselectedWithDoubleClick()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortUnselectedWithDoubleClick()
{
    QFETCH(QString, fileContent);
    QFETCH(int, declarationStartIndex);
    QFETCH(int, declarationLength);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");
    
    QFontMetrics metrics = displayEditor_.fontMetrics();

    QPoint positionInEditor;    
    positionInEditor.setX(8*metrics.width(' ') + metrics.width('c'));
    positionInEditor.setY(3*metrics.height());

    QTest::mouseDClick(displayEditor_.viewport(), Qt::LeftButton, Qt::NoModifier, positionInEditor);    

    QColor unselectedColor = KactusColors::MISSING_COMPONENT;
    verifyNotHighlightedBeforeDeclaration(declarationStartIndex, unselectedColor);
    verifyDeclarationIsHighlighted(declarationStartIndex, declarationLength, unselectedColor);
    verifyNotHighlightedAfterDeclartion(declarationStartIndex, declarationLength, unselectedColor);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortUnselectedFromEditor_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortUnselectedWithDoubleClick_data()
{
    testPortIsHighlighted_data();
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testWellFormedGenericIsParsed()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenericIsParsed()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, expectedName);
    QFETCH(QString, expectedType);
    QFETCH(QString, expectedDefaultValue);
    QFETCH(QString, expectedDescription);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    QCOMPARE(createdGenerics_->count(), 1);

    QSharedPointer<ModelParameter> createdModelParamter = 
        createdGenerics_->first().first().value<QSharedPointer<ModelParameter> >();
    QCOMPARE(createdModelParamter->getName(), expectedName);
    QCOMPARE(createdModelParamter->getDataType(), expectedType);
    QCOMPARE(createdModelParamter->getValue(), expectedDefaultValue);
    QCOMPARE(createdModelParamter->getDescription(), expectedDescription);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testWellFormedGenericIsParsed_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenericIsParsed_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("expectedName");
    QTest::addColumn<QString>("expectedType");
    QTest::addColumn<QString>("expectedDefaultValue");
    QTest::addColumn<QString>("expectedDescription");

    QTest::newRow("name and type") << 
        "entity test is\n"
        "    generic (\n"
        "        freq : integer\n"
        "   );"
        "end test;"
        << "freq" << "integer" << "" << "";

    QTest::newRow("name, type and default value on a single line") << 
        "entity test is\n"
        "    generic (dataWidth_g : integer := 16);"
        "end test;"
        << "dataWidth_g" << "integer" << "16" << "";

    QTest::newRow("name type, default value and description") << 
        "entity test is\n"
        "    generic (\n"
        "        outputFile : string := \"target.out\" -- Some file name\n"
        "   );"
        "end test;"
        << "outputFile" << "string" << "\"target.out\"" << "Some file name";

    QTest::newRow("name, type and default value on a separate lines") << 
        "entity test is\n"
        "    generic (\n"
        "freq \n"
        ": \n"
        "integer \n"
        ":= 500000 --Freq for core.\n"
        ");"
        "end test;"
        << "freq" << "integer" << "500000" << "Freq for core.";
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testMultipleGenericsAreParsed()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testMultipleGenericsAreParsed()
{
    QFETCH(QString, fileContent);
    QFETCH(int, expectedNumberOfGenerics);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    QCOMPARE(createdGenerics_->count(), expectedNumberOfGenerics);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testMultipleGenericsAreParsed_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testMultipleGenericsAreParsed_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("expectedNumberOfGenerics");

    QTest::newRow("three standard generics") << 
        "entity test is\n"
        "   generic (\n"
        "       local_memory_start_addr : integer := 16#1000#;\n"
        "       local_memory_addr_bits  : integer := 12;\n"
        "       code_file               : string  := \"master.tbl\""
        "   );\n"
        "end test;"
        << 3;

    QTest::newRow("three generics on a single line") << 
        "entity test is\n"
        "   generic (local_memory_start_addr : integer := 16#1000#;local_memory_addr_bits  : integer := 12;  width_g	: integer := 8);\n"
        "end test;"
        << 3;

    QTest::newRow("four generics with unusual whitespaces") << 
        "entity test is\n"
        "generic (local_memory_start_addr\n" 
        ": integer := 16#1000#;local_memory_addr_bits  :\n"
        "integer := 12;code_file :string:=\"master.tbl\"\n"
        ";  width_g	: integer \n"
        ":= 8)\n"
        ";\n"
        "end test;"
        << 4;

    QTest::newRow("only first generics are parsed from multiple generic declarations") << 
        "entity test is\n"
        "   generic (\n"
        "       local_memory_start_addr : integer := 16#1000#;\n"
        "       local_memory_addr_bits  : integer := 12\n"
        "   );\n"
        "   generic (\n"
        "       extra1 : integer := 1;\n"
        "       extra2  : integer;\n"
        "       extra3  : integer\n"
        "   );\n"
        "end test;"
        << 2;

    QTest::newRow("multiple definitions") << 
        "entity test is\n"
        "   generic (\n"
        "       dataWidth_g, addressWidth_g, ackWidth_g : integer\n"
        "   );\n"
        "end test;"
        << 3;
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testGenericIsHighlighted()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenericIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(int, declarationStartIndex);
    QFETCH(int, declarationLength);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    QColor selectedColor = KactusColors::HW_BUS_COMPONENT;
    verifyNotHighlightedBeforeDeclaration(declarationStartIndex, selectedColor);
    verifyDeclarationIsHighlighted(declarationStartIndex, declarationLength, selectedColor);
    verifyNotHighlightedAfterDeclartion(declarationStartIndex, declarationLength, selectedColor);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testGenericIsHighlighted_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenericIsHighlighted_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("declarationStartIndex");
    QTest::addColumn<int>("declarationLength");

    QTest::newRow("simplest possible generic declaration") <<
        "entity test is\n"
        "    generic (\n"
        "        freq : integer\n"
        "   );"
        "end test;"
        << 37 << 14;

    QTest::newRow("generic declaration with default value") <<
        "entity test is\n"
        "    generic (\n"
        "        dataWidth_g : integer := 16);"
        "end test;"
        << 37 << 27;

    QTest::newRow("generic declaration with description") <<
        "entity test is\n"
        "    generic (\n"
        "        outputFile : string := \"target.out\" -- Some file name\n"
        "   );"
        "end test;"
        << 37 << 53;

    QTest::newRow("generic declaration followed by another generic") <<
        "entity test is\n"
        "    generic (\n"
        "        local_memory_start_addr : integer := 16#1000#;\n"
        "        local_memory_addr_bits  : integer := 12\n"
        "   );\n"
        "end test;"
        << 37 << 45;

    QTest::newRow("generic declaration with multiple generic") <<
        "entity test is\n"
        "    generic (\n"
        "        freq, dataWidth : integer --Configuration\n"
        "   );\n"
        "end test;"
        << 37 << 41;

    QTest::newRow("name type and default value on separate lines") << 
        "entity test is\n"
        "generic (local_memory_start_addr\n" 
        ": integer \n"
        ":= 16#1000#\n"
        ")\n"
        ";\n"
        "end test;"
        << 24 << 46;
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testGenerictUnselectedFromEditor()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenerictUnselectedFromEditor()
{
    QFETCH(QString, fileContent);
    QFETCH(int, declarationStartIndex);
    QFETCH(int, declarationLength);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    QSharedPointer<ModelParameter> createdModelParameter = 
        createdGenerics_->first().first().value<QSharedPointer<ModelParameter> >();
    parser_.editorRemovedModelParameter(createdModelParameter);    

    QColor unselectedColor = KactusColors::MISSING_COMPONENT;
    verifyNotHighlightedBeforeDeclaration(declarationStartIndex, unselectedColor);
    verifyDeclarationIsHighlighted(declarationStartIndex, declarationLength, unselectedColor);
    verifyNotHighlightedAfterDeclartion(declarationStartIndex, declarationLength, unselectedColor);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortUnselectedFromEditor_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenerictUnselectedFromEditor_data()
{
    testGenericIsHighlighted_data();
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testGenericUnselectedWithDoubleClick()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenericUnselectedWithDoubleClick()
{
    QFETCH(QString, fileContent);
    QFETCH(int, declarationStartIndex);
    QFETCH(int, declarationLength);

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    QFontMetrics metrics = displayEditor_.fontMetrics();

    QPoint positionInEditor;    
    positionInEditor.setX(8*metrics.width(' ') + metrics.width('c'));
    positionInEditor.setY(3*metrics.height());

    QTest::mouseDClick(displayEditor_.viewport(), Qt::LeftButton, Qt::NoModifier, positionInEditor);    

    QColor unselectedColor = KactusColors::MISSING_COMPONENT;
    verifyNotHighlightedBeforeDeclaration(declarationStartIndex, unselectedColor);
    verifyDeclarationIsHighlighted(declarationStartIndex, declarationLength, unselectedColor);
    verifyNotHighlightedAfterDeclartion(declarationStartIndex, declarationLength, unselectedColor);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testGenericUnselectedWithDoubleClick_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenericUnselectedWithDoubleClick_data()
{
    testGenericIsHighlighted_data();
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testFontInsideEntityIsBlackAndOutsideEntityGray()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testFontInsideEntityIsBlackAndOutsideEntityGray()
{
    QString fileContent = 
        "library IEEE;\n"
        "entity test is\n"        
        "end test;\n"
        "begin rtl of test\n";

    writeToInputFile(fileContent);

    parser_.parseFile(".input.vhd");

    verifySectionFontColorIs(0, 13, QColor("gray"));

    verifySectionFontColorIs(14, 39, QColor("black"));

    verifySectionFontColorIs(40, 58, QColor("gray"));
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::verifySectionFontColorIs()
//-----------------------------------------------------------------------------
void tst_VhdlParser::verifySectionFontColorIs(int startIndex, int endIndex, QColor const& expectedFontColor)
{
    QTextCursor cursor = displayEditor_.textCursor();

    int sectionLength = endIndex - startIndex;
    for (int i = 0; i < sectionLength; i++)
    {
        cursor.setPosition(startIndex + i);
        QVERIFY2(cursor.charFormat().foreground().color() == expectedFontColor,
            "Highlight is not applied to declaration.");
    }
}

QTEST_MAIN(tst_VhdlParser)

#include "tst_VhdlParser.moc"
