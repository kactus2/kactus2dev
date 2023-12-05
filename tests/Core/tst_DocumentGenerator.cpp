//-----------------------------------------------------------------------------
// File: tst_DocumentGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.02.2015
//
// Description:
// Unit test for class documentGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <kactusGenerators/DocumentGenerator/documentgenerator.h>
#include <kactusGenerators/DocumentGenerator/HtmlWriter.h>

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
#include <IPXACTmodels/Component/EnumeratedValue.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QWidget>

class tst_DocumentGenerator : public QObject
{
    Q_OBJECT

public:
    tst_DocumentGenerator();

private slots:
    
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    //! Test cases.

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
     *  Compare output to expected output.
     */
    void checkOutputFile(QString const& expectedOutput);

    /*!
     *  Create a model parameter used in the tests.
     *
     *      @param [in] name            Name of the parameter.
     *      @param [in] dataType        Data type of the parameter.
     *      @param [in] value           Value of the parameter.
     *      @param [in] description     Description of the parameter.
     *      @param [in] uuID            Id of the parameter.
     */
   /* QSharedPointer<ModelParameter> createTestModelParameter(QString const& name, QString const& dataType,
        QString const& value, QString const& description, QString const& uuID, QString const& arrayLeft,
        QString const& arrayRight);*/

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

    QSharedPointer<Field> createTestField (QString const& name, QString const& description, QString const& offset,
        QString const& width);

    QSharedPointer<Register> createTestRegister (QString const& name, QString const& offset, QString const& size,
        QString const& dimension, QString const& description);

    QSharedPointer<AddressBlock> createTestAddressBlock (QString const& name, QString const& description,
        QString const& baseAddress, QString const& range, QString const& width,
        QList <QSharedPointer <Register> > registers);
    
    QSharedPointer<AddressBlock> createAddressBlockWithRegisterData(QString const& name, QString const& description,
        QString const& baseAddress, QString const& range, QString const& width,
        QList <QSharedPointer <RegisterBase> > registerData);

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

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::tst_DocumentGenerator()
//-----------------------------------------------------------------------------
tst_DocumentGenerator::tst_DocumentGenerator() :
topComponent_(), topComponentVlnv_(), output_(), generationTime_(), library_(this), designWidgetFactory_(&library_),
    expressionFormatterFactory_(), targetPath_("./generatorOutput.html"), generatorParentWidget_(new QWidget)
{

}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::init()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    topComponentVlnv_ = vlnv;
    topComponent_ = QSharedPointer<Component>(new Component(topComponentVlnv_, Document::Revision::Std14));

    library_.clear();

    QVERIFY(!QFile::exists(targetPath_));
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::cleanup()
{
    topComponent_.clear();
    output_.clear();

    QFile::remove(targetPath_);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testInvalidVlnvInConstructor()
{
    VLNV invalidVlnv(VLNV::COMPONENT, "invalid", "library", "component", "0");

    DocumentGenerator generator (&library_, invalidVlnv, &designWidgetFactory_,
        &expressionFormatterFactory_, 1, generatorParentWidget_);

    QSignalSpy spy(&generator, SIGNAL(errorMessage(QString const&)));

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);
    
    generator.setFormat(DocumentGenerator::HTML);
    generator.writeDocumentation(stream, targetPath_);

    targetFile.close();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toString(), QString("VLNV was not found in the library."));
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testFileHeaderIsPrinted()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testFileHeaderIsWritten()
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
    generator->setFormat(DocumentGenerator::DocumentFormat::HTML);
    generator->writeHeader(stream);

    targetFile.close();

    QString expectedOutput(getDoctypeString() + "\n"
    "<html>\n"
        "\t<head>\n"
        "\t" + getEncodingString() + "\n"
            "\t\t<title>Kactus2 generated documentation for component " + topComponent_->getVlnv().getName() +
            " " + topComponent_->getVlnv().getVersion() + "</title>\n"
        "\t</head>\n"
        "\t<body>\n"
            "\t\t<h6>This document was generated by Kactus2 on " +
            generationTime_.toString("dd.MM.yyyy hh:mm:ss") + " by user testUser</h6>\n");

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testTableOfContentsIsWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testTableOfContentsIsWrittenWithOnlyTopComponent()
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

    QSharedPointer<BusInterface> busInterface (new BusInterface());
    busInterface->setName("busInterface");

    QSharedPointer<FileSet> fileset (new FileSet("fileSet"));

    QSharedPointer<View> view (new View("view"));

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

    generator->setFormat(DocumentGenerator::DocumentFormat::HTML);
    generator->writeTableOfContents(stream);

    targetFile.close();

    QString expectedOutput(
        "\t\t<a href=\"#" + topComponent_->getVlnv().toString() + "\">1. Component" + getSpaceString() +
        topComponent_->getVlnv().getVendor() + " - " + topComponent_->getVlnv().getLibrary() + " - " +
        topComponent_->getVlnv().getName() + " - " + topComponent_->getVlnv().getVersion() + "</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".kts_params\">1.1. Kactus2 attributes</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".parameters\">1.2. General parameters</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".memoryMaps\">1.3. Memory maps</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".ports\">1.4. Ports</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".interfaces\">1.5. Bus interfaces</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".fileSets\">1.6. File sets</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".views\">1.7. Views</a><br>\n"
            );

