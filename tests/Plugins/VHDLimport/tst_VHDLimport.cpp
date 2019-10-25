//-----------------------------------------------------------------------------
// File: tst_VHDLimport.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.09.2014
//
// Description:
// Unit test for class VHDLimport.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <Plugins/VHDLimport/VHDLimport.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <wizards/ComponentWizard/ImportEditor/ImportHighlighter.h>

#include <QPlainTextEdit>
#include <QSharedPointer>
#include <common/NameGenerationPolicy.h>


Q_DECLARE_METATYPE(QSharedPointer<ModuleParameter>)

class tst_VHDLimport : public QObject
{
    Q_OBJECT

public:
    tst_VHDLimport();

private slots:
    void init();
    void cleanup();

    //! Testcases:
    void testSupportedFileTypes();

    void nothingParsedFromMalformedEntity();
    void nothingParsedFromMalformedEntity_data();

    void testEntityIsFound();
    void testEntityIsFound_data();
    
    void testPortIsHighlighted();
    void testPortIsHighlighted_data();

    void testGenericIsHighlighted();
    void testGenericIsHighlighted_data();

    void testFontInsideEntityIsBlack();
    void testFontInsideEntityIsBlack_data();

    void testModelParameterIsAssignedToPort();
    void testModelParameterIsAssignedToPort_data();
    void testModelParameterAsPortBoundary();

    void testModelParameterIsAssignedToModelParameter();

    void testPortsAndModelParametersAreNotParsedOutsideEntity();
     
    void testParameterNotFoundInFileIsRemoved();
    void testExistingModelParameterIdDoesNotChange();

    void testExistingPortIsSetAsPhantom();
    void testExistingPortIsOverriden();

    void testModelNameAndEnvironmentIsImportedToView();
    void testArchitecturePrecedesConfigurationForModelName();
    void testConfigurationIsImportedToViewIfNoArchitectureAvailable();

    void testMultipleEntities();
    void testMultipleEntities_data();

private:

    void runParser(QString& input, int const& entityNumber = 0);

    QString getEntityDeclaration(QString const& input, int const& entityNumber);

    QPlainTextEdit displayEditor_;

    VHDLimport parser_;
    
    ImportHighlighter* highlighter_;

    QSharedPointer<Component> importComponent_;

    QSignalSpy* createdPorts_;
    QSignalSpy* createdGenerics_;

    QSignalSpy* removedPorts_;
    QSignalSpy* removedGenerics_;
    
