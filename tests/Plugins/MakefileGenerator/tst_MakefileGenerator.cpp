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
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>
#include <Plugins/common/NameGenerationPolicy.h>

#include <QRegExp>

class tst_MakefileGenerator : public QObject
{
    Q_OBJECT

public:
    tst_MakefileGenerator();

private slots:
    void baseCase();

	// Overrides.
    void fileBuildOverride();
    void fileSetBuildOverride();
    void fileFlagReplace();
    void fileSetFlagReplace();
    void swSWViewFlagReplace();

	// Hardware and references.
    void hwBuilder();
    void hwBuilderWithNoSoftView();
    void hwRef();
    void hwandswRef();
    void instanceHeaders();

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

    QSharedPointer<Component> createSW(QString swName, QString hwInstanceName, QSharedPointer<Design> design, QString softViewName,
		 QSharedPointer<DesignConfiguration> desgconf, QSharedPointer<SWView>& softView, QString swInstanceName);

    QSharedPointer<FileSet> addFileSet(QSharedPointer<Component> component, QString fileSetName, QSharedPointer<SWView> view);

    QSharedPointer<File> addFileToSet(QSharedPointer<FileSet> fileSet, QString fileName, QString fileType="cSource", bool isInclude=false);

    void createSWSWView(QSharedPointer<SWView> softView, QString softViewName, QSharedPointer<Component> sw);

    QSharedPointer<Component> createHW(QString const& hwInstanceName, QSharedPointer<Design> design, QString const& hardViewName,
		 QSharedPointer<DesignConfiguration> designConfig, QSharedPointer<SWView>& hardView, QString const& hwName="hardware");

    QSharedPointer<Component> createDesign(QSharedPointer<Design> &design, QSharedPointer<DesignConfiguration> &desgconf);

    void verifyOutputContains(QString instanceName, QString const& expectedOutput);

    void setFileBuilder(QSharedPointer<File> file, QString command, QString flags, bool replace);

    void addFileSetBuilder(QSharedPointer<FileSet> fileSet, QString command, QString fileType, QString flags, bool replace);

	void addCmd2View(QSharedPointer<SWView> view, QString command, QString fileType, QString flags, bool replace);

	void addAPI(QSharedPointer<Component> asw, QString name, DependencyDirection dir);

	void addAPIConnection(QSharedPointer<Design> design, QString com1, QString bus1, QString com2, QString bus2);

    //! The test mock for library interface.
    LibraryMock library_;

    //! Output directory for the generator
    QString outputDir_;

    //! Mock utility to be used in tests
    PluginUtilityAdapter utilityMock_;
};

tst_MakefileGenerator::tst_MakefileGenerator(): library_( this ), utilityMock_(&library_, 0, "", this)
{
}

// A rather basic and secure case testing that all the needed lines come for one file.
void tst_MakefileGenerator::baseCase()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    createHW("hardware_0", design, "firmware", desgconf, hardView);
    addCmd2View(hardView, "gcc", "cSource", "-hw", false);

    QSharedPointer<Component> sw = createSW("software", "hardware_0", design, "default", desgconf, softView, "software_0");

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, outputDir_ );
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

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);
	addCmd2View(hardView, "gcc", "cSource", "", false);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc -o", "cSource", "-sw", false);

    setFileBuilder(file, "python", "-l", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("python") );
	QCOMPARE( data->swObjects.first()->flags.trimmed(), QString("-l -sw") );
	QCOMPARE( data->softViewFlags.contains("-sw"), true );
}

// Case used to test if file set builder properly overrides other builders.
void tst_MakefileGenerator::fileSetBuildOverride()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);
	addCmd2View(hardView, "gcc", "cSource", "", false);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc -o", "cSource", "-sw", false);
    addFileSetBuilder(fileSet, "javac -beef", "cSource", "-lrt", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("javac -beef") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-lrt -sw") );
	QCOMPARE( data->softViewFlags.contains("-sw"), true );
}

// Case used to test if file flags properly replace other flags if needed.
void tst_MakefileGenerator::fileFlagReplace()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);
	addCmd2View(hardView, "gcc", "cSource", "", false);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    setFileBuilder(file, "", "-u", true);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("gcc") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-u") );
	QCOMPARE( data->softViewFlags.contains("-sw"), true );
}

// Case used to test if file set flags properly replace other flags if needed.
void tst_MakefileGenerator::fileSetFlagReplace()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);
	addCmd2View(hardView, "gcc", "cSource", "", false);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);
    addFileSetBuilder(fileSet, "javac -beef", "cSource", "-lrt", true);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->swObjects.first()->compiler, QString("javac -beef") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-lrt") );
	QCOMPARE( data->softViewFlags.contains("-sw"), true );
}

// Software view of the software component should be able to replace flags of hardware component.
void tst_MakefileGenerator::swSWViewFlagReplace()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);
	addCmd2View(hardView, "gcc", "cSource", "", false);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(sw, "someFileSet", softView);

    addFileToSet(fileSet, "array.c");

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", true);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 1 );

	QCOMPARE( data->hardPart->buildCmd->getCommand(), QString("gcc") );
	QCOMPARE( data->softViewFlags.contains("-sw"), true );
}

