//-----------------------------------------------------------------------------
// File: tst_documentGenerator.cpp
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

#include <tests/MockObjects/LibraryMock.h>
#include <tests/MockObjects/DesignWidgetFactoryMock.h>
#include <tests/MockObjects/HWDesignWidgetMock.h>

#include <common/utils.h>

#include <IPXACTmodels/design.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/PortMap.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/view.h>

#include <QWidget>

class tst_documentGenerator : public QObject
{
    Q_OBJECT

public:
    tst_documentGenerator();

private slots:
    
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    //! Test cases.

    void testFileHeaderIsWritten();
    void testTableOfContentsIsWrittenWithOnlyTopComponent();

    void testModelParametersWrittenWithOnlyTopComponent();
    void testModelParametersWithReferences();

    void testParametersWrittenWithOnlyTopComponent();

    void testPortsWrittenWithOnlyTopComponent();

    void testBusInterfacesWrittenWithoutPorts();

    void testFileSetsWrittenForTopComponent();

    void testViewsWrittenForTopComponent();

    void testEndOfDocumentWrittenForTopComponent();

private:
    void readOutputFile();

    QSharedPointer<ModelParameter> createTestModelParameter(QString const& name, QString const& dataType,
        QString const& value, QString const& description, QString const& uuID);

    QSharedPointer<Parameter> createTestParameter(QString const& name, QString const& value,
        QString const& description, QString const& uuID);

    QSharedPointer<Port> createTestPort(QString const& name, QString const& leftBound, QString const& rightBound,
        QString const& defaultValue);

    QString getSpaceString();

    QString getIndentString();

    QString getTableString();

    QString getEncodingString();

    QString getDoctypeString();

    QString getValidW3CStrictString();

    DocumentGenerator* createTestGenerator();

    QSharedPointer<Component> topComponent_;

    QString output_;

    QDateTime generationTime_;

    LibraryMock library_;

    DesignWidgetFactoryMock designWidgetFactory_;

    QWidget* generatorParentWidget_;
};

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::tst_documentGenerator()
//-----------------------------------------------------------------------------
tst_documentGenerator::tst_documentGenerator() :
topComponent_(), output_(), generationTime_(), library_(this), designWidgetFactory_(&library_),
    generatorParentWidget_(new QWidget)
{

}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_documentGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_documentGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::init()
//-----------------------------------------------------------------------------
void tst_documentGenerator::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    topComponent_ = QSharedPointer<Component>(new Component(vlnv));

    library_.clear();

    QVERIFY(!QFile::exists("./generatorOutput.html"));
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_documentGenerator::cleanup()
{
    topComponent_.clear();
    output_.clear();

    QFile::remove("./generatorOutput.html");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testFileHeaderIsPrinted()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testFileHeaderIsWritten()
{
    QCoreApplication::setOrganizationName("TUT");
    QCoreApplication::setApplicationName("TestRunner");
    QSettings settings;
    settings.setValue("General/Username", "testUser");

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);
    
    generationTime_ = QDateTime::currentDateTime();

    generator->writeHtmlHeader(stream);

    targetFile.close();

    QString expectedOutput(getDoctypeString() + "\n"
    "<html>\n"
        "\t<head>\n"
        "\t" + getEncodingString() + "\n"
            "\t\t<title>Kactus2 generated documentation for component " + topComponent_->getVlnv()->getName() +
            " " + topComponent_->getVlnv()->getVersion() + "</title>\n"
        "\t</head>\n"
        "\t<body>\n"
            "\t\t<h6>This document was generated by Kactus2 on " +
            generationTime_.toString("dd.MM.yyyy hh:mm:ss") + " by user testUser</h6>\n");

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
// Function: tst_documentGenerator::testTableOfContentsIsWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testTableOfContentsIsWrittenWithOnlyTopComponent()
{
    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    unsigned int runningNumber = 0;

    generator->writeTableOfContents(runningNumber, stream);

    targetFile.close();

    QString expectedOutput(
        "\t\t<a href=\"#" + topComponent_->getVlnv()->toString() + "\">1. Component" + getSpaceString() +
            topComponent_->getVlnv()->getVendor() + " - " + topComponent_->getVlnv()->getLibrary() + " - " +
            topComponent_->getVlnv()->getName() + " - " + topComponent_->getVlnv()->getVersion() + "</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv()->toString() +
            ".kts_params\">1.1. Kactus2 attributes</a><br>\n");

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
// Function: tst_documentGenerator::testModelParametersWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testModelParametersWrittenWithOnlyTopComponent()
{
    QSharedPointer<ModelParameter> modelParameter = createTestModelParameter("modelParam", "integer", "14", "",
        "MODEL-ID");
    topComponent_->getModel()->addModelParameter(modelParameter);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeModelParameters(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv()->toString() + ".modelParams\">0.1 Model parameters</a></h2>\n"
            "\t\t\t" + getTableString() + "List of model parameters defined for the component\">\n"
                "\t\t\t\t<tr>\n"
                    "\t\t\t\t\t<th>Name</th>\n"
                    "\t\t\t\t\t<th>Data type</th>\n"
                    "\t\t\t\t\t<th>Default value</th>\n"
                    "\t\t\t\t\t<th>Description</th>\n"
                "\t\t\t\t</tr>\n"
                "\t\t\t\t<tr>\n"
                    "\t\t\t\t\t<td>" + modelParameter->getName() + "</td>\n"
                    "\t\t\t\t\t<td>" + modelParameter->getDataType() + "</td>\n"
                    "\t\t\t\t\t<td>" + modelParameter->getValue() + "</td>\n"
                    "\t\t\t\t\t<td>" + modelParameter->getDescription() + "</td>\n"
                "\t\t\t\t</tr>\n"
            "\t\t\t</table>\n");

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
// Function: tst_documentGenerator::testModelParametersWithReferences()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testModelParametersWithReferences()
{
    QSharedPointer <ModelParameter> modelParameter = createTestModelParameter("modelParam", "integer", "14", "",
        "ID-model");
    topComponent_->getModel()->addModelParameter(modelParameter);

    QSharedPointer <ModelParameter> refParameter = createTestModelParameter("refParam", "integer", "ID-model", "",
        "ID-ref");
    topComponent_->getModel()->addModelParameter(refParameter);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeModelParameters(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv()->toString() + ".modelParams\">0.1 Model parameters</a></h2>\n"
        "\t\t\t" + getTableString() + "List of model parameters defined for the component\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Data type</th>\n"
        "\t\t\t\t\t<th>Default value</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + modelParameter->getName() + "</td>\n"
        "\t\t\t\t\t<td>" + modelParameter->getDataType() + "</td>\n"
        "\t\t\t\t\t<td>" + modelParameter->getValue() + "</td>\n"
        "\t\t\t\t\t<td>" + modelParameter->getDescription() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + refParameter->getName() + "</td>\n"
        "\t\t\t\t\t<td>" + refParameter->getDataType() + "</td>\n"
        "\t\t\t\t\t<td>modelParam</td>\n"
        "\t\t\t\t\t<td>" + refParameter->getDescription() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n");

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
// Function: tst_documentGenerator::testParametersWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testParametersWrittenWithOnlyTopComponent()
{
    QSharedPointer <Parameter> parameter = createTestParameter("parameter", "10", "Description", "ID-parameter");
    QSharedPointer <Parameter> refParameter = createTestParameter("refParameter", "ID-parameter", "Describe this",
        "ID-refer");

    QList <QSharedPointer <Parameter> > componentParameters;
    componentParameters.append(parameter);
    componentParameters.append(refParameter);

    topComponent_->setParameters(componentParameters);
    topComponent_->setComponentHierarchy(KactusAttribute::KTS_GLOBAL);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeParameters(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv()->toString() + ".kts_params\">0.1 Kactus2 attributes</a></h2>\n"
        "\t\t<p>\n"
        "\t\t\t<strong>" + getIndentString() + "Product hierarchy: </strong>" +
        KactusAttribute::valueToString(topComponent_->getComponentHierarchy()) + "<br>\n"
        "\t\t\t<strong>" + getIndentString() + "Component implementation: </strong>" +
        KactusAttribute::valueToString(topComponent_->getComponentImplementation()) + "<br>\n"
        "\t\t\t<strong>" + getIndentString() + "Component firmness: </strong>" +
        KactusAttribute::valueToString(topComponent_->getComponentFirmness()) + "<br>\n"
        "\t\t</p>\n"
        "\t\t<h2><a id=\"" + topComponent_->getVlnv()->toString() + ".parameters\">0.2 General parameters</a></h2>\n"
        "\t\t\t" + getTableString() + "List of parameters defined for the component\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Value</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + parameter->getName() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getValue() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getDescription() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + refParameter->getName() + "</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td>" + refParameter->getDescription() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>"
        );

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
// Function: tst_documentGenerator::testPortsWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testPortsWrittenWithOnlyTopComponent()
{
    QList <QSharedPointer <Parameter> > componentParameters;
    QSharedPointer <Parameter> parameter = createTestParameter("parameter", "10", "Description", "ID-parameter");
    componentParameters.append(parameter);

    QSharedPointer <Port> portRef = createTestPort("portRef", "ID-parameter", "4", "ID-parameter");
    portRef->setLeftBound(10);
    portRef->setRightBound(4);

    topComponent_->addPort(portRef);

    topComponent_->setParameters(componentParameters);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;
    
    generator->writePorts(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv()->toString() + ".ports\">0.1 Ports</a></h2>\n"
        "\t\t\t" + getTableString() + "List of all ports the component has.\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Direction</th>\n"
        "\t\t\t\t\t<th>Width</th>\n"
        "\t\t\t\t\t<th>Left bound</th>\n"
        "\t\t\t\t\t<th>Right bound</th>\n"
        "\t\t\t\t\t<th>Port type</th>\n"
        "\t\t\t\t\t<th>Type definition</th>\n"
        "\t\t\t\t\t<th>Default value</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td><a id=\"" + topComponent_->getVlnv()->toString() + ".port." + portRef->getName() + "\">" +
        portRef->getName() + "</a></td>\n"
        "\t\t\t\t\t<td>" + General::direction2Str(portRef->getDirection()) + "</td>\n"
        "\t\t\t\t\t<td>" + QString::number(portRef->getPortSize()) + "</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td>" + portRef->getRightBoundExpression() + "</td>\n"
        "\t\t\t\t\t<td>" + portRef->getTypeName() + "Port type</td>\n"
        "\t\t\t\t\t<td>" + portRef->getTypeDefinition(portRef->getTypeName()) + "Type definition</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td>" + portRef->getDescription() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n");

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
// Function: tst_documentGenerator::testBusInterfacesWrittenWithoutPorts()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testBusInterfacesWrittenWithoutPorts()
{
    QSharedPointer <BusInterface> busInterface (new BusInterface);
    busInterface->setName("interface");
    busInterface->setInterfaceMode(General::MASTER);

    topComponent_->addBusInterface(busInterface);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeInterfaces(stream, subHeaderNumber);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv()->toString() + ".interfaces\">0.1 Bus interfaces</a></h2>\n"
        "\t\t\t<h3>0.1.1 " + busInterface->getName() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Interface mode:</strong> " +
        General::interfaceMode2Str(busInterface->getInterfaceMode()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Ports used in this interface:</strong>\n"
        );

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
// Function: tst_documentGenerator::testFileSetsWrittenForTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testFileSetsWrittenForTopComponent()
{
    DocumentGenerator* generator = createTestGenerator();

    QSharedPointer<FileSet> testFileSet (new FileSet);
    testFileSet->setName("testFileSet");
    testFileSet->setDescription("example description");
    testFileSet->setGroups("documentation");

    topComponent_->addFileSet(testFileSet);

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeFileSets(stream, subHeaderNumber);

    targetFile.close();

    QStringList groupIdentifiers = testFileSet->getGroups();
    QString groups;
    for (int i = 0; i < groupIdentifiers.size(); ++i)
    {
        groups.append(groupIdentifiers.at(i));
        if (i != groupIdentifiers.size() - 1)
        {
            groups.append(", ");
        }
    }

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv()->toString() + ".fileSets\">0.1 File sets</a></h2>\n"
        "\t\t\t<h3><a id=\"" + topComponent_->getVlnv()->toString() + ".fileSet." + testFileSet->getName() +
        "\">0.1.1 " + testFileSet->getName() + "</a></h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testFileSet->getDescription() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Identifiers:</strong> " + groups + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h4>" + getIndentString() + "0.1.1.1 Files</h4>\n"
        "\t\t\t" + getTableString() + "List of files contained in this file set.\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>File name</th>\n"
        "\t\t\t\t\t<th>Logical name</th>\n"
        "\t\t\t\t\t<th>Build command</th>\n"
        "\t\t\t\t\t<th>Build flags</th>\n"
        "\t\t\t\t\t<th>Specified file types</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n");

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
// Function: tst_documentGenerator::testViewsWrittenForTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testViewsWrittenForTopComponent()
{
    DocumentGenerator* generator = createTestGenerator();

    View* flatView (new View);

    topComponent_->addView(flatView);

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;
    QStringList pictureList;

    generator->writeViews(stream, subHeaderNumber, pictureList);

    targetFile.close();

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv()->toString() + ".views\">0.1 Views</a></h2>\n"
        "\t\t\t<h3>0.1.1 View: " + flatView->getName() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t<strong>" + getIndentString() + "Type: </strong>non-hierarchical<br>\n"
        );

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
// Function: tst_documentGenerator::testEndOfDocumentWrittenForTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testEndOfDocumentWrittenForTopComponent()
{
    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile("./generatorOutput.html");
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generator->writeEndOfDocument(stream);

    targetFile.close();

    QString expectedOutput(
        getValidW3CStrictString() +
        "\t</body>\n"
        "</html>\n"
        );

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
// Function: tst_documentGenerator::readOutputFile()
//-----------------------------------------------------------------------------
void tst_documentGenerator::readOutputFile()
{
    QFile outputFile("./generatorOutput.html");

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    output_ = outputFile.readAll();
    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestModelParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModelParameter> tst_documentGenerator::createTestModelParameter(QString const& name,
    QString const& dataType, QString const& value, QString const& description, QString const& uuID)
{
    QSharedPointer<ModelParameter> modelParameter (new ModelParameter);
    modelParameter->setName(name);
    modelParameter->setDataType(dataType);
    modelParameter->setValue(value);
    modelParameter->setDescription(description);
    modelParameter->setValueId(uuID);

    return modelParameter;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> tst_documentGenerator::createTestParameter(QString const& name, QString const& value,
    QString const& description, QString const& uuID)
{
    QSharedPointer<Parameter> parameter (new Parameter);
    parameter->setName(name);
    parameter->setValue(value);
    parameter->setDescription(description);
    parameter->setValueId(uuID);

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> tst_documentGenerator::createTestPort(QString const& name, QString const& leftBound,
    QString const& rightBound, QString const& defaultValue)
{
    QSharedPointer<Port> newPort (new Port);
    newPort->setDirection(General::DIRECTION_PHANTOM);
    newPort->setName(name);
    newPort->setLeftBoundExpression(leftBound);
    newPort->setRightBoundExpression(rightBound);
    newPort->setDefaultValue(defaultValue);

    return newPort;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getSpaceString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getSpaceString()
{
    return QString("&nbsp;");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getIndentString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getIndentString()
{
    return QString("&nbsp;&nbsp;&nbsp;");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getTableString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getTableString()
{
    return QString("<table frame=\"box\" rules=\"all\" border=\"1\" cellPadding=\"3\" title=\"");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getEncodingString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getEncodingString()
{
    return QString("<meta http-equiv=\"Content-Type\"content=\"text/html; charset=utf-8\">");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getDoctypeString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getDoctypeString()
{
    return QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getValidW3CStrictString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getValidW3CStrictString()
{
    return QString("\t\t<p>\n"
        "\t\t\t<a href=\"http://validator.w3.org/check?uri=referer\">\n"
        "\t\t\t<img src=\"http://www.w3.org/Icons/valid-html401\""
        "alt=\"Valid HTML 4.01 Strict\" height=\"31\""
        "width=\"88\">\n"
        "\t\t\t</a>\n"
        "\t\t</p>\n");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestGenerator()
//-----------------------------------------------------------------------------
DocumentGenerator* tst_documentGenerator::createTestGenerator()
{
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_);
    library_.addComponent(topComponent_);

    DocumentGenerator* generator (new DocumentGenerator(&library_, &designWidgetFactory_, generatorParentWidget_));

    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    generator->setComponent(vlnv);

    return generator;
}

QTEST_MAIN(tst_documentGenerator)

#include "tst_documentGenerator.moc"
