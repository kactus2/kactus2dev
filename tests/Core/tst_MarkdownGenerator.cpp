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
    QSharedPointer<Component> topComponent_;

    VLNV topComponentVlnv_;

    LibraryMock library_;

    QString targetPath_;
};

tst_MarkdownGenerator::tst_MarkdownGenerator() :
    library_(this),
    targetPath_("./generatorOutput.md")
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
}

void tst_MarkdownGenerator::cleanup()
{
    topComponent_.clear();
    QFile::remove(targetPath_);
}

void tst_MarkdownGenerator::testInvalidVlnvInConstructor()
{
    VLNV invalidVlnv(VLNV::COMPONENT, "invalid", "library", "component", "0");
}

void tst_MarkdownGenerator::testFileHeaderIsWritten()
{
}

void tst_MarkdownGenerator::testTableOfContentsIsWrittenWithOnlyTopComponent()
{
}

void tst_MarkdownGenerator::testParametersWrittenWithOnlyTopComponent()
{
}

void tst_MarkdownGenerator::testMemoryMapsWrittenWithTopComponent()
{
}

void tst_MarkdownGenerator::testAddressBlocksWrittenWithTopComponent()
{
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
