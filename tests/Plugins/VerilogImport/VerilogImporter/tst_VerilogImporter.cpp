//-----------------------------------------------------------------------------
// File: tst_VerilogImporter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.09.2014
//
// Description:
// Unit test for class VerilogImporter.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QSharedPointer>
#include <QTextCursor>
#include <QPlainTextEdit>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>

#include <Plugins/VerilogImport/VerilogImporter.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <wizards/ComponentWizard/ImportEditor/ImportHighlighter.h>

class tst_VerilogImporter : public QObject
{
    Q_OBJECT

public:
    tst_VerilogImporter();

private slots:
    void init();

    //! Test cases:
    void testFileTypes();

    void testNothingIsParsedFromMalformedInput();
    void testNothingIsParsedFromMalformedInput_data();

    void testPortIsHighlighted();
    void testPortIsHighlighted_data();

    void testParameterIsHighlighted();
    void testParameterIsHighlighted_data();

    void testModuleIsHighlighted();
    void testModuleIsHighlighted_data();

    void testParameterInPortDeclaration();
    void testParameterInPortDeclaration_data();

    void testModelNameAndEnvironmentIsImportedToView();
    void testModelNameAndEnvironmentIsImportedToView_data();

private:

    void runParser(QString const& input);

    void verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, QColor const& highlightColor);
    void verifyDeclarationIsHighlighted(const int declarationStartIndex, 
        const int declarationLength, QColor const& expectedHighlight) const;
    void verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
        const int declarationLength, QColor const& highlightColor) const;

    void verifySectionFontColorIs(int startIndex, int endIndex, QColor const& expectedFontColor);
   

    QSharedPointer<Component> importComponent_;

    QPlainTextEdit displayEditor_;

    ImportHighlighter* highlighter_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::tst_VerilogImporter()
