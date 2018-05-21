//-----------------------------------------------------------------------------
// File: tst_MakefileGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 02.09.2014
//
// Description:
// Unit test for classes MakefileParser and MakefileGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/kactusExtensions/SystemView.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/MakefileGenerator/SWStackParser.h>
#include <Plugins/MakefileGenerator/MakefileParser.h>
#include <Plugins/MakefileGenerator/MakefileGenerator.h>
#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>
#include <common/NameGenerationPolicy.h>

#include <QRegExp>

class tst_MakefileGenerator : public QObject
{
    Q_OBJECT

public:
    tst_MakefileGenerator();

private slots:

    void cleanupTestCase();

    void baseCase();

	// Overrides.
    void fileBuildOverride();
    void fileSetBuildOverride();
    void fileFlagReplace();
    void fileSetFlagReplace();
    void swSWViewFlagReplace();

	// Hardware and references.
    void hwBuilder();
    void hwBuilderWithNosoftInsta();
    void hwRef();
    void hwandswRef();
    void instanceFiles();

	// Multiple things.
    void multipleFiles();
    void multipleFileSets();
    void multipleComponents();
    void multipleHardWare();
	void multipleHardWareMedRefs();
	void multipleInstances();

	// Missing things.
	void noHardWare();
	void noFilesComponent();
	void noFileType();

	// API usage.
    void apiUsage();
    void threeLevelStack();
    void fullCircularapiUsage();
	void circularapiUsage();

	// Conflicts.
	void sameFile();
	void sameFileSeparateExe();
	void sameFileDiffCompiler();
	void sameFileDiffFlags();

	// Generation.
	void basicGeneration();
	void multiObjectGeneration();
	void multiFileGeneration();
	void noCompiler();
	void allTheWay();

private:
    void getFile(QSharedPointer<File>& file, QSharedPointer<Component> component, QString fileName);

    QSharedPointer<Component> createSW(QString swName, QString hwInstanceName, QSharedPointer<Design> design, QString softInstaName,
		 QSharedPointer<DesignConfiguration> desgconf, QSharedPointer<ComponentInstantiation>& softInsta, QString ComponentInstanceName);

    QSharedPointer<FileSet> addFileSet(QSharedPointer<Component> component, QString fileSetName, QSharedPointer<ComponentInstantiation> insta);

    QSharedPointer<File> addFileToSet(QSharedPointer<FileSet> fileSet, QString fileName, QString fileType="cSource", bool isInclude=false);

    void createSWView(QSharedPointer<ComponentInstantiation> softInsta, QString softInstaName, QSharedPointer<Component> sw);

    QSharedPointer<Component> createHW(QString const& hwInstanceName, QSharedPointer<Design> design, QString const& hardInstaName,
		 QSharedPointer<DesignConfiguration> designConfig, QSharedPointer<ComponentInstantiation>& hardInsta, QString const& hwName="hardware");

    QSharedPointer<Component> createDesign(QSharedPointer<Design> &design, QSharedPointer<DesignConfiguration> &desgconf);

    void verifyOutputContains(QString instanceName, QString const& expectedOutput);

    void setFileBuilder(QSharedPointer<File> file, QString command, QString flags, bool replace);

    void addFileSetBuilder(QSharedPointer<FileSet> fileSet, QString command, QString fileType, QString flags, bool replace);

	void addCmd2Insta(QSharedPointer<ComponentInstantiation> insta, QString command, QString fileType, QString flags, bool replace);

	void addAPI(QSharedPointer<Component> asw, QString name, DependencyDirection dir);

	void addAPIConnection(QSharedPointer<Design> design, QString com1, QString bus1, QString com2, QString bus2);

    //! The test mock for library interface.
    LibraryMock library_;

    //! Output directory for the generator
    QString outputDir_;

    MessagePasser messageChannel_;

    //! Mock utility to be used in tests
    PluginUtilityAdapter utilityMock_;    
};

tst_MakefileGenerator::tst_MakefileGenerator(): library_( this ), outputDir_("testOutput"), messageChannel_(), 
    utilityMock_(&library_, &messageChannel_, QString(), 0)
{
}

//-----------------------------------------------------------------------------
// Function: tst_MakefileGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_MakefileGenerator::cleanupTestCase()
{
   QDir outputDirectory(outputDir_);
   outputDirectory.removeRecursively();
}

// A rather basic and secure case testing that all the needed lines come for one file.
void tst_MakefileGenerator::baseCase()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    createHW("hardware_0", design, "firmware", desgconf, hardInsta);
    addCmd2Insta(hardInsta, "gcc", "cSource", "-hw", false);

    QSharedPointer<Component> sw = createSW("software", "hardware_0", design, "default", desgconf, softInsta, "software_0");

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softInsta);

    addFileToSet(fileSet, "array.c");
    addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.first()->compiler, QString("gcc") );
	QCOMPARE( data->swObjects.first()->flags, QString(" -sw -hw") );
	QCOMPARE( data->name, QString("software_0") );
}

// Case used to test if file builder properly overrides other builders.
void tst_MakefileGenerator::fileBuildOverride()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);
	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softInsta);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addCmd2Insta(softInsta, "gcc -o", "cSource", "-sw", false);

    setFileBuilder(file, "python", "-l", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("python") );
	QCOMPARE( data->swObjects.first()->flags.trimmed(), QString("-l -sw") );
	QCOMPARE( data->componentInstantiationFlags.contains("-sw"), true );
}

// Case used to test if file set builder properly overrides other builders.
void tst_MakefileGenerator::fileSetBuildOverride()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);
	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softInsta);

    addFileToSet(fileSet, "array.c");
    addCmd2Insta(softInsta, "gcc -o", "cSource", "-sw", false);
    addFileSetBuilder(fileSet, "javac -beef", "cSource", "-lrt", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("javac -beef") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-lrt -sw") );
	QCOMPARE( data->componentInstantiationFlags.contains("-sw"), true );
}

// Case used to test if file flags properly replace other flags if needed.
void tst_MakefileGenerator::fileFlagReplace()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);
	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softInsta);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

    setFileBuilder(file, "", "-u", true);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("gcc") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-u") );
	QCOMPARE( data->componentInstantiationFlags.contains("-sw"), true );
}

// Case used to test if file set flags properly replace other flags if needed.
void tst_MakefileGenerator::fileSetFlagReplace()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);
	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softInsta);

    addFileToSet(fileSet, "array.c");
    addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);
    addFileSetBuilder(fileSet, "javac -beef", "cSource", "-lrt", true);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("javac -beef") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-lrt") );
	QCOMPARE( data->componentInstantiationFlags.contains("-sw"), true );
}

// Software view of the software component should be able to replace flags of hardware component.
void tst_MakefileGenerator::swSWViewFlagReplace()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);
	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(sw, "someFileSet", softInsta);

    addFileToSet(fileSet, "array.c");

    addCmd2Insta(hardInsta, "gcc", "cSource", "-hw", false);
    addCmd2Insta(softInsta, "gcc", "cSource", "-sw", true);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->hardPart->buildCmd->getCommand(), QString("gcc") );
	QCOMPARE( data->componentInstantiationFlags.contains("-sw"), true );
}

