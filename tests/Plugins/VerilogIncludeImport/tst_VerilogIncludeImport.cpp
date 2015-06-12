//-----------------------------------------------------------------------------
// File: tst_VerilogIncludeImport.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.03.2015
//
// Description:
// Unit test for class VerilogIncludeImport.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QPlainTextEdit>

#include <Plugins/VerilogIncludeImport/VerilogIncludeImport.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <wizards/ComponentWizard/ImportEditor/ImportHighlighter.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>

class tst_VerilogIncludeImport : public QObject
{
    Q_OBJECT

public:
    tst_VerilogIncludeImport();

private slots:
    void init();
    void testFileTypes();

    void testNothingIsParsedFromMalformedInput();
    void testNothingIsParsedFromMalformedInput_data();

    void testDefineIsParsed();
    void testDefineIsParsed_data();

    void testMultipleDefinitions();
    void testMultipleDefinitions_data();
    
    void testDefinitionWithArgumentsIsNotParsed();
    void testDefinitionWithArgumentsIsNotParsed_data();
    void testDefineIsHighlighted();
    void testDefineIsHighlighted_data();

    void testWithoutHighlighter();

    void testExistingParameterIsUpdated();

private:
    void runParser(QString const& input);

    void verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, QColor const& highlightColor);

    void verifyDeclarationIsHighlighted(const int declarationStartIndex,
        const int declarationLength, QColor const& expectedHighlight) const;

    void verifyNotHighlightedAfterDeclartion(const int declarationStartIndex,
        const int declarationLength, QColor const& highlightColor) const;

    QSharedPointer<Component> importComponent_;

    QPlainTextEdit displayEditor_;

    ImportHighlighter* highlighter_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::()