    checkOutputFile(expectedOutput);
}

void tst_DocumentGenerator::testAttributesWrittenWithOnlyTopComponent()
{
    topComponent_->setHierarchy(KactusAttribute::FLAT);
    topComponent_->setImplementation(KactusAttribute::SW);
    topComponent_->setFirmness(KactusAttribute::FIXED);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    int subHeaderNumber = 1;

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generator->setFormat(DocumentGenerator::DocumentFormat::HTML);
    generator->writeKactusAttributes(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".attributes\">1.1 Kactus2 attributes</a></h2>\n"
        "\t\t<p>\n"
        "\t\t\t<strong>" + getIndentString() + "Product hierarchy: </strong>" +
        KactusAttribute::hierarchyToString(topComponent_->getHierarchy()) + "<br>\n"
        "\t\t\t<strong>" + getIndentString() + "Component implementation: </strong>" +
        KactusAttribute::implementationToString(topComponent_->getImplementation()) + "<br>\n"
        "\t\t\t<strong>" + getIndentString() + "Component firmness: </strong>" +
        KactusAttribute::firmnessToString(topComponent_->getFirmness()) + "<br>\n"
        "\t\t</p>\n"
    );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testParametersWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testParametersWrittenWithOnlyTopComponent()
{
    QSharedPointer <Parameter> parameter = createTestParameter("parameter", "10", "Description", "ID_parameter",
        "1", "0");
    QSharedPointer <Parameter> refParameter = createTestParameter("refParameter", "ID_parameter", "Describe this",
        "ID-refer", "", "");

    topComponent_->getParameters()->append(parameter);
    topComponent_->getParameters()->append(refParameter);
    topComponent_->setHierarchy(KactusAttribute::FLAT);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 2;

    generator->setFormat(DocumentGenerator::DocumentFormat::HTML);
    generator->writeParameters(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".parameters\">1.2 General parameters</a></h2>\n"
        "\t\t\t" + getTableString() + "List of parameters defined for the component\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Type</th>\n"
        "\t\t\t\t\t<th>Value</th>\n"
        "\t\t\t\t\t<th>Resolve</th>\n"
        "\t\t\t\t\t<th>Bit vector left</th>\n"
        "\t\t\t\t\t<th>Bit vector right</th>\n"
        "\t\t\t\t\t<th>Array left</th>\n"
        "\t\t\t\t\t<th>Array right</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + parameter->name() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getType() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getValue() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getValueResolve() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getVectorLeft() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getVectorRight() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getArrayLeft() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getArrayRight() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + refParameter->name() + "</td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"        
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td>" + refParameter->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>"
        );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testMemoryMapsWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testMemoryMapsWrittenWithTopComponent()
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
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".memoryMaps\">1.1 Memory maps</a></h2>\n"
        "\t\t\t<h3>1.1.1 Memory map " + memoryMap->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + memoryMap->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Address unit bits (AUB):</strong> " + 
        memoryMap->getAddressUnitBits() + "<br>\n"
        "\t\t\t</p>"
        );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testAddressBlocksWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testAddressBlocksWrittenWithTopComponent()
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
        "\t\t\t<h3>1.1.1.1 Address block " + testAddressBlock->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testAddressBlock->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Usage:</strong> " + General::usage2Str(testAddressBlock->getUsage()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Base address [AUB]:</strong> " + testAddressBlock->getBaseAddress() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Range [AUB]:</strong> " + testAddressBlock->getRange() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Width [AUB]:</strong> " + testAddressBlock->getWidth() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Access:</strong> " + AccessTypes::access2Str(testAddressBlock->getAccess()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Volatile:</strong> " + testAddressBlock->getVolatile() + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h4>Address block '" + testAddressBlock->name() + "' contains the following registers:</h4>\n"
        "\t\t\t" + getTableString() + "\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Register name</th>\n"
        "\t\t\t\t\t<th>Offset [AUB]</th>\n"
        "\t\t\t\t\t<th>Size [bits]</th>\n"
        "\t\t\t\t\t<th>Dimension</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + testRegister->name() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getAddressOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getSize() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getDimension() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getVolatile() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testRegister->getAccess()) + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + testRegister2->name() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister2->getAddressOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister2->getSize() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister2->getDimension() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister2->getVolatile() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testRegister2->getAccess()) + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testExpressionsInAddressBlocks()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testExpressionsInAddressBlocks()
{
    QSharedPointer <Parameter> targetParameter = createTestParameter("target", "4", "", "ID_TARGET", "", "");

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> testAddressBlock = createTestAddressBlock("addressBlock", "example", "'h0",
        "ID_TARGET", "ID_TARGET + 2", QList <QSharedPointer <Register> > ());
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
        "\t\t\t<h3>1.1.1.1 Address block " + testAddressBlock->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testAddressBlock->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Usage:</strong> " + General::usage2Str(testAddressBlock->getUsage()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Base address [AUB]:</strong> 'h0<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Range [AUB]:</strong> target<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Width [AUB]:</strong> target + 2<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Access:</strong> " + AccessTypes::access2Str(testAddressBlock->getAccess()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Volatile:</strong> " + testAddressBlock->getVolatile() + "<br>\n"
        "\t\t\t</p>\n"
        );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testRegistersWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testRegistersWrittenWithTopComponent()
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
        "\t\t\t<h3>1.1.1.1.1 Register " + testRegister->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testRegister->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Offset [AUB]:</strong> " + testRegister->getAddressOffset() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Size [bits]:</strong> " + testRegister->getSize() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Dimension:</strong> " + testRegister->getDimension() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Volatile:</strong> " + testRegister->getVolatile() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Access:</strong> " + AccessTypes::access2Str(testRegister->getAccess()) + "<br>\n"
        "\t\t\t</p>\n"
        );

    checkOutputFile(expectedOutput);
}