// See if hardware builder is taken account properly.
void tst_MakefileGenerator::hwBuilder()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(sw, "someFileSet", softInsta);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2Insta(hardInsta, "super_asm", "cSource", "-hw", false);
    addCmd2Insta(softInsta, "", "cSource", "-sw", false);

    setFileBuilder(file, "", "-u", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("super_asm") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-u -lrt -sw -hw") );
}

// No software view in software components means no makefile.
void tst_MakefileGenerator::hwBuilderWithNosoftInsta()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);
	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(sw, "someFileSet", softInsta);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2Insta(hardInsta, "super_asm", "cSource", "-hw", false);

    setFileBuilder(file, "", "-u", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");

	QCOMPARE( stackParser.getParsedData()->size(), 0 );

	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QCOMPARE( makeParser.getParsedData()->size(), 0 );
}

// See if file set of hardware references work.
void tst_MakefileGenerator::hwRef()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    QSharedPointer<Component> hw = createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(hw, "someFileSet", hardInsta);

    addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2Insta(hardInsta, "super_asm", "cSource", "-hw", false);
    addCmd2Insta(softInsta, "", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, hw, "array.c");
    setFileBuilder(file, "", "-u", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("super_asm") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-u -lrt -hw") );
}

// See if hardware's own file set references function, while there are those of software component.
void tst_MakefileGenerator::hwandswRef()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    QSharedPointer<Component> hw = createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<FileSet> hfileSet = addFileSet(hw, "hardFileSet", hardInsta);

    addFileToSet(hfileSet, "harray.c");
    addFileSetBuilder(hfileSet, "", "cSource", "-hset", false);
    addCmd2Insta(hardInsta, "super_asm", "cSource", "-hw", false);

    QSharedPointer<File> hfile;
    getFile(hfile, hw, "harray.c");
    setFileBuilder(hfile, "", "-hu", false);

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QString sfileSetName = "softFileSet";
    addFileSet(sw, sfileSetName, softInsta);

    QSharedPointer<FileSet> sfileSet = sw->getFileSet(sfileSetName);
    addFileToSet(sfileSet, "sarray.c");
    addFileSetBuilder(sfileSet, "", "cSource", "-sset", false);
    addCmd2Insta(softInsta, "", "cSource", "-sw", false);

    QSharedPointer<File> sfile;
    getFile(sfile, sw, "sarray.c");
    setFileBuilder(sfile, "", "-su", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 2 );

	QCOMPARE( data->swObjects.first()->compiler, QString("super_asm") );
	QCOMPARE( data->swObjects.first()->fileName, QString("sarray.c") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-su -sset -sw -hw") );
	QCOMPARE( data->swObjects.last()->compiler, QString("super_asm") );
	QCOMPARE( data->swObjects.last()->fileName, QString("harray.c") );
	QCOMPARE( data->swObjects.last()->flags.simplified(), QString("-hu -hset -hw") );

	QCOMPARE( data->hardPart->buildCmd->getCommand(), QString("super_asm") );
	QCOMPARE( data->hardPart->buildCmd->getFlags(), QString("-hw") );
	QCOMPARE( data->componentInstantiationFlags.contains("-sw"), true );
}

// Instance specific files must be included to the makefile.
void tst_MakefileGenerator::instanceFiles()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta, ComponentInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softInsta);
    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");

    QString topFileSetName = NameGenerationPolicy::instanceFilesetName( "sysview", "software_0" );
	QSharedPointer<FileSet> topFileSet = addFileSet(topComponent, topFileSetName, QSharedPointer<ComponentInstantiation>());
    addFileToSet(topFileSet, "array.h", "cSource", true);
	
	QSharedPointer<ComponentInstance> referrringInstance = design->getComponentInstances()->first();
	referrringInstance->setFileSetRef( topFileSetName );

    QString topFileSetName2 = "testFileSet";
    QSharedPointer<FileSet> topFileSet2 = addFileSet(topComponent, topFileSetName2, QSharedPointer<ComponentInstantiation>());
    addFileToSet(topFileSet2, "joku.h", "cSource", true);

    addCmd2Insta(hardInsta, "gcc", "cSource", "-hw", false);
    addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
    QVERIFY( stackParser.getParsedData()->first()->instanceFileSet );
}

// Generate makefile with multiple source and header files.
void tst_MakefileGenerator::multipleFiles()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softInsta);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addFileToSet(fileSet, "support.c");
    addFileToSet(fileSet, "additional.c");
    addFileToSet(fileSet, "hiterbehn.c");
    addFileToSet(fileSet, "array.h", "cSource", true);
    addFileToSet(fileSet, "support.h", "cSource", true);

    addCmd2Insta(hardInsta, "gcc", "cSource", "-hw", false);
    addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 4 );

	QCOMPARE( data->swObjects.at(0)->compiler, QString("gcc" ) );
	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.at(0)->flags.simplified(), QString("-sw -hw") );
	QCOMPARE( data->swObjects.at(1)->compiler, QString("continental" ) );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data->swObjects.at(1)->flags.simplified(), QString("-y -sw -hw") );
	QCOMPARE( data->swObjects.at(2)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(3)->fileName, QString("hiterbehn.c") );
}

// Generate makefile when multiple file sets are featured.
void tst_MakefileGenerator::multipleFileSets()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> softInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

    QString ComponentInstanceName = "software_0";
    QString softInstaName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

    QSharedPointer<FileSet> afileSet = addFileSet(sw, "alphaFileSet", softInsta);
    QSharedPointer<FileSet> bfileSet = addFileSet(sw, "betaFileSet", softInsta);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2Insta(hardInsta, "gcc", "cSource", "-hw", false);
    addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 4 );

	QCOMPARE( data->swObjects.at(0)->compiler, QString("gcc" ) );
	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.at(0)->flags.simplified(), QString("-sw -hw") );
	QCOMPARE( data->swObjects.at(1)->compiler, QString("continental" ) );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data->swObjects.at(1)->flags.simplified(), QString("-y -sw -hw") );
	QCOMPARE( data->swObjects.at(2)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(3)->fileName, QString("hiterbehn.c") );
}