//-----------------------------------------------------------------------------
tst_VerilogIncludeImport::tst_VerilogIncludeImport(): importComponent_(0), displayEditor_(), 
    highlighter_(new ImportHighlighter(&displayEditor_, this))
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::init()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::init()
{
    importComponent_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testFileTypes()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testFileTypes()
{
    VerilogIncludeImport importer;

    QStringList supportedFileTypes = importer.getSupportedFileTypes();

    QVERIFY(supportedFileTypes.contains("verilogSource"));
    QVERIFY(supportedFileTypes.contains("verilogSource-95"));
    QVERIFY(supportedFileTypes.contains("verilogSource-2001"));
    QVERIFY(supportedFileTypes.contains("systemVerilogSource"));
    QCOMPARE(supportedFileTypes.count(), 4);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testNothingIsParsedFromMalformedInput()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testNothingIsParsedFromMalformedInput()
{
    QFETCH(QString, input);

    runParser(input);

    QCOMPARE(importComponent_->getModel()->getModelParameters()->count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testNothingIsParsedFromMalformedInput_data()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testNothingIsParsedFromMalformedInput_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("Empty input") << "";
    QTest::newRow("Commented out definition") << "//`define ZERO 0\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testDefineIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testDefineIsParsed()
{
    QFETCH(QString, input);
    QFETCH(QString, expectedName);
    QFETCH(QString, expectedValue);
    QFETCH(QString, expectedDescription);

    runParser(input);

    QCOMPARE(importComponent_->getModel()->getModelParameters()->count(), 1);

    QSharedPointer<ModelParameter> createdParameter = importComponent_->getModel()->getModelParameters()->first();
    QCOMPARE(createdParameter->getName(), expectedName);
    QCOMPARE(createdParameter->getValue(), expectedValue);
    QCOMPARE(createdParameter->getDescription(), expectedDescription);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testDefineIsParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testDefineIsParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedName");
    QTest::addColumn<QString>("expectedValue");
    QTest::addColumn<QString>("expectedDescription");

    QTest::newRow("Name and value") << "`define ZERO 0\n" << "ZERO" << "0" << "";
    QTest::newRow("Value with whitespace") <<  
        "`define BASIC_STRING \"Test string\"\n" << "BASIC_STRING" << "\"Test string\"" << "";
    QTest::newRow("Value on multiple lines") <<  
        "`define LONG_STRING \"Test string \\\n"
                              "spanning multiple \\\n"
                              "lines.\"\n" << "LONG_STRING" << "\"Test string spanning multiple lines.\"" << "";
    QTest::newRow("Define without value gets default value one") << "`define DEFAULT \n" << "DEFAULT" << "1" << "";
    QTest::newRow("Value with trailing comment") <<  
        "`define COMMENTED 1 // Define description is given in comments.\n" 
        << "COMMENTED" << "1" << "Define description is given in comments.";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testMultipleDefinitions()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testMultipleDefinitions()
{
    QFETCH(QString, input);
    QFETCH(int, defineCount);

    runParser(input);

    QCOMPARE(importComponent_->getModel()->getModelParameters()->count(), defineCount);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testMultipleDefinitions_data()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testMultipleDefinitions_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("defineCount");

    QTest::newRow("Definitions on separate lines") << 
        "`define ZERO 0\n"
        "`define ONE 1\n"
        << 2;

    QTest::newRow("Definitions on same line") << 
        "`define ZERO 0 `define ONE 1\n"
        << 1;

    QTest::newRow("Definitions and single-line comments mixed") << 
        "`define ZERO 0\n"
        "// `define ONE 1\n"
        "`define TWO 2\n"
        << 2;

    QTest::newRow("Definitions and multi-line comments mixed") << 
        "`define ZERO 0\n"
        "/* `define ONE 1\n"
        "`define TWO 2\n"
        "*/"
        << 1;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testDefinitionWithArgumentsIsNotParsed()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testDefinitionWithArgumentsIsNotParsed()
{
    QFETCH(QString, input);

    runParser(input);

    QCOMPARE(importComponent_->getModel()->getModelParameters()->count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testDefinitionWithArgumentsIsNotParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testDefinitionWithArgumentsIsNotParsed_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("Simple max macro with arguments") << "`define max(a,b) ((a) > (b) ? (a) : (b))\n\n";
    QTest::newRow("Whitespace between macro name and argument parenthesis") 
        << "`define max (a,b) ((a) > (b) ? (a) : (b))\n\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testDefineIsHighlighted()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testDefineIsHighlighted()
{
    QFETCH(QString, input);
    QFETCH(QString, defineDeclaration);

    runParser(input);

    QVERIFY2(importComponent_->getModel()->getModelParameters()->count() != 0, 
        "No model parameters parsed from input.");

    int begin = input.indexOf(defineDeclaration);

    verifyNotHighlightedBeforeDeclaration(begin, ImportColors::MODELPARAMETER);
    verifyDeclarationIsHighlighted(begin, defineDeclaration.length(), ImportColors::MODELPARAMETER);
    verifyNotHighlightedAfterDeclartion(begin, defineDeclaration.length(), ImportColors::MODELPARAMETER);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testDefineIsHighlighted_data()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testDefineIsHighlighted_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("defineDeclaration");

    QTest::newRow("Name and value") << "`define ZERO 0\n\n" << "`define ZERO 0\n";
    QTest::newRow("Value on multiple lines") <<  
        "`define LONG_STRING \"Test string \\\n"
        "spanning multiple \\\n"
        "lines.\"\n\n" << "`define LONG_STRING \"Test string \\\n"
        "spanning multiple \\\n"
        "lines.\"\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testWithoutHighlighter()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testWithoutHighlighter()
{
    VerilogIncludeImport parser;
    parser.import("`define ZERO 0\n\n", importComponent_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::testExistingParameterIsUpdated()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::testExistingParameterIsUpdated()
{
    QSharedPointer<ModelParameter> existingParameter(new ModelParameter());
    existingParameter->setName("EXISTING");
    existingParameter->setValue("1");
    existingParameter->setDescription("This parameter has already been defined");
    importComponent_->getModel()->getModelParameters()->append(existingParameter);

    runParser("`define EXISTING 2 // This parameter has been updated.\n\n");

    QCOMPARE(importComponent_->getModel()->getModelParameters()->count(), 1);
    QCOMPARE(importComponent_->getModel()->getModelParameters()->first()->getDescription(), 
        QString("This parameter has been updated."));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::runParser()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::runParser(QString const& input)
{
    displayEditor_.setPlainText(input);

    VerilogIncludeImport parser;
    parser.setHighlighter(highlighter_);

    parser.import(input, importComponent_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::verifyNotHighlightedBeforeDeclaration()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, 
    QColor const& highlightColor)
{
    QTextCursor cursor = displayEditor_.textCursor();
    cursor.setPosition(declarationStartIndex);

    if (cursor.atBlockStart())
    {
        cursor.setPosition(qMax(declarationStartIndex - 1, 0));
    }

    QColor appliedColor = cursor.charFormat().background().color();

    QVERIFY2(appliedColor != highlightColor, "Highlight applied before declaration.");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogIncludeImport::verifyDeclarationIsHighlighed()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::verifyDeclarationIsHighlighted(const int declarationStartIndex, 
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
// Function: tst_VerilogIncludeImport::verifyNotHighlightedAfterDeclartion()
//-----------------------------------------------------------------------------
void tst_VerilogIncludeImport::verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
    const int declarationLength, QColor const& highlightColor) const
{
    QTextCursor cursor = displayEditor_.textCursor();
    cursor.setPosition(declarationStartIndex + declarationLength + 1);

    QColor appliedColor = cursor.charFormat().background().color();

    QVERIFY2(appliedColor != highlightColor, "Highlight applied after declaration.");
}

QTEST_MAIN(tst_VerilogIncludeImport)

#include "tst_VerilogIncludeImport.moc"