//-----------------------------------------------------------------------------
tst_VerilogImporter::tst_VerilogImporter(): importComponent_(0), displayEditor_(), 
    highlighter_(new ImportHighlighter(&displayEditor_, this))
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::init()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::init()
{
    importComponent_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testFileTypes()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testFileTypes()
{
    VerilogImporter importer;

    QStringList supportedFileTypes = importer.getSupportedFileTypes();

    QVERIFY(supportedFileTypes.contains("verilogSource"));
    QVERIFY(supportedFileTypes.contains("verilogSource-95"));
    QVERIFY(supportedFileTypes.contains("verilogSource-2001"));
    QCOMPARE(supportedFileTypes.count(), 3);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testNothingIsParsedFromMalformedInput()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testNothingIsParsedFromMalformedInput()
{
    QFETCH(QString, input);

    runParser(input);

    QCOMPARE(importComponent_->getPorts().count(), 0);
    QCOMPARE(importComponent_->getModelParameters().count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testNothingIsParsedFromMalformedInput_data()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testNothingIsParsedFromMalformedInput_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("empty input") << "";

    QTest::newRow("empty module name") <<
        "module (input clk);\n"
        "endmodule";

    QTest::newRow("no module ending") <<
        "module broken(input clk);\n";

    /*
    QTest::newRow("Commented out module") <<
        "//module test(input clk);\n"
        "//endmodule\n";*/

    QTest::newRow("No semicolon after ports") <<
        "module test (\n"
        "    input clk,\n"
        "    input rst\n"
        ") // Missing semicolon.\n"
        "endmodule";

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::runParser()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::runParser(QString const& input)
{
    displayEditor_.setPlainText(input);

    VerilogImporter parser;
    parser.setHighlighter(highlighter_);
    parser.import(input, importComponent_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testPortIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, portDeclaration);

    runParser(fileContent);

    QVERIFY2(importComponent_->getPorts().count() != 0, "No ports parsed from input.");

    int begin = fileContent.indexOf(portDeclaration);
    
    verifyNotHighlightedBeforeDeclaration(begin, ImportColors::PORT);
    verifyDeclarationIsHighlighted(begin, portDeclaration.length(), ImportColors::PORT);
    verifyNotHighlightedAfterDeclartion(begin, portDeclaration.length(), ImportColors::PORT);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testPortIsHighlighted_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("portDeclaration");

    QTest::newRow("Simplest possible port declaration") <<
        "module test (\n"
        "        input clk\n"
        "   );\n"
        "endmodule"
        << "input clk";   

    QTest::newRow("Port declaration with description") <<
        "module test (\n"
        "        input clk // Clk from top\n"
        "   );\n"
        "endmodule"
        << "input clk // Clk from top";
    
    QTest::newRow("Port declaration followed by another port") <<
        "module TEST(\n"
        "        input clk, // Clk from top\n"
        "        input enable\n"
        "   );\n"
        "endmodule"
        << "input clk, // Clk from top";
    
    QTest::newRow("Port declaration with multiple ports") <<
        "module test (\n"
        "        input clk, enable //One bit ports\n"
        "   );\n"
        "endmodule"
        << "input clk, enable //One bit ports";
    
    QTest::newRow("Direction, type and name on separate lines") << 
        "module test (input\n"
        "reg\n"
        "clk // Clk from top.\n"
        ");\n"
        "endmodule;"
        << 
        "input\n"
        "reg\n"
        "clk // Clk from top.";
    
    QTest::newRow("Double comment line") << 
        "module test (\n"
        "    input clk,\n"
        "    // first comment.\n"
        "    //second comment.\n"
        "    // third comment.\n"
        "    input rst_n\n"
        "    );\n"
        "endmodule;"
        << 
        "input clk,";

    QTest::newRow("Simplest possible 1995-style port declaration") <<
        "module test (clk);"
        "   input clk;\n"
        "endmodule"
        << "input clk;";

    QTest::newRow("1995-style port declaration followed by another port") <<
        "module TEST(clk, enable);\n"
        "        input clk; // Clk from top\n"
        "        input enable;\n"
        "endmodule"
        << "input clk; // Clk from top";


    QTest::newRow("1995-stype port declarations with multiple ports") <<
        "module test (clk, enable);\n"
        "        input clk, enable; //One bit ports\n"
        "endmodule"
        << "input clk, enable; //One bit ports";

    QTest::newRow("1995-stype port with direction and name on separate lines") << 
        "module test (clk);\n"
        "input\n"        
        "clk ; // Clk from top.\n"
        "endmodule;"
        << 
        "input\n"        
        "clk ; // Clk from top.";
        
    QTest::newRow("1995-style ports with double comment line") << 
        "module test (clk, rst_n); \n"
        "    input clk;\n"
        "    // first comment.\n"
        "    //second comment.\n"
        "    // third comment.\n"
        "    input rst_n\n"
        "\n"
        "endmodule;"
        << 
        "input clk;";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::verifyNotHighlightedBeforeDeclaration()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, 
    QColor const& highlightColor)
{
    QTextCursor cursor = displayEditor_.textCursor();
    cursor.setPosition(declarationStartIndex);

    if (cursor.atBlockStart())
    {
        cursor.setPosition(declarationStartIndex - 1);
    }

    QColor appliedColor = cursor.charFormat().background().color();

    QVERIFY2(appliedColor != highlightColor, "Highlight applied before declaration.");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::verifyDeclarationIsHighlighed()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::verifyDeclarationIsHighlighted(const int declarationStartIndex, 
    const int declarationLength, QColor const& expectedHighlight) const
{
    QTextCursor cursor = displayEditor_.textCursor();

    for (int i = 1; i <= declarationLength; i++)
    {
        cursor.setPosition(declarationStartIndex + i);

        QColor appliedColor = cursor.charFormat().background().color();

        if (appliedColor != expectedHighlight)
        {
            QString correctlyHighligted(displayEditor_.toPlainText().mid(declarationStartIndex, i - 1));
            QString errorMessage("Highlight ended after '" + correctlyHighligted + "'");

            QFAIL(errorMessage.toLocal8Bit());           
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::verifyNotHighlightedAfterDeclartion()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
    const int declarationLength, QColor const& highlightColor) const
{
    QTextCursor cursor = displayEditor_.textCursor();
    cursor.setPosition(declarationStartIndex + declarationLength + 1);

    QColor appliedColor = cursor.charFormat().background().color();

    QVERIFY2(appliedColor != highlightColor, "Highlight applied after declaration.");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testParameterIsHighlighted()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testParameterIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, parameterDeclaration);

    runParser(fileContent);

    QVERIFY2(importComponent_->getModelParameters().count() != 0, "No parameters parsed from input.");

    int begin = fileContent.indexOf(parameterDeclaration);

    verifyNotHighlightedBeforeDeclaration(begin, ImportColors::MODELPARAMETER);
    verifyDeclarationIsHighlighted(begin, parameterDeclaration.length(), ImportColors::MODELPARAMETER);
    verifyNotHighlightedAfterDeclartion(begin, parameterDeclaration.length(), ImportColors::MODELPARAMETER);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testParameterIsHighlighted_data()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testParameterIsHighlighted_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("parameterDeclaration");

    QTest::newRow("Simplest possible parameter declaration") <<
        "module test #(parameter size = 8) (\n"
        "   );\n"
        "endmodule"
        << "parameter size = 8";   

    QTest::newRow("Parameter declaration with description") <<
        "module test #(\n"
        "    parameter size = 8 // Size of ports.\n"
        ") ();\n"
        "endmodule"
        << "parameter size = 8 // Size of ports.";  

    QTest::newRow("Parameter declaration followed by another parameter") <<
        "module test #(\n"
        "    parameter size = 8, // Size of ports.\n"
        "    parameter width = 16"
        ") ();\n"
        "endmodule"
        << "parameter size = 8, // Size of ports.";  
  
    QTest::newRow("Parameter declaration with multiple parameters") <<
        "module test #(\n"
        "    parameter size = 8, width = 16, dimension = 1 // Constants for ports.\n"
        ") ();\n"
        "endmodule"
        << "parameter size = 8, width = 16, dimension = 1 // Constants for ports.";

    QTest::newRow("Type, name and value on separate lines") << 
        "module test #(\n"
        "    parameter\n"
        "size \n"
        "= 8 // Line breaks.\n"
        ") ();\n"
        "endmodule"
        << 
        "parameter\n"
        "size \n"
        "= 8 // Line breaks.";
                
    QTest::newRow("Double comment line") << 
        "module test #(\n"
        "    parameter size = 8,\n"
        "    // First comment line.\n"
        "    // Second comment line.\n"
        "    parameter width = 16"
        ") ();\n"
        "endmodule"
        << 
        "parameter size = 8,";
 
    QTest::newRow("Simplest possible 1995-style parameter declaration") <<
        "module test ();"
        "   parameter size = 8;\n"
        "endmodule"
        << "parameter size = 8;";

    QTest::newRow("1995-style parameter declaration followed by another parameter") <<
        "module test ();"
        "   parameter size = 8; // Size of ports.\n"
        "   parameter width = 16;"
        "endmodule"
        << "parameter size = 8; // Size of ports.";
       
    QTest::newRow("1995-stype parameter declarations with multiple parameter") <<
        "module test ();\n"
        "        parameter size = 8, width = 16; // Port sizes.\n"
        "endmodule"
        << "parameter size = 8, width = 16; // Port sizes.";

    QTest::newRow("1995-stype parameter with type, name and value on separate lines") << 
        "module test ();"
        "   parameter\n"
        "size\n"
        "= 8; // Port size.\n"
        "endmodule"
        << 
        "parameter\n"
        "size\n"
        "= 8; // Port size.";
    
    QTest::newRow("1995-style parameter with double comment line") << 
        "module test();\n"
        "    parameter size = 8;\n"
        "    // First comment line.\n"
        "    // Second comment line.\n"
        "    parameter width = 16;"
        "endmodule"
        << 
        "parameter size = 8;";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testParameterInPortDeclaration()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testParameterInPortDeclaration()
{
    QFETCH(QString, fileContent);
    QFETCH(int, expectedLeftBound);
    QFETCH(int, expectedRightBound);

    runParser(fileContent);

    QVERIFY2(importComponent_->getPorts().count() != 0, "No ports parsed from input.");

    QSharedPointer<Port> createdPort = importComponent_->getPorts().first();
    QCOMPARE(createdPort->getLeftBound(), expectedLeftBound);
    QCOMPARE(createdPort->getRightBound(), expectedRightBound);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testParameterInPortDeclaration_data()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testParameterInPortDeclaration_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("expectedLeftBound");
    QTest::addColumn<int>("expectedRightBound");

    QTest::newRow("Parameter as left port bound") <<
        "module test #(parameter left = 4) (\n"
        "   input [left:0] data\n"
        ");\n"
        "endmodule"
        << 4 << 0;

    QTest::newRow("Parameter as right port bound") <<
        "module test #(parameter right = 4) (\n"
        "   input [0:right] data\n"
        ");\n"
        "endmodule"
        << 0 << 4;

    QTest::newRow("Parameters in left bound equation") <<
        "module test #(\n"
        "   parameter data_bits = 32,\n"
        "   parameter addr_bits = 8\n"
        ")(\n"
        "   input [data_bits + addr_bits:0] bus\n"
        ");\n"
        "endmodule"
        << 32 + 8 << 0;

    QTest::newRow("Parameters and constants in left bound equation") <<
        "module test #(\n"
        "   parameter data_bits = 8,\n"
        "   parameter data_bus_count = 3"        
        ")(\n"
        "   input [data_bits ** data_bus_count - 1:0] data_bus\n"
        ");\n"
        "endmodule"
        << 8*8*8 - 1 << 0;

    QTest::newRow("Parameters in other parameters") <<
        "module test #(\n"
        "   parameter data_bits = 8,\n"
        "   parameter addr_bits = 4,\n"
        "   parameter port_width = data_bits + addr_bits"        
        ")(\n"
        "   input [port_width - 1:0] data_bus\n"
        ");\n"
        "endmodule"
        << 8 + 4 - 1 << 0;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testModuleIsHighlighted()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testModuleIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, module);

    runParser(fileContent);

    int begin = fileContent.indexOf(module);

    verifySectionFontColorIs(1, begin, QColor("gray"));
    verifySectionFontColorIs(begin, module.length(), QColor("black"));
    verifySectionFontColorIs(begin +  module.length(), fileContent.length(), QColor("gray"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testModuleIsHighlighted_data()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testModuleIsHighlighted_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("module");
   
    QTest::newRow("Empty module") <<
        "module basic();\n"
        "endmodule\n"
        <<
        "module basic();\n"
        "endmodule\n";

    QTest::newRow("Comment before and after module") <<
        "// Testbench for another module.\n"
        "module tb();\n"
        "endmodule\n"
        "// Testbench ends here.\n"
        <<
        "module tb();\n"
        "endmodule\n";

    QTest::newRow("Ports and parameters") <<
        "// File header for half_adder.\n"
        "module half_adder(\n"        
        "    // inputs:\n"
        "    input bit1,\n"
        "    input bit2,\n"
        "    // outputs:\n"
        "    output sum,\n"
        "    output carry\n"
        ");\n"
        "    xor(sum, bit1, bit2);\n"
        "    and(carry, bit1, bit2);\n"
        "endmodule\n"
        "\n"
        <<
        "module half_adder(\n"        
        "    // inputs:\n"
        "    input bit1,\n"
        "    input bit2,\n"
        "    // outputs:\n"
        "    output sum,\n"
        "    output carry\n"
        ");\n"
        "    xor(sum, bit1, bit2);\n"
        "    and(carry, bit1, bit2);\n"
        "endmodule\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::verifySectionFontColorIs()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::verifySectionFontColorIs(int startIndex, int endIndex, QColor const& expectedFontColor)
{
    QTextCursor cursor = displayEditor_.textCursor();

    int sectionLength = endIndex - startIndex;

    for (int i = 0; i < sectionLength; i++)
    {
        cursor.setPosition(startIndex + i);
        QColor fontColor = cursor.charFormat().foreground().color();
        
        if (fontColor != expectedFontColor)
        {
            QString errorMessage("Font color is " + fontColor.name() +" not " + expectedFontColor.name() + 
                " as expected after '" + displayEditor_.toPlainText().mid(startIndex, i) + "'");
            QFAIL(errorMessage.toLocal8Bit());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testModelNameAndEnvironmentIsImportedToView()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testModelNameAndEnvironmentIsImportedToView()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, modelName);

    runParser(fileContent);

    QVERIFY2(importComponent_->hasView("flat"), "No view 'flat' found in component.");

    QCOMPARE(importComponent_->getViews().first()->getModelName(), modelName);
    QCOMPARE(importComponent_->getViews().first()->getLanguage(), QString("verilog"));
    QCOMPARE(importComponent_->getViews().first()->getEnvIdentifiers().first(), QString("verilog:Kactus2:"));

    verifyDeclarationIsHighlighted(fileContent.lastIndexOf(modelName), modelName.length(), ImportColors::VIEWNAME);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testModelNameAndEnvironmentIsImportedToView_data()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testModelNameAndEnvironmentIsImportedToView_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("modelName");

    QTest::newRow("Empty module") <<
        "module basic();\n"
        "endmodule\n"
        <<
        "basic";
    
    QTest::newRow("Module without port brackets") <<
        "module tb;\n"
        "endmodule\n"
        <<
        "tb";

    QTest::newRow("Ports and parameters") <<
        "// File header for half_adder.\n"
        "module half_adder(\n"        
        "    // inputs:\n"
        "    input bit1,\n"
        "    input bit2,\n"
        "    // outputs:\n"
        "    output sum,\n"
        "    output carry\n"
        ");\n"
        "    xor(sum, bit1, bit2);\n"
        "    and(carry, bit1, bit2);\n"
        "endmodule\n"
        "\n"
        <<
        "half_adder";
}


QTEST_MAIN(tst_VerilogImporter)

#include "tst_VerilogImporter.moc"