void tst_DocumentGenerator::testAddressBlockRegisterFilesWrittenWithTopComponent()
{
    QSharedPointer<RegisterFile> testRegisterFileParent(new RegisterFile("testRegisterFileParent", "8", "16"));
    QSharedPointer<RegisterFile> testRegisterFileChild(new RegisterFile("testRegisterFileChild", "64", "512"));

    testRegisterFileParent->setDescription("this is a description");

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

    QString expectedOutput(
        "\t\t\t<h3>1.1.1.1 Address block testAddressBlock</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testAddressBlock->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Usage:</strong> " + General::usage2Str(testAddressBlock->getUsage()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Base address [AUB]:</strong> 'h0<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Range [AUB]:</strong> 4<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Width [AUB]:</strong> 32<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Access:</strong> " + AccessTypes::access2Str(testAddressBlock->getAccess()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Volatile:</strong> " + testAddressBlock->getVolatile() + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h4>Address block 'testAddressBlock' contains the following register files:</h4>\n"
        "\t\t\t<h3>1.1.1.1.1 Register file testRegisterFileParent</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> this is a description<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Offset [AUB]:</strong> " + testRegisterFileParent->getAddressOffset() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Range [AUB]:</strong> " + testRegisterFileParent->getRange() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Dimension:</strong> <br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h4>Register file testRegisterFileParent contains the following registers:</h4>\n"
        "\t\t\t" + getTableString() + "\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Register name</th>\n"
        "\t\t\t\t\t<th>Offset [AUB]</th>\n"
        "\t\t\t\t\t<th>Size [bits]</th>\n"
        "\t\t\t\t\t<th>Dimension</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + registerInParentRegisterFile->name() + "</td>\n"
        "\t\t\t\t\t<td>" + registerInParentRegisterFile->getAddressOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + registerInParentRegisterFile->getSize() + "</td>\n"
        "\t\t\t\t\t<td>" + registerInParentRegisterFile->getDimension() + "</td>\n"
        "\t\t\t\t\t<td>" + registerInParentRegisterFile->getVolatile() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(registerInParentRegisterFile->getAccess()) + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        "\t\t\t<h3>1.1.1.1.1.1 Register testRegister</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + registerInParentRegisterFile->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Offset [AUB]:</strong> " + registerInParentRegisterFile->getAddressOffset() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Size [bits]:</strong> " + registerInParentRegisterFile->getSize() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Dimension:</strong> " + registerInParentRegisterFile->getDimension() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Volatile:</strong> " + registerInParentRegisterFile->getVolatile() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Access:</strong> " + AccessTypes::access2Str(registerInParentRegisterFile->getAccess()) + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h3>1.1.1.1.1.2 Register file testRegisterFileChild</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testRegisterFileChild->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Offset [AUB]:</strong> " + testRegisterFileChild->getAddressOffset() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Range [AUB]:</strong> " + testRegisterFileChild->getRange() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Dimension:</strong> <br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h4>Register file testRegisterFileChild contains the following registers:</h4>\n"
        "\t\t\t" + getTableString() + "\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Register name</th>\n"
        "\t\t\t\t\t<th>Offset [AUB]</th>\n"
        "\t\t\t\t\t<th>Size [bits]</th>\n"
        "\t\t\t\t\t<th>Dimension</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + registerInChildRegisterFile->name() + "</td>\n"
        "\t\t\t\t\t<td>" + registerInChildRegisterFile->getAddressOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + registerInChildRegisterFile->getSize() + "</td>\n"
        "\t\t\t\t\t<td>" + registerInChildRegisterFile->getDimension() + "</td>\n"
        "\t\t\t\t\t<td>" + registerInChildRegisterFile->getVolatile() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(registerInChildRegisterFile->getAccess()) + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        "\t\t\t<h3>1.1.1.1.1.2.1 Register testRegister2</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + registerInChildRegisterFile->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Offset [AUB]:</strong> " + registerInChildRegisterFile->getAddressOffset() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Size [bits]:</strong> " + registerInChildRegisterFile->getSize() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Dimension:</strong> " + registerInChildRegisterFile->getDimension() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Volatile:</strong> " + registerInChildRegisterFile->getVolatile() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Access:</strong> " + AccessTypes::access2Str(registerInChildRegisterFile->getAccess()) + "<br>\n"
        "\t\t\t</p>\n"
    );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testFieldsWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testFieldsWrittenWithTopComponent()
{
    QSharedPointer<Field> testField = createTestField("testField", "Example Field", "2", "8");    
    QSharedPointer<FieldReset> resetValue(new FieldReset());
    resetValue->setResetValue("testReset");
    resetValue->setResetMask("testMask");

    testField->getResets()->append(resetValue);

    QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > enumerations(new QList<QSharedPointer <EnumeratedValue > >());
    QSharedPointer<EnumeratedValue> testEnumeratedValue(new EnumeratedValue("testEnumeration", "1"));
    enumerations->append(testEnumeratedValue);

    testField->setEnumeratedValues(enumerations);

    QSharedPointer<Register> fieldRegister =createTestRegister("FieldRegister", "10", "10", "10", "");
    fieldRegister->getFields()->append(testField);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generator->writeFields(fieldRegister, stream, { 1, 1, 1, 1, 1 });

    targetFile.close();

    QString expectedOutput(
        "\t\t\t<h4>Register '" + fieldRegister->name() + "' contains the following fields:</h4>\n"
        "\t\t\t" + getTableString() + "List of fields contained within register " + fieldRegister->name() +
        ".\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Field name</th>\n"
        "\t\t\t\t\t<th>Offset [bits]</th>\n"
        "\t\t\t\t\t<th>Width [bits]</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t\t<th>Resets</th>\n"        
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td><a id=\"" + topComponent_->getVlnv().toString() + ".field." + testField->name() +
        "\">" + testField->name() + "</a></td>\n"
        "\t\t\t\t\t<td>" + testField->getBitOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + testField->getBitWidth() + "</td>\n"
        "\t\t\t\t\t<td>" + testField->getVolatile().toString() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testField->getAccess()) + "</td>\n"
        "\t\t\t\t\t<td>HARD : testReset</td>\n"        
        "\t\t\t\t\t<td>" + testField->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        "\t\t\t<h3>1.1.1.1.1.1 Field " + testField->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Offset [bits]:</strong> " + testField->getBitOffset() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Width [bits]:</strong> " + testField->getBitWidth() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Volatile:</strong> " + testField->getVolatile().toString() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Access:</strong> " + AccessTypes::access2Str(testField->getAccess()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Resets:</strong> HARD : testReset<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testField->description() + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h4>Enumerations:</h4>\n"
        "\t\t\t" + getTableString() + "\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Value</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>testEnumeration</td>\n"
        "\t\t\t\t\t<td>1</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testMemoryMapToFieldWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testMemoryMapToFieldWrittenWithTopComponent()
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

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".memoryMaps\">1.1 Memory maps</a></h2>\n"
        "\t\t\t<h3>1.1.1 Memory map " + testMemoryMap->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " +
        testMemoryMap->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Address unit bits (AUB):</strong> " +
        testMemoryMap->getAddressUnitBits() + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h3>1.1.1.1 Address block " + testAddressBlock->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testAddressBlock->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Usage:</strong> " + General::usage2Str(testAddressBlock->getUsage()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Base address [AUB]:</strong> " + testAddressBlock->getBaseAddress() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Range [AUB]:</strong> " + testAddressBlock->getRange() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Width [AUB]:</strong> " + testAddressBlock->getWidth() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Access:</strong> " + AccessTypes::access2Str(testAddressBlock->getAccess()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Volatile:</strong> " + testAddressBlock->getVolatile() + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h4>Address block '" + testAddressBlock->name() + "' contains the following registers:</h4>\n"
        "\t\t\t" + getTableString() +"\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Register name</th>\n"
        "\t\t\t\t\t<th>Offset [AUB]</th>\n"
        "\t\t\t\t\t<th>Size [bits]</th>\n"
        "\t\t\t\t\t<th>Dimension</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + testRegister->name() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getAddressOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getSize() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getDimension() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getVolatile() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testRegister->getAccess()) + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        "\t\t\t<h3>1.1.1.1.1 Register " + testRegister->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testRegister->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Offset [AUB]:</strong> " + testRegister->getAddressOffset() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Size [bits]:</strong> " + testRegister->getSize() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Dimension:</strong> " + testRegister->getDimension() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Volatile:</strong> " + testRegister->getVolatile() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Access:</strong> " + AccessTypes::access2Str(testRegister->getAccess()) + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h4>Register '" + testRegister->name() + "' contains the following fields:</h4>\n"
        "\t\t\t" + getTableString() + "List of fields contained within register " + testRegister->name() +
        ".\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Field name</th>\n"
        "\t\t\t\t\t<th>Offset [bits]</th>\n"
        "\t\t\t\t\t<th>Width [bits]</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t\t<th>Resets</th>\n"        
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td><a id=\"" + topComponent_->getVlnv().toString() + ".field." +
        testField->name() + "\">" + testField->name() + "</a></td>\n"
        "\t\t\t\t\t<td>" + testField->getBitOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + testField->getBitWidth() + "</td>\n"
        "\t\t\t\t\t<td>" + testField->getVolatile().toString() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testField->getAccess()) + "</td>\n"
        "\t\t\t\t\t<td>HARD : 8'h3</td>\n"        
        "\t\t\t\t\t<td>" + testField->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testPortsWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testPortsWrittenWithOnlyTopComponent()
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
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".ports\">1.1 Ports</a></h2>\n"
        "\t\t\t" + getTableString() + "List of all ports the component has.\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Direction</th>\n"
        "\t\t\t\t\t<th>Left bound</th>\n"
        "\t\t\t\t\t<th>Right bound</th>\n"
        "\t\t\t\t\t<th>Port type</th>\n"
        "\t\t\t\t\t<th>Type definition</th>\n"
        "\t\t\t\t\t<th>Default value</th>\n"
        "\t\t\t\t\t<th>Array left</th>\n"
        "\t\t\t\t\t<th>Array right</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td><a id=\"" + topComponent_->getVlnv().toString() + ".port." + portRef->name() + "\">" +
        portRef->name() + "</a></td>\n"
        "\t\t\t\t\t<td>" + DirectionTypes::direction2Str(portRef->getDirection()) + "</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td>" + portRef->getRightBound() + "</td>\n"
        "\t\t\t\t\t<td>" + portRef->getTypeName() + "</td>\n"
        "\t\t\t\t\t<td>" + portRef->getTypeDefinition(portRef->getTypeName()) + "</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td>" + portRef->getArrayLeft() + "</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td>" + portRef->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n");

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testBusInterfacesWrittenWithoutPorts()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testBusInterfacesWrittenWithoutPorts()
{
    QSharedPointer <BusInterface> busInterface (new BusInterface);
    busInterface->setName("interface");
    busInterface->setInterfaceMode(General::MASTER);
    
    QSharedPointer <BusInterface> busInterfaceWithSystemMode (new BusInterface);
    busInterfaceWithSystemMode->setName("interface2");
    busInterfaceWithSystemMode->setInterfaceMode(General::SYSTEM);
    busInterfaceWithSystemMode->setSystem("systemGroup");
    
    VLNV firstVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "FirstComponent", "1.0");
    QSharedPointer<ConfigurableVLNVReference> absDefVlnv(new ConfigurableVLNVReference(firstVlnv));

    QSharedPointer<AbstractionType> absDef(new AbstractionType);
    absDef->setAbstractionRef(absDefVlnv);

    VLNV secondVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "SecondComponent", "1.0");
    QSharedPointer<ConfigurableVLNVReference> absDefVlnv2(new ConfigurableVLNVReference(secondVlnv));

    QSharedPointer<AbstractionType> absDef2(new AbstractionType);
    absDef2->setAbstractionRef(absDefVlnv2);

    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionDefs(new QList<QSharedPointer<AbstractionType> >);
    abstractionDefs->append(absDef);
    abstractionDefs->append(absDef2);

    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionDefs2(new QList<QSharedPointer<AbstractionType> >);
    abstractionDefs2->append(absDef2);

    busInterface->setAbstractionTypes(abstractionDefs);
    busInterfaceWithSystemMode->setAbstractionTypes(abstractionDefs2);

    VLNV firstBusDefVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "FirstBusDef", "1.0");
    ConfigurableVLNVReference busDef1(firstBusDefVlnv);
    
    VLNV secondBusDefVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "SecondBusDef", "1.0");
    ConfigurableVLNVReference busDef2(secondBusDefVlnv);

    busInterface->setBusType(busDef1);
    busInterfaceWithSystemMode->setBusType(busDef2);

    topComponent_->getBusInterfaces()->append(busInterface);
    topComponent_->getBusInterfaces()->append(busInterfaceWithSystemMode);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeInterfaces(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".interfaces\">1.1 Bus interfaces</a></h2>\n"
        "\t\t\t<h3>1.1.1 Bus interface " + busInterface->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + busInterface->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Interface mode:</strong> " +
        General::interfaceMode2Str(busInterface->getInterfaceMode()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Bus definition:</strong> Test:TestLibrary:FirstBusDef:1.0<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Abstraction definitions:</strong> Test:TestLibrary:FirstComponent:1.0, Test:TestLibrary:SecondComponent:1.0<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Ports used in this interface:</strong> None<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h3>1.1.2 Bus interface " + busInterfaceWithSystemMode->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + busInterfaceWithSystemMode->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Interface mode:</strong> system<br>\n"
        "\t\t\t" + getIndentString() + "<strong>System group:</strong> systemGroup<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Bus definition:</strong> Test:TestLibrary:SecondBusDef:1.0<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Abstraction definitions:</strong> Test:TestLibrary:SecondComponent:1.0<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Ports used in this interface:</strong> None<br>\n"
        "\t\t\t</p>\n"
        );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testFileSetsWrittenForTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testFileSetsWrittenForTopComponent()
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
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".fileSets\">1.1 File sets</a></h2>\n"
        "\t\t\t<h3>1.1.1 File set " + testFileSet->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testFileSet->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Identifiers:</strong> " + groups + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Default file builders:</strong>\n"
        "\t\t\t</p>\n"
        "\t\t\t" + getTableString() + "Default file build commands" + "\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>File type</th>\n"
        "\t\t\t\t\t<th>Command</th>\n"
        "\t\t\t\t\t<th>Flags</th>\n"
        "\t\t\t\t\t<th>Replace default flags</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + testFileBuilder->getFileType().type_ + "</td>\n"
        "\t\t\t\t\t<td>" + testFileBuilder->getCommand() + "</td>\n"
        "\t\t\t\t\t<td>" + testFileBuilder->getFlags() + "</td>\n"
        "\t\t\t\t\t<td>" + testFileBuilder->getReplaceDefaultFlags() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        "\t\t\t<h4>1.1.1.1 Files</h4>\n"
        "\t\t\t" + getTableString() + "List of files contained in this file set." + "\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>File name</th>\n"
        "\t\t\t\t\t<th>Logical name</th>\n"
        "\t\t\t\t\t<th>Build command</th>\n"
        "\t\t\t\t\t<th>Build flags</th>\n"
        "\t\t\t\t\t<th>Specified file types</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td><a href=\"" + testFile1PathFromDoc + "\">" + testFile1->name() + "</a></td>\n"
        "\t\t\t\t\t<td>" + testFile1->getLogicalName() + "</td>\n"
        "\t\t\t\t\t<td>" + testFile1->getBuildCommand()->getCommand() + "</td>\n"
        "\t\t\t\t\t<td>" + testFile1->getBuildCommand()->getFlags() + "</td>\n"
        "\t\t\t\t\t<td>" + testFile1->getFileTypeNames().join(",<br>") + "</td>\n"
        "\t\t\t\t\t<td>" + testFile1->getDescription() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td><a href=\"" + testFile2PathFromDoc + "\">" + testFile2->name() + "</a></td>\n"
        "\t\t\t\t\t<td>" + testFile2->getLogicalName() + "</td>\n"
        "\t\t\t\t\t<td>" + testFile2->getBuildCommand()->getCommand() + "</td>\n"
        "\t\t\t\t\t<td>" + testFile2->getBuildCommand()->getFlags() + "</td>\n"
        "\t\t\t\t\t<td>" + testFile2->getFileTypeNames().join(",<br>") + "</td>\n"
        "\t\t\t\t\t<td>" + testFile2->getDescription() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
    );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testViewsWrittenForTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testViewsWrittenForTopComponent()
{
    QSharedPointer<View> flatView(new View("testView"));

    topComponent_->getViews()->append(flatView);

    VLNV firstVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "FirstComponent", "1.0");
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(new ConfigurableVLNVReference(firstVlnv));
    QSharedPointer<ComponentInstance> firstInstance(new ComponentInstance("firstInstance", instanceVLNV));

    QSharedPointer<Component> refComponent = QSharedPointer<Component>(new Component(firstVlnv, Document::Revision::Std14));

    QList <QSharedPointer<Parameter> > componentParameters;

    QSharedPointer<Parameter> targetParameter = createTestParameter("firstParameter", "10",
        "", "ID_TARGET", "", "");

    QSharedPointer<ModuleParameter> testModuleParameter(new ModuleParameter);
    testModuleParameter->setName("testModuleParameter");
    testModuleParameter->setValue("1");
    testModuleParameter->setDescription("module param description");
    testModuleParameter->setDataType("testDataType");
    testModuleParameter->setType("int");
    testModuleParameter->setUsageType("typed");

    QSharedPointer<ComponentInstantiation> componentInstantiation(new ComponentInstantiation());
    componentInstantiation->setName("testInstantiation");
    componentInstantiation->getParameters()->append(targetParameter);
    componentInstantiation->getModuleParameters()->append(testModuleParameter);
    componentInstantiation->setLanguage("C");
    componentInstantiation->setLanguageStrictness(true);
    componentInstantiation->setLibraryName("testLibrary");
    componentInstantiation->setPackageName("testPackage");
    componentInstantiation->setModuleName("testModuleName");


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
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".views\">1.1 Views</a></h2>\n"
        "\t\t\t<h3>1.1.1 View: testView</h3>\n"
        "\t\t\t<h4>1.1.1.1 Component instantiation: testInstantiation</h4>\n"
        "\t\t\t<p>\n"
        "\t\t\t\t&nbsp;&nbsp;&nbsp;<strong>Language: </strong>C <strong>strict</strong><br>\n" 
        "\t\t\t\t&nbsp;&nbsp;&nbsp;<strong>Library: </strong>testLibrary<br>\n" 
        "\t\t\t\t&nbsp;&nbsp;&nbsp;<strong>Package: </strong>testPackage<br>\n" 
        "\t\t\t\t&nbsp;&nbsp;&nbsp;<strong>Module name: </strong>testModuleName<br>\n" 
        "\t\t\t</p>\n"
        "\t\t\t<p><strong>Module parameters:</strong></p>\n"
        "\t\t\t<table frame=\"box\" rules=\"all\" border=\"1\" cellPadding=\"3\" title=\"\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Type</th>\n"
        "\t\t\t\t\t<th>Value</th>\n"
        "\t\t\t\t\t<th>Data type</th>\n"
        "\t\t\t\t\t<th>Usage type</th>\n"
        "\t\t\t\t\t<th>Resolve</th>\n"
        "\t\t\t\t\t<th>Bit vector left</th>\n"
        "\t\t\t\t\t<th>Bit vector right</th>\n"
        "\t\t\t\t\t<th>Array left</th>\n"
        "\t\t\t\t\t<th>Array right</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>testModuleParameter</td>\n"
        "\t\t\t\t\t<td>int</td>\n"
        "\t\t\t\t\t<td>1</td>\n"
        "\t\t\t\t\t<td>testDataType</td>\n"
        "\t\t\t\t\t<td>typed</td>\n"
        "\t\t\t\t\t<td>" + testModuleParameter->getValueResolve() + "</td>\n"
        "\t\t\t\t\t<td>" + testModuleParameter->getVectorLeft() + "</td>\n"
        "\t\t\t\t\t<td>" + testModuleParameter->getVectorRight() + "</td>\n"
        "\t\t\t\t\t<td>" + testModuleParameter->getArrayLeft() + "</td>\n"
        "\t\t\t\t\t<td>" + testModuleParameter->getArrayRight() + "</td>\n"
        "\t\t\t\t\t<td>" + testModuleParameter->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        "\t\t\t<p><strong>Parameters:</strong></p>\n"
        "\t\t\t<table frame=\"box\" rules=\"all\" border=\"1\" cellPadding=\"3\" title=\"\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Type</th>\n"
        "\t\t\t\t\t<th>Value</th>\n"
        "\t\t\t\t\t<th>Resolve</th>\n"
        "\t\t\t\t\t<th>Bit vector left</th>\n"
        "\t\t\t\t\t<th>Bit vector right</th>\n"
        "\t\t\t\t\t<th>Array left</th>\n"
        "\t\t\t\t\t<th>Array right</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>firstParameter</td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td>10</td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testHierarchicalDesignIsWritten()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testDesignIsWritten()
{
    VLNV designVlnv(VLNV::DESIGN, "Test", "TestLibrary", "TestDesign", "1.0");
    QSharedPointer<Design> design = QSharedPointer<Design>(new Design(designVlnv, Document::Revision::Std14));
    library_.writeModelToFile("C:/Test/TestLibrary/TestDesign/1.0/TestDesign.1.0.xml", design);
    library_.addComponent(design);

    VLNV firstVlnv (VLNV::COMPONENT, "Test", "TestLibrary", "FirstComponent", "1.0");
    QSharedPointer<Component> refComponent = QSharedPointer<Component>(new Component(firstVlnv, Document::Revision::Std14));

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

    topComponent_->getDesignInstantiations()->append(designInstantiation);

    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;
    QStringList files;

    generator->writeViews(stream, subHeaderNumber, files);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"Test:TestLibrary:TestComponent:1.0.views\">1.1 Views</a></h2>\n"
        "\t\t\t<h3>1.1.1 View: HierarchicalView</h3>\n"
        "\t\t\t<h4>1.1.1.1 Design instantiation: design_instantiation</h4>\n"
        "\t\t\t<p>\n"
        "\t\t\t\t&nbsp;&nbsp;&nbsp;<strong>Design: </strong>Test:TestLibrary:TestDesign:1.0<br>\n"
        "\t\t\t\t&nbsp;&nbsp;&nbsp;<strong>IP-Xact file: </strong><a href=\"\">TestDesign.1.0.xml</a><br>\n"
        "\t\t\t</p>\n"
        "\t\t\tDiagram of design Test:TestLibrary:TestDesign:1.0:<br>\n"
        "\t\t\t<img src=\"Test.TestLibrary.TestComponent.1.0.HierarchicalView.png\" alt=\"View: HierarchicalView preview picture\"><br>\n"
        "\t\t\t<br>\n"
        );

    readOutputFile();

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::testEndOfDocumentWrittenForTopComponent()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::testEndOfDocumentWrittenForTopComponent()
{
    QScopedPointer<DocumentGenerator> generator(createTestGenerator());

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generator->writeEndOfDocument(stream);

    targetFile.close();

    QString expectedOutput(
        "\t</body>\n"
        "</html>\n"
        );

    checkOutputFile(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::createTestGenerator()
//-----------------------------------------------------------------------------
DocumentGenerator* tst_DocumentGenerator::createTestGenerator()
{
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_);
    library_.addComponent(topComponent_);

    int componentNumber = 1;

    DocumentGenerator* generator (new DocumentGenerator(&library_, topComponentVlnv_, &designWidgetFactory_, 
        &expressionFormatterFactory_, componentNumber, generatorParentWidget_));

    generator->setFormat(DocumentGenerator::HTML);
    return generator;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::readOutputFile()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::readOutputFile()
{
    QFile outputFile(targetPath_);

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    output_ = outputFile.readAll();
    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::checkOutputFile()
//-----------------------------------------------------------------------------
void tst_DocumentGenerator::checkOutputFile(QString const& expectedOutput)
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

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::createTestParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> tst_DocumentGenerator::createTestParameter(QString const& name, QString const& value,
    QString const& description, QString const& uuID, QString const& arrayLeft, QString const& arrayRight)
{
    QSharedPointer<Parameter> parameter (new Parameter);
    parameter->setName(name);
    parameter->setValue(value);
    parameter->setDescription(description);
    parameter->setValueId(uuID);
    parameter->setArrayLeft(arrayLeft);
    parameter->setArrayRight(arrayRight);

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::createTestPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> tst_DocumentGenerator::createTestPort(QString const& name, QString const& leftBound,
    QString const& rightBound, QString const& defaultValue, QString const& arrayLeft, QString const& arrayRight)
{
    QSharedPointer<Port> newPort (new Port);
    newPort->setDirection(DirectionTypes::DIRECTION_PHANTOM);
    newPort->setName(name);
    newPort->setLeftBound(leftBound);
    newPort->setRightBound(rightBound);
    newPort->setDefaultValue(defaultValue);
    newPort->setArrayLeft(arrayLeft);
    newPort->setArrayRight(arrayRight);

    return newPort;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::createConfigurableElementvalues()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ConfigurableElementValue> > tst_DocumentGenerator::createConfigurableElementvalues(QSharedPointer <Component> component)
{    
    ExpressionFormatter* refExpressionFormatter = expressionFormatterFactory_.makeExpressionFormatter(component);

    QList<QSharedPointer<ConfigurableElementValue> > instanceConfigurableElementValues;

    foreach (QSharedPointer<Parameter> parameter, *component->getParameters())
    {
        instanceConfigurableElementValues.append(QSharedPointer<ConfigurableElementValue>(
            new ConfigurableElementValue(refExpressionFormatter->formatReferringExpression(parameter->getValue()), parameter->name())));
    }

    delete refExpressionFormatter;
    refExpressionFormatter = 0;

    return instanceConfigurableElementValues;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::createTestField()
//-----------------------------------------------------------------------------
QSharedPointer <Field> tst_DocumentGenerator::createTestField(QString const& name, QString const& description,
    QString const& offset, QString const& width)
{
    QSharedPointer <Field> testField (new Field);
    testField->setName(name);
    testField->setDescription(description);
    testField->setBitOffset(offset);
    testField->setBitWidth(width);
    testField->setVolatile(false);
    testField->setAccess(AccessTypes::READ_ONLY);

    return testField;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::createTestRegister()
//-----------------------------------------------------------------------------
QSharedPointer <Register> tst_DocumentGenerator::createTestRegister(QString const& name, QString const& offset,
    QString const& dimension, QString const& size, QString const& description)
{
    QSharedPointer<Register> testRegister (new Register);
    testRegister->setName(name);
    testRegister->setAddressOffset(offset);
    testRegister->setSize(size);
    testRegister->setDimension(dimension);
    testRegister->setDescription(description);
    testRegister->setVolatile(true);
    testRegister->setAccess(AccessTypes::READ_WRITE);
    //testRegister->setRegisterValue("2");
    //testRegister->setRegisterMask("4");

    return testRegister;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::createTestAddressBlock()
//-----------------------------------------------------------------------------
QSharedPointer <AddressBlock> tst_DocumentGenerator::createTestAddressBlock(QString const& name,
    QString const& description, QString const& baseAddress, QString const& range, QString const& width,
    QList<QSharedPointer<Register> > registers)
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

    foreach (QSharedPointer<RegisterBase> testRegister, registers)
    {
        testAddressBlock->getRegisterData()->append(testRegister);
    }

    return testAddressBlock;
}

QSharedPointer<AddressBlock> tst_DocumentGenerator::createAddressBlockWithRegisterData(QString const& name,
    QString const& description, QString const& baseAddress, QString const& range,
    QString const& width, QList<QSharedPointer<RegisterBase>> registerData)
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

    for (QSharedPointer<RegisterBase> testRegisterData : registerData)
    {
        testAddressBlock->getRegisterData()->append(testRegisterData);
    }

    return testAddressBlock;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::createTestMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer <MemoryMap> tst_DocumentGenerator::createTestMemoryMap(QString const& name,
    QString const& descprition, int addressUnitbits, QList<QSharedPointer<AddressBlock> > addressBlocks)
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap);
    memoryMap->setName(name);
    memoryMap->setAddressUnitBits(QString::number(addressUnitbits));
    memoryMap->setDescription(descprition);

    foreach (QSharedPointer <AddressBlock> includedAddressBlock, addressBlocks)
    {
        memoryMap->getMemoryBlocks()->append(includedAddressBlock);
    }

    return memoryMap;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::getSpaceString()
//-----------------------------------------------------------------------------
QString tst_DocumentGenerator::getSpaceString()
{
    return QString("&nbsp;");
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::getIndentString()
//-----------------------------------------------------------------------------
QString tst_DocumentGenerator::getIndentString()
{
    return QString("&nbsp;&nbsp;&nbsp;");
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::getTableString()
//-----------------------------------------------------------------------------
QString tst_DocumentGenerator::getTableString()
{
    return QString("<table frame=\"box\" rules=\"all\" border=\"1\" cellPadding=\"3\" title=\"");
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::getEncodingString()
//-----------------------------------------------------------------------------
QString tst_DocumentGenerator::getEncodingString()
{
    return QString("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">");
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::getDoctypeString()
//-----------------------------------------------------------------------------
QString tst_DocumentGenerator::getDoctypeString()
{
    return QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">");
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentGenerator::getValidW3CStrictString()
//-----------------------------------------------------------------------------
QString tst_DocumentGenerator::getValidW3CStrictString()
{
    return QString("\t\t<p>\n"
        "\t\t\t<a href=\"https://validator.w3.org/#validate_by_upload\">\n"
        "\t\t\t<img src=\"http://www.w3.org/Icons/valid-html401\""
        "alt=\"Valid HTML 4.01 Strict\" height=\"31\""
        "width=\"88\">\n"
        "\t\t\t</a>\n"
        "\t\t</p>\n");
}

QTEST_MAIN(tst_DocumentGenerator)

#include "tst_DocumentGenerator.moc"
