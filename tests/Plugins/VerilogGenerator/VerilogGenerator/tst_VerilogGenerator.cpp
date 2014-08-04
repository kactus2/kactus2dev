//-----------------------------------------------------------------------------
// File: tst_VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Unit test for class VerilogGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComponentInstance.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/vlnv.h>

#include <Plugins/VerilogGenerator/VerilogGenerator.h>

class tst_VerilogGenerator : public QObject
{
    Q_OBJECT

public:
    tst_VerilogGenerator();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testTopLevelComponent();
    void testConsecutiveParseCalls();
    void testFileHeaderIsPrinted();

private:

    void addPort( QString const& portName, int portSize, General::Direction direction );
    void addModelParameter( QString const& name, QString const& value );

    void runGenerator();

    void verifyOutputContains(QString const& expectedOutput);

    void compareLineByLine(QString const& expectedOutput);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    void readOutputFile();


    //! The top level component for which the generator is run.
    QSharedPointer<Component> component_;
    
    //! The generator output as a string.
    QString outputString_;


    QDateTime generationTime_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::tst_VerilogGenerator()
//-----------------------------------------------------------------------------
tst_VerilogGenerator::tst_VerilogGenerator(): component_(), outputString_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::init()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_ = QSharedPointer<Component>(new Component(vlnv));

    QVERIFY(!QFile::exists("./generatorOutput.v"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanup()
{
    component_.clear();

    outputString_.clear();

    QFile::remove("./generatorOutput.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testTopLevelComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testTopLevelComponent()
{
    addPort("clk", 1, General::IN);
    addPort("rst_n", 1, General::IN);
    addPort("dataOut", 8, General::OUT);
    addPort("dataIn", 8, General::IN);
    addModelParameter("dataWidth", "8");
    addModelParameter("freq", "100000");

    runGenerator();

    verifyOutputContains(QString("module TestComponent(clk, dataIn, rst_n, dataOut);\n"
        "    parameter dataWidth = 8;\n"
        "    parameter freq = 100000;\n"
        "    // These ports are not in any interface\n"         
        "    input clk;\n"
        "    input [7:0] dataIn;\n"
        "    input rst_n;\n"
        "    output [7:0] dataOut;\n"
        "\n"
        "endmodule\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addPort( QString const& portName, int portSize, General::Direction direction )
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction, portSize - 1, 0, "", true));
    component_->addPort(port);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addModelParameter()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addModelParameter( QString const& name, QString const& value )
{
    QSharedPointer<ModelParameter> parameter = QSharedPointer<ModelParameter>(new ModelParameter());
    parameter->setName(name);
    parameter->setValue(value);
    component_->getModel()->addModelParameter(parameter);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::runGenerator()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::runGenerator()
{
    VerilogGenerator generator;

    generator.parse(component_);

    generationTime_ =  QDateTime::currentDateTime();

    generator.generate("./generatorOutput.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFileHeaderIsPrinted()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFileHeaderIsPrinted()
{
    component_->setDescription("Component description\nspanning multiple\nlines.");
        
    runGenerator();
    
    compareLineByLine(QString(
        "//-----------------------------------------------------------------------------\n"
        "// File          : generatorOutput.v\n"
        "// Creation date : " + generationTime_.date().toString(Qt::LocalDate) + "\n"
        "// Creation time : " + generationTime_.time().toString(Qt::LocalDate) + "\n"
        "// Description   : Component description\n"
        "//                 spanning multiple\n"
        "//                 lines.\n"
        "// Created by    : user\n"
        "// This file was generated with Kactus2 verilog generator\n"
        "// based on IP-XACT component Test:TestLibrary:TestComponent:1.0\n"
        "// whose XML file is C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml\n"
        "//-----------------------------------------------------------------------------\n"
        "\n"
        "module TestComponent();\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testConsecutiveParseCalls()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testConsecutiveParseCalls()
{
    VLNV secondVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "Override", "1.0");
    QSharedPointer<Component> secondComponent = QSharedPointer<Component>(new Component());
    secondComponent->setVlnv(secondVlnv);

    VerilogGenerator generator;

    generator.parse(component_);
    generator.parse(secondComponent);

    generator.generate("./generatorOutput.v");

    verifyOutputContains(QString("module Override();\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::verifyOutputContains()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::verifyOutputContains(QString const& expectedOutput)
{
    readOutputFile();

    if (!outputString_.contains(expectedOutput))
    {
        QStringList outputLines = outputString_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        int firstLine = outputLines.indexOf(expectedLines.first());

        if (firstLine == -1)
        {
            compareLineByLine(expectedOutput);
        }
        else
        {
            int lineCount = qMin(outputLines.count(), expectedLines.count());
            for (int i = firstLine; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i), expectedLines.at(i));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::compareLineByLine()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::compareLineByLine(QString const& expectedOutput)
{
    readOutputFile();

    QStringList outputLines = outputString_.split("\n");
    QStringList expectedLines = expectedOutput.split("\n");

    int lineCount = qMin(outputLines.count(), expectedLines.count());
    for (int i = 0; i < lineCount; i++)
    {
        QCOMPARE(outputLines.at(i), expectedLines.at(i));
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::readOutputFile()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::readOutputFile()
{
    QFile outputFile("./generatorOutput.v");

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    outputString_ = outputFile.readAll();
}

QTEST_APPLESS_MAIN(tst_VerilogGenerator)

#include "tst_VerilogGenerator.moc"