    void verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
        const int declarationLength, QColor const& highlightColor) const;

    void verifyDeclarationIsHighlighted(const int declarationStartIndex, const int 
        declarationLength, QColor const& expectedHighlight) const;

    void verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, QColor const& highlightColor);

    void verifySectionFontColorIs(int startIndex, int endIndex, QColor const& expectedFontColor);
   

};

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::tst_VHDLimport()
//-----------------------------------------------------------------------------
tst_VHDLimport::tst_VHDLimport(): displayEditor_(), parser_(), 
    highlighter_(new ImportHighlighter(&displayEditor_, this)),
    createdPorts_(0), createdGenerics_(0), importComponent_(0)
{
    qRegisterMetaType<QSharedPointer<Port> >();
    qRegisterMetaType<QSharedPointer<ModuleParameter> >();

    parser_.setHighlighter(highlighter_);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::init()
//-----------------------------------------------------------------------------
void tst_VHDLimport::init()
{
    importComponent_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::cleanup()
//-----------------------------------------------------------------------------
void tst_VHDLimport::cleanup()
{
 
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testSupportedFileTypes()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testSupportedFileTypes()
{
    QStringList supportedTypes = parser_.getSupportedFileTypes();

    QVERIFY(supportedTypes.contains("vhdlSource"));
    QVERIFY(supportedTypes.contains("vhdlSource-93"));
    QVERIFY(supportedTypes.contains("vhdlSource-87"));
    QCOMPARE(supportedTypes.count(), 3);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::nothingParsedFromMalformedEntity()
//-----------------------------------------------------------------------------
void tst_VHDLimport::nothingParsedFromMalformedEntity()
{
    QFETCH(QString, fileContent);

    runParser(fileContent);

	QVERIFY(!importComponent_->hasPorts());
	QCOMPARE(importComponent_->getComponentInstantiations()->size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::nothingParsedFromMalformedEntity_data()
//-----------------------------------------------------------------------------
void tst_VHDLimport::nothingParsedFromMalformedEntity_data()
{
    QTest::addColumn<QString>("fileContent");

    QTest::newRow("empty file") << "";

    QTest::newRow("no entity defined") << 
        "architecture rtl of empty is\n"
        "begin\n"
        "end;";

    /*QTest::newRow("no begin of ports") << 
        "entity test is"
        "   clk: in std_logic\n"
        ");\n"
        "end test;";*/

    QTest::newRow("no closing bracket for ports") << 
        "entity noPortBracket is"
        "port (\n"
        "   clk: in std_logic\n"
        "-- no closing bracket after ports.\n"
        "end noPortBracket;";

    /*QTest::newRow("no begin of generics") << 
        "entity test is"
        "   local_memory_addr_bits  : integer\n"
        ");\n"
        "end test;";*/

    QTest::newRow("no closing bracket for generics") << 
        "entity noGenericBracket is"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        "-- no closing bracket after generics.\n"
        "end noGenericBracket;";

    QTest::newRow("no end for entity") << 
        "entity noBracket is"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        ");\n"
        "port (\n"
        "   clk: in std_logic\n"
        ");";
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testEntityIsFound()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testEntityIsFound()
{
    QFETCH(QString, fileContent);

    runParser(fileContent);

    QCOMPARE(importComponent_->getComponentInstantiations()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testEntityIsFound_data()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testEntityIsFound_data()
{
    QTest::addColumn<QString>("fileContent");

    QTest::newRow("empty entity") << "entity emtpyEntity is\n"
        "end emtpyEntity;\n";

    QTest::newRow("No name in end declaration") << "entity testEntity is\n"
        "port (\n"
        "   clk: in std_logic\n"
        ");\n"
        "end entity;\n";

    QTest::newRow("No keyword entity in end declaration") << "entity testEntity is\n"
        "port (\n"
        "   clk: in std_logic\n"
        ");\n"
        "end testEntity;\n";

    QTest::newRow("No name or keyword entity in end declaration") << "entity testEntity is\n"
        "port (\n"
        "   clk: in std_logic\n"
        ");\n"
        "end;\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::runParser()
//-----------------------------------------------------------------------------
void tst_VHDLimport::runParser(QString& input, int const& selectedComponent /* = 0 */)
{
    QString inspect = input.replace("\r\n", "\n");

    QString entityDeclaration = getEntityDeclaration(inspect, selectedComponent);

    displayEditor_.setPlainText(inspect);
    parser_.import(inspect, entityDeclaration, importComponent_);
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getEntityDeclaration()
//-----------------------------------------------------------------------------
QString tst_VHDLimport::getEntityDeclaration(QString const& input, int const& entityNumber)
{
    QStringList fileEntities;

    QRegularExpression entityExpression("ENTITY\\s+(\\w+)\\s+IS\\s+.*END(?:\\s+ENTITY)?(?:\\s+\\1)?\\s*[;]",
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator matchIterator = entityExpression.globalMatch(input);
    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        if (match.hasMatch())
        {
            fileEntities.append(match.captured());
        }
    }

    if (!fileEntities.isEmpty() && fileEntities.size() > entityNumber)
    {
        return fileEntities.at(entityNumber);
    }
    else
    {
        return input;
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testPortIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, portDeclaration);

    runParser(fileContent);

    int begin = fileContent.indexOf(portDeclaration);
        
    verifyNotHighlightedBeforeDeclaration(begin, ImportColors::PORT);
    verifyDeclarationIsHighlighted(begin, portDeclaration.length(), ImportColors::PORT);
    verifyNotHighlightedAfterDeclartion(begin, portDeclaration.length(), ImportColors::PORT);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testPortIsHighlighted_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("portDeclaration");
    
    QTest::newRow("simplest possible port declaration") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic\n"
        "   );\n"
        "end test;"
        << "clk : in std_logic";

    QTest::newRow("port declaration with default value") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic := '1'\n"
        "   );\n"
        "end test;"
        << "clk : in std_logic := '1'";

    QTest::newRow("port declaration with description") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic -- Clk from top\n"
        "   );\n"
        "end test;"
        << "clk : in std_logic -- Clk from top";

    QTest::newRow("port declaration followed by another port") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic; -- Clk from top\n"
        "        enable : in std_logic\n"
        "   );\n"
        "end test;"
        << "clk : in std_logic; -- Clk from top";

    QTest::newRow("port declaration with multiple ports") <<
        "entity test is\n"
        "    port (\n"
        "        clk, enable : in std_logic -- One bit ports\n"
        "   );\n"
        "end test;"
        << "clk, enable : in std_logic -- One bit ports";

    QTest::newRow("name type, direction and default value on separate lines") << 
        "entity test is\n"
        "port (clk\n"
        ":\n"
        "in\n"
        "std_logic := '1' -- Clk from top.\n"
        ");\n"
        "end test;"
        << 
        "clk\n"
        ":\n"
        "in\n"
        "std_logic := '1' -- Clk from top.";

    QTest::newRow("Double comment line") << 
        "entity test is\n"
        "    port ( \n"
        "        clk       : IN     std_logic;\n"
        "      --first comment.\n"
        "      --second comment.\n"
        "      -- third comment.\n"
        "        rst_n     : IN     std_logic\n"
        "    );\n"
        "end test;"
        << 
        "clk       : IN     std_logic";
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::verifyNotHighlightedBeforeDeclaration()
//-----------------------------------------------------------------------------
void tst_VHDLimport::verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, QColor const& highlightColor)
{
    QTextCursor cursor = displayEditor_.textCursor();
    cursor.setPosition(declarationStartIndex);

    QVERIFY2(cursor.charFormat().background().color() != highlightColor, 
        "Highlight for imported declaration should not begin before declaration.");
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::verifyDeclarationIsHighlighed()
//-----------------------------------------------------------------------------
void tst_VHDLimport::verifyDeclarationIsHighlighted(const int declarationStartIndex, 
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
// Function: tst_VHDLimport::verifyNotHighlightedAfterDeclartion()
//-----------------------------------------------------------------------------
void tst_VHDLimport::verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
    const int declarationLength, QColor const& highlightColor) const
{
    QTextCursor cursor = displayEditor_.textCursor();

    cursor.setPosition(declarationStartIndex + declarationLength + 1);
    QVERIFY2(cursor.charFormat().background().color() != highlightColor,
        "Highlight for imported declaration should end after declaration.");
}


//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testGenericIsHighlighted()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testGenericIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(int, declarationStartIndex);
    QFETCH(int, declarationLength);

    runParser(fileContent);

    verifyNotHighlightedBeforeDeclaration(declarationStartIndex, ImportColors::MODELPARAMETER);
    verifyDeclarationIsHighlighted(declarationStartIndex, declarationLength, ImportColors::MODELPARAMETER);
    verifyNotHighlightedAfterDeclartion(declarationStartIndex, declarationLength, ImportColors::MODELPARAMETER);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testGenericIsHighlighted_data()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testGenericIsHighlighted_data()
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
// Function: tst_VHDLimport::testFontInsideEntityIsBlackAndOutsideEntityGray()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testFontInsideEntityIsBlack()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, entity);

    runParser(fileContent);

    int entityBegin = fileContent.indexOf(entity);
    int entityEnd = fileContent.indexOf(entity) + entity.length();

    verifySectionFontColorIs(entityBegin, entityEnd, QColor("black"));
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testFontInsideEntityIsBlackAndOutsideEntityGray_data()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testFontInsideEntityIsBlack_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("entity");

    QTest::newRow("Library usage and empty entity") <<
        "-- Comment on first line.\n"
        "library IEEE;\n"
        "ENTITY test IS\n"        
        "end test;\n"
        "begin rtl of test\n"
        <<  
        "ENTITY test IS\n"        
        "end test;\n";

    QTest::newRow("Windows newline") <<
        "-- Comment.\r\n"
        "entity test is\r\n"
        "end test;\r\n"
        "begin rtl of test\r\n"
        "end rtl;\r\n"
        << 
        "entity test is\n"
        "end test;\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::verifySectionFontColorIs()
//-----------------------------------------------------------------------------
void tst_VHDLimport::verifySectionFontColorIs(int startIndex, int endIndex, QColor const& expectedFontColor)
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

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testModelParameterIsAssignedToPort()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testModelParameterIsAssignedToPort()
{
    QFETCH(QString, fileContent);
    QFETCH(int, expectedParameterCount);
    QFETCH(QString, expectedLeft);
    QFETCH(QString, expectedRight);

    runParser(fileContent);

    QCOMPARE(importComponent_->getPorts()->count(), 1);
    QSharedPointer<Port> createdPort = importComponent_->getPorts()->first();

    QCOMPARE(importComponent_->getModel()->getComponentInstantiations()->count(), 1);
    QCOMPARE(importComponent_->getModel()->getComponentInstantiations()->first()->getModuleParameters()->count(), 
        expectedParameterCount);
    QCOMPARE(importComponent_->getParameters()->count(), expectedParameterCount);


    unsigned int i = 1;
    for (auto parameter : *importComponent_->getParameters())
    {
        QString pattern("$" + QString::number(i));
        expectedLeft.replace(pattern, parameter->getValueId());
        expectedRight.replace(pattern, parameter->getValueId());
        ++i;
    }

    QCOMPARE(createdPort->getLeftBound(), expectedLeft);
    QCOMPARE(createdPort->getRightBound(), expectedRight);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testModelParameterIsAssignedToPort_data()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testModelParameterIsAssignedToPort_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("expectedParameterCount");
    QTest::addColumn<QString>("expectedLeft");
    QTest::addColumn<QString>("expectedRight");

    QTest::newRow("Parametrized port width.") <<
        "entity test is\n"
        "   generic (\n"
        "       dataWidth_g : integer := 31\n"
        "   );\n"
        "   port (\n"
        "       data : out std_logic_vector(dataWidth_g downto 0)\n"
        "   );\n"
        "end test;"
        << 1 << "$1" << "0" ;

    QTest::newRow("Parametrized port width with subtraction.") <<
        "entity test is\n"
        "   generic (\n"
        "       dataWidth_g : integer := 16\n"
        "   );\n"
        "   port (\n"
        "       data : out std_logic_vector(dataWidth_g-1 downto 0)\n"
        "   );\n"
        "end test;"
        << 1 << "$1-1" << "0" ;

    QTest::newRow("Parametrized port width with equation of generics.") <<
        "entity test is\n"
        "   generic (\n"        
        "       addrWidth_g : integer := 8;\n"
        "       dataWidth_g : integer := 16\n"
        "   );\n"
        "   port (\n"
        "       data : out std_logic_vector(dataWidth_g+addrWidth_g-1 downto 0)\n"
        "   );\n"
        "end test;"
        << 2 << "$2+$1-1" << "0";

    QTest::newRow("Parametrized port width with parentheses.") <<
        "entity test is\n"
        "   generic (\n"
        "       addrWidth_g : integer := 8;\n"
        "       dataWidth_g : integer := 16\n"
        "   );\n"
        "   port (\n"
        "       data : out std_logic_vector((dataWidth_g/2)*addrWidth_g DOWNTO 0)\n"        
        "   );\n"
        "end test;"
        << 2 << "($2/2)*$1" << "0";

    QTest::newRow("Parametrized port width with power of two.") <<
        "entity test is\n"
        "   generic (\n"
        "       max_g : integer := 8\n"
        "   );\n"
        "   port (\n"
        "       data : out std_logic_vector(max_g**2-1 downto 0)\n"
        "   );\n"
        "end test;"
        << 1 << "$1**2-1" << "0" ;
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testModelParameterAsPortBoundary()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testModelParameterAsPortBoundary()
{
    QString fileContent = 
        "entity test is\n"
        "   generic (\n"        
        "       dataWidth_g : integer := 8\n"
        "   );\n"
        "   port (\n"
        "       data : out std_logic_vector(dataWidth_g-1 downto 0)\n"
        "   );\n"
        "end test;";

    runParser(fileContent);

	QSharedPointer<Port> createdPort = importComponent_->getPorts()->first();

	QSharedPointer<ComponentInstantiation> componentInstantiation = importComponent_->
		getComponentInstantiations()->first();
	QSharedPointer<ModuleParameter> createdGeneric = componentInstantiation->getModuleParameters()->first();
    QSharedPointer<Parameter> equivalentParameter = importComponent_->getParameters()->first();

    QCOMPARE(createdGeneric->getValue(), equivalentParameter->getValueId());
	QCOMPARE(createdPort->getLeftBound(), QString("%1-1").arg(equivalentParameter->getValueId()));
	QCOMPARE(createdPort->getRightBound(), QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testModelParameterIsAssignedToModelParameter()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testModelParameterIsAssignedToModelParameter()
{
    QString fileContent(
        "entity test is\n"
        "   generic (\n"
        "       width_g : integer := 8;\n"
        "       dataWidth_g : integer := width_g\n"        
        "   );\n"
        "end test;");

    runParser(fileContent);

    QVERIFY2(!importComponent_->getComponentInstantiations()->isEmpty(), "Did not create a component instantiation.");

    //! Check value reference chain dataWidth_g (MP) -> dataWidth_g (P) -> width_g (P).
    QSharedPointer<ModuleParameter> createdGeneric = 
        importComponent_->getComponentInstantiations()->first()->getModuleParameters()->last();
    QCOMPARE(createdGeneric->getValue(), importComponent_->getParameters()->last()->getValueId());
    QCOMPARE(importComponent_->getParameters()->last()->getValue(), 
        importComponent_->getParameters()->first()->getValueId());
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testPortsAndModelParametersAreNotParsedOutsideEntity()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testPortsAndModelParametersAreNotParsedOutsideEntity()
{
    QString fileContent(
        "entity empty is\n"
        "end empty;\n"
        "\n"
        "architecture structural of empty is\n"
        "\n"
        "component dut"
        "   generic (\n"        
        "       local_memory_addr_bits  : integer := 12\n"
        "   );\n"
        "   port (\n"
        "       clk : in std_logic\n"
        "   );\n"
        "end dut;\n"
        "\n"
        "end structural;\n");

    runParser(fileContent);

    QCOMPARE(importComponent_->getPorts()->count(), 0);
    QCOMPARE(importComponent_->getComponentInstantiations()->first()->getModuleParameters()->count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testParameterNotFoundInFileIsRemoved()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testParameterNotFoundInFileIsRemoved()
{
    QSharedPointer<ModuleParameter> existingParameter(new ModuleParameter());
    existingParameter->setName("oldParameter");

	QSharedPointer<ComponentInstantiation> importComponentInstantiation( new ComponentInstantiation );
	importComponentInstantiation->setName(NameGenerationPolicy::componentInstantiationName("vhdl"));
	importComponentInstantiation->getModuleParameters()->append(existingParameter);
	importComponent_->getModel()->getComponentInstantiations()->append(importComponentInstantiation);

    QString fileContent = 
        "entity test is\n"
        "   generic (\n"        
        "       dataWidth_g : integer := 8\n"
        "   );\n"
        "end test;";

    runParser(fileContent);

    QSharedPointer<ModuleParameter> importedParameter = importComponent_->getComponentInstantiations()
		->first()->getModuleParameters()->first();
    QCOMPARE(importedParameter->name(), QString("dataWidth_g"));
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testExistingModelParameterIdDoesNotChange()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testExistingModelParameterIdDoesNotChange()
{
    QSharedPointer<ModuleParameter> existingParameter(new ModuleParameter());
    existingParameter->setName("dataWidth_g");
    existingParameter->setValueId("existingId");

	QSharedPointer<ComponentInstantiation> importComponentInstantiation( new ComponentInstantiation );
	importComponentInstantiation->setName(NameGenerationPolicy::componentInstantiationName("vhdl"));
	importComponentInstantiation->getModuleParameters()->append(existingParameter);
	importComponent_->getModel()->getComponentInstantiations()->append(importComponentInstantiation);

    QString fileContent = 
        "entity test is\n"
        "   generic (\n"        
        "       dataWidth_g : integer := 8\n"
        "   );\n"
        "end test;";

    runParser(fileContent);

	QSharedPointer<ModuleParameter> importedParameter = importComponent_->getComponentInstantiations()
		->first()->getModuleParameters()->first();
    QCOMPARE(importedParameter->getValueId(), QString("existingId"));
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testExistingPortIsSetAsPhantom()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testExistingPortIsSetAsPhantom()
{
    QSharedPointer<Port> existingPort(new Port("oldPort"));
    existingPort->setDirection(DirectionTypes::IN);
    importComponent_->getPorts()->append(existingPort);

    QString input =  
        "entity test is\n"
        "   port (\n"
        "       newPort : out std_logic\n"
        "   );\n"
        "end test;\n";

    runParser(input);

    QCOMPARE(importComponent_->getPorts()->count(), 2);
    QSharedPointer<Port> importedPort = importComponent_->getPort("oldPort");
    QCOMPARE(importedPort->getDirection(), DirectionTypes::DIRECTION_PHANTOM);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testExistingPortIsOverriden()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testExistingPortIsOverriden()
{
    QSharedPointer<Port> existingPort(new Port("oldPort"));
    existingPort->setDirection(DirectionTypes::OUT);
    existingPort->setTypeDefinition("std_logic", "IEEE.std_logic_1164.all");
    importComponent_->getPorts()->append(existingPort);

    QString input =  
        "entity test is\n"
        "   port (\n"
        "       oldPort : in std_logic\n"
        "   );\n"
        "end test;\n";

    runParser(input);

    QCOMPARE(importComponent_->getPorts()->count(), 1);
    QCOMPARE(importComponent_->getPort("oldPort")->getDirection(), DirectionTypes::IN);
    QCOMPARE(importComponent_->getPort("oldPort")->getTypeDefinition("std_logic"), QString("IEEE.std_logic_1164.all"));
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testModelNameAndEnvironmentIsImportedToView()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testModelNameAndEnvironmentIsImportedToView()
{
    QString fileContent(
        "entity testbench is\n"
        "end entity testbench;\n"
        "\n"
        "ARCHITECTURE structural OF testbench IS\n"
        "\n"
        "component dut"
        "   port (\n"
        "       clk : in std_logic\n"
        "   );\n"
        "END dut;\n"
        "\n"
        "END ARCHITECTURE structural;\n");

    runParser(fileContent);

    QVERIFY2(importComponent_->hasView("flat_vhdl"), "No view 'flat' found in component.");


	QSharedPointer<ComponentInstantiation> cimp = importComponent_->getComponentInstantiations()->first();

    QCOMPARE(cimp->getModuleName(), QString("testbench"));
    QCOMPARE(cimp->getArchitectureName(), QString("structural"));
    QCOMPARE(cimp->getLanguage(), QString("VHDL"));
	QCOMPARE(importComponent_->getViews()->first()->getEnvIdentifiers()->first()->language, QString("VHDL"));
	QCOMPARE(importComponent_->getViews()->first()->getEnvIdentifiers()->first()->tool, QString("Kactus2"));

    QString architecture = "ARCHITECTURE structural OF testbench";
    QString modelNameSection = "structural OF testbench";

    verifySectionFontColorIs(fileContent.indexOf(architecture), architecture.length(), QColor("black"));
    verifyDeclarationIsHighlighted(fileContent.indexOf(modelNameSection), modelNameSection.length(), 
        ImportColors::VIEWNAME);    
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testArchitecturePrecedesConfigurationForModelName()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testArchitecturePrecedesConfigurationForModelName()
{
    QString fileContent(
        "entity testbench is\n"
        "end testbench;\n"
        "\n"
        "ARCHITECTURE structural OF testbench IS\n"
        "\n"
        "component Dut"
        "   port (\n"
        "       clk : in std_logic\n"
        "   );\n"
        "END Dut;\n"
        "\n"
        "END ARCHITECTURE structural;\n"
        "\n"
        "CONFIGURATION behavioral OF testbench is\n"
        "    FOR default_architecture\n"
        "         FOR ALL : Dut USE ENTITY WORK.half_adder(rtl)\n"
        "    END FOR;"
        "end CONFIGURATION behavioral;\n");

    runParser(fileContent);

    QVERIFY2(importComponent_->hasView("flat_vhdl"), "No view 'flat' found in component.");
    QCOMPARE(importComponent_->getComponentInstantiations()->first()->getModuleName(), QString("testbench"));
    QCOMPARE(importComponent_->getComponentInstantiations()->first()->getArchitectureName(), QString("structural"));

    QString architecture = "ARCHITECTURE structural OF testbench IS";
    QString modelNameSection = "structural OF testbench";

    verifySectionFontColorIs(fileContent.indexOf(architecture), architecture.length(), QColor("black"));
    verifyDeclarationIsHighlighted(fileContent.indexOf(modelNameSection), modelNameSection.length(), 
        ImportColors::VIEWNAME);    
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testConfigurationIsImportedToViewIfNoArchitectureAvailable()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testConfigurationIsImportedToViewIfNoArchitectureAvailable()
{
    QString fileContent(
        "entity testbench is\n"
        "end testbench;\n"
        "\n"
        "CONFIGURATION behavioral OF testbench is\n"
        "    FOR default_architecture\n"
        "         FOR ALL : Dut USE ENTITY WORK.half_adder(rtl)\n"
        "    END FOR;"
        "end CONFIGURATION behavioral;\n");

    runParser(fileContent);

    QVERIFY2(importComponent_->hasView("flat_vhdl"), "No view 'flat' found in component.");
    QCOMPARE(importComponent_->getComponentInstantiations()->first()->getModuleName(), QString("behavioral"));

    QString configuration = "CONFIGURATION behavioral OF testbench";
    QString modelNameSection = "behavioral";

    verifySectionFontColorIs(fileContent.indexOf(configuration), configuration.length(), QColor("black"));
    verifyDeclarationIsHighlighted(fileContent.indexOf(modelNameSection), modelNameSection.length(), 
        ImportColors::VIEWNAME);    
}


//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testMultipleEntities()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testMultipleEntities()
{
    QFETCH(QString, fileContent);
    QFETCH(int, entityNumber);
    QFETCH(QString, entityName);

    runParser(fileContent, entityNumber);

    QCOMPARE(importComponent_->getComponentInstantiations()->size(), 1);

    QSharedPointer<ComponentInstantiation> entityInstantiation =
        importComponent_->getComponentInstantiations()->first();

    QCOMPARE(entityInstantiation->getModuleName(), entityName);
}

//-----------------------------------------------------------------------------
// Function: tst_VHDLimport::testMultipleEntities_data()
//-----------------------------------------------------------------------------
void tst_VHDLimport::testMultipleEntities_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("entityNumber");
    QTest::addColumn<QString>("entityName");

    QTest::newRow("select first entity") <<
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
        "end secondEntity;\n" <<
        0 << "firstEntity";

    QTest::newRow("select second entity") <<
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
        "end secondEntity;\n" <<
        1 << "secondEntity";
}

QTEST_MAIN(tst_VHDLimport)

#include "tst_VHDLimport.moc"
