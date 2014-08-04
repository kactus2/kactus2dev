//-----------------------------------------------------------------------------
// File: tst_VerilogHeaderWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Unit test for class VerilogHeaderWriter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>

class tst_VerilogHeaderWriter : public QObject
{
    Q_OBJECT

public:
    tst_VerilogHeaderWriter();

private slots:
    void init();
    void cleanup();
    void testVlnv();
    void testVlnv_data();
    void testGenerationTime();
    void testDescription();
    void testDescription_data();

private:
    void compareLineByLine(QString const& expectedOutput);
   
    QString outputString_;

    QTextStream outputStream_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogHeaderWriter::tst_VerilogHeaderWriter()
//-----------------------------------------------------------------------------
tst_VerilogHeaderWriter::tst_VerilogHeaderWriter() : outputString_(), outputStream_()
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogHeaderWriter::init()
//-----------------------------------------------------------------------------
void tst_VerilogHeaderWriter::init()
{
    outputStream_.setString(&outputString_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogHeaderWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_VerilogHeaderWriter::cleanup()
{
    outputString_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogHeaderWriter::()
//-----------------------------------------------------------------------------
void tst_VerilogHeaderWriter::testVlnv()
{
    QFETCH(QString, vendor);
    QFETCH(QString, library);
    QFETCH(QString, name);
    QFETCH(QString, version);

    VLNV vlnv(VLNV::COMPONENT, vendor, library, name, version);
    QDateTime generationTime(QDate(2014, 4, 1), QTime(14,14,14));

    VerilogHeaderWriter writer(vlnv, "", "");
    writer.write(outputStream_, "output.v", "", generationTime);

    compareLineByLine(QString(
        "//-----------------------------------------------------------------------------\n"
        "// File          : output.v\n"
        "// Creation date : 1.4.2014\n"
        "// Creation time : 14:14:14\n"
        "// Description   : \n"
        "// Created by    : \n"
        "// This file was generated with Kactus2 verilog generator\n"
        "// based on IP-XACT component " + vendor + ":" + library + ":" + name + ":" + version + "\n"
        "// whose XML file is \n"
        "//-----------------------------------------------------------------------------\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogHeaderWriter::testVlnv_data()
//-----------------------------------------------------------------------------
void tst_VerilogHeaderWriter::testVlnv_data()
{
    QTest::addColumn<QString>("vendor");
    QTest::addColumn<QString>("library");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("version");

    QTest::newRow("all empty") << "" << "" << "" << "";
    QTest::newRow("common") << "Test" << "TestLib" << "Component" << "1.0";
    QTest::newRow("numbers") << "L4Factory" << "Lib932" << "IP11" << "1.1.1.1.1";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogHeaderWriter::testGenerationTime()
//-----------------------------------------------------------------------------
void tst_VerilogHeaderWriter::testGenerationTime()
{
    VLNV vlnv(VLNV::COMPONENT, "Tester", "TestLibrary", "TestComponent", "0.1");

    QDateTime generationTime(QDate(2014, 1, 14), QTime(12, 0, 40));

    VerilogHeaderWriter writer(vlnv, "", "");
    writer.write(outputStream_, "TestComponent.v", "", generationTime);       

    compareLineByLine(QString(
        "//-----------------------------------------------------------------------------\n"
        "// File          : TestComponent.v\n"
        "// Creation date : 14.1.2014\n"
        "// Creation time : 12:00:40\n"
        "// Description   : \n"
        "// Created by    : \n"
        "// This file was generated with Kactus2 verilog generator\n"
        "// based on IP-XACT component Tester:TestLibrary:TestComponent:0.1\n"
        "// whose XML file is \n"
        "//-----------------------------------------------------------------------------\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogHeaderWriter::testGenerationTime()
//-----------------------------------------------------------------------------
void tst_VerilogHeaderWriter::testDescription()
{
    QFETCH(QString, description);
    QFETCH(QString, expectedOutput);

    VLNV vlnv(VLNV::COMPONENT, "Tester", "TestLibrary", "TestComponent", "0.1");

    QDateTime generationTime(QDate(2014, 4, 14), QTime(14, 14, 14));

    VerilogHeaderWriter writer(vlnv, "", "");
    writer.write(outputStream_, "TestComponent.v", description, generationTime);       

    compareLineByLine(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogHeaderWriter::testDescription_data()
//-----------------------------------------------------------------------------
void tst_VerilogHeaderWriter::testDescription_data()
{
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("empty description") << "" 
        <<
        "//-----------------------------------------------------------------------------\n"
        "// File          : TestComponent.v\n"
        "// Creation date : 14.4.2014\n"
        "// Creation time : 14:14:14\n"
        "// Description   : \n"
        "// Created by    : \n"
        "// This file was generated with Kactus2 verilog generator\n"
        "// based on IP-XACT component Tester:TestLibrary:TestComponent:0.1\n"
        "// whose XML file is \n"
        "//-----------------------------------------------------------------------------\n";

    QTest::newRow("One-line description") << "Component description." 
        <<
        "//-----------------------------------------------------------------------------\n"
        "// File          : TestComponent.v\n"
        "// Creation date : 14.4.2014\n"
        "// Creation time : 14:14:14\n"
        "// Description   : Component description.\n"
        "// Created by    : \n"
        "// This file was generated with Kactus2 verilog generator\n"
        "// based on IP-XACT component Tester:TestLibrary:TestComponent:0.1\n"
        "// whose XML file is \n"
        "//-----------------------------------------------------------------------------\n";

    QTest::newRow("Multiline description") << "Component description\n"
        "on several\n"
        "separate\n"
        "lines." 
        <<
        "//-----------------------------------------------------------------------------\n"
        "// File          : TestComponent.v\n"
        "// Creation date : 14.4.2014\n"
        "// Creation time : 14:14:14\n"
        "// Description   : Component description\n"
        "//                 on several\n"
        "//                 separate\n"
        "//                 lines.\n"
        "// Created by    : \n"
        "// This file was generated with Kactus2 verilog generator\n"
        "// based on IP-XACT component Tester:TestLibrary:TestComponent:0.1\n"
        "// whose XML file is \n"
        "//-----------------------------------------------------------------------------\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::compareLineByLine()
//-----------------------------------------------------------------------------
void tst_VerilogHeaderWriter::compareLineByLine(QString const& expectedOutput)
{
    QStringList outputLines = outputString_.split("\n");
    QStringList expectedLines = expectedOutput.split("\n");

    int lineCount = qMin(outputLines.count(), expectedLines.count());
    for (int i = 0; i < lineCount; i++)
    {
        QCOMPARE(outputLines.at(i), expectedLines.at(i));
    }
}

QTEST_APPLESS_MAIN(tst_VerilogHeaderWriter)

#include "tst_VerilogHeaderWriter.moc"
