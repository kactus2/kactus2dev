//-----------------------------------------------------------------------------
// File: tst_CatalogReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.02.2017
//
// Description:
// Unit test for class CatalogReader.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/CatalogReader.h>
#include <IPXACTmodels/Catalog/IpxactFile.h>

class tst_CatalogReader : public QObject
{
    Q_OBJECT

public:
    tst_CatalogReader();

private slots:

    void testReadMinimalCatalog();

    void testReadTopComments();
    void testProcessingInstructionsAreParsed();

    void testReadOtherCatalogs();
    void testReadBusDefinitions();
    void testReadAbstractionDefinitions();
    void testReadComponents();
    void testReadAbstractors();
    void testReadDesigns();
    void testReadDesignConfigurations();
    void testReadGeneratorChains();
    void testReadTypeDefinitions();

    void testReadAllElements();
    /*
    void testOtherCatalogsAreWritten();
    void testBusDefinitionsAreWritten();
    void testAbstractionDefinitionsAreWritten();
    void testComponentsAreWritten();
    void testAbstractorsAreWritten();
    void testDesignsAreWritten();
    void testDesignConfigurationsAreWritten();
    void testGeneratorChainsAreWritten();
    
    void testVendorExtensions();*/

private:

    //QSharedPointer<VendorExtension> createTestVendorExtension();

};

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::tst_Catalog()
//-----------------------------------------------------------------------------
tst_CatalogReader::tst_CatalogReader()
{
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadMinimalCatalog()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadMinimalCatalog()
{
    QDomDocument document;
    document.setContent(QString(        
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
            "<ipxact:description>Catalog description</ipxact:description>"
        "</ipxact:catalog>\n"));

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    VLNV catalogVLNV = testCatalog->getVlnv();
    QCOMPARE(catalogVLNV.getVendor(), QString("tut.fi"));
    QCOMPARE(catalogVLNV.getLibrary(), QString("TestLibrary"));
    QCOMPARE(catalogVLNV.getName(), QString("MinimalCatalog"));
    QCOMPARE(catalogVLNV.getVersion(), QString("1.0"));

    QCOMPARE(testCatalog->getDescription(), QString("Catalog description"));
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadTopComments()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadTopComments()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<!--Commented line 1-->"
        "<!--Commented line 2-->"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestCatalog</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
         "<!--Comment not to include-->"       
        "</ipxact:catalog>"
        "<!--Comment not to include-->"));

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getTopComments().size(), 2);
    QCOMPARE(testCatalog->getTopComments().first(), QString("Commented line 1"));   
    QCOMPARE(testCatalog->getTopComments().last(), QString("Commented line 2"));   
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testProcessingInstructionsAreParsed()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testProcessingInstructionsAreParsed()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<!-- Header comments -->"
        "<?xml-stylesheet href=\"style.css\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestCatalog</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"        
        "</ipxact:catalog>"
        "<!--Comment not to include-->"));

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getXmlProcessingInstructions().count(), 1);

    QPair<QString, QString> styleInstruction = testCatalog->getXmlProcessingInstructions().first();
    QCOMPARE(styleInstruction.first, QString("xml-stylesheet"));
    QCOMPARE(styleInstruction.second, QString("href=\"style.css\""));

}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadOtherCatalogs()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadOtherCatalogs()
{ 
    QDomDocument document;
    document.setContent(QString(    
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
            "<ipxact:catalogs>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"SubCatalog\" version=\"1.0\"/>"
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

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getCatalogs()->size(), 2); 

    QSharedPointer<IpxactFile> firstCatalogFile = testCatalog->getCatalogs()->first();

    QCOMPARE(firstCatalogFile->getVlnv(), VLNV(VLNV::CATALOG, "tut.fi", "TestLibrary", "SubCatalog", "1.0"));
    QCOMPARE(firstCatalogFile->getName(), QString("./mini.xml"));

    QSharedPointer<IpxactFile> secondCatalogFile = testCatalog->getCatalogs()->last();

    QCOMPARE(secondCatalogFile->getVlnv(), VLNV(VLNV::CATALOG, "tut.fi", "TestLibrary", "CompleteCatalog", "1.0"));
    QCOMPARE(secondCatalogFile->getDescription(), QString("Test file description"));
    QCOMPARE(secondCatalogFile->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadBusDefinitions()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadBusDefinitions()
{ 
    QDomDocument document;
    document.setContent(QString(    
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

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getBusDefinitions()->size(), 2); 

    QSharedPointer<IpxactFile> firstBusFile = testCatalog->getBusDefinitions()->first();

    QCOMPARE(firstBusFile->getVlnv(), VLNV(VLNV::BUSDEFINITION, "tut.fi", "TestLibrary", "MiniBus", "1.0"));
    QCOMPARE(firstBusFile->getName(), QString("./mini.xml"));

    QSharedPointer<IpxactFile> secondBusFile = testCatalog->getBusDefinitions()->last();

    QCOMPARE(secondBusFile->getVlnv(), VLNV(VLNV::BUSDEFINITION, "tut.fi", "TestLibrary", "CompleteBus", "1.0"));
    QCOMPARE(secondBusFile->getDescription(), QString("Test file description"));
    QCOMPARE(secondBusFile->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadAbstractionDefinitions()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadAbstractionDefinitions()
{ 
    QDomDocument document;
    document.setContent(QString(    
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
            "<ipxact:abstractionDefinitions>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"MiniAbstraction\" version=\"1.0\"/>"
                    "<ipxact:name>./mini.xml</ipxact:name>"          
                "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"CompleteAbstraction\" version=\"1.0\"/>"
                    "<ipxact:name>./complete.xml</ipxact:name>"
                    "<ipxact:description>Test file description</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:abstractionDefinitions>"
        "</ipxact:catalog>\n"));

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getAbstractionDefinitions()->size(), 2); 

    QSharedPointer<IpxactFile> firstAbstractionFile = testCatalog->getAbstractionDefinitions()->first();

    QCOMPARE(firstAbstractionFile->getVlnv(), VLNV(VLNV::ABSTRACTIONDEFINITION, "tut.fi", "TestLibrary", "MiniAbstraction", "1.0"));
    QCOMPARE(firstAbstractionFile->getName(), QString("./mini.xml"));

    QSharedPointer<IpxactFile> secondAbstractionFile = testCatalog->getAbstractionDefinitions()->last();

    QCOMPARE(secondAbstractionFile->getVlnv(), VLNV(VLNV::ABSTRACTIONDEFINITION, "tut.fi", "TestLibrary", "CompleteAbstraction", "1.0"));
    QCOMPARE(secondAbstractionFile->getDescription(), QString("Test file description"));
    QCOMPARE(secondAbstractionFile->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadComponents()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadComponents()
{ 
    QDomDocument document;
    document.setContent(QString(    
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
            "<ipxact:components>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"SubComponent\" version=\"1.0\"/>"
                    "<ipxact:name>./mini.xml</ipxact:name>"          
                "</ipxact:ipxactFile>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"CompleteComponent\" version=\"1.0\"/>"
                    "<ipxact:name>./complete.xml</ipxact:name>"
                    "<ipxact:description>Test file description</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:components>"
        "</ipxact:catalog>\n"));

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getComponents()->size(), 2); 

    QSharedPointer<IpxactFile> firstComponentFile = testCatalog->getComponents()->first();

    QCOMPARE(firstComponentFile->getVlnv(), VLNV(VLNV::COMPONENT, "tut.fi", "TestLibrary", "SubComponent", "1.0"));
    QCOMPARE(firstComponentFile->getName(), QString("./mini.xml"));

    QSharedPointer<IpxactFile> secondComponentFile = testCatalog->getComponents()->last();

    QCOMPARE(secondComponentFile->getVlnv(), VLNV(VLNV::COMPONENT, "tut.fi", "TestLibrary", "CompleteComponent", "1.0"));
    QCOMPARE(secondComponentFile->getDescription(), QString("Test file description"));
    QCOMPARE(secondComponentFile->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadAbstractors()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadAbstractors()
{ 
    QDomDocument document;
    document.setContent(QString(    
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

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getAbstractors()->size(), 2); 

    QSharedPointer<IpxactFile> firstAbstractorFile = testCatalog->getAbstractors()->first();

    QCOMPARE(firstAbstractorFile->getVlnv(), VLNV(VLNV::ABSTRACTOR, "tut.fi", "TestLibrary", "MiniAbstractor", "1.0"));
    QCOMPARE(firstAbstractorFile->getName(), QString("./mini.xml"));

    QSharedPointer<IpxactFile> secondAbstractorFile = testCatalog->getAbstractors()->last();

    QCOMPARE(secondAbstractorFile->getVlnv(), VLNV(VLNV::ABSTRACTOR, "tut.fi", "TestLibrary", "CompleteAbstractor", "1.0"));
    QCOMPARE(secondAbstractorFile->getDescription(), QString("Test file description"));
    QCOMPARE(secondAbstractorFile->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadDesigns()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadDesigns()
{ 
    QDomDocument document;
    document.setContent(QString(    
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
            "<ipxact:designs>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"SubDesign\" version=\"1.0\"/>"
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

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getDesigns()->size(), 2); 

    QSharedPointer<IpxactFile> firstDesignFile = testCatalog->getDesigns()->first();

    QCOMPARE(firstDesignFile->getVlnv(), VLNV(VLNV::DESIGN, "tut.fi", "TestLibrary", "SubDesign", "1.0"));
    QCOMPARE(firstDesignFile->getName(), QString("./mini.xml"));

    QSharedPointer<IpxactFile> secondDesignFile = testCatalog->getDesigns()->last();

    QCOMPARE(secondDesignFile->getVlnv(), VLNV(VLNV::DESIGN, "tut.fi", "TestLibrary", "FullDesign", "1.0"));
    QCOMPARE(secondDesignFile->getDescription(), QString("Test file description"));
    QCOMPARE(secondDesignFile->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadDesignConfigurations()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadDesignConfigurations()
{ 
    QDomDocument document;
    document.setContent(QString(    
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
            "<ipxact:designConfigurations>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"SubDesign\" version=\"1.0\"/>"
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

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getDesignConfigurations()->size(), 2); 

    QSharedPointer<IpxactFile> firstDesignFile = testCatalog->getDesignConfigurations()->first();

    QCOMPARE(firstDesignFile->getVlnv(), VLNV(VLNV::DESIGNCONFIGURATION, "tut.fi", "TestLibrary", "SubDesign", "1.0"));
    QCOMPARE(firstDesignFile->getName(), QString("./mini.xml"));

    QSharedPointer<IpxactFile> secondDesignFile = testCatalog->getDesignConfigurations()->last();

    QCOMPARE(secondDesignFile->getVlnv(), VLNV(VLNV::DESIGNCONFIGURATION, "tut.fi", "TestLibrary", "FullDesign", "1.0"));
    QCOMPARE(secondDesignFile->getDescription(), QString("Test file description"));
    QCOMPARE(secondDesignFile->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadGeneratorChains()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadGeneratorChains()
{ 
    QDomDocument document;
    document.setContent(QString(    
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

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getGeneratorChains()->size(), 2); 

    QSharedPointer<IpxactFile> firstDesignFile = testCatalog->getGeneratorChains()->first();

    QCOMPARE(firstDesignFile->getVlnv(), VLNV(VLNV::GENERATORCHAIN, "tut.fi", "TestLibrary", "MiniChain", "1.0"));
    QCOMPARE(firstDesignFile->getName(), QString("./mini.xml"));

    QSharedPointer<IpxactFile> secondDesignFile = testCatalog->getGeneratorChains()->last();

    QCOMPARE(secondDesignFile->getVlnv(), VLNV(VLNV::GENERATORCHAIN, "tut.fi", "TestLibrary", "CompleteChain", "1.0"));
    QCOMPARE(secondDesignFile->getDescription(), QString("Test file description"));
    QCOMPARE(secondDesignFile->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadTypeDefinitions()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadTypeDefinitions()
{
    QDomDocument document;
    document.setContent(QString(    
        "<?xml version=\"1.0\"?>"
        "<ipxact:catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">"
            "<ipxact:vendor>tut.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestCatalog</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:displayName>testDisplayName</ipxact:displayName>"
            "<ipxact:shortDescription>a short description</ipxact:shortDescription>"
            "<ipxact:typeDefinitions>"
                "<ipxact:ipxactFile>"
                    "<ipxact:vlnv vendor=\"tut.fi\" library=\"TestLibrary\" name=\"CompleteTypeDefinition\" version=\"1.0\"/>"
                    "<ipxact:name>./typedef.xml</ipxact:name>"
                    "<ipxact:description>Test file description</ipxact:description>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:ipxactFile>"
            "</ipxact:typeDefinitions>"
        "</ipxact:catalog>\n"));

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);
    QCOMPARE(testCatalog->getTypeDefinitions()->size(), 1);
    QCOMPARE(testCatalog->getShortDescription(), QString("a short description"));
    QCOMPARE(testCatalog->getDisplayName(), QString("testDisplayName"));

    QSharedPointer<IpxactFile> typeDefFile = testCatalog->getTypeDefinitions()->first();
    QCOMPARE(typeDefFile->getVlnv(), VLNV(VLNV::TYPEDEFINITION, "tut.fi", "TestLibrary", "CompleteTypeDefinition", "1.0"));
    QCOMPARE(typeDefFile->getName(), QString("./typedef.xml"));
    QCOMPARE(typeDefFile->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogReader::testReadAllElements()
//-----------------------------------------------------------------------------
void tst_CatalogReader::testReadAllElements()
{
    QDomDocument document;
    document.setContent(QString(    
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
            "<kactus2:version>3.4.0</kactus2:version>"
        "</ipxact:vendorExtensions>"
        "</ipxact:catalog>\n"));

    QSharedPointer<Catalog> testCatalog = CatalogReader::createCatalogFrom(document);

    QCOMPARE(testCatalog->getCatalogs()->size(), 1); 
    QCOMPARE(testCatalog->getBusDefinitions()->size(), 1); 
    QCOMPARE(testCatalog->getAbstractionDefinitions()->size(), 1); 
    QCOMPARE(testCatalog->getComponents()->size(), 1); 
    QCOMPARE(testCatalog->getAbstractors()->size(), 1); 
    QCOMPARE(testCatalog->getDesigns()->size(), 1); 
    QCOMPARE(testCatalog->getDesignConfigurations()->size(), 1); 
    QCOMPARE(testCatalog->getGeneratorChains()->size(), 1); 

    QCOMPARE(testCatalog->getVersion(), QString("3.4.0")); 
}


QTEST_APPLESS_MAIN(tst_CatalogReader)

#include "tst_CatalogReader.moc"
