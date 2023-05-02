#include <QtTest>

#include <kactusGenerators/DocumentGenerator/documentgenerator.h>

#include <tests/MockObjects/LibraryMock.h>
#include <tests/MockObjects/DesignWidgetFactoryMock.h>
#include <tests/MockObjects/HWDesignWidgetMock.h>

#include <KactusAPI/include/ExpressionFormatterFactoryImplementation.h>

#include <KactusAPI/include/utils.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/RegisterFile.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QWidget>
#include <QList>

class tst_MarkdownGenerator : public QObject
{
    Q_OBJECT

public:
    tst_MarkdownGenerator();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testInvalidVlnvInConstructor();

    void testFileHeaderIsWritten();
    void testTableOfContentsIsWrittenWithOnlyTopComponent();

    void testAttributesWrittenWithOnlyTopComponent();
    void testParametersWrittenWithOnlyTopComponent();

    void testMemoryMapsWrittenWithTopComponent();
    void testAddressBlocksWrittenWithTopComponent();
    void testExpressionsInAddressBlocks();
    void testRegistersWrittenWithTopComponent();
    void testAddressBlockRegisterFilesWrittenWithTopComponent();
    void testFieldsWrittenWithTopComponent();
    void testMemoryMapToFieldWrittenWithTopComponent();

    void testPortsWrittenWithOnlyTopComponent();

    void testBusInterfacesWrittenWithoutPorts();

    void testFileSetsWrittenForTopComponent();

    void testViewsWrittenForTopComponent();

    void testDesignIsWritten();

    void testEndOfDocumentWrittenForTopComponent();

private:
    /*!
        *  Create the generator used in most test cases.
        */
    DocumentGenerator* createTestGenerator();

    /*!
        *  Read the output file of the generator.
        */
    void readOutputFile();

    /*!
        *  Compare expected output to the actual output
        */
    void checkOutputFile(QString const& expectedOutput);

    /*!
        *  Create a parameter used in the tests.
        *
        *      @param [in] name            Name of the parameter.
        *      @param [in] value           Value of the parameter.
        *      @param [in] description     Description of the parameter.
        *      @param [in] uuID            Id of the parameter.
        */
    QSharedPointer<Parameter> createTestParameter(QString const& name, QString const& value,
        QString const& description, QString const& uuID, QString const& arrayLeft, QString const& arrayRight);

    /*!
        *  Create a port used in the tests.
        *
        *      @param [in] name            Name of the port.
        *      @param [in] leftBound       The left bound of the port.
        *      @param [in] rightBound      The right bound of the port.
        *      @param [in] defaultValue    The default value of the port.
        */
    QSharedPointer<Port> createTestPort(QString const& name, QString const& leftBound, QString const& rightBound,
        QString const& defaultValue, QString const& arrayLeft, QString const& arrayRight);

    /*!
        *  Create a map for configurable element values.
        *
        *      @param [in] component   The component, whose configurable element values are being created.
        */
    QList<QSharedPointer<ConfigurableElementValue> > createConfigurableElementvalues(QSharedPointer <Component> component);

    QSharedPointer<Field> createTestField(QString const& name, QString const& description, QString const& offset,
        QString const& width);

    QSharedPointer<Register> createTestRegister(QString const& name, QString const& offset, QString const& size,
        QString const& dimension, QString const& description);

    QSharedPointer<AddressBlock> createTestAddressBlock(QString const& name, QString const& description,
        QString const& baseAddress, QString const& range, QString const& width,
        QList <QSharedPointer <Register> > registers);

    QSharedPointer<AddressBlock> createAddressBlockWithRegisterData(QString const& name, QString const& description,
        QString const& baseAddress, QString const& range, QString const& width,
        QList <QSharedPointer <RegisterBase> > registers);

    QSharedPointer<MemoryMap> createTestMemoryMap(QString const& name, QString const& description,
        int addressUnitbits, QList<QSharedPointer <AddressBlock> > addressBlocks);

    //! Get the string used to describe a space.
    QString getSpaceString();

    //! Get the string used to describe a table.
    QString getTableString();

    //! Get the string used to describe the encoding.
    QString getEncodingString();

    //! Get the string used to describe the document type.
    QString getDoctypeString();

    //! Get the string used to describe valid w3c strict.
    QString getValidW3CStrictString();

    QSharedPointer<Component> topComponent_;

    VLNV topComponentVlnv_;

    QString output_;

    QDateTime generationTime_;

    LibraryMock library_;

    DesignWidgetFactoryMock designWidgetFactory_;

    ExpressionFormatterFactoryImplementation expressionFormatterFactory_;

    QString targetPath_;

    QWidget* generatorParentWidget_;
};

tst_MarkdownGenerator::tst_MarkdownGenerator() :
    topComponent_(),
    topComponentVlnv_(),
    output_(),
    generationTime_(),
    library_(this),
    designWidgetFactory_(&library_),
    expressionFormatterFactory_(),
    targetPath_("./generatorOutput.md"),
    generatorParentWidget_(new QWidget)
{
}

void tst_MarkdownGenerator::initTestCase()
{
    topComponentVlnv_ = VLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
}

void tst_MarkdownGenerator::cleanupTestCase()
{
}

void tst_MarkdownGenerator::init()
{
    topComponent_ = QSharedPointer<Component>(new Component(topComponentVlnv_));
    library_.clear();

    QVERIFY(!QFile::exists(targetPath_));
}

void tst_MarkdownGenerator::cleanup()
{
    topComponent_.clear();
    output_.clear();

    QFile::remove(targetPath_);
}