// Generate makefile when multiple components are featured.
void tst_MakefileGenerator::multipleComponents()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> asoftInsta;
    QSharedPointer<ComponentInstantiation> bsoftInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

    QSharedPointer<Component> asw = createSW("testware", hwInstanceName, design, "default", desgconf, asoftInsta,"testware_0");
    QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftInsta,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftInsta);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftInsta);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

	addCmd2Insta(hardInsta, "hopo", "cSource", "-hw", false);
	addCmd2Insta(asoftInsta, "", "cSource", "-amw", false);
    addCmd2Insta(bsoftInsta, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse(topComponent);

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 2 );
	QSharedPointer<MakeFileData> data1 = datas->first();
	QCOMPARE( data1->swObjects.size(), 2 );
	QCOMPARE( data1->componentInstantiationFlags.contains("-amw"), true );
	QCOMPARE( data1->name, QString("testware_0") );

	QCOMPARE( data1->swObjects.at(0)->compiler, QString("hopo" ) );
	QCOMPARE( data1->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data1->swObjects.at(0)->flags.simplified(), QString("-amw -hw") );
	QCOMPARE( data1->swObjects.at(1)->compiler, QString("continental" ) );
	QCOMPARE( data1->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data1->swObjects.at(1)->flags.simplified(), QString("-y -amw -hw") );

	QSharedPointer<MakeFileData> data2 = datas->last();
	QCOMPARE( data2->swObjects.size(), 2 );
	QCOMPARE( data2->componentInstantiationFlags.contains("-bmw"), true );
	QCOMPARE( data2->name, QString("stackware_0") );

	QCOMPARE( data2->swObjects.at(0)->fileName, QString("additional.c") );
	QCOMPARE( data2->swObjects.at(0)->compiler, QString("asm-meister" ) );
	QCOMPARE( data2->swObjects.at(0)->flags.simplified(), QString("-bmw -hw") );
	QCOMPARE( data2->swObjects.at(1)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data2->swObjects.at(1)->compiler, QString("asm-meister" ) );
	QCOMPARE( data2->swObjects.at(1)->flags.simplified(), QString("-bmw -hw") );
}

// Must work if there is multiple hardware where instances are mapped to.
void tst_MakefileGenerator::multipleHardWare()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> ahardInsta;
    QSharedPointer<ComponentInstantiation> bhardInsta;
    QSharedPointer<ComponentInstantiation> asoftInsta;
    QSharedPointer<ComponentInstantiation> bsoftInsta;

    createHW("Lenit_0", design, "firmware", desgconf, ahardInsta, "Lenit");
    createHW("MAR_0", design, "firmware", desgconf, bhardInsta, "MAR");

    QSharedPointer<Component> asw = createSW("testware", "Lenit_0", design, "default", desgconf, asoftInsta,"testware_0");
    QSharedPointer<Component> bsw = createSW("stackware", "MAR_0", design, "default", desgconf, bsoftInsta,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftInsta);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftInsta);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(afileSet, "array.h", "cSource", true);

    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2Insta(ahardInsta, "hopo", "cSource", "-ahw", false);
    addCmd2Insta(bhardInsta, "juu -f", "cSource", "-bhw", false);

	addCmd2Insta(asoftInsta, "", "cSource", "", false);
    addCmd2Insta(bsoftInsta, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 2 );
	QSharedPointer<MakeFileData> data1 = datas->first();
	QCOMPARE( data1->swObjects.size(), 2 );
	QCOMPARE( data1->name, QString("testware_0") );
	QCOMPARE( data1->hardPart->buildCmd->getCommand(), QString("hopo") );
	QCOMPARE( data1->hardPart->buildCmd->getFlags(), QString("-ahw") );

	QCOMPARE( data1->swObjects.at(0)->compiler, QString("hopo" ) );
	QCOMPARE( data1->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data1->swObjects.at(0)->flags.simplified(), QString("-ahw") );
	QCOMPARE( data1->swObjects.at(1)->compiler, QString("continental" ) );
	QCOMPARE( data1->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data1->swObjects.at(1)->flags.simplified(), QString("-y -ahw") );

	QSharedPointer<MakeFileData> data2 = datas->last();
	QCOMPARE( data2->swObjects.size(), 2 );
	QCOMPARE( data2->componentInstantiationFlags.contains("-bmw"), true );
	QCOMPARE( data2->name, QString("stackware_0") );
	QCOMPARE( data2->hardPart->buildCmd->getCommand(), QString("juu -f") );
	QCOMPARE( data2->hardPart->buildCmd->getFlags(), QString("-bhw") );
	QCOMPARE( data2->componentInstantiationFlags.contains("-bmw"), true );

	QCOMPARE( data2->swObjects.at(0)->fileName, QString("additional.c") );
	QCOMPARE( data2->swObjects.at(0)->compiler, QString("asm-meister" ) );
	QCOMPARE( data2->swObjects.at(0)->flags.simplified(), QString("-bmw -bhw") );
	QCOMPARE( data2->swObjects.at(1)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data2->swObjects.at(1)->compiler, QString("asm-meister" ) );
	QCOMPARE( data2->swObjects.at(1)->flags.simplified(), QString("-bmw -bhw") );
}

// Multiple hardware, along with their references to their own files.
void tst_MakefileGenerator::multipleHardWareMedRefs()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> ahardInsta;
    QSharedPointer<ComponentInstantiation> bhardInsta;
    QSharedPointer<ComponentInstantiation> asoftInsta;
    QSharedPointer<ComponentInstantiation> bsoftInsta;

    QSharedPointer<Component> hw = createHW("Lenit_0", design, "firmware", desgconf, ahardInsta, "Lenit");
    createHW("MAR_0", design, "firmware", desgconf, bhardInsta, "MAR");

    QSharedPointer<Component> asw = createSW("testware", "Lenit_0", design, "default", desgconf, asoftInsta,"testware_0");
    QSharedPointer<Component> bsw = createSW("stackware", "MAR_0", design, "default", desgconf, bsoftInsta,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftInsta);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftInsta);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(afileSet, "array.h", "cSource", true);

    QString hfileSetName = "hardFileSet";
    QSharedPointer<FileSet> hfileSet = addFileSet(hw, hfileSetName, ahardInsta);

    addFileToSet(hfileSet, "harray.c");
    addFileSetBuilder(hfileSet, "", "cSource", "-hset", false);

    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2Insta(ahardInsta, "hopo", "cSource", "-ahw", false);
    addCmd2Insta(bhardInsta, "juu", "cSource", "-bhw", false);

	addCmd2Insta(asoftInsta, "", "cSource", "", false);
    addCmd2Insta(bsoftInsta, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 2 );
	QSharedPointer<MakeFileData> data1 = datas->first();
	QCOMPARE( data1->swObjects.size(), 3 );
	QCOMPARE( data1->name, QString("testware_0") );

	QCOMPARE( data1->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data1->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data1->swObjects.at(2)->fileName, QString("harray.c") );

	QSharedPointer<MakeFileData> data2 = datas->last();
	QCOMPARE( data2->swObjects.size(), 2 );
	QCOMPARE( data2->name, QString("stackware_0") );

	QCOMPARE( data2->swObjects.at(0)->fileName, QString("additional.c") );
	QCOMPARE( data2->swObjects.at(1)->fileName, QString("hiterbehn.c") );
}

