#include <QtTest>

#include <kactusGenerators/DocumentGenerator/documentgenerator.h>
#include <kactusGenerators/DocumentGenerator/ViewDocumentGenerator.h>

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

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QWidget>

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

    QSharedPointer<MemoryMap> createTestMemoryMap(QString const& name, QString const& description,
        int addressUnitbits, QList<QSharedPointer <AddressBlock> > addressBlocks);

    //! Get the string used to describe a space.
    QString getSpaceString();

    //! Get the string used to describe an indent.
    QString getIndentString();

    //! Get the string used to describe a table.
    QString getTableString();

    //! Get the string used to describe the encoding.
    QString getEncodingString();

    //! Get the string used to describe the document type.
    QString getDoctypeString();

    //! Get the string used to describe valid w3c strict.
    QString getValidW3CStrictString();
    //-----------------------------------------------------------------------------
    // Function: createViewGenerator()
    //-----------------------------------------------------------------------------
    ViewDocumentGenerator* createViewGenerator();

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
        &expressionFormatterFactory_, generatorParentWidget_);
    
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
        generationTime_.toString("dd.MM.yyyy hh:mm:ss") + " by user testUser\n"
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
        "## 0.1 Kactus2 attributes <a id=\"" + topComponent_->getVlnv().toString() + ".attributes\">  \n"
        "\n"
        "**Product hierarchy:** Flat  \n"
        "**Component implementation:** SW  \n"
        "**Component firmness:** Fixed  \n"
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
        "## 0.2 General parameters <a id=\"" + vlnvString + ".parameters\">  \n\n"
        "|Name|Type|Value|Resolve|Bit vector left|Bit vector right|Array left|Array right|Description|\n"
        "|:----|:----|:----|:----|:----|:----|:----|:----|:----|\n"
        "|" + parameter->name() + "|" + parameter->getType() + "|" +
            parameter->getValue() + "|" + parameter->getValueResolve()  + "|" +
            parameter->getVectorLeft() + "|" + parameter->getVectorRight() + "|" +
            parameter->getArrayLeft() + "|" + parameter->getArrayRight() + "|" +
            parameter->description() + "|\n"
        "|" + refParameter->name() + "|" + "" + "|" +
            "parameter" + "|" + "" + "|" +
            "" + "|" + "" + "|" +
            "" + "|" + "" + "|" +
            refParameter->description() + "|\n"
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
        "## 0.1 Memory maps <a id=\"" + topComponent_->getVlnv().toString() + ".memoryMaps\">  \n\n"
        "### 0.1.1 " + memoryMap->name() + " <a id=\"" + topComponent_->getVlnv().toString() + 
            ".memoryMap." + memoryMap->name() + "\">  \n"
        "**Description:** " + memoryMap->description() + "  \n"
        "**Address unit bits (AUB):** " + memoryMap->getAddressUnitBits() + "  \n"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testAddressBlocksWrittenWithTopComponent()
{
    QList <QSharedPointer <Register> > registers;
    QSharedPointer <Register> testRegister = createTestRegister("register", "4", "2", "2", "");
    registers.append(testRegister);

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
        "### 0.1.1.1 " + testAddressBlock->name() + " <a id=\"" + topComponent_->getVlnv().toString() + ".addressBlock." + testAddressBlock->name() + "\">  \n"
        "\n"
        "**Description:** " + testAddressBlock->description() + "  \n"
        "|Usage|Base address [AUB]|Range [AUB]|Width [AUB]|Access|Volatile|\n"
        "|:----|:----|:----|:----|:----|:----|\n" +
        "|" + General::usage2Str(testAddressBlock->getUsage()) +
        "|" + testAddressBlock->getBaseAddress() +
        "|" + testAddressBlock->getRange() +
        "|" + testAddressBlock->getWidth() +
        "|" + AccessTypes::access2Str(testAddressBlock->getAccess()) +
        "|" + testAddressBlock->getVolatile() + "|"
    );

    checkOutputFile(expectedOutput);
}

void tst_MarkdownGenerator::testExpressionsInAddressBlocks()
{
}

void tst_MarkdownGenerator::testRegistersWrittenWithTopComponent()
{
}

void tst_MarkdownGenerator::testFieldsWrittenWithTopComponent()
{
}

void tst_MarkdownGenerator::testMemoryMapToFieldWrittenWithTopComponent()
{
}

void tst_MarkdownGenerator::testPortsWrittenWithOnlyTopComponent()
{
}

void tst_MarkdownGenerator::testBusInterfacesWrittenWithoutPorts()
{
}

void tst_MarkdownGenerator::testFileSetsWrittenForTopComponent()
{
}

void tst_MarkdownGenerator::testViewsWrittenForTopComponent()
{
}

void tst_MarkdownGenerator::testDesignIsWritten()
{
}

void tst_MarkdownGenerator::testEndOfDocumentWrittenForTopComponent()
{
}

DocumentGenerator* tst_MarkdownGenerator::createTestGenerator()
{
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_);
    library_.addComponent(topComponent_);

    DocumentGenerator* generator(new DocumentGenerator(&library_, topComponentVlnv_, &designWidgetFactory_,
        &expressionFormatterFactory_, generatorParentWidget_));

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
    return QList<QSharedPointer<ConfigurableElementValue>>();
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

ViewDocumentGenerator* tst_MarkdownGenerator::createViewGenerator()
{
    return nullptr;
}

QTEST_MAIN(tst_MarkdownGenerator)

#include "tst_MarkdownGenerator.moc"