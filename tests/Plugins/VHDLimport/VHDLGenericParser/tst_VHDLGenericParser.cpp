//-----------------------------------------------------------------------------
// File: tst_Module.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 22.09.2014
//
// Description:
// Unit test for class VHDLGenericParser.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QObject>

#include <Plugins/VHDLimport/VHDLGenericParser.h>

#include <IPXACTmodels/component.h>

class tst_VHDLGenericParser : public QObject
{
    Q_OBJECT

public:
    tst_VHDLGenericParser();

private slots:
    void init();

    void testGenericIsParsed();
    void testGenericIsParsed_data();
    
    void testMultipleGenericsAreParsed();
    void testMultipleGenericsAreParsed_data();
    
    void testCommentedGenericIsNotParsed();
    
    void testModelParametersAreNotParsedOutsideEntity();

private:
        
    void runParser(QString const& fileContent);
   
    //! The parser to test.
    VHDLGenericParser parser_;

    //! The component to add parsed model parameters to.
    QSharedPointer<Component> importComponent_;
};

//-----------------------------------------------------------------------------
// Function: tst_VHDLGenericParser::tst_VHDLGenericParser()
//-----------------------------------------------------------------------------
tst_VHDLGenericParser::tst_VHDLGenericParser(): parser_(this), importComponent_(new Component())
{

}

//-----------------------------------------------------------------------------
// Function: tst_VHDLGenericParser::init()
//-----------------------------------------------------------------------------
void tst_VHDLGenericParser::init()
{
    importComponent_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLGenericParser::testWellFormedGenericIsParsed()
//-----------------------------------------------------------------------------
void tst_VHDLGenericParser::testGenericIsParsed()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, expectedName);
    QFETCH(QString, expectedType);
    QFETCH(QString, expectedDefaultValue);
    QFETCH(QString, expectedDescription);

    runParser(fileContent);

    QCOMPARE(importComponent_->getModelParameters().count(), 1);

    QSharedPointer<ModelParameter> createdModelParamter = importComponent_->getModelParameters().first();
    QCOMPARE(createdModelParamter->getName(), expectedName);
    QCOMPARE(createdModelParamter->getDataType(), expectedType);
    QCOMPARE(createdModelParamter->getValue(), expectedDefaultValue);
    QCOMPARE(createdModelParamter->getDescription(), expectedDescription);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLGenericParser::testWellFormedGenericIsParsed_data()
//-----------------------------------------------------------------------------
void tst_VHDLGenericParser::testGenericIsParsed_data()
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
// Function: tst_VHDLGenericParser::runParser()
//-----------------------------------------------------------------------------
void tst_VHDLGenericParser::runParser(QString const& fileContent)
{
    parser_.import(fileContent, importComponent_);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLGenericParser::testMultipleGenericsAreParsed()
//-----------------------------------------------------------------------------
void tst_VHDLGenericParser::testMultipleGenericsAreParsed()
{
    QFETCH(QString, fileContent);
    QFETCH(int, expectedNumberOfGenerics);

    runParser(fileContent);

    QCOMPARE(importComponent_->getModelParameters().count(), expectedNumberOfGenerics);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLGenericParser::testMultipleGenericsAreParsed_data()
//-----------------------------------------------------------------------------
void tst_VHDLGenericParser::testMultipleGenericsAreParsed_data()
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

    QTest::newRow("comments along generics") << 
        "entity test is\n"
        "   generic (--here there be generics\n"
        "       local_memory_start_addr : integer := 16#1000#;\n"
        "-- first comment\n"
        "-- second comment\n"
        "       local_memory_addr_bits  : integer := 12\n"
        "-- last comment\n"
        "   );\n"
        "end test;"
        << 2;
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLGenericParser::testCommentedGenericIsNotParsed()
//-----------------------------------------------------------------------------
void tst_VHDLGenericParser::testCommentedGenericIsNotParsed()
{
    QString fileContent =
        "entity test is\n"
        "   generic (\n"
        "--       dataWidth_g : integer\n"
        "   );\n"
        "end test;";

    runParser(fileContent);

    QCOMPARE(importComponent_->getModelParameters().count(), 0);
}


//-----------------------------------------------------------------------------
// Function: tst_VHDLGenericParser::testModelParametersAreNotParsedOutsideEntity()
//-----------------------------------------------------------------------------
void tst_VHDLGenericParser::testModelParametersAreNotParsedOutsideEntity()
{
    QString fileContent(
        "entity empty is\n"
        "end empty;\n"
        "\n"
        "architecture structural of empty is\n"
        "\n"
        "component dut"
        "   generic (\n"        
        "       local_memory_addr_bits  : integer := 12;\n"
        "       local_memory_data_bits  : integer := 32\n"
        "   );\n"
        "end dut;\n"
        "\n"
        "end structural;\n");

    runParser(fileContent);

    QCOMPARE(importComponent_->getModelParameters().count(), 0);
}

QTEST_APPLESS_MAIN(tst_VHDLGenericParser)

#include "tst_VHDLGenericParser.moc"