// Must work with multiple instances of the same component.
void tst_MakefileGenerator::multipleInstances()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<ComponentInstantiation> hardInsta;
	QSharedPointer<ComponentInstantiation> softInsta;

	QString hwInstanceName = "hardware_0";
	QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QSharedPointer<Component> sw = createSW("stackware", hwInstanceName, design, "default", desgconf, softInsta,
        "stackware_0");

	QSharedPointer<ConfigurableVLNVReference> swvlvnv( new ConfigurableVLNVReference(sw->getVlnv()) );
	QSharedPointer<ComponentInstance> softInstance(new ComponentInstance);
	softInstance->setInstanceName("stackware_1");
	softInstance->setComponentRef(swvlvnv);
	softInstance->setMapping(hwInstanceName);

	desgconf->addViewConfiguration("stackware_1",sw->getViews()->first()->name());

	QSharedPointer<QList<QSharedPointer<ComponentInstance> > > ComponentInstances = design->getComponentInstances();
	ComponentInstances->append(softInstance);
	design->setComponentInstances(ComponentInstances);
	library_.writeModelToFile("testOutput/stackware",sw);

	QSharedPointer<FileSet> fileSet = addFileSet(sw, "FileSet", softInsta);

	addFileToSet(fileSet, "additional.c");
	addFileToSet(fileSet, "hiterbehn.c");
	addFileToSet(fileSet, "array.h", "cSource", true);
	addFileToSet(fileSet, "support.h", "cSource", true);

	addCmd2Insta(hardInsta, "hopo", "cSource", "-hw", false);
	addCmd2Insta(softInsta, "asm-meister", "cSource", "-bmw", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 2 );
	QSharedPointer<MakeFileData> data1 = datas->first();
	QCOMPARE( data1->swObjects.size(), 2 );
	QCOMPARE( data1->name, QString("stackware_0") );

	QSharedPointer<MakeFileData> data2 = datas->last();
	QCOMPARE( data2->swObjects.size(), 2 );
	QCOMPARE( data2->name, QString("stackware_1") );
}

// No hardware means no stuff.
void tst_MakefileGenerator::noHardWare()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> softInsta;

    QSharedPointer<Component> sw = createSW("software", "", design, "default", desgconf, softInsta,"software_0");

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softInsta);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

    MakefileGenerator generator( makeParser, &utilityMock_, stackParser.getGeneralFileSet() );
	generator.generate(outputDir_, outputDir_, "tsydemi");
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 0 );
}

// Yield no makefile, if no files are present.
void tst_MakefileGenerator::noFilesComponent()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<ComponentInstantiation> hardInsta;
	QSharedPointer<ComponentInstantiation> asoftInsta;
	QSharedPointer<ComponentInstantiation> bsoftInsta;

	QString hwInstanceName = "hardware_0";
	QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QSharedPointer<Component> asw = createSW("whatware", hwInstanceName, design, "default", desgconf, asoftInsta,"whatware_0");
	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftInsta,"stackware_0");

	QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftInsta);
	QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftInsta);

	addFileToSet(bfileSet, "additional.c");
	addFileToSet(bfileSet, "hiterbehn.c");
	addFileToSet(bfileSet, "support.h", "cSource", true);

	addCmd2Insta(hardInsta, "hopo", "cSource", "-hw", false);
	addCmd2Insta(bsoftInsta, "asm-meister", "cSource", "-bmw", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 2 );

	QCOMPARE( data->swObjects.at(0)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("hiterbehn.c") );
}

// File with unsupported file does not appear in our lists.
void tst_MakefileGenerator::noFileType()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<ComponentInstantiation> hardInsta;
	QSharedPointer<ComponentInstantiation> softInsta;

	QString hwInstanceName = "hardware_0";
	QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QString ComponentInstanceName = "software_0";
	QString softInstaName = "default";

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

	QString fileSetName = "someFileSet";
	addFileSet(sw, fileSetName, softInsta);

	QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
	addFileToSet(fileSet, "hiterbehn.c", "topSecret");
	addFileToSet(fileSet, "array.c");
	addFileToSet(fileSet, "support.h", "cSource", true);

	addCmd2Insta(hardInsta, "gcc", "cSource", "-hw", false);
	addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
}

// Must include API dependencies to the makefile.
void tst_MakefileGenerator::apiUsage()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> asoftInsta;
	QSharedPointer<ComponentInstantiation> bsoftInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QSharedPointer<Component> asw = createSW("testware", hwInstanceName, design, "default", desgconf, asoftInsta,"testware_0");
	addAPI(asw, "apina", DEPENDENCY_REQUESTER);

	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftInsta,"stackware_0");
	addAPI(bsw, "banaani", DEPENDENCY_PROVIDER);

	addAPIConnection(design,"testware_0","apina","stackware_0","banaani");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftInsta);
	QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftInsta);

	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);
	addCmd2Insta(asoftInsta, "", "cSource", "", false);
	addCmd2Insta(bsoftInsta, "", "cSource", "", false);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse(topComponent);

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE(datas->size(), 1);
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE(data->swObjects.size(), 4);

	QCOMPARE(data->swObjects.at(0)->fileName, QString("array.c"));
	QCOMPARE(data->swObjects.at(1)->fileName, QString("support.c"));
	QCOMPARE(data->swObjects.at(2)->fileName, QString("additional.c"));
	QCOMPARE(data->swObjects.at(3)->fileName, QString("hiterbehn.c"));
}

// A more complex situation with API dependency.
void tst_MakefileGenerator::threeLevelStack()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> asoftInsta;
    QSharedPointer<ComponentInstantiation> bsoftInsta;
    QSharedPointer<ComponentInstantiation> gsoftInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QSharedPointer<Component> asw = createSW("testware", hwInstanceName, design, "default", desgconf, asoftInsta,"testware_0");
	addAPI(asw, "apina", DEPENDENCY_REQUESTER);

	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftInsta,"stackware_0");
	addAPI(bsw, "banaani", DEPENDENCY_PROVIDER);

    QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", desgconf, gsoftInsta,"driver_0");

	addAPI(bsw, "stackDriver", DEPENDENCY_REQUESTER);
	addAPI(gsw, "driverStack", DEPENDENCY_PROVIDER);

	addAPIConnection(design, "testware_0","apina","stackware_0","banaani");
	addAPIConnection(design, "stackware_0","stackDriver","driver_0","driverStack");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftInsta);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftInsta);
	QSharedPointer<FileSet> cfileSet = addFileSet(gsw, "gamaFileSet", gsoftInsta);

	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);
	addCmd2Insta(asoftInsta, "", "cSource", "", false);
	addCmd2Insta(bsoftInsta, "", "cSource", "", false);
	addCmd2Insta(gsoftInsta, "", "cSource", "", false);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 5 );

	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data->swObjects.at(2)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(3)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data->swObjects.at(4)->fileName, QString("ok.c") );
}

// If no top level in the stack, there can be no build!
void tst_MakefileGenerator::fullCircularapiUsage()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> asoftInsta;
    QSharedPointer<ComponentInstantiation> bsoftInsta;
    QSharedPointer<ComponentInstantiation> gsoftInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QSharedPointer<Component> asw = createSW("topware", hwInstanceName, design, "default", desgconf, asoftInsta,"topware_0");
	addAPI(asw, "apina", DEPENDENCY_REQUESTER);

	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftInsta,"stackware_0");
	addAPI(bsw, "banaani", DEPENDENCY_PROVIDER);

	QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", desgconf, gsoftInsta,"driver_0");
	addAPI(bsw, "stackDriver", DEPENDENCY_REQUESTER);
	addAPI(gsw, "driverStack", DEPENDENCY_PROVIDER);
	addAPI(gsw, "bottomUp", DEPENDENCY_REQUESTER);
	addAPI(asw, "upBottom", DEPENDENCY_PROVIDER);

	addAPIConnection(design, "topware_0","apina","stackware_0","banaani");
	addAPIConnection(design, "stackware_0","stackDriver","driver_0","driverStack");
	addAPIConnection(design, "topware_0","upBottom","driver_0","bottomUp");

	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);
	addCmd2Insta(asoftInsta, "", "cSource", "", false);
	addCmd2Insta(bsoftInsta, "", "cSource", "", false);
	addCmd2Insta(gsoftInsta, "", "cSource", "", false);

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftInsta);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftInsta);
    QSharedPointer<FileSet> cfileSet = addFileSet(gsw, "gamaFileSet", gsoftInsta);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 0 );
}

// Must be able to handle circular dependencies in lower levels.
void tst_MakefileGenerator::circularapiUsage()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<ComponentInstantiation> hardInsta;
    QSharedPointer<ComponentInstantiation> asoftInsta;
    QSharedPointer<ComponentInstantiation> bsoftInsta;
    QSharedPointer<ComponentInstantiation> gsoftInsta;

    QString hwInstanceName = "hardware_0";
    QString hardInstaName = "firmware";
    createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

    QSharedPointer<Component> asw = createSW("testware", hwInstanceName, design, "default", desgconf, asoftInsta,"testware_0");
	addAPI(asw, "apina", DEPENDENCY_REQUESTER);

	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftInsta,"stackware_0");
	addAPI(bsw, "banaani", DEPENDENCY_PROVIDER);

    QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", desgconf, gsoftInsta,"driver_0");

    QSharedPointer<ApiInterface> stackDriver = QSharedPointer<ApiInterface>(new ApiInterface );
	addAPI(bsw, "stackDriver", DEPENDENCY_REQUESTER);

	QSharedPointer<ApiInterface> driverStack = QSharedPointer<ApiInterface>(new ApiInterface );
	addAPI(gsw, "driverStack", DEPENDENCY_PROVIDER);

	QSharedPointer<ApiInterface> bottomUp = QSharedPointer<ApiInterface>(new ApiInterface );
	addAPI(gsw, "bottomUp", DEPENDENCY_REQUESTER);

	QSharedPointer<ApiInterface> upBottom = QSharedPointer<ApiInterface>(new ApiInterface );
	addAPI(bsw, "upBottom", DEPENDENCY_PROVIDER);;

	addAPIConnection(design, "testware_0","apina","stackware_0","banaani");
	addAPIConnection(design, "stackware_0","stackDriver","driver_0","driverStack");
	addAPIConnection(design, "stackware_0","upBottom","driver_0","bottomUp");

	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);
	addCmd2Insta(asoftInsta, "", "cSource", "", false);
	addCmd2Insta(bsoftInsta, "", "cSource", "", false);
	addCmd2Insta(gsoftInsta, "", "cSource", "", false);

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftInsta);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftInsta);
    QSharedPointer<FileSet> cfileSet = addFileSet(gsw, "gamaFileSet", gsoftInsta);

    addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(afileSet, "array.h", "cSource", true);

    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );

	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 5 );

	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data->swObjects.at(2)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(3)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data->swObjects.at(4)->fileName, QString("ok.c") );
}

// Same file in separate executables should not cause conflict
void tst_MakefileGenerator::sameFileSeparateExe()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<ComponentInstantiation> hardInsta;
	QSharedPointer<ComponentInstantiation> asoftInsta;
	QSharedPointer<ComponentInstantiation> bsoftInsta;

	QString hwInstanceName = "hardware_0";
	QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QSharedPointer<Component> asw = createSW("testware", hwInstanceName, design, "default", desgconf, asoftInsta,"testware_0");
	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftInsta,"stackware_0");

	QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftInsta);
	QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftInsta);

	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);
	addCmd2Insta(asoftInsta, "", "cSource", "", false);
	addCmd2Insta(bsoftInsta, "", "cSource", "", false);

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c");

	addCmd2Insta(hardInsta, "hopo", "cSource", "-hw", false);
	addCmd2Insta(bsoftInsta, "asm-meister", "cSource", "-bmw", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QCOMPARE( makeParser.getParsedData()->first()->conflicts.size(), 0 );
	QCOMPARE( makeParser.getParsedData()->last()->conflicts.size(), 0 );
}

// Must detect if the same file is used by multiple component instances or file sets!
void tst_MakefileGenerator::sameFileDiffCompiler()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<ComponentInstantiation> hardInsta;
	QSharedPointer<ComponentInstantiation> softInsta;

	QString hwInstanceName = "hardware_0";
	QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QString ComponentInstanceName = "software_0";
	QString softInstaName = "default";

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf, softInsta,ComponentInstanceName);

	QSharedPointer<FileSet> afileSet = addFileSet(sw, "alphaFileSet", softInsta);
	QSharedPointer<FileSet> bfileSet = addFileSet(sw, "betaFileSet", softInsta);

	addCmd2Insta(hardInsta, "gcc", "cSource", "", false);
	addCmd2Insta(softInsta, "", "cSource", "", false);

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c");

	addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

	addFileSetBuilder(afileSet, "javac -beef", "cSource", "-lrt -pthread", false);
	addFileSetBuilder(bfileSet, "", "cSource", "-pthread -lrt", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QVector<QSet<QSharedPointer<MakeObjectData> > > conflicts = makeParser.getParsedData()->first()->conflicts;

	QCOMPARE( conflicts.size(), 2 );

	QCOMPARE( conflicts.first().size(), 2 );
	QCOMPARE( conflicts.last().size(), 2 );
	QSharedPointer<MakeObjectData> eka = *(conflicts.first().begin());
	QSharedPointer<MakeObjectData> toka = *(conflicts.first().end() - 1);
	QCOMPARE( eka->path, toka->path );
	QCOMPARE( eka->fileName, toka->fileName );

	QCOMPARE( eka->fileName, QString("array.c") );

	eka = *(conflicts.last().begin());
	toka = *(conflicts.last().end() - 1);
	QCOMPARE( eka->path, toka->path );
	QCOMPARE( eka->fileName, toka->fileName );

	QCOMPARE( eka->fileName, QString("support.c") );
}

// Must detect if the same file is used by multiple component instances or file sets!
void tst_MakefileGenerator::sameFileDiffFlags()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<ComponentInstantiation> hardInsta;
	QSharedPointer<ComponentInstantiation> softInsta;

	QString hwInstanceName = "hardware_0";
	QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QString ComponentInstanceName = "software_0";
	QString softInstaName = "default";

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf,
		softInsta,ComponentInstanceName);

	QSharedPointer<FileSet> afileSet = addFileSet(sw, "alphaFileSet", softInsta);
	QSharedPointer<FileSet> bfileSet = addFileSet(sw, "betaFileSet", softInsta);

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c");

	addCmd2Insta(hardInsta, "gcc", "cSource", "-hw", false);
	addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

	addFileSetBuilder(afileSet, "javac -beef", "cSource", "-lrt -pthread", false);
	addFileSetBuilder(bfileSet, "javac -beef", "cSource", "-pthread", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QVector<QSet<QSharedPointer<MakeObjectData> > > conflicts = makeParser.getParsedData()->first()->conflicts;

	QCOMPARE( conflicts.size(), 2 );

	QCOMPARE( conflicts.first().size(), 2 );
	QCOMPARE( conflicts.last().size(), 2 );
	QSharedPointer<MakeObjectData> eka = *(conflicts.first().begin());
	QSharedPointer<MakeObjectData> toka = *(conflicts.first().end() - 1);
	QCOMPARE( eka->path, toka->path );
	QCOMPARE( eka->fileName, toka->fileName );

	QCOMPARE( eka->fileName, QString("array.c") );

	eka = *(conflicts.last().begin());
	toka = *(conflicts.last().end() - 1);
	QCOMPARE( eka->path, toka->path );
	QCOMPARE( eka->fileName, toka->fileName );

	QCOMPARE( eka->fileName, QString("support.c") );
}

// Conflict is reported even if the file is actually completely same.
void tst_MakefileGenerator::sameFile()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<ComponentInstantiation> hardInsta;
	QSharedPointer<ComponentInstantiation> softInsta;

	QString hwInstanceName = "hardware_0";
	QString hardInstaName = "firmware";
	createHW(hwInstanceName, design, hardInstaName, desgconf, hardInsta);

	QString ComponentInstanceName = "software_0";
	QString softInstaName = "default";

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softInstaName, desgconf,
		softInsta,ComponentInstanceName);

	QSharedPointer<FileSet> afileSet = addFileSet(sw, "alphaFileSet", softInsta);
	QSharedPointer<FileSet> bfileSet = addFileSet(sw, "betaFileSet", softInsta);

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c", "cSource", true);
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c", "cSource", true);

	addCmd2Insta(hardInsta, "gcc", "cSource", "-hw", false);
	addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

	addFileSetBuilder(afileSet, "javac -beef", "cSource", "-lrt -pthread", false);
	addFileSetBuilder(bfileSet, "javac -beef", "cSource", "-lrt -pthread", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QVector<QSet<QSharedPointer<MakeObjectData> > > conflicts = makeParser.getParsedData()->first()->conflicts;

	QCOMPARE(conflicts.size(), 1);
}

// See if a really basic generation case works.
void tst_MakefileGenerator::basicGeneration()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	MakefileParser makeParser( &library_, stackParser );
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QSharedPointer<MakeFileData> makeData = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData);

	makeData->hardPart = QSharedPointer<StackPart>( new StackPart );
	makeData->hardPart->buildCmd = QSharedPointer<FileBuilder>( new FileBuilder );
	makeData->hardPart->buildCmd->setCommand("gcc");
	makeData->name = "software_0";
	makeData->hardPart->buildCmd->setFlags("-joku");
	makeData->componentInstantiationFlags.append("-jotain");
	makeData->makeName = makeData->name + ".mak";

	QSharedPointer<MakeObjectData> mod( new MakeObjectData );
	makeData->swObjects.append( mod );
	mod->fileName = "array.c";
	mod->compiler = "gcc";
	mod->flags = "-sw -hw";

	MakefileGenerator generator( makeParser, &utilityMock_, stackParser.getGeneralFileSet() );
	generator.generate(outputDir_,outputDir_,"tsydemi");

	verifyOutputContains("software_0", "_OBJ= array.c.o");
	verifyOutputContains("software_0", "OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))");
	verifyOutputContains("software_0", "ODIR= obj");
	verifyOutputContains("software_0", "ENAME= software_0");
	verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -joku -jotain");
	verifyOutputContains("software_0", "EBUILDER= gcc");
	verifyOutputContains("software_0", "$(ENAME): $(OBJ)\n\t$(EBUILDER) -o $(ENAME) $(OBJ) $(EFLAGS)");
	verifyOutputContains("software_0", "gcc " + DEFAULT_OBJECT_FLAGS + " $(ODIR)/array.c.o /array.c $(INCLUDES)" +
		" $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
}

// Must be able to generate multiple objects.
void tst_MakefileGenerator::multiObjectGeneration()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	MakefileParser makeParser( &library_, stackParser );
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QSharedPointer<MakeFileData> makeData = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData);

	makeData->hardPart = QSharedPointer<StackPart>( new StackPart );
	makeData->hardPart->buildCmd = QSharedPointer<FileBuilder>( new FileBuilder() );
	makeData->hardPart->buildCmd->setCommand("gcc");
    makeData->name = "software_0";
    makeData->makeName = makeData->name + ".mak";

	QSharedPointer<MakeObjectData> mod1( new MakeObjectData );
	makeData->swObjects.append( mod1 );
	mod1->fileName = "array.c";
	mod1->compiler = "gcc";
	mod1->flags = "-sw -hw";

	QSharedPointer<MakeObjectData> mod2( new MakeObjectData );
	makeData->swObjects.append( mod2 );
	mod2->fileName = "support.c";
	mod2->compiler = "super-asm";
	mod2->flags = "-a -b";

	MakefileGenerator generator( makeParser, &utilityMock_, stackParser.getGeneralFileSet() );
	generator.generate(outputDir_,outputDir_,"tsydemi");

	verifyOutputContains("software_0", "_OBJ= array.c.o support.c.o");
	verifyOutputContains("software_0", "gcc " + DEFAULT_OBJECT_FLAGS + 
		" $(ODIR)/array.c.o /array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
	verifyOutputContains("software_0", "super-asm " + DEFAULT_OBJECT_FLAGS + 
		" $(ODIR)/support.c.o /support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -a -b");
}

// Must be able to generate multiple files.
void tst_MakefileGenerator::multiFileGeneration()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	MakefileParser makeParser( &library_, stackParser );
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QSharedPointer<MakeFileData> makeData1 = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData1);

	makeData1->hardPart = QSharedPointer<StackPart>( new StackPart );
	makeData1->hardPart->buildCmd = QSharedPointer<FileBuilder>( new FileBuilder() );
	makeData1->hardPart->buildCmd->setCommand("gcc");
	makeData1->name = "software_0";
	makeData1->hardPart->buildCmd->setFlags("-joku");
    makeData1->componentInstantiationFlags.append("-jotain");
    makeData1->makeName = makeData1->name + ".mak";

	QSharedPointer<MakeObjectData> mod1( new MakeObjectData );
	makeData1->swObjects.append( mod1 );
	mod1->fileName = "array.c";
	mod1->compiler = "gcc";
	mod1->flags = "-sw -hw";

	QSharedPointer<MakeFileData> makeData2 = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData2);

	makeData2->hardPart = QSharedPointer<StackPart>(new StackPart);
	makeData2->hardPart->buildCmd = QSharedPointer<FileBuilder>(new FileBuilder());
	makeData2->hardPart->buildCmd->setCommand("j33");
	makeData2->name = "testware_0";
	makeData2->hardPart->buildCmd->setFlags("-yks");
	makeData2->componentInstantiationFlags.append("-kaks");
	makeData2->makeName = makeData2->name + ".mak";

	QSharedPointer<MakeObjectData> mod2( new MakeObjectData );
	makeData2->swObjects.append( mod2 );
	mod2->fileName = "support.c";
	mod2->compiler = "super-asm";
	mod2->flags = "-a -b";

	MakefileGenerator generator( makeParser, &utilityMock_, stackParser.getGeneralFileSet() );
	generator.generate(outputDir_,outputDir_,"tsydemi");

	verifyOutputContains("software_0", "_OBJ= array.c.o");
	verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -joku -jotain");
	verifyOutputContains("software_0", "ENAME= software_0");
	verifyOutputContains("software_0", "EBUILDER= gcc");
	verifyOutputContains("software_0", "gcc " + DEFAULT_OBJECT_FLAGS +
		" $(ODIR)/array.c.o /array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");

	verifyOutputContains("testware_0", "_OBJ= support.c.o");
	verifyOutputContains("testware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -yks -kaks");
	verifyOutputContains("testware_0", "ENAME= testware_0");
	verifyOutputContains("testware_0", "EBUILDER= j33");
	verifyOutputContains("testware_0", "super-asm " + DEFAULT_OBJECT_FLAGS +
		" $(ODIR)/support.c.o /support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -a -b");
}

// File with no compiler must not be in makefile!
void tst_MakefileGenerator::noCompiler()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	MakefileParser makeParser( &library_, stackParser );
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QSharedPointer<MakeFileData> makeData = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData);

	makeData->hardPart = QSharedPointer<StackPart>( new StackPart );
	makeData->hardPart->buildCmd = QSharedPointer<FileBuilder>( new FileBuilder() );
	makeData->hardPart->buildCmd->setCommand("gcc");
	makeData->name = "software_0";

	QSharedPointer<MakeObjectData> mod1( new MakeObjectData );
	makeData->swObjects.append( mod1 );
	mod1->fileName = "joku.c";
	mod1->compiler = "";
	mod1->flags = "-sw -hw";

	QSharedPointer<MakeObjectData> mod2( new MakeObjectData );
	makeData->swObjects.append( mod2 );
	mod2->fileName = "array.c";
	mod2->compiler = "gcc";
	mod2->flags = "-sw -hw";

	MakefileGenerator generator( makeParser, &utilityMock_, stackParser.getGeneralFileSet() );
	generator.generate(outputDir_,outputDir_,"tsydemi");

	verifyOutputContains("software_0", "_OBJ= array.c.o");
}

// A case used to test if correct makefile comes from original parsing results.
void tst_MakefileGenerator::allTheWay()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<ComponentInstantiation> hardInsta;
	QSharedPointer<ComponentInstantiation> softInsta;

	createHW("hardware_0", design, "firmware", desgconf, hardInsta);
	addCmd2Insta(hardInsta, "gcc", "cSource", "-hw", false);

	QSharedPointer<Component> sw = createSW("software", "hardware_0", design, "default", desgconf, softInsta, "software_0");

	QString fileSetName = "someFileSet";
	QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softInsta);

	addFileToSet(fileSet, "array.c");
	addCmd2Insta(softInsta, "gcc", "cSource", "-sw", false);

	SWStackParser stackParser(&library_, topComponent, design, desgconf);
	stackParser.parse("tsydemi");

	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	MakefileGenerator generator( makeParser, &utilityMock_, stackParser.getGeneralFileSet() );
	generator.generate(outputDir_,outputDir_,"tsydemi");

	verifyOutputContains("software_0", "_OBJ= array.c.o");
	verifyOutputContains("software_0", "OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))");
	verifyOutputContains("software_0", "ODIR= obj");
	verifyOutputContains("software_0", "ENAME= software_0");
	verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS)");
	verifyOutputContains("software_0", "EBUILDER= gcc");
	verifyOutputContains("software_0", "$(ENAME): $(OBJ)\n\t$(EBUILDER) -o $(ENAME) $(OBJ) $(EFLAGS)");
	verifyOutputContains("software_0", "gcc " + DEFAULT_OBJECT_FLAGS +
		" $(ODIR)/array.c.o /array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
}

QSharedPointer<Component> tst_MakefileGenerator::createDesign(QSharedPointer<Design>& design,
    QSharedPointer<DesignConfiguration>& desgconf)
{
    VLNV designVLNV("", "vendor", "lib", "design", "1.0");
    design = QSharedPointer<Design>(new Design(designVLNV));

    VLNV configurationVLNV("", "vendor", "lib", "design-conf", "1.0");
    desgconf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(configurationVLNV));
    desgconf->setDesignRef(designVLNV);

    VLNV topVLNV("","vendor","lib","master-plan","1.0");
    QSharedPointer<Component> topComponent = QSharedPointer<Component>(new Component(topVLNV));
    topComponent->setImplementation(KactusAttribute::SYSTEM);
    
    library_.addComponent(topComponent);
    library_.writeModelToFile(outputDir_ + "/master-plan",topComponent);

    outputDir_ = QFileInfo(library_.getPath(topVLNV)).absolutePath();

    QSharedPointer<SystemView> systemView( new SystemView("sysview") );
    systemView->setHierarchyRef(configurationVLNV);
	
    QList<QSharedPointer<SystemView> > systemViews;
	systemViews.append(systemView);
	topComponent->setSystemViews(systemViews);

    return topComponent;
}

QSharedPointer<File> tst_MakefileGenerator::addFileToSet(QSharedPointer<FileSet> fileSet, QString fileName,
    QString fileType/*="cSource"*/, bool isInclude/*=false*/)
{
    QSettings settings;

    QSharedPointer<File> file = fileSet->addFile(fileName, settings);
    file->addFileType(fileType);
    file->setIncludeFile(isInclude);

	return file;
}

QSharedPointer<Component> tst_MakefileGenerator::createSW(QString swName, QString hwInstanceName,
    QSharedPointer<Design> design, QString softInstaName,  QSharedPointer<DesignConfiguration> desgconf,
    QSharedPointer<ComponentInstantiation>& softInsta, QString ComponentInstanceName)
{
    VLNV swvlvnv("","vendor","lib",swName,"1.0");
	QSharedPointer<ConfigurableVLNVReference> swvlvnv2( new ConfigurableVLNVReference( swvlvnv ) );
    QSharedPointer<Component> swComponent = QSharedPointer<Component>(new Component(swvlvnv));
    QSharedPointer<ComponentInstance> softInstance( new ComponentInstance );
    softInstance->setInstanceName(ComponentInstanceName);
    softInstance->setComponentRef(swvlvnv2);
    softInstance->setMapping(hwInstanceName);

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > ComponentInstances = design->getComponentInstances();
    ComponentInstances->append(softInstance);
    design->setComponentInstances(ComponentInstances);
    library_.addComponent(swComponent);
    library_.writeModelToFile("testOutput/" + swName, swComponent);

    softInsta = QSharedPointer<ComponentInstantiation>(new ComponentInstantiation);
    softInsta->setName(softInstaName);
    swComponent->getComponentInstantiations()->append(softInsta);

    QSharedPointer<View> softView = QSharedPointer<View>(new View);
    softView->setName(softInstaName + "_view");
    softView->setComponentInstantiationRef(softInstaName);
    swComponent->getViews()->append(softView);

	desgconf->addViewConfiguration(ComponentInstanceName, softView->name());

    return swComponent;
}

QSharedPointer<Component> tst_MakefileGenerator::createHW(QString const& hwInstanceName,
    QSharedPointer<Design> design, QString const& hardInstaName, QSharedPointer<DesignConfiguration> designConfig, 
    QSharedPointer<ComponentInstantiation>& hardInsta, QString const& hwName/*="hardware"*/)
{
	VLNV hwVLNV("", "vendor", "lib", hwName, "1.0");	
    QSharedPointer<Component> hwComponent = QSharedPointer<Component>(new Component(hwVLNV));
   
    QSharedPointer<ConfigurableVLNVReference> vlnvRef(new ConfigurableVLNVReference(hwVLNV));
    QSharedPointer<ComponentInstance> hwInstance(new ComponentInstance(hwInstanceName, vlnvRef));
    hwInstance->setDescription("esim");
    hwInstance->setUuid(hwInstanceName);

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances = design->getComponentInstances();
    instances->append(hwInstance);
    library_.addComponent(hwComponent);
    library_.writeModelToFile("testOutput/" + hwName, hwComponent);

    hardInsta = QSharedPointer<ComponentInstantiation>(new ComponentInstantiation);
    hardInsta->setName(hardInstaName);
    hwComponent->getComponentInstantiations()->append(hardInsta);

    QSharedPointer<View> hardView = QSharedPointer<View>(new View);
    hardView->setName(hardInstaName + "_view");
    hardView->setComponentInstantiationRef(hardInstaName);
    hwComponent->getViews()->append(hardView);

	designConfig->addViewConfiguration(hwInstanceName, hardView->name());

    return hwComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_MakefileGenerator::verifyOutputContains()
//-----------------------------------------------------------------------------
void tst_MakefileGenerator::verifyOutputContains(QString instanceName, QString const& expectedOutput)
{
    QFile outputFile(outputDir_ + "/" + instanceName + MAKEFILE_FRAGMENT_SUFFIX);

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    QString outputString = outputFile.readAll();
    outputFile.close();

    outputString = outputString.replace( QRegExp("[ ]+"), " " );

    if (!outputString.contains(expectedOutput))
    {
        QStringList outputLines = outputString.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            QCOMPARE(outputString, expectedOutput);
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
    else if(outputString.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " +
            QString::number(outputString.count(expectedOutput)) + " times in output.").toLocal8Bit());
    }
}

void tst_MakefileGenerator::getFile(QSharedPointer<File>& file, QSharedPointer<Component> component, QString fileName)
{
    foreach( QSharedPointer<FileSet> fset, *component->getFileSets())
    {
        foreach( QSharedPointer<File> possibleFile, *fset->getFiles())
        {
            if ( possibleFile->name() == fileName )
            {
                file = possibleFile;
                return;
            }
        }
    }
}

void tst_MakefileGenerator::addCmd2Insta(QSharedPointer<ComponentInstantiation> insta, QString command, QString fileType,
    QString flags, bool replace)
{
    QSharedPointer<FileBuilder> cmd = QSharedPointer<FileBuilder>(new FileBuilder());
    cmd->setCommand(command);
    cmd->setFileType(fileType);
    cmd->setFlags(flags);

    if (replace)
    {
        cmd->setReplaceDefaultFlags("1");
    }
    else
    {
        cmd->setReplaceDefaultFlags("0");
    }

	insta->getDefaultFileBuilders()->append(cmd);
}

QSharedPointer<FileSet> tst_MakefileGenerator::addFileSet(QSharedPointer<Component> component, QString fileSetName,
    QSharedPointer<ComponentInstantiation> insta)
{
    QSharedPointer<FileSet> fileSet(new FileSet(fileSetName, "sourceFiles"));

	if (insta)
	{
		insta->getFileSetReferences()->append(fileSetName);
	}
	
	component->getFileSets()->append(fileSet);

    return fileSet;
}

void tst_MakefileGenerator::addFileSetBuilder(QSharedPointer<FileSet> fileSet, QString command, QString fileType, QString flags, bool replace)
{
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > filebuilderList = fileSet->getDefaultFileBuilders();
    QSharedPointer<FileBuilder> fileBuilder = QSharedPointer<FileBuilder>(new FileBuilder);
    fileBuilder->setCommand(command);
    fileBuilder->setFileType(fileType);
    fileBuilder->setFlags(flags);
    fileBuilder->setReplaceDefaultFlags("false");

	if (replace)
	{
		fileBuilder->setReplaceDefaultFlags("true");
	}

	filebuilderList->append(fileBuilder);
}

void tst_MakefileGenerator::setFileBuilder(QSharedPointer<File> file, QString command, QString flags, bool replace)
{
    QSharedPointer<BuildCommand> buildCmd( new BuildCommand() );
    buildCmd->setCommand(command);
    buildCmd->setFlags(flags);
    buildCmd->setReplaceDefaultFlags("false");

	if ( replace )
	{
		buildCmd->setReplaceDefaultFlags("true");
	}

    file->setBuildcommand( buildCmd );
}

void tst_MakefileGenerator::addAPI(QSharedPointer<Component> asw, QString name, DependencyDirection dir)
{
	QSharedPointer<ApiInterface> apina = QSharedPointer<ApiInterface>(new ApiInterface );
	QList<QSharedPointer<ApiInterface> > apit = asw->getApiInterfaces();
	apina->setDependencyDirection(dir);
	apina->setName(name);
	apit.append(apina);
	asw->setApiInterfaces( apit );
}

void tst_MakefileGenerator::addAPIConnection(QSharedPointer<Design> design, QString com1,
	QString bus1, QString com2, QString bus2)
{
	QList<QSharedPointer<ApiInterconnection> > deps = design->getApiConnections();
	QSharedPointer<ApiInterconnection> dependency1( new ApiInterconnection );
	QSharedPointer<ActiveInterface> air1( new ActiveInterface(com1, bus1));
	QSharedPointer<ActiveInterface> air2( new ActiveInterface(com2, bus2));
	dependency1->setStartInterface(air1);
	dependency1->getActiveInterfaces()->append(air2);
	deps.append(dependency1);
	design->setApiConnections(deps);
}

QTEST_APPLESS_MAIN(tst_MakefileGenerator)

#include "tst_MakefileGenerator.moc"