void tst_MarkdownGenerator::testInvalidVlnvInConstructor()
{
    VLNV invalidVlnv(VLNV::COMPONENT, "invalid", "library", "component", "0");
    
    DocumentGenerator generator(&library_, invalidVlnv, &designWidgetFactory_,
        &expressionFormatterFactory_, 1, generatorParentWidget_);
    
    QSignalSpy spy(&generator, SIGNAL(errorMessage(QString const&)));
    
    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);
    
    generator.writeDocumentation(stream, targetPath_);
    
    targetFile.close();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toString(), QString("VLNV was not found in the library."));
}

void tst_MarkdownGenerator::testFileHeaderIsWritten()
{
    QCoreApplication::setOrganizationName("TUT");
    QCoreApplication::setApplicationName("TestRunner");
    QSettings settings;
    settings.setValue("General/Username", "testUser");

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generationTime_ = QDateTime::currentDateTime();

    generator->writeHeader(stream);
    targetFile.close();

    QString expectedOutput(
        "###### This document was generated by Kactus2 on " +
        generationTime_.toString("dd.MM.yyyy hh:mm:ss") + " by user testUser  \n"
        "\n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testTableOfContentsIsWrittenWithOnlyTopComponent()
{
    QList <QSharedPointer<Parameter> > componentParameters;
    QSharedPointer<Parameter> parameter = createTestParameter("parameter", "1", "", "P-ID", "", "");
    componentParameters.append(parameter);

    QList <QSharedPointer <Register> > registers;
    QSharedPointer <Register> testRegister = createTestRegister("register", "1", "1", "1", "");
    QSharedPointer <Field> testField = createTestField("field", "", "1", "8");
    testRegister->getFields()->append(testField);
    registers.append(testRegister);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> testAddressBlock = createTestAddressBlock("addressBlock", "", "'h0", "4", "32",
        registers);
    addressBlocks.append(testAddressBlock);

    QList <QSharedPointer <MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> testMemoryMap = createTestMemoryMap("memoryMap", "", 8, addressBlocks);
    memoryMaps.append(testMemoryMap);

    QSharedPointer<Port> port = createTestPort("port", "10", "1", "", "", "");

    QSharedPointer<BusInterface> busInterface(new BusInterface());
    busInterface->setName("busInterface");

    QSharedPointer<FileSet> fileset(new FileSet("fileSet"));

    QSharedPointer<View> view(new View("view"));

    topComponent_->getParameters()->append(componentParameters);
    topComponent_->getPorts()->append(port);
    topComponent_->getBusInterfaces()->append(busInterface);
    topComponent_->getFileSets()->append(fileset);
    topComponent_->getViews()->append(view);
    topComponent_->getMemoryMaps()->append(memoryMaps);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    unsigned int runningNumber = 0;

    generator->writeTableOfContents(stream);

    targetFile.close();
    
    QString expectedOutput(
        "1. [Component " + topComponent_->getVlnv().getVendor() + " - " + topComponent_->getVlnv().getLibrary() +
        " - " + topComponent_->getVlnv().getName() + " - " + topComponent_->getVlnv().getVersion() + "]" +
        "(#" + topComponent_->getVlnv().toString() + ")  " + "\n"

        "\t" + "1.1. [Kactus2 attributes](#" + topComponent_->getVlnv().toString() + ".attributes)" + "  " + "\n" +
        "\t" + "1.2. [General parameters](#" + topComponent_->getVlnv().toString() + ".parameters)" + "  " + "\n" +
        "\t" + "1.3. [Memory maps](#" + topComponent_->getVlnv().toString() + ".memoryMaps)" + "  " + "\n" +
        "\t" + "1.4. [Ports](#" + topComponent_->getVlnv().toString() + ".ports)" + "  " + "\n" +
        "\t" + "1.5. [Bus interfaces](#" + topComponent_->getVlnv().toString() + ".interfaces)" + "  " + "\n" +
        "\t" + "1.6. [File sets](#" + topComponent_->getVlnv().toString() + ".fileSets)" + "  " + "\n" +
        "\t" + "1.7. [Views](#" + topComponent_->getVlnv().toString() + ".views)" + "  " + "\n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testAttributesWrittenWithOnlyTopComponent()
{
    topComponent_->setHierarchy(KactusAttribute::FLAT);
    topComponent_->setImplementation(KactusAttribute::SW);
    topComponent_->setFirmness(KactusAttribute::FIXED);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    int subHeaderNumber = 1;

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generator->writeKactusAttributes(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "## 1.1 Kactus2 attributes <a id=\"" + topComponent_->getVlnv().toString() + ".attributes\">  \n"
        "\n"
        "**Product hierarchy:** Flat  \n"
        "**Component implementation:** SW  \n"
        "**Component firmness:** Fixed  \n"
        "\n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testParametersWrittenWithOnlyTopComponent()
{
    QSharedPointer <Parameter> parameter = createTestParameter("parameter", "10", "Description", "ID_parameter",
        "1", "0");
    QSharedPointer <Parameter> refParameter = createTestParameter("refParameter", "ID_parameter", "Describe this",
        "ID-refer", "", "");

    topComponent_->getParameters()->append(parameter);
    topComponent_->getParameters()->append(refParameter);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 2;
    
    generator->writeParameters(stream, subHeaderNumber);

    targetFile.close();

    QString vlnvString(topComponent_->getVlnv().toString());

    QString expectedOutput(
        "## 1.2 General parameters <a id=\"" + vlnvString + ".parameters\">  \n"
        "\n"
        "|Name|Type|Value|Resolve|Bit vector left|Bit vector right|Array left|Array right|Description|  \n"
        "|:----|:----|:----|:----|:----|:----|:----|:----|:----|  \n"
        "|" + parameter->name() + "|" + parameter->getType() + "|" +
            parameter->getValue() + "|" + parameter->getValueResolve()  + "|" +
            parameter->getVectorLeft() + "|" + parameter->getVectorRight() + "|" +
            parameter->getArrayLeft() + "|" + parameter->getArrayRight() + "|" +
            parameter->description() + "|  \n"
        "|" + refParameter->name() + "|" + "" + "|" +
            "parameter" + "|" + "" + "|" +
            "" + "|" + "" + "|" +
            "" + "|" + "" + "|" +
            refParameter->description() + "|  \n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testMemoryMapsWrittenWithTopComponent()
{
    QList <QSharedPointer <AddressBlock> > addressBlocks;

    QSharedPointer<MemoryMap> memoryMap = createTestMemoryMap("memoryMap", "example Description", 8, addressBlocks);

    topComponent_->getMemoryMaps()->append(memoryMap);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeMemoryMaps(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "## 1.1 Memory maps <a id=\"" + topComponent_->getVlnv().toString() + ".memoryMaps\">  \n"
        "\n"
        "### 1.1.1 Memory map " + memoryMap->name() + "  \n"
        "\n"
        "**Description:** " + memoryMap->description() + "  \n"
        "\n"
        "**Address unit bits (AUB):** " + memoryMap->getAddressUnitBits() + "  \n"
        "\n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testAddressBlocksWrittenWithTopComponent()
{
    QList <QSharedPointer <Register> > registers;
    QSharedPointer <Register> testRegister = createTestRegister("register", "4", "2", "2", "");
    QSharedPointer <Register> testRegister2 = createTestRegister("register2", "8", "16", "2", "");
    registers.append(testRegister);
    registers.append(testRegister2);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> testAddressBlock = createTestAddressBlock("addressBlock", "example", "'h0", "4",
        "32", registers);
    addressBlocks.append(testAddressBlock);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeAddressBlocks(addressBlocks, stream, subHeaderNumber, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "### 1.1.1.1 Address block " + testAddressBlock->name() + "  \n"
        "\n"
        "**Description:** " + testAddressBlock->description() + "  \n"
        "**Usage:** " + General::usage2Str(testAddressBlock->getUsage()) + "  \n"
        "**Base address [AUB]:** " + testAddressBlock->getBaseAddress() + "  \n"
        "**Range [AUB]:** " + testAddressBlock->getRange() + "  \n"
        "**Width [AUB]:** " + testAddressBlock->getWidth() + "  \n"
        "**Access:** " + AccessTypes::access2Str(testAddressBlock->getAccess()) + "  \n"
        "**Volatile:** " + testAddressBlock->getVolatile() + "  \n"
        "\n"
        "#### Address block '" + testAddressBlock->name() + "' contains the following registers:  \n"
        "\n"
        "|Register name|Offset [AUB]|Size [bits]|Dimension|Volatile|Access|  \n"
        "|:----|:----|:----|:----|:----|:----|  \n"
        "|" + testRegister->name() +
            "|" + testRegister->getAddressOffset() +
            "|" + testRegister->getSize() +
            "|" + testRegister->getDimension() +
            "|" + testRegister->getVolatile() +
            "|" + AccessTypes::access2Str(testRegister->getAccess()) + "|  \n"
        "|" + testRegister2->name() +
            "|" + testRegister2->getAddressOffset() +
            "|" + testRegister2->getSize() +
            "|" + testRegister2->getDimension() +
            "|" + testRegister2->getVolatile() +
            "|" + AccessTypes::access2Str(testRegister2->getAccess()) + "|  \n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testExpressionsInAddressBlocks()
{
    QSharedPointer <Parameter> targetParameter = createTestParameter("target", "4", "", "ID_TARGET", "", "");

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> testAddressBlock = createTestAddressBlock("addressBlock", "example", "'h0",
        "ID_TARGET", "ID_TARGET + 2", QList <QSharedPointer <Register> >());
    addressBlocks.append(testAddressBlock);

    topComponent_->getParameters()->append(targetParameter);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeAddressBlocks(addressBlocks, stream, subHeaderNumber, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "### 1.1.1.1 Address block " + testAddressBlock->name() + "  \n"
        "\n"
        "**Description:** " + testAddressBlock->description() + "  \n"
        "**Usage:** " + General::usage2Str(testAddressBlock->getUsage()) + "  \n"
        "**Base address [AUB]:** 'h0  \n"
        "**Range [AUB]:** target  \n"
        "**Width [AUB]:** target + 2  \n"
        "**Access:** " + AccessTypes::access2Str(testAddressBlock->getAccess()) + "  \n"
        "**Volatile:** " + testAddressBlock->getVolatile() + "  \n"
        "\n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testRegistersWrittenWithTopComponent()
{
    QList <QSharedPointer <Register> > registers;
    QSharedPointer <Register> testRegister = createTestRegister("register", "4", "2", "2", "exampleDescription");
    registers.append(testRegister);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeRegisters(registers, stream, subHeaderNumber, subHeaderNumber, subHeaderNumber, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "### 1.1.1.1.1 Register " + testRegister->name() + "  \n"
        "\n"
        "**Description:** " + testRegister->description() + "  \n"
        "**Offset [AUB]:** " + testRegister->getAddressOffset() + "  \n"
        "**Size [bits]:** " + testRegister->getSize() + "  \n"
        "**Dimension:** " + testRegister->getDimension() + "  \n"
        "**Volatile:** " + testRegister->getVolatile() + "  \n"
        "**Access:** " + AccessTypes::access2Str(testRegister->getAccess()) + "  \n"
        "\n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testAddressBlockRegisterFilesWrittenWithTopComponent()
{
    QSharedPointer<RegisterFile> testRegisterFileParent(new RegisterFile("testRegisterFileParent", "8", "16"));
    QSharedPointer<RegisterFile> testRegisterFileChild(new RegisterFile("testRegisterFileChild", "64", "512"));

    QSharedPointer<Register> registerInParentRegisterFile = createTestRegister("testRegister", "4", "2", "0", "example description");
    QSharedPointer<Register> registerInChildRegisterFile = createTestRegister("testRegister2", "8", "2", "0", "example description2");

    QSharedPointer<QList <QSharedPointer<RegisterBase> > > childRegisterData(new QList<QSharedPointer<RegisterBase> >({ registerInChildRegisterFile }));
    testRegisterFileChild->setRegisterData(childRegisterData);

    QSharedPointer<QList <QSharedPointer<RegisterBase> > > parentRegisterData(new QList<QSharedPointer<RegisterBase> >({ testRegisterFileChild, registerInParentRegisterFile }));
    testRegisterFileParent->setRegisterData(parentRegisterData);

    QSharedPointer<QList<QSharedPointer<RegisterBase> > >addressBlockRegisterData(new QList < QSharedPointer<RegisterBase> >({ testRegisterFileParent }));
    auto testAddressBlock = createAddressBlockWithRegisterData("testAddressBlock", "address block description", "'h0", "4", "32", *addressBlockRegisterData);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    addressBlocks.append(testAddressBlock);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeAddressBlocks(addressBlocks, stream, subHeaderNumber, subHeaderNumber);

    targetFile.close();
    targetFile.copy("OUTPUT.md");

    QString expectedOutput(
        "### 1.1.1.1 Address block testAddressBlock  \n"
        "\n"
        "**Description:** " + testAddressBlock->description() + "  \n"
        "**Usage:** " + General::usage2Str(testAddressBlock->getUsage()) + "  \n"
        "**Base address [AUB]:** 'h0  \n"
        "**Range [AUB]:** 4  \n"
        "**Width [AUB]:** 32  \n"
        "**Access:** " + AccessTypes::access2Str(testAddressBlock->getAccess()) + "  \n"
        "**Volatile:** " + testAddressBlock->getVolatile() + "  \n"
        "\n"
        "#### Address block 'testAddressBlock' contains the following register files:  \n"
        "\n"
        "### 1.1.1.1.1 Register file testRegisterFileParent  \n"
        "\n"
        "### 1.1.1.1.1.1 Register testRegister  \n"
        "\n"
        "**Description:** " + registerInParentRegisterFile->description() + "  \n"
        "**Offset [AUB]:** " + registerInParentRegisterFile->getAddressOffset() + "  \n"
        "**Size [bits]:** " + registerInParentRegisterFile->getSize() + "  \n"
        "**Dimension:** " + registerInParentRegisterFile->getDimension() + "  \n"
        "**Volatile:** " + registerInParentRegisterFile->getVolatile() + "  \n"
        "**Access:** " + AccessTypes::access2Str(registerInParentRegisterFile->getAccess()) + "  \n"
        "\n"
        "### 1.1.1.1.1.2 Register file testRegisterFileChild  \n"
        "\n"
        "### 1.1.1.1.1.2.1 Register testRegister2  \n"
        "\n"
        "**Description:** " + registerInChildRegisterFile->description() + "  \n"
        "**Offset [AUB]:** " + registerInChildRegisterFile->getAddressOffset() + "  \n"
        "**Size [bits]:** " + registerInChildRegisterFile->getSize() + "  \n"
        "**Dimension:** " + registerInChildRegisterFile->getDimension() + "  \n"
        "**Volatile:** " + registerInChildRegisterFile->getVolatile() + "  \n"
        "**Access:** " + AccessTypes::access2Str(registerInChildRegisterFile->getAccess()) + "  \n"
        "\n"
    );
    QFile testFile("TESTOUTPUT.md");
    testFile.open(QFile::WriteOnly);
    QTextStream stream2(&testFile);

    stream2 << expectedOutput;
    testFile.close();
    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testFieldsWrittenWithTopComponent()
{
    QSharedPointer<Field> testField = createTestField("testField", "Example Field", "2", "8");
    QSharedPointer<FieldReset> resetValue(new FieldReset());
    resetValue->setResetValue("testReset");
    resetValue->setResetMask("testMask");

    testField->getResets()->append(resetValue);


    QSharedPointer<Register> fieldRegister = createTestRegister("FieldRegister", "10", "10", "10", "");
    fieldRegister->getFields()->append(testField);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generator->writeFields(fieldRegister, stream);

    targetFile.close();

    QString expectedOutput(
        "#### Register '" + fieldRegister->name() + "' contains the following fields:  \n"
        "\n"
        "|Field name|Offset [bits]|Width [bits]|Volatile|Access|Resets|Description|  \n"
        "|:----|:----|:----|:----|:----|:----|:----|  \n"
        "|" + testField->name() + " <a id=\"" + topComponent_->getVlnv().toString() + 
            ".field." + testField->name() + "\">"
        "|" + testField->getBitOffset() +
        "|" + testField->getBitWidth() +
        "|" + testField->getVolatile().toString() +
        "|" + AccessTypes::access2Str(testField->getAccess()) +
        "|" + "HARD : testReset" +
        "|" + testField->description() + "|  \n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testMemoryMapToFieldWrittenWithTopComponent()
{
    QSharedPointer<Field> testField = createTestField("testField", "", "2", "8");
    QSharedPointer<FieldReset> resetValue(new FieldReset());
    resetValue->setResetValue("8'h3");
    resetValue->setResetMask("00000011");

    testField->getResets()->append(resetValue);

    QList <QSharedPointer <Register> > registers;
    QSharedPointer<Register> testRegister = createTestRegister("testRegister", "10", "4", "2", "Describing reg.");
    testRegister->getFields()->append(testField);
    registers.append(testRegister);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> testAddressBlock = createTestAddressBlock("testAddress", "Describe.", "'h0", "4",
        "32", registers);
    addressBlocks.append(testAddressBlock);

    QList <QSharedPointer <MemoryMap> > memoryMaps;
    QSharedPointer <MemoryMap> testMemoryMap = createTestMemoryMap("testMemoryMap", "", 8, addressBlocks);
    memoryMaps.append(testMemoryMap);

    topComponent_->getMemoryMaps()->append(memoryMaps);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeMemoryMaps(stream, subHeaderNumber);

    targetFile.close();

    QString vlnvString(topComponent_->getVlnv().toString());

    QString expectedOutput(
        "## 1.1 Memory maps <a id=\"" + vlnvString + ".memoryMaps\">  \n"
        "\n"
        "### 1.1.1 Memory map " + testMemoryMap->name() + "  \n"
        "\n"
        "**Address unit bits (AUB):** " + testMemoryMap->getAddressUnitBits() + "  \n"
        "\n"
        "### 1.1.1.1 Address block " + testAddressBlock->name() + "  \n"
        "\n"
        "**Description:** " + testAddressBlock->description() + "  \n"
        "**Usage:** " + General::usage2Str(testAddressBlock->getUsage()) + "  \n"
        "**Base address [AUB]:** " + testAddressBlock->getBaseAddress() + "  \n"
        "**Range [AUB]:** " + testAddressBlock->getRange() + "  \n"
        "**Width [AUB]:** " + testAddressBlock->getWidth() + "  \n"
        "**Access:** " + AccessTypes::access2Str(testAddressBlock->getAccess()) + "  \n"
        "**Volatile:** " + testAddressBlock->getVolatile() + "  \n"
        "\n"
        "#### Address block '" + testAddressBlock->name() + "' contains the following registers:  \n"
        "\n"
        "|Register name|Offset [AUB]|Size [bits]|Dimension|Volatile|Access|  \n"
        "|:----|:----|:----|:----|:----|:----|  \n"
        "|" + testRegister->name() +
        "|" + testRegister->getAddressOffset() +
        "|" + testRegister->getSize() +
        "|" + testRegister->getDimension() +
        "|" + testRegister->getVolatile() +
        "|" + AccessTypes::access2Str(testRegister->getAccess()) + "|  \n"
        "### 1.1.1.1.1 Register " + testRegister->name() + "  \n"
        "\n"
        "**Description:** " + testRegister->description() + "  \n"
        "**Offset [AUB]:** " + testRegister->getAddressOffset() + "  \n"
        "**Size [bits]:** " + testRegister->getSize() + "  \n"
        "**Dimension:** " + testRegister->getDimension() + "  \n"
        "**Volatile:** " + testRegister->getVolatile() + "  \n"
        "**Access:** " + AccessTypes::access2Str(testRegister->getAccess()) + "  \n"
        "\n"
        "#### Register '" + testRegister->name() + "' contains the following fields:  \n"
        "\n"
        "|Field name|Offset [bits]|Width [bits]|Volatile|Access|Resets|Description|  \n"
        "|:----|:----|:----|:----|:----|:----|:----|  \n"
        "|" + testField->name() + " <a id=\"" + topComponent_->getVlnv().toString() +
        ".field." + testField->name() + "\">"
        "|" + testField->getBitOffset() +
        "|" + testField->getBitWidth() +
        "|" + testField->getVolatile().toString() +
        "|" + AccessTypes::access2Str(testField->getAccess()) +
        "|" + "HARD : 8'h3" +
        "|" + testField->description() + "|  \n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testPortsWrittenWithOnlyTopComponent()
{
    QList <QSharedPointer <Parameter> > componentParameters;
    QSharedPointer <Parameter> parameter = createTestParameter("parameter", "10", "Description", "ID_parameter",
        "", "");
    componentParameters.append(parameter);

    QSharedPointer <Port> portRef = createTestPort("portRef", "ID_parameter", "4", "ID_parameter", "2",
        "ID_parameter");

    topComponent_->getPorts()->append(portRef);

    topComponent_->getParameters()->append(componentParameters);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writePorts(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "## 1.1 Ports <a id=\"" + topComponent_->getVlnv().toString() + ".ports\">  \n"
        "\n"
        "|Name|Direction|Left bound|Right bound|Port type|Type definition|Default value|"
        "Array left|Array right|Description|  \n"
        "|:----|:----|:----|:----|:----|:----|:----|:----|:----|:----|  \n"
        "|" + portRef->name() + " <a id=\"" + topComponent_->getVlnv().toString() + ".port."
        + portRef->name() + "\">" +
        "|" + DirectionTypes::direction2Str(portRef->getDirection()) +
        "|" + "parameter" +
        "|" + portRef->getRightBound() +
        "|" + portRef->getTypeName() +
        "|" + portRef->getTypeDefinition(portRef->getTypeName()) +
        "|" + "parameter" +
        "|" + portRef->getArrayLeft() +
        "|" + "parameter" +
        "|" + portRef->description() + "|  \n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testBusInterfacesWrittenWithoutPorts()
{
    QSharedPointer <BusInterface> busInterface(new BusInterface);
    busInterface->setName("interface");
    busInterface->setInterfaceMode(General::MASTER);

    topComponent_->getBusInterfaces()->append(busInterface);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeInterfaces(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "## 1.1 Bus interfaces <a id=\"" + topComponent_->getVlnv().toString() + ".interfaces\">  \n"
        "\n"
        "### 1.1.1 Bus interface " + busInterface->name() + "  \n"
        "\n"
        "**Interface mode:** " + General::interfaceMode2Str(busInterface->getInterfaceMode()) + "  \n"
        "**Ports used in this interface:** None  \n"
        "\n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testFileSetsWrittenForTopComponent()
{
    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QSharedPointer<FileSet> testFileSet(new FileSet);
    testFileSet->setName("testFileSet");
    testFileSet->setDescription("example description");
    
    QSharedPointer <QStringList> testFileSetGroups(new QStringList({ "documentation", "testing" }));
    
    testFileSet->setGroups(testFileSetGroups);

    QSharedPointer<FileBuilder> testFileBuilder(new FileBuilder);
    testFileBuilder->setFileType("vhdlSource");
    testFileBuilder->setCommand("vcom");

    QSharedPointer<File> testFile1(new File);
    testFile1->setName("testFile1.vhd");
    testFile1->setDescription("a test file 1");
    testFile1->addFileType("vhdlSource");
    testFile1->addFileType("vhdlSource-87");
    testFile1->setLogicalName("testLogicalName");
    
    QSharedPointer<BuildCommand> testFile1BuildCommand(new BuildCommand);
    testFile1BuildCommand->setCommand("vcom");
    testFile1BuildCommand->setFlags("");
    testFile1->setBuildcommand(testFile1BuildCommand);

    QSharedPointer<File> testFile2(new File);
    testFile2->setName("testFile2.v");
    testFile2->setDescription("a test file 2");
    testFile2->addFileType("verilogSource");
    testFile2->addFileType("vhdlSource");

    QSharedPointer<BuildCommand> testFile2BuildCommand(new BuildCommand);
    testFile2BuildCommand->setCommand("make");
    testFile2BuildCommand->setFlags("--verbose");
    testFile2->setBuildcommand(testFile2BuildCommand);

    testFileSet->addFile(testFile1);
    testFileSet->addFile(testFile2);

    testFileBuilder->setFlags("");

    QSharedPointer<QList <QSharedPointer<FileBuilder>>> defaultFileBuilders(new QList<QSharedPointer<FileBuilder> >({ testFileBuilder }));
    testFileSet->setDefaultFileBuilders(defaultFileBuilders);
    topComponent_->getFileSets()->append(testFileSet);

    QString testFile1AbsPath = General::getAbsolutePath(library_.getPath(topComponent_->getVlnv()), testFile1->name());
    QString testFile1PathFromDoc = General::getRelativePath(targetPath_, testFile1AbsPath);
    
    QString testFile2AbsPath = General::getAbsolutePath(library_.getPath(topComponent_->getVlnv()), testFile2->name());
    QString testFile2PathFromDoc = General::getRelativePath(targetPath_, testFile2AbsPath);

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeFileSets(stream, subHeaderNumber);

    targetFile.close();

    QString groups = testFileSet->getGroups()->join(", ");

    QString expectedOutput(
        "## 1.1 File sets <a id=\"" + topComponent_->getVlnv().toString() + ".fileSets\">  \n"
        "\n"
        "### 1.1.1 File set " + testFileSet->name() + "  \n"
        "\n"
        "**Description:** " + testFileSet->description() + "  \n"
        "\n"
        "**Identifiers:** " + groups + "  \n"
        "\n"
        "**Default file builders:**  \n"
        "\n"
        "|File type|Command|Flags|Replace default flags|  \n"
        "|:----|:----|:----|:----|  \n"
        "|vhdlSource|vcom|||  \n"
        "#### 1.1.1.1 Files  \n"
        "\n"
        "|File name|Logical name|Build command|Build flags|Specified file types|Description|  \n"
        "|:----|:----|:----|:----|:----|:----|  \n"
        "|[" + testFile1->name() + "](" + testFile1PathFromDoc + ")"
            "|" + testFile1->getLogicalName() +
            "|" + testFile1->getBuildCommand()->getCommand() +
            "|" + testFile1->getBuildCommand()->getFlags() +
            "|" + testFile1->getFileTypes()->join(",<br>") +
            "|" + testFile1->getDescription() + "|  \n"
        "|[" + testFile2->name() + "](" + testFile2PathFromDoc + ")"
            "|" + testFile2->getLogicalName() +
            "|" + testFile2->getBuildCommand()->getCommand() +
            "|" + testFile2->getBuildCommand()->getFlags() +
            "|" + testFile2->getFileTypes()->join(",<br>") +
            "|" + testFile2->getDescription() + "|  \n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testViewsWrittenForTopComponent()
{
    QSharedPointer<View> flatView(new View("testView"));

    topComponent_->getViews()->append(flatView);

    VLNV firstVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "FirstComponent", "1.0");
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(new ConfigurableVLNVReference(firstVlnv));
    QSharedPointer<ComponentInstance> firstInstance(new ComponentInstance("firstInstance", instanceVLNV));

    QSharedPointer<Component> refComponent = QSharedPointer<Component>(new Component(firstVlnv));

    QList <QSharedPointer<Parameter> > componentParameters;

    QSharedPointer<Parameter> targetParameter = createTestParameter("firstParameter", "10",
        "", "ID_TARGET", "", "");

    QSharedPointer<ComponentInstantiation> componentInstantiation(new ComponentInstantiation());
    componentInstantiation->setName("testInstantiation");
    componentInstantiation->setLanguage("C");
    componentInstantiation->setLanguageStrictness(true);
    componentInstantiation->setLibraryName("testLibrary");
    componentInstantiation->setPackageName("testPackage");
    componentInstantiation->setModuleName("testModuleName");

    componentInstantiation->getParameters()->append(targetParameter);

    topComponent_->getComponentInstantiations()->append(componentInstantiation);
    flatView->setComponentInstantiationRef("testInstantiation");

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;
    QStringList pictureList;

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    generator->writeViews(stream, subHeaderNumber, pictureList);

    targetFile.close();

    QString expectedOutput(
        "## 1.1 Views <a id=\"" + topComponent_->getVlnv().toString() + ".views\">  \n"
        "\n"
        "### 1.1.1 View: testView  \n"
        "\n"
        "#### 1.1.1.1 Component instantiation: testInstantiation  \n"
        "\n"
        "**Language:** C **strict**  \n"
        "\n"
        "**Library:** testLibrary  \n"
        "\n"
        "**Package:** testPackage  \n"
        "\n"
        "**Module name:** testModuleName  \n"
        "\n"
        "Parameters:  \n"
        "\n"
        "|Name|Type|Value|Resolve|Bit vector left|Bit vector right|Array left|Array right|Description|  \n"
        "|:----|:----|:----|:----|:----|:----|:----|:----|:----|  \n"
        "|firstParameter||10|||||||  \n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testDesignIsWritten()
{
    VLNV designVlnv(VLNV::DESIGN, "Test", "TestLibrary", "TestDesign", "1.0");
    QSharedPointer<Design> design = QSharedPointer<Design>(new Design(designVlnv));
    library_.writeModelToFile("C:/Test/TestLibrary/TestDesign/1.0/TestDesign.1.0.xml", design);
    library_.addComponent(design);

    VLNV firstVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "FirstComponent", "1.0");
    QSharedPointer<Component> refComponent = QSharedPointer<Component>(new Component(firstVlnv));

    QList <QSharedPointer<Parameter> > componentParameters;

    QSharedPointer<Parameter> targetParameter = createTestParameter("firstParameter", "10", "", "ID_TARGET", "",
        "");
    QSharedPointer<Parameter> referParameter = createTestParameter("referer", "ID_TARGET", "", "ID-REF", "", "");
    componentParameters.append(targetParameter);
    componentParameters.append(referParameter);

    refComponent->getParameters()->append(componentParameters);

    library_.addComponent(refComponent);

    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(new ConfigurableVLNVReference(firstVlnv));
    QSharedPointer<ComponentInstance> firstInstance(new ComponentInstance("firstInstance", instanceVLNV));
    firstInstance->getConfigurableElementValues()->append(createConfigurableElementvalues(refComponent));

    design->getComponentInstances()->append(firstInstance);

    QSharedPointer<View> hierarchicalView(new View);
    hierarchicalView->setName("HierarchicalView");
    hierarchicalView->setDesignInstantiationRef("design_instantiation");
    
    
    topComponent_->getViews()->append(hierarchicalView);

    QSharedPointer<DesignInstantiation> designInstantiation(new DesignInstantiation("design_instantiation"));
    designInstantiation->setDesignReference(
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(designVlnv)));

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());
    
    topComponent_->getDesignInstantiations()->append(designInstantiation);

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;
    QStringList files;

    generator->writeViews(stream, subHeaderNumber, files);

    targetFile.close();

    QString expectedOutput(
        "## 1.1 Views <a id=\"Test:TestLibrary:TestComponent:1.0.views\">  \n"
        "\n"
        "### 1.1.1 View: HierarchicalView  \n"
        "\n"
        "#### 1.1.1.1 Design instantiation: design_instantiation  \n"
        "\n"
        "**Design:** Test:TestLibrary:TestDesign:1.0  \n"
        "**IP-Xact file:** [TestDesign.1.0.xml]()  \n"
        "Diagram of design Test:TestLibrary:TestDesign:1.0:  \n"
        "![View: HierarchicalView preview picture](Test.TestLibrary.TestComponent.1.0.HierarchicalView.png)  \n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testEndOfDocumentWrittenForTopComponent()
{
}

DocumentGenerator* tst_MarkdownGenerator::createTestGenerator()
{
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_);
    library_.addComponent(topComponent_);

    DocumentGenerator* generator(new DocumentGenerator(&library_, topComponentVlnv_, &designWidgetFactory_,
        &expressionFormatterFactory_, 1, generatorParentWidget_));

    generator->setFormat(DocumentGenerator::DocumentFormat::MD);
    return generator;
}

void tst_MarkdownGenerator::readOutputFile()
{
    QFile outputFile(targetPath_);

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    output_ = outputFile.readAll();
    outputFile.close();
}

void tst_MarkdownGenerator::checkOutputFile(QString const& expectedOutput)
{
    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

QSharedPointer<Parameter> tst_MarkdownGenerator::createTestParameter(QString const& name, QString const& value, QString const& description, QString const& uuID, QString const& arrayLeft, QString const& arrayRight)
{
    QSharedPointer<Parameter> parameter(new Parameter);
    parameter->setName(name);
    parameter->setValue(value);
    parameter->setDescription(description);
    parameter->setValueId(uuID);
    parameter->setArrayLeft(arrayLeft);
    parameter->setArrayRight(arrayRight);

    return parameter;
}

QSharedPointer<Port> tst_MarkdownGenerator::createTestPort(QString const& name, QString const& leftBound, QString const& rightBound, QString const& defaultValue, QString const& arrayLeft, QString const& arrayRight)
{
    QSharedPointer<Port> newPort(new Port);
    newPort->setDirection(DirectionTypes::DIRECTION_PHANTOM);
    newPort->setName(name);
    newPort->setLeftBound(leftBound);
    newPort->setRightBound(rightBound);
    newPort->setDefaultValue(defaultValue);
    newPort->setArrayLeft(arrayLeft);
    newPort->setArrayRight(arrayRight);

    return newPort;
}

QList<QSharedPointer<ConfigurableElementValue>> tst_MarkdownGenerator::createConfigurableElementvalues(QSharedPointer<Component> component)
{
    ExpressionFormatter* refExpressionFormatter = expressionFormatterFactory_.makeExpressionFormatter(component);

    QList<QSharedPointer<ConfigurableElementValue> > instanceConfigurableElementValues;

    foreach(QSharedPointer<Parameter> parameter, *component->getParameters())
    {
        instanceConfigurableElementValues.append(QSharedPointer<ConfigurableElementValue>(
            new ConfigurableElementValue(refExpressionFormatter->formatReferringExpression(parameter->getValue()), parameter->name())));
    }

    delete refExpressionFormatter;
    refExpressionFormatter = nullptr;

    return instanceConfigurableElementValues;
}

QSharedPointer<Field> tst_MarkdownGenerator::createTestField(QString const& name, QString const& description, QString const& offset, QString const& width)
{
    QSharedPointer <Field> testField(new Field);
    testField->setName(name);
    testField->setDescription(description);
    testField->setBitOffset(offset);
    testField->setBitWidth(width);
    testField->setVolatile(false);
    testField->setAccess(AccessTypes::READ_ONLY);

    return testField;
}

QSharedPointer<Register> tst_MarkdownGenerator::createTestRegister(QString const& name, QString const& offset, QString const& size, QString const& dimension, QString const& description)
{
    QSharedPointer<Register> testRegister(new Register);
    testRegister->setName(name);
    testRegister->setAddressOffset(offset);
    testRegister->setSize(size);
    testRegister->setDimension(dimension);
    testRegister->setDescription(description);
    testRegister->setVolatile(true);
    testRegister->setAccess(AccessTypes::READ_WRITE);

    return testRegister;
}

QSharedPointer<AddressBlock> tst_MarkdownGenerator::createTestAddressBlock(QString const& name, QString const& description, QString const& baseAddress, QString const& range, QString const& width, QList<QSharedPointer<Register>> registers)
{
    QSharedPointer <AddressBlock> testAddressBlock(new AddressBlock);
    testAddressBlock->setName(name);
    testAddressBlock->setUsage(General::REGISTER);
    testAddressBlock->setDescription(description);
    testAddressBlock->setBaseAddress(baseAddress);
    testAddressBlock->setRange(range);
    testAddressBlock->setWidth(width);
    testAddressBlock->setAccess(AccessTypes::READ_WRITE);
    testAddressBlock->setVolatile(true);

    for (QSharedPointer<RegisterBase> testRegister : registers)
    {
        testAddressBlock->getRegisterData()->append(testRegister);
    }

    return testAddressBlock;
}

QSharedPointer<AddressBlock> tst_MarkdownGenerator::createAddressBlockWithRegisterData(QString const& name,
    QString const& description, QString const& baseAddress, QString const& range,
    QString const& width, QList<QSharedPointer<RegisterBase>> registers)
{
    QSharedPointer <AddressBlock> testAddressBlock(new AddressBlock);
    testAddressBlock->setName(name);
    testAddressBlock->setUsage(General::REGISTER);
    testAddressBlock->setDescription(description);
    testAddressBlock->setBaseAddress(baseAddress);
    testAddressBlock->setRange(range);
    testAddressBlock->setWidth(width);
    testAddressBlock->setAccess(AccessTypes::READ_WRITE);
    testAddressBlock->setVolatile(true);

    for (QSharedPointer<RegisterBase> testRegister : registers)
    {
        testAddressBlock->getRegisterData()->append(testRegister);
    }

    return testAddressBlock;
}

QSharedPointer<MemoryMap> tst_MarkdownGenerator::createTestMemoryMap(QString const& name, QString const& description, int addressUnitbits, QList<QSharedPointer<AddressBlock>> addressBlocks)
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap);
    memoryMap->setName(name);
    memoryMap->setAddressUnitBits(QString::number(addressUnitbits));
    memoryMap->setDescription(description);

    for (QSharedPointer<AddressBlock> includedAddressBlock : addressBlocks)
    {
        memoryMap->getMemoryBlocks()->append(includedAddressBlock);
    }

    return memoryMap;
}

QString tst_MarkdownGenerator::getSpaceString()
{
    return QString();
}

QString tst_MarkdownGenerator::getTableString()
{
    return QString();
}

QString tst_MarkdownGenerator::getEncodingString()
{
    return QString();
}

QString tst_MarkdownGenerator::getDoctypeString()
{
    return QString();
}

QString tst_MarkdownGenerator::getValidW3CStrictString()
{
    return QString();
}

QTEST_MAIN(tst_MarkdownGenerator)

#include "tst_MarkdownGenerator.moc"