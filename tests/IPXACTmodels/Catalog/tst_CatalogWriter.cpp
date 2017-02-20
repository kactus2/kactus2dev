//-----------------------------------------------------------------------------
// File: tst_CatalogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.01.2017
//
// Description:
// Unit test for class CatalogWriter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/CatalogWriter.h>
#include <IPXACTmodels/Catalog/IpxactFile.h>

class tst_CatalogWriter : public QObject
{
    Q_OBJECT

public:
    tst_CatalogWriter();

private slots:

    void testWriteMinimalCatalog();

    void testTopCommentsAreWritten();
    void testProcessingInstructionsAreWritten();

    void testOtherCatalogsAreWritten();
    void testBusDefinitionsAreWritten();
    void testAbstractionDefinitionsAreWritten();
    void testComponentsAreWritten();
    void testAbstractorsAreWritten();
    void testDesignsAreWritten();
    void testDesignConfigurationsAreWritten();
    void testGeneratorChainsAreWritten();
    
    void testVendorExtensions();

    void testWriteAllElements();

private:

    QSharedPointer<VendorExtension> createTestVendorExtension();

};

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::tst_Catalog()
//-----------------------------------------------------------------------------
tst_CatalogWriter::tst_CatalogWriter()
{
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testWriteMinimalCatalog()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testWriteMinimalCatalog()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "MinimalCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tut.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalCatalog</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testTopCommentsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testTopCommentsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "MinimalCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setTopComments("Commented section");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<!--Commented section-->"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tut.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalCatalog</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testProcessingInstructionsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testProcessingInstructionsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "StyledCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->addXmlProcessingInstructions("xml-stylesheet", "href=\"style.css\" attribute=\"value\"");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString("<?xml version=\"1.0\"?>"
        "<?xml-stylesheet href=\"style.css\" attribute=\"value\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tut.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>StyledCatalog</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testOtherCatalogsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testOtherCatalogsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setDescription(QStringLiteral("Catalog description"));

    VLNV minimalVLNV(VLNV::CATALOG, "tut.fi", "TestLibrary", "MiniCatalog", "1.0");
    QSharedPointer<IpxactFile> miniCatalog(new IpxactFile());
    miniCatalog->setVlnv(minimalVLNV);
    miniCatalog->setName("./mini.xml");

    VLNV completeVLNV(VLNV::CATALOG, "tut.fi", "TestLibrary", "CompleteCatalog", "1.0");
    QSharedPointer<IpxactFile> completeCatalog(new IpxactFile());
    completeCatalog->setVlnv(completeVLNV);
    completeCatalog->setName("./complete.xml");
    completeCatalog->setDescription("Test file description");
    completeCatalog->getVendorExtensions()->append(createTestVendorExtension());

    catalog->getCatalogs()->append(miniCatalog);
    catalog->getCatalogs()->append(completeCatalog);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tut.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestCatalog</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:description>Catalog description</ipxact:description>"
            "<ipxact:catalogs>"
                "<ipxact:ipxactFile>"
                "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"MiniCatalog\" version=\"1.0\"/>"
                "<ipxact:name>./mini.xml</ipxact:name>"          
            "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"CompleteCatalog\" version=\"1.0\"/>"
                    "<ipxact:name>./complete.xml</ipxact:name>"
                    "<ipxact:description>Test file description</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:catalogs>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testBusDefinitionsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testBusDefinitionsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setDescription(QStringLiteral("Catalog description"));

    VLNV minimalVLNV(VLNV::BUSDEFINITION, "tut.fi", "TestLibrary", "MiniBus", "1.0");
    QSharedPointer<IpxactFile> miniBus(new IpxactFile());
    miniBus->setVlnv(minimalVLNV);
    miniBus->setName("./mini.xml");

    VLNV completeVLNV(VLNV::BUSDEFINITION, "tut.fi", "TestLibrary", "CompleteBus", "1.0");
    QSharedPointer<IpxactFile> completeBus(new IpxactFile());
    completeBus->setVlnv(completeVLNV);
    completeBus->setName("./complete.xml");
    completeBus->setDescription("Test file description");
    completeBus->getVendorExtensions()->append(createTestVendorExtension());

    catalog->getBusDefinitions()->append(miniBus);
    catalog->getBusDefinitions()->append(completeBus);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>tut.fi</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>TestCatalog</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:description>Catalog description</ipxact:description>"
            "<ipxact:busDefinitions>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"MiniBus\" version=\"1.0\"/>"
                    "<ipxact:name>./mini.xml</ipxact:name>"          
                "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"CompleteBus\" version=\"1.0\"/>"
                    "<ipxact:name>./complete.xml</ipxact:name>"
                    "<ipxact:description>Test file description</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:busDefinitions>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testAbstractionDefinitionsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testAbstractionDefinitionsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setDescription(QStringLiteral("Catalog description"));

    VLNV minimalVLNV(VLNV::ABSTRACTIONDEFINITION, "tut.fi", "TestLibrary", "MiniBus", "1.0");
    QSharedPointer<IpxactFile> miniAbstraction(new IpxactFile());
    miniAbstraction->setVlnv(minimalVLNV);
    miniAbstraction->setName("./mini.xml");

    VLNV completeVLNV(VLNV::ABSTRACTIONDEFINITION, "tut.fi", "TestLibrary", "CompleteBus", "1.0");
    QSharedPointer<IpxactFile> completeAbstraction(new IpxactFile());
    completeAbstraction->setVlnv(completeVLNV);
    completeAbstraction->setName("./complete.xml");
    completeAbstraction->setDescription("Test file description");
    completeAbstraction->getVendorExtensions()->append(createTestVendorExtension());

    catalog->getAbstractionDefinitions()->append(miniAbstraction);
    catalog->getAbstractionDefinitions()->append(completeAbstraction);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>tut.fi</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>TestCatalog</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:description>Catalog description</ipxact:description>"
            "<ipxact:abstractionDefinitions>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"MiniBus\" version=\"1.0\"/>"
                    "<ipxact:name>./mini.xml</ipxact:name>"          
                "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"CompleteBus\" version=\"1.0\"/>"
                    "<ipxact:name>./complete.xml</ipxact:name>"
                    "<ipxact:description>Test file description</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:abstractionDefinitions>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testComponentsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testComponentsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setDescription(QStringLiteral("This catalog contains two components"));

    VLNV minimalVLNV(VLNV::COMPONENT, "tut.fi", "TestLibrary", "SubComponent", "1.0");
    QSharedPointer<IpxactFile> subComponent(new IpxactFile());
    subComponent->setVlnv(minimalVLNV);
    subComponent->setName("./sub.xml");

    VLNV completeVLNV(VLNV::COMPONENT, "tut.fi", "TestLibrary", "TopComponent", "1.0");
    QSharedPointer<IpxactFile> topComponent(new IpxactFile());
    topComponent->setVlnv(completeVLNV);
    topComponent->setName("./top.xml");
    topComponent->setDescription("This is the top level component");
    topComponent->getVendorExtensions()->append(createTestVendorExtension());

    catalog->getComponents()->append(subComponent);
    catalog->getComponents()->append(topComponent);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>tut.fi</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>TestCatalog</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:description>This catalog contains two components</ipxact:description>"
            "<ipxact:components>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"SubComponent\" version=\"1.0\"/>"
                    "<ipxact:name>./sub.xml</ipxact:name>"          
                "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"TopComponent\" version=\"1.0\"/>"
                    "<ipxact:name>./top.xml</ipxact:name>"
                    "<ipxact:description>This is the top level component</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:components>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testAbstractorsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testAbstractorsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setDescription(QStringLiteral("Catalog description"));

    VLNV minimalVLNV(VLNV::ABSTRACTOR, "tut.fi", "TestLibrary", "MiniAbstractor", "1.0");
    QSharedPointer<IpxactFile> miniAbstractor(new IpxactFile());
    miniAbstractor->setVlnv(minimalVLNV);
    miniAbstractor->setName("./mini.xml");

    VLNV completeVLNV(VLNV::ABSTRACTOR, "tut.fi", "TestLibrary", "CompleteAbstractor", "1.0");
    QSharedPointer<IpxactFile> completeAbstactor(new IpxactFile());
    completeAbstactor->setVlnv(completeVLNV);
    completeAbstactor->setName("./complete.xml");
    completeAbstactor->setDescription("Test file description");
    completeAbstactor->getVendorExtensions()->append(createTestVendorExtension());

    catalog->getAbstractors()->append(miniAbstractor);
    catalog->getAbstractors()->append(completeAbstactor);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>tut.fi</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>TestCatalog</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:description>Catalog description</ipxact:description>"
            "<ipxact:abstractors>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"MiniAbstractor\" version=\"1.0\"/>"
                    "<ipxact:name>./mini.xml</ipxact:name>"          
                "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"CompleteAbstractor\" version=\"1.0\"/>"
                    "<ipxact:name>./complete.xml</ipxact:name>"
                    "<ipxact:description>Test file description</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:abstractors>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testDesignsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testDesignsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setDescription(QStringLiteral("Catalog description"));

    VLNV minimalVLNV(VLNV::DESIGN, "tut.fi", "TestLibrary", "MiniDesign", "1.0");
    QSharedPointer<IpxactFile> miniDesign(new IpxactFile());
    miniDesign->setVlnv(minimalVLNV);
    miniDesign->setName("./mini.xml");

    VLNV completeVLNV(VLNV::DESIGN, "tut.fi", "TestLibrary", "FullDesign", "1.0");
    QSharedPointer<IpxactFile> completeDesign(new IpxactFile());
    completeDesign->setVlnv(completeVLNV);
    completeDesign->setName("./complete.xml");
    completeDesign->setDescription("Test file description");
    completeDesign->getVendorExtensions()->append(createTestVendorExtension());

    catalog->getDesigns()->append(miniDesign);
    catalog->getDesigns()->append(completeDesign);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>tut.fi</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>TestCatalog</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:description>Catalog description</ipxact:description>"
            "<ipxact:designs>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"MiniDesign\" version=\"1.0\"/>"
                    "<ipxact:name>./mini.xml</ipxact:name>"          
                "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"FullDesign\" version=\"1.0\"/>"
                    "<ipxact:name>./complete.xml</ipxact:name>"
                    "<ipxact:description>Test file description</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:designs>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testDesignConfigurationsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testDesignConfigurationsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setDescription(QStringLiteral("Catalog description"));

    VLNV minimalVLNV(VLNV::DESIGNCONFIGURATION, "tut.fi", "TestLibrary", "MiniDesign", "1.0");
    QSharedPointer<IpxactFile> miniConfiguration(new IpxactFile());
    miniConfiguration->setVlnv(minimalVLNV);
    miniConfiguration->setName("./mini.xml");

    VLNV completeVLNV(VLNV::DESIGNCONFIGURATION, "tut.fi", "TestLibrary", "FullDesign", "1.0");
    QSharedPointer<IpxactFile> completeConfiguration(new IpxactFile());
    completeConfiguration->setVlnv(completeVLNV);
    completeConfiguration->setName("./complete.xml");
    completeConfiguration->setDescription("Test file description");
    completeConfiguration->getVendorExtensions()->append(createTestVendorExtension());

    catalog->getDesignConfigurations()->append(miniConfiguration);
    catalog->getDesignConfigurations()->append(completeConfiguration);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>tut.fi</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>TestCatalog</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:description>Catalog description</ipxact:description>"
            "<ipxact:designConfigurations>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"MiniDesign\" version=\"1.0\"/>"
                    "<ipxact:name>./mini.xml</ipxact:name>"          
                "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"FullDesign\" version=\"1.0\"/>"
                    "<ipxact:name>./complete.xml</ipxact:name>"
                     "<ipxact:description>Test file description</ipxact:description>"
                     "<ipxact:vendorExtensions>"
                            "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:designConfigurations>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testGeneratorChainsAreWritten()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testGeneratorChainsAreWritten()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setDescription(QStringLiteral("Catalog description"));

    VLNV minimalVLNV(VLNV::GENERATORCHAIN, "tut.fi", "TestLibrary", "MiniChain", "1.0");
    QSharedPointer<IpxactFile> miniChain(new IpxactFile());
    miniChain->setVlnv(minimalVLNV);
    miniChain->setName("./mini.xml");

    VLNV completeVLNV(VLNV::GENERATORCHAIN, "tut.fi", "TestLibrary", "CompleteChain", "1.0");
    QSharedPointer<IpxactFile> completeChain(new IpxactFile());
    completeChain->setVlnv(completeVLNV);
    completeChain->setName("./complete.xml");
    completeChain->setDescription("Test file description");
    completeChain->getVendorExtensions()->append(createTestVendorExtension());

    catalog->getGeneratorChains()->append(miniChain);
    catalog->getGeneratorChains()->append(completeChain);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>tut.fi</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>TestCatalog</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:description>Catalog description</ipxact:description>"
            "<ipxact:generatorChains>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"MiniChain\" version=\"1.0\"/>"
                    "<ipxact:name>./mini.xml</ipxact:name>"          
                "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"CompleteChain\" version=\"1.0\"/>"
                    "<ipxact:name>./complete.xml</ipxact:name>"
                    "<ipxact:description>Test file description</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:generatorChains>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testVendorExtensions()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testVendorExtensions()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setVersion("3.0.0");
    catalog->getVendorExtensions()->append(createTestVendorExtension());

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tut.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestCatalog</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:vendorExtensions>"
                "<kactus2:version>3.0.0</kactus2:version>"
                "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::testWriteAllElements()
//-----------------------------------------------------------------------------
void tst_CatalogWriter::testWriteAllElements()
{
    VLNV vlnv(VLNV::CATALOG, "tut.fi", "TestLibrary", "CompleteCatalog", "1.0");
    QSharedPointer<Catalog> catalog(new Catalog());
    catalog->setVlnv(vlnv);
    catalog->setDescription(QStringLiteral("Catalog with all elements"));
    catalog->setVersion("3.0.0");

    VLNV catalogVLNV(VLNV::CATALOG, "tut.fi", "TestLibrary", "SubCatalog", "1.0");
    QSharedPointer<IpxactFile> otherCatalog(new IpxactFile());
    otherCatalog->setVlnv(catalogVLNV);
    otherCatalog->setName("./catalog.xml");

    VLNV busVLNV(VLNV::BUSDEFINITION, "tut.fi", "TestLibrary", "Bus", "1.0");
    QSharedPointer<IpxactFile> busDefinition(new IpxactFile());
    busDefinition->setVlnv(busVLNV);
    busDefinition->setName("./bus.xml");

    VLNV abstractionVLNV(VLNV::ABSTRACTIONDEFINITION, "tut.fi", "TestLibrary", "Abstraction", "1.0");
    QSharedPointer<IpxactFile> abstraction(new IpxactFile());
    abstraction->setVlnv(abstractionVLNV);
    abstraction->setName("./abstraction.xml");

    VLNV componentVLNV(VLNV::COMPONENT, "tut.fi", "TestLibrary", "Component", "1.0");
    QSharedPointer<IpxactFile> component(new IpxactFile());
    component->setVlnv(componentVLNV);
    component->setName("./component.xml");

    VLNV abstractorVLNV(VLNV::ABSTRACTOR, "tut.fi", "TestLibrary", "Abstractor", "1.0");
    QSharedPointer<IpxactFile> abstractor(new IpxactFile());
    abstractor->setVlnv(abstractorVLNV);
    abstractor->setName("./abstractor.xml");

    VLNV designVLNV(VLNV::DESIGN, "tut.fi", "TestLibrary", "Design", "1.0");
    QSharedPointer<IpxactFile> design(new IpxactFile());
    design->setVlnv(designVLNV);
    design->setName("./design.xml");

    VLNV configurationVLNV(VLNV::DESIGNCONFIGURATION, "tut.fi", "TestLibrary", "Configuration", "1.0");
    QSharedPointer<IpxactFile> designConfiguration(new IpxactFile());
    designConfiguration->setVlnv(configurationVLNV);
    designConfiguration->setName("./configuration.xml");

    VLNV chainVLNV(VLNV::GENERATORCHAIN, "tut.fi", "TestLibrary", "TestChain", "1.0");
    QSharedPointer<IpxactFile> generatorChain(new IpxactFile());
    generatorChain->setVlnv(chainVLNV);
    generatorChain->setName("./chain.xml");

    catalog->getCatalogs()->append(otherCatalog);
    catalog->getBusDefinitions()->append(busDefinition);
    catalog->getAbstractionDefinitions()->append(abstraction);
    catalog->getComponents()->append(component);
    catalog->getAbstractors()->append(abstractor);
    catalog->getDesigns()->append(design);
    catalog->getDesignConfigurations()->append(designConfiguration);
    catalog->getGeneratorChains()->append(generatorChain);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    CatalogWriter catalogWriter;

    catalogWriter.writeCatalog(xmlStreamWriter, catalog);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>tut.fi</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>CompleteCatalog</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:description>Catalog with all elements</ipxact:description>"
        "<ipxact:catalogs>"
            "<ipxact:ipxactFile>"
                "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"SubCatalog\" version=\"1.0\"/>"
                "<ipxact:name>./catalog.xml</ipxact:name>"          
            "</ipxact:ipxactFile>"        
        "</ipxact:catalogs>"
        "<ipxact:busDefinitions>"
            "<ipxact:ipxactFile>"
                "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"Bus\" version=\"1.0\"/>"
                "<ipxact:name>./bus.xml</ipxact:name>"          
            "</ipxact:ipxactFile>"        
        "</ipxact:busDefinitions>"
        "<ipxact:abstractionDefinitions>"
            "<ipxact:ipxactFile>"
                "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"Abstraction\" version=\"1.0\"/>"
                "<ipxact:name>./abstraction.xml</ipxact:name>"          
            "</ipxact:ipxactFile>"        
        "</ipxact:abstractionDefinitions>" 
        "<ipxact:components>"
            "<ipxact:ipxactFile>"
                "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"Component\" version=\"1.0\"/>"
                "<ipxact:name>./component.xml</ipxact:name>"          
            "</ipxact:ipxactFile>"        
        "</ipxact:components>" 
        "<ipxact:abstractors>"
            "<ipxact:ipxactFile>"
                "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"Abstractor\" version=\"1.0\"/>"
                "<ipxact:name>./abstractor.xml</ipxact:name>"          
            "</ipxact:ipxactFile>"        
        "</ipxact:abstractors>" 
        "<ipxact:designs>"
            "<ipxact:ipxactFile>"
                "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"Design\" version=\"1.0\"/>"
                "<ipxact:name>./design.xml</ipxact:name>"          
            "</ipxact:ipxactFile>"        
        "</ipxact:designs>" 
        "<ipxact:designConfigurations>"
            "<ipxact:ipxactFile>"
                "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"Configuration\" version=\"1.0\"/>"
                "<ipxact:name>./configuration.xml</ipxact:name>"          
            "</ipxact:ipxactFile>"        
        "</ipxact:designConfigurations>" 
        "<ipxact:generatorChains>"
            "<ipxact:ipxactFile>"
                "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"TestChain\" version=\"1.0\"/>"
                "<ipxact:name>./chain.xml</ipxact:name>"          
            "</ipxact:ipxactFile>"        
        "</ipxact:generatorChains>" 
        "<ipxact:vendorExtensions>"
            "<kactus2:version>3.0.0</kactus2:version>"
        "</ipxact:vendorExtensions>"
        "</ipxact:catalog>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogWriter::createTestVendorExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> tst_CatalogWriter::createTestVendorExtension()
{
    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("vendorAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));
    return testExtension;
}


QTEST_APPLESS_MAIN(tst_CatalogWriter)

#include "tst_CatalogWriter.moc"