// See if hardware builder is taken account properly.
void tst_MakefileGenerator::hwBuilder()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(sw, "someFileSet", softView);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2View(hardView, "super_asm", "cSource", "-hw", false);
    addCmd2View(softView, "", "cSource", "-sw", false);

    setFileBuilder(file, "", "-u", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
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
void tst_MakefileGenerator::hwBuilderWithNoSoftView()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);
	addCmd2View(hardView, "gcc", "cSource", "", false);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(sw, "someFileSet", softView);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2View(hardView, "super_asm", "cSource", "-hw", false);

    setFileBuilder(file, "", "-u", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );

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

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    QSharedPointer<Component> hw = createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(hw, "someFileSet", hardView);

    addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2View(hardView, "super_asm", "cSource", "-hw", false);
    addCmd2View(softView, "", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, hw, "array.c");
    setFileBuilder(file, "", "-u", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
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

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    QSharedPointer<Component> hw = createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<FileSet> hfileSet = addFileSet(hw, "hardFileSet", hardView);

    addFileToSet(hfileSet, "harray.c");
    addFileSetBuilder(hfileSet, "", "cSource", "-hset", false);
    addCmd2View(hardView, "super_asm", "cSource", "-hw", false);

    QSharedPointer<File> hfile;
    getFile(hfile, hw, "harray.c");
    setFileBuilder(hfile, "", "-hu", false);

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString sfileSetName = "softFileSet";
    addFileSet(sw, sfileSetName, softView);

    QSharedPointer<FileSet> sfileSet = sw->getFileSet(sfileSetName);
    addFileToSet(sfileSet, "sarray.c");
    addFileSetBuilder(sfileSet, "", "cSource", "-sset", false);
    addCmd2View(softView, "", "cSource", "-sw", false);

    QSharedPointer<File> sfile;
    getFile(sfile, sw, "sarray.c");
    setFileBuilder(sfile, "", "-su", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
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
	QCOMPARE( data->softViewFlags.contains("-sw"), true );
}

// Instance specific header files must be included to the makefile.
void tst_MakefileGenerator::instanceHeaders()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView, swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);
    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");

    QString topFileSetName = NameGenerationPolicy::instanceFilesetName( "sysview", "software_0" );
	QSharedPointer<FileSet> topFileSet = addFileSet(topComponent,topFileSetName,QSharedPointer<SWView>());
    addFileToSet(topFileSet, "array.h", "cSource", true);
	
	QSharedPointer<SWInstance> referrringInstance = design->getSWInstances().first();
	referrringInstance->setFileSetRef( topFileSetName );

    QString topFileSetName2 = "esa-pekka";
    QSharedPointer<FileSet> topFileSet2 = addFileSet(topComponent,topFileSetName2,QSharedPointer<SWView>());
    addFileToSet(topFileSet2, "joku.h", "cSource", true);

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 2 );

	QCOMPARE( data->swObjects.first()->file->isIncludeFile(), true );
	QCOMPARE( data->swObjects.first()->compiler, QString("gcc") );
	QCOMPARE( data->swObjects.first()->fileName, QString("array.h") );
	QCOMPARE( data->swObjects.first()->flags.simplified(), QString("-sw -hw") );
}

// Generate makefile with multiple source and header files.
void tst_MakefileGenerator::multipleFiles()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addFileToSet(fileSet, "support.c");
    addFileToSet(fileSet, "additional.c");
    addFileToSet(fileSet, "hiterbehn.c");
    addFileToSet(fileSet, "array.h", "cSource", true);
    addFileToSet(fileSet, "support.h", "cSource", true);

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 6 );

	QCOMPARE( data->swObjects.at(0)->compiler, QString("gcc" ) );
	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.at(0)->flags.simplified(), QString("-sw -hw") );
	QCOMPARE( data->swObjects.at(1)->compiler, QString("continental" ) );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data->swObjects.at(1)->flags.simplified(), QString("-y -sw -hw") );
	QCOMPARE( data->swObjects.at(2)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(3)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data->swObjects.at(4)->fileName, QString("array.h") );
	QCOMPARE( data->swObjects.at(4)->file->isIncludeFile(), true );
	QCOMPARE( data->swObjects.at(5)->fileName, QString("support.h") );
	QCOMPARE( data->swObjects.at(5)->file->isIncludeFile(), true );
}

// Generate makefile when multiple file sets are featured.
void tst_MakefileGenerator::multipleFileSets()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QSharedPointer<FileSet> afileSet = addFileSet(sw, "alphaFileSet", softView);
    QSharedPointer<FileSet> bfileSet = addFileSet(sw, "betaFileSet", softView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 6 );

	QCOMPARE( data->swObjects.at(0)->compiler, QString("gcc" ) );
	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.at(0)->flags.simplified(), QString("-sw -hw") );
	QCOMPARE( data->swObjects.at(1)->compiler, QString("continental" ) );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data->swObjects.at(1)->flags.simplified(), QString("-y -sw -hw") );
	QCOMPARE( data->swObjects.at(2)->fileName, QString("array.h") );
	QCOMPARE( data->swObjects.at(2)->file->isIncludeFile(), true );
	QCOMPARE( data->swObjects.at(3)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(4)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data->swObjects.at(5)->fileName, QString("support.h") );
	QCOMPARE( data->swObjects.at(5)->file->isIncludeFile(), true );
}

// Generate makefile when multiple components are featured.
void tst_MakefileGenerator::multipleComponents()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> asoftView;
    QSharedPointer<SWView> bsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

    QSharedPointer<Component> asw = createSW("crapware", hwInstanceName, design, "default", desgconf, asoftView,"crapware_0");
    QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftView,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

	addCmd2View(hardView, "hopo", "cSource", "-hw", false);
	addCmd2View(asoftView, "", "cSource", "-amw", false);
    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 2 );
	QSharedPointer<MakeFileData> data1 = datas->first();
	QCOMPARE( data1->swObjects.size(), 3 );
	QCOMPARE( data1->softViewFlags.contains("-amw"), true );
	QCOMPARE( data1->name, QString("crapware_0") );

	QCOMPARE( data1->swObjects.at(0)->compiler, QString("hopo" ) );
	QCOMPARE( data1->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data1->swObjects.at(0)->flags.simplified(), QString("-amw -hw") );
	QCOMPARE( data1->swObjects.at(1)->compiler, QString("continental" ) );
	QCOMPARE( data1->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data1->swObjects.at(1)->flags.simplified(), QString("-y -amw -hw") );
	QCOMPARE( data1->swObjects.at(2)->fileName, QString("array.h") );
	QCOMPARE( data1->swObjects.at(2)->file->isIncludeFile(), true );

	QSharedPointer<MakeFileData> data2 = datas->last();
	QCOMPARE( data2->swObjects.size(), 3 );
	QCOMPARE( data2->softViewFlags.contains("-bmw"), true );
	QCOMPARE( data2->name, QString("stackware_0") );

	QCOMPARE( data2->swObjects.at(0)->fileName, QString("additional.c") );
	QCOMPARE( data2->swObjects.at(0)->compiler, QString("asm-meister" ) );
	QCOMPARE( data2->swObjects.at(0)->flags.simplified(), QString("-bmw -hw") );
	QCOMPARE( data2->swObjects.at(1)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data2->swObjects.at(1)->compiler, QString("asm-meister" ) );
	QCOMPARE( data2->swObjects.at(1)->flags.simplified(), QString("-bmw -hw") );
	QCOMPARE( data2->swObjects.at(2)->fileName, QString("support.h") );
	QCOMPARE( data2->swObjects.at(2)->file->isIncludeFile(), true );
}

// Must work if there is multiple hardware where instances are mapped to.
void tst_MakefileGenerator::multipleHardWare()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> ahardView;
    QSharedPointer<SWView> bhardView;
    QSharedPointer<SWView> asoftView;
    QSharedPointer<SWView> bsoftView;

    createHW("Lenit_0", design, "firmware", desgconf, ahardView, "Lenit");
    createHW("MAR_0", design, "firmware", desgconf, bhardView, "MAR");

    QSharedPointer<Component> asw = createSW("crapware", "Lenit_0", design, "default", desgconf, asoftView,"crapware_0");
    QSharedPointer<Component> bsw = createSW("stackware", "MAR_0", design, "default", desgconf, bsoftView,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(afileSet, "array.h", "cSource", true);

    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(ahardView, "hopo", "cSource", "-ahw", false);
    addCmd2View(bhardView, "juu -f", "cSource", "-bhw", false);

	addCmd2View(asoftView, "", "cSource", "", false);
    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 2 );
	QSharedPointer<MakeFileData> data1 = datas->first();
	QCOMPARE( data1->swObjects.size(), 3 );
	QCOMPARE( data1->name, QString("crapware_0") );
	QCOMPARE( data1->hardPart->buildCmd->getCommand(), QString("hopo") );
	QCOMPARE( data1->hardPart->buildCmd->getFlags(), QString("-ahw") );

	QCOMPARE( data1->swObjects.at(0)->compiler, QString("hopo" ) );
	QCOMPARE( data1->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data1->swObjects.at(0)->flags.simplified(), QString("-ahw") );
	QCOMPARE( data1->swObjects.at(1)->compiler, QString("continental" ) );
	QCOMPARE( data1->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data1->swObjects.at(1)->flags.simplified(), QString("-y -ahw") );
	QCOMPARE( data1->swObjects.at(2)->fileName, QString("array.h") );
	QCOMPARE( data1->swObjects.at(2)->file->isIncludeFile(), true );

	QSharedPointer<MakeFileData> data2 = datas->last();
	QCOMPARE( data2->swObjects.size(), 3 );
	QCOMPARE( data2->softViewFlags.contains("-bmw"), true );
	QCOMPARE( data2->name, QString("stackware_0") );
	QCOMPARE( data2->hardPart->buildCmd->getCommand(), QString("juu -f") );
	QCOMPARE( data2->hardPart->buildCmd->getFlags(), QString("-bhw") );
	QCOMPARE( data2->softViewFlags.contains("-bmw"), true );

	QCOMPARE( data2->swObjects.at(0)->fileName, QString("additional.c") );
	QCOMPARE( data2->swObjects.at(0)->compiler, QString("asm-meister" ) );
	QCOMPARE( data2->swObjects.at(0)->flags.simplified(), QString("-bmw -bhw") );
	QCOMPARE( data2->swObjects.at(1)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data2->swObjects.at(1)->compiler, QString("asm-meister" ) );
	QCOMPARE( data2->swObjects.at(1)->flags.simplified(), QString("-bmw -bhw") );
	QCOMPARE( data2->swObjects.at(2)->fileName, QString("support.h") );
	QCOMPARE( data2->swObjects.at(2)->file->isIncludeFile(), true );
}

// Multiple hardware, along with their references to their own files.
void tst_MakefileGenerator::multipleHardWareMedRefs()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> ahardView;
    QSharedPointer<SWView> bhardView;
    QSharedPointer<SWView> asoftView;
    QSharedPointer<SWView> bsoftView;

    QSharedPointer<Component> hw = createHW("Lenit_0", design, "firmware", desgconf, ahardView, "Lenit");
    createHW("MAR_0", design, "firmware", desgconf, bhardView, "MAR");

    QSharedPointer<Component> asw = createSW("crapware", "Lenit_0", design, "default", desgconf, asoftView,"crapware_0");
    QSharedPointer<Component> bsw = createSW("stackware", "MAR_0", design, "default", desgconf, bsoftView,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(afileSet, "array.h", "cSource", true);

    QString hfileSetName = "hardFileSet";
    QSharedPointer<FileSet> hfileSet = addFileSet(hw, hfileSetName, ahardView);

    addFileToSet(hfileSet, "harray.c");
    addFileSetBuilder(hfileSet, "", "cSource", "-hset", false);

    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(ahardView, "hopo", "cSource", "-ahw", false);
    addCmd2View(bhardView, "juu", "cSource", "-bhw", false);

	addCmd2View(asoftView, "", "cSource", "", false);
    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 2 );
	QSharedPointer<MakeFileData> data1 = datas->first();
	QCOMPARE( data1->swObjects.size(), 4 );
	QCOMPARE( data1->name, QString("crapware_0") );

	QCOMPARE( data1->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data1->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data1->swObjects.at(2)->fileName, QString("array.h") );
	QCOMPARE( data1->swObjects.at(2)->file->isIncludeFile(), true );
	QCOMPARE( data1->swObjects.at(3)->fileName, QString("harray.c") );

	QSharedPointer<MakeFileData> data2 = datas->last();
	QCOMPARE( data2->swObjects.size(), 3 );
	QCOMPARE( data2->name, QString("stackware_0") );

	QCOMPARE( data2->swObjects.at(0)->fileName, QString("additional.c") );
	QCOMPARE( data2->swObjects.at(1)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data2->swObjects.at(2)->fileName, QString("support.h") );
}

// Must work with multiple instances of the same component.
void tst_MakefileGenerator::multipleInstances()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<SWView> hardView;
	QSharedPointer<SWView> softView;

	QString hwInstanceName = "hardware_0";
	QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QSharedPointer<Component> sw = createSW("stackware", hwInstanceName, design, "default", desgconf, softView,"stackware_0");

	QSharedPointer<ConfigurableVLNVReference> swvlvnv( new ConfigurableVLNVReference(sw->getVlnv()) );
	QSharedPointer<SWInstance> softInstance(new SWInstance);
	softInstance->setInstanceName("stackware_1");
	softInstance->setComponentRef(swvlvnv);
	softInstance->setMapping(hwInstanceName);

	desgconf->addViewConfiguration("stackware_1","default");

	QList<QSharedPointer<SWInstance> > swInstances = design->getSWInstances();
	swInstances.append(softInstance);
	design->setSWInstances(swInstances);
	library_.writeModelToFile("polku/stackware",sw);

	QSharedPointer<FileSet> fileSet = addFileSet(sw, "FileSet", softView);

	addFileToSet(fileSet, "additional.c");
	addFileToSet(fileSet, "hiterbehn.c");
	addFileToSet(fileSet, "array.h", "cSource", true);
	addFileToSet(fileSet, "support.h", "cSource", true);

	addCmd2View(hardView, "hopo", "cSource", "-hw", false);
	addCmd2View(softView, "asm-meister", "cSource", "-bmw", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 2 );
	QSharedPointer<MakeFileData> data1 = datas->first();
	QCOMPARE( data1->swObjects.size(), 4 );
	QCOMPARE( data1->name, QString("stackware_0") );

	QSharedPointer<MakeFileData> data2 = datas->last();
	QCOMPARE( data2->swObjects.size(), 4 );
	QCOMPARE( data2->name, QString("stackware_1") );
}

// No hardware means no stuff.
void tst_MakefileGenerator::noHardWare()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> softView;

    QSharedPointer<Component> sw = createSW("software", "", design, "default", desgconf, softView,"software_0");

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

    MakefileGenerator generator( makeParser, &utilityMock_, stackParser.getGeneralFileSet() );
	generator.generate(outputDir_,outputDir_,"tsydemi");
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 0 );
}

// Yield no makefile, if no files are present.
void tst_MakefileGenerator::noFilesComponent()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<SWView> hardView;
	QSharedPointer<SWView> asoftView;
	QSharedPointer<SWView> bsoftView;

	QString hwInstanceName = "hardware_0";
	QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QSharedPointer<Component> asw = createSW("whatware", hwInstanceName, design, "default", desgconf, asoftView,"whatware_0");
	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftView,"stackware_0");

	QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
	QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

	addFileToSet(bfileSet, "additional.c");
	addFileToSet(bfileSet, "hiterbehn.c");
	addFileToSet(bfileSet, "support.h", "cSource", true);

	addCmd2View(hardView, "hopo", "cSource", "-hw", false);
	addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 3 );

	QCOMPARE( data->swObjects.at(0)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data->swObjects.at(2)->fileName, QString("support.h") );
}

// File with unsupported file does not appear in our lists.
void tst_MakefileGenerator::noFileType()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<SWView> hardView;
	QSharedPointer<SWView> softView;

	QString hwInstanceName = "hardware_0";
	QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QString swInstanceName = "software_0";
	QString softViewName = "default";

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

	QString fileSetName = "someFileSet";
	addFileSet(sw, fileSetName, softView);

	QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
	addFileToSet(fileSet, "hiterbehn.c", "topSecret");
	addFileToSet(fileSet, "array.c");
	addFileToSet(fileSet, "support.h", "cSource", true);

	addCmd2View(hardView, "gcc", "cSource", "-hw", false);
	addCmd2View(softView, "gcc", "cSource", "-sw", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 2 );

	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("support.h") );
}

// Must include API dependencies to the makefile.
void tst_MakefileGenerator::apiUsage()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> asoftView;
	QSharedPointer<SWView> bsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QSharedPointer<Component> asw = createSW("crapware", hwInstanceName, design, "default", desgconf, asoftView,"crapware_0");
	addAPI(asw, "apina", DEPENDENCY_REQUESTER);

	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftView,"stackware_0");
	addAPI(bsw, "banaani", DEPENDENCY_PROVIDER);

	addAPIConnection(design,"crapware_0","apina","stackware_0","banaani");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
	QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

	addCmd2View(hardView, "gcc", "cSource", "", false);
	addCmd2View(asoftView, "", "cSource", "", false);
	addCmd2View(bsoftView, "", "cSource", "", false);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    SWStackParser stackParser( &library_ );
	stackParser.parse(topComponent, desgconf, design, "tsydemi");
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE(datas->size(), 1);
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE(data->swObjects.size(), 6);

	QCOMPARE(data->swObjects.at(0)->fileName, QString("array.c"));
	QCOMPARE(data->swObjects.at(1)->fileName, QString("support.c"));
	QCOMPARE(data->swObjects.at(2)->fileName, QString("array.h"));
	QCOMPARE(data->swObjects.at(2)->file->isIncludeFile(), true);
	QCOMPARE(data->swObjects.at(3)->fileName, QString("additional.c"));
	QCOMPARE(data->swObjects.at(4)->fileName, QString("hiterbehn.c"));
	QCOMPARE(data->swObjects.at(5)->fileName, QString("support.h"));
	QCOMPARE(data->swObjects.at(5)->file->isIncludeFile(), true);
}

// A more complex situation with API dependency.
void tst_MakefileGenerator::threeLevelStack()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> asoftView;
    QSharedPointer<SWView> bsoftView;
    QSharedPointer<SWView> gsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QSharedPointer<Component> asw = createSW("crapware", hwInstanceName, design, "default", desgconf, asoftView,"crapware_0");
	addAPI(asw, "apina", DEPENDENCY_REQUESTER);

	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftView,"stackware_0");
	addAPI(bsw, "banaani", DEPENDENCY_PROVIDER);

    QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", desgconf, gsoftView,"driver_0");

	addAPI(bsw, "stackDriver", DEPENDENCY_REQUESTER);
	addAPI(gsw, "driverStack", DEPENDENCY_PROVIDER);

	addAPIConnection(design, "crapware_0","apina","stackware_0","banaani");
	addAPIConnection(design, "stackware_0","stackDriver","driver_0","driverStack");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);
	QSharedPointer<FileSet> cfileSet = addFileSet(gsw, "gamaFileSet", gsoftView);

	addCmd2View(hardView, "gcc", "cSource", "", false);
	addCmd2View(asoftView, "", "cSource", "", false);
	addCmd2View(bsoftView, "", "cSource", "", false);
	addCmd2View(gsoftView, "", "cSource", "", false);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );
	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 8 );

	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data->swObjects.at(2)->fileName, QString("array.h") );
	QCOMPARE( data->swObjects.at(2)->file->isIncludeFile(), true );
	QCOMPARE( data->swObjects.at(3)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(4)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data->swObjects.at(5)->fileName, QString("support.h") );
	QCOMPARE( data->swObjects.at(5)->file->isIncludeFile(), true );
	QCOMPARE( data->swObjects.at(6)->fileName, QString("ok.c") );
	QCOMPARE( data->swObjects.at(7)->fileName, QString("ok.h") );
	QCOMPARE( data->swObjects.at(7)->file->isIncludeFile(), true );
}

// If no top level in the stack, there can be no build!
void tst_MakefileGenerator::fullCircularapiUsage()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> asoftView;
    QSharedPointer<SWView> bsoftView;
    QSharedPointer<SWView> gsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QSharedPointer<Component> asw = createSW("topware", hwInstanceName, design, "default", desgconf, asoftView,"topware_0");
	addAPI(asw, "apina", DEPENDENCY_REQUESTER);

	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftView,"stackware_0");
	addAPI(bsw, "banaani", DEPENDENCY_PROVIDER);

	QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", desgconf, gsoftView,"driver_0");
	addAPI(bsw, "stackDriver", DEPENDENCY_REQUESTER);
	addAPI(gsw, "driverStack", DEPENDENCY_PROVIDER);
	addAPI(gsw, "bottomUp", DEPENDENCY_REQUESTER);
	addAPI(asw, "upBottom", DEPENDENCY_PROVIDER);

	addAPIConnection(design, "topware_0","apina","stackware_0","banaani");
	addAPIConnection(design, "stackware_0","stackDriver","driver_0","driverStack");
	addAPIConnection(design, "topware_0","upBottom","driver_0","bottomUp");

	addCmd2View(hardView, "gcc", "cSource", "", false);
	addCmd2View(asoftView, "", "cSource", "", false);
	addCmd2View(bsoftView, "", "cSource", "", false);
	addCmd2View(gsoftView, "", "cSource", "", false);

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);
    QSharedPointer<FileSet> cfileSet = addFileSet(gsw, "gamaFileSet", gsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
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

    QSharedPointer<SWView> hardView;
    QSharedPointer<SWView> asoftView;
    QSharedPointer<SWView> bsoftView;
    QSharedPointer<SWView> gsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

    QSharedPointer<Component> asw = createSW("crapware", hwInstanceName, design, "default", desgconf, asoftView,"crapware_0");
	addAPI(asw, "apina", DEPENDENCY_REQUESTER);

	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftView,"stackware_0");
	addAPI(bsw, "banaani", DEPENDENCY_PROVIDER);

    QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", desgconf, gsoftView,"driver_0");

    QSharedPointer<ApiInterface> stackDriver = QSharedPointer<ApiInterface>(new ApiInterface );
	addAPI(bsw, "stackDriver", DEPENDENCY_REQUESTER);

	QSharedPointer<ApiInterface> driverStack = QSharedPointer<ApiInterface>(new ApiInterface );
	addAPI(gsw, "driverStack", DEPENDENCY_PROVIDER);

	QSharedPointer<ApiInterface> bottomUp = QSharedPointer<ApiInterface>(new ApiInterface );
	addAPI(gsw, "bottomUp", DEPENDENCY_REQUESTER);

	QSharedPointer<ApiInterface> upBottom = QSharedPointer<ApiInterface>(new ApiInterface );
	addAPI(bsw, "upBottom", DEPENDENCY_PROVIDER);;

	addAPIConnection(design, "crapware_0","apina","stackware_0","banaani");
	addAPIConnection(design, "stackware_0","stackDriver","driver_0","driverStack");
	addAPIConnection(design, "stackware_0","upBottom","driver_0","bottomUp");

	addCmd2View(hardView, "gcc", "cSource", "", false);
	addCmd2View(asoftView, "", "cSource", "", false);
	addCmd2View(bsoftView, "", "cSource", "", false);
	addCmd2View(gsoftView, "", "cSource", "", false);

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);
    QSharedPointer<FileSet> cfileSet = addFileSet(gsw, "gamaFileSet", gsoftView);

    addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(afileSet, "array.h", "cSource", true);

    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser(&library_, stackParser );
	makeParser.parse( topComponent );

	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	QCOMPARE( datas->size(), 1 );

	QSharedPointer<MakeFileData> data = datas->first();
	QCOMPARE( data->swObjects.size(), 8 );

	QCOMPARE( data->swObjects.at(0)->fileName, QString("array.c") );
	QCOMPARE( data->swObjects.at(1)->fileName, QString("support.c") );
	QCOMPARE( data->swObjects.at(2)->fileName, QString("array.h") );
	QCOMPARE( data->swObjects.at(2)->file->isIncludeFile(), true );
	QCOMPARE( data->swObjects.at(3)->fileName, QString("additional.c") );
	QCOMPARE( data->swObjects.at(4)->fileName, QString("hiterbehn.c") );
	QCOMPARE( data->swObjects.at(5)->fileName, QString("support.h") );
	QCOMPARE( data->swObjects.at(5)->file->isIncludeFile(), true );
	QCOMPARE( data->swObjects.at(6)->fileName, QString("ok.c") );
	QCOMPARE( data->swObjects.at(7)->fileName, QString("ok.h") );
	QCOMPARE( data->swObjects.at(7)->file->isIncludeFile(), true );
}

// Same file in separate executables should not cause conflict
void tst_MakefileGenerator::sameFileSeparateExe()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<SWView> hardView;
	QSharedPointer<SWView> asoftView;
	QSharedPointer<SWView> bsoftView;

	QString hwInstanceName = "hardware_0";
	QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QSharedPointer<Component> asw = createSW("crapware", hwInstanceName, design, "default", desgconf, asoftView,"crapware_0");
	QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", desgconf, bsoftView,"stackware_0");

	QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
	QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

	addCmd2View(hardView, "gcc", "cSource", "", false);
	addCmd2View(asoftView, "", "cSource", "", false);
	addCmd2View(bsoftView, "", "cSource", "", false);

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c");

	addCmd2View(hardView, "hopo", "cSource", "-hw", false);
	addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
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

	QSharedPointer<SWView> hardView;
	QSharedPointer<SWView> softView;

	QString hwInstanceName = "hardware_0";
	QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QString swInstanceName = "software_0";
	QString softViewName = "default";

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

	QSharedPointer<FileSet> afileSet = addFileSet(sw, "alphaFileSet", softView);
	QSharedPointer<FileSet> bfileSet = addFileSet(sw, "betaFileSet", softView);

	addCmd2View(hardView, "gcc", "cSource", "", false);
	addCmd2View(softView, "", "cSource", "", false);

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c");

	addCmd2View(softView, "gcc", "cSource", "-sw", false);

	addFileSetBuilder(afileSet, "javac -beef", "cSource", "-lrt -pthread", false);
	addFileSetBuilder(bfileSet, "", "cSource", "-pthread -lrt", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
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

	QSharedPointer<SWView> hardView;
	QSharedPointer<SWView> softView;

	QString hwInstanceName = "hardware_0";
	QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QString swInstanceName = "software_0";
	QString softViewName = "default";

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf,
		softView,swInstanceName);

	QSharedPointer<FileSet> afileSet = addFileSet(sw, "alphaFileSet", softView);
	QSharedPointer<FileSet> bfileSet = addFileSet(sw, "betaFileSet", softView);

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c");

	addCmd2View(hardView, "gcc", "cSource", "-hw", false);
	addCmd2View(softView, "gcc", "cSource", "-sw", false);

	addFileSetBuilder(afileSet, "javac -beef", "cSource", "-lrt -pthread", false);
	addFileSetBuilder(bfileSet, "javac -beef", "cSource", "-pthread", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
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

// No conflict, ff the files are completely same, having the same flags, compiler, and include status.
void tst_MakefileGenerator::sameFile()
{
	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desgconf;
	QSharedPointer<Component> topComponent = createDesign(design, desgconf);

	QSharedPointer<SWView> hardView;
	QSharedPointer<SWView> softView;

	QString hwInstanceName = "hardware_0";
	QString hardViewName = "firmware";
	createHW(hwInstanceName, design, hardViewName, desgconf, hardView);

	QString swInstanceName = "software_0";
	QString softViewName = "default";

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf,
		softView,swInstanceName);

	QSharedPointer<FileSet> afileSet = addFileSet(sw, "alphaFileSet", softView);
	QSharedPointer<FileSet> bfileSet = addFileSet(sw, "betaFileSet", softView);

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c", "cSource", true);
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c", "cSource", true);

	addCmd2View(hardView, "gcc", "cSource", "-hw", false);
	addCmd2View(softView, "gcc", "cSource", "-sw", false);

	addFileSetBuilder(afileSet, "javac -beef", "cSource", "-lrt -pthread", false);
	addFileSetBuilder(bfileSet, "javac -beef", "cSource", "-pthread -lrt", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, "tsydemi" );
	MakefileParser makeParser( &library_, stackParser );
	makeParser.parse( topComponent );

	QVector<QSet<QSharedPointer<MakeObjectData> > > conflicts = makeParser.getParsedData()->first()->conflicts;

	QCOMPARE( conflicts.size(), 0 );
}

// See if a really basic generation case works.
void tst_MakefileGenerator::basicGeneration()
{
	SWStackParser stackParser( &library_ );
	MakefileParser makeParser( &library_, stackParser );
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	auto makeData = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData);

	makeData->hardPart = QSharedPointer<StackPart>( new StackPart );
	makeData->hardPart->buildCmd = QSharedPointer<SWFileBuilder>( new SWFileBuilder );
	makeData->hardPart->buildCmd->setCommand("gcc");
	makeData->name = "software_0";
	makeData->hardPart->buildCmd->setFlags("-joku");
	makeData->softViewFlags.append("-jotain");
	makeData->targetPath = outputDir_ + "/sw_tsydemi/" + makeData->name;

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
	SWStackParser stackParser( &library_ );
	MakefileParser makeParser( &library_, stackParser );
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	auto makeData = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData);

	makeData->hardPart = QSharedPointer<StackPart>( new StackPart );
	makeData->hardPart->buildCmd = QSharedPointer<SWFileBuilder>( new SWFileBuilder() );
	makeData->hardPart->buildCmd->setCommand("gcc");
	makeData->name = "software_0";
	makeData->targetPath = outputDir_ + "/sw_tsydemi/" + makeData->name;

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
	SWStackParser stackParser( &library_ );
	MakefileParser makeParser( &library_, stackParser );
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	auto makeData1 = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData1);

	makeData1->hardPart = QSharedPointer<StackPart>( new StackPart );
	makeData1->hardPart->buildCmd = QSharedPointer<SWFileBuilder>( new SWFileBuilder() );
	makeData1->hardPart->buildCmd->setCommand("gcc");
	makeData1->name = "software_0";
	makeData1->hardPart->buildCmd->setFlags("-joku");
	makeData1->softViewFlags.append("-jotain");
	makeData1->targetPath = outputDir_ + "/sw_tsydemi/" + makeData1->name;

	QSharedPointer<MakeObjectData> mod1( new MakeObjectData );
	makeData1->swObjects.append( mod1 );
	mod1->fileName = "array.c";
	mod1->compiler = "gcc";
	mod1->flags = "-sw -hw";

	auto makeData2 = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData2);

	makeData2->hardPart = QSharedPointer<StackPart>( new StackPart );
	makeData2->hardPart->buildCmd = QSharedPointer<SWFileBuilder>( new SWFileBuilder() );
	makeData2->hardPart->buildCmd->setCommand("j33");
	makeData2->name = "crapware_0";
	makeData2->hardPart->buildCmd->setFlags("-yks");
	makeData2->softViewFlags.append("-kaks");
	makeData2->targetPath = outputDir_ + "/sw_tsydemi/" + makeData2->name;

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

	verifyOutputContains("crapware_0", "_OBJ= support.c.o");
	verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -yks -kaks");
	verifyOutputContains("crapware_0", "ENAME= crapware_0");
	verifyOutputContains("crapware_0", "EBUILDER= j33");
	verifyOutputContains("crapware_0", "super-asm " + DEFAULT_OBJECT_FLAGS +
		" $(ODIR)/support.c.o /support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -a -b");
}

// File with no compiler must not be in makefile!
void tst_MakefileGenerator::noCompiler()
{
	SWStackParser stackParser( &library_ );
	MakefileParser makeParser( &library_, stackParser );
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > datas = makeParser.getParsedData();
	auto makeData = QSharedPointer<MakeFileData>( new MakeFileData );
	datas->append(makeData);

	makeData->hardPart = QSharedPointer<StackPart>( new StackPart );
	makeData->hardPart->buildCmd = QSharedPointer<SWFileBuilder>( new SWFileBuilder() );
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

	QSharedPointer<SWView> hardView;
	QSharedPointer<SWView> softView;

	createHW("hardware_0", design, "firmware", desgconf, hardView);
	addCmd2View(hardView, "gcc", "cSource", "-hw", false);

	QSharedPointer<Component> sw = createSW("software", "hardware_0", design, "default", desgconf, softView, "software_0");

	QString fileSetName = "someFileSet";
	QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

	addFileToSet(fileSet, "array.c");
	addCmd2View(softView, "gcc", "cSource", "-sw", false);

	SWStackParser stackParser( &library_ );
	stackParser.parse( topComponent, desgconf, design, outputDir_ );

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
    library_.writeModelToFile("polku/master-plan",topComponent);

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

QSharedPointer<Component> tst_MakefileGenerator::createSW(QString swName, QString hwInstanceName, QSharedPointer<Design> design,
	QString softViewName,  QSharedPointer<DesignConfiguration> desgconf, QSharedPointer<SWView>& softView, QString swInstanceName)
{
    VLNV swvlvnv("","vendor","lib",swName,"1.0");
	QSharedPointer<ConfigurableVLNVReference> swvlvnv2( new ConfigurableVLNVReference( swvlvnv ) );
    QSharedPointer<Component> sw = QSharedPointer<Component>(new Component(swvlvnv));
    QSharedPointer<SWInstance> softInstance( new SWInstance );
    softInstance->setInstanceName(swInstanceName);
    softInstance->setComponentRef(swvlvnv2);
    softInstance->setMapping(hwInstanceName);

    QList<QSharedPointer<SWInstance> > swInstances = design->getSWInstances();
    swInstances.append(softInstance);
    design->setSWInstances(swInstances);
    library_.addComponent(sw);
    library_.writeModelToFile("polku/" + swName,sw);

    softView = QSharedPointer<SWView>( new SWView() );
	softView->setName(softViewName);
	QList<QSharedPointer<SWView> > views;
	views.append(softView);
	sw->setSWViews(views);

	desgconf->addViewConfiguration(swInstanceName,softViewName);

    return sw;
}

QSharedPointer<Component> tst_MakefileGenerator::createHW(QString const& hwInstanceName,
    QSharedPointer<Design> design, QString const& hardViewName, QSharedPointer<DesignConfiguration> designConfig, 
    QSharedPointer<SWView>& softwareView, QString const& hwName)
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
    library_.writeModelToFile("polku/" + hwName, hwComponent);

    softwareView = QSharedPointer<SWView>( new SWView );
    softwareView->setName(hardViewName);
	QList<QSharedPointer<SWView> > views;
	views.append(softwareView);
    hwComponent->setSWViews(views);

	designConfig->addViewConfiguration(hwInstanceName,hardViewName);

    return hwComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_MakefileGenerator::verifyOutputContains()
//-----------------------------------------------------------------------------
void tst_MakefileGenerator::verifyOutputContains(QString instanceName, QString const& expectedOutput)
{
    QFile outputFile(outputDir_ + "/sw_tsydemi/" + instanceName + "/" + MAKEFILE_NAME);

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

void tst_MakefileGenerator::addCmd2View(QSharedPointer<SWView> view, QString command, QString fileType,
    QString flags, bool replace)
{
    QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > coms = view->getSWBuildCommands();
    QSharedPointer<SWFileBuilder> cmd = QSharedPointer<SWFileBuilder>(new SWFileBuilder());
    cmd->setCommand(command);
    cmd->setFileType(fileType);
    cmd->setFlags(flags);
    if (replace)
    {
        cmd->setReplaceDefaultFlags("true");
    }
    else
    {
        cmd->setReplaceDefaultFlags("false");
    }
	coms->append(cmd);
}

QSharedPointer<FileSet> tst_MakefileGenerator::addFileSet(QSharedPointer<Component> component, QString fileSetName,
    QSharedPointer<SWView> view)
{
    QSharedPointer<FileSet> fileSet(new FileSet(fileSetName, "sourceFiles"));

	if ( view )
	{
		QStringList fileSets = view->getFileSetRefs();
		fileSets.append(fileSetName);
		view->setFileSetRefs(fileSets);
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
