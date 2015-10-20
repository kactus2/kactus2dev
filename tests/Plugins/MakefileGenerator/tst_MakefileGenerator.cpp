//-----------------------------------------------------------------------------
// File: tst_MakefileGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.09.2014
//
// Description:
// Unit test for classes MakefileParser and MakefileGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/kactusExtensions/SWView.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>
#include <IPXACTmodels/Component/file.h>
#include <IPXACTmodels/Component/fileset.h>
#include <IPXACTmodels/kactusExtensions/ApiInterconnection.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/MakefileGenerator/MakefileParser.h>
#include <Plugins/MakefileGenerator/MakefileGenerator.h>
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>

#include <QRegExp>

class tst_MakefileGenerator : public QObject
{
    Q_OBJECT

public:
    tst_MakefileGenerator();

private slots:
    void baseCase();

    void fileBuildOverride();
    void fileSetBuildOverride();

    void fileFlagReplace();

    void fileSetFlagReplace();
    void includeFile();
    void swSWViewFlagReplace();
    void hwBuilder();
    void hwBuilderWithNoSoftView();
    void hwRef();
    void hwandswRef();
    void instanceHeaders();

    void multipleFiles();
    void multipleFileSets();
    void multipleComponents();
    void noFilesComponent();
    void multipleHardWare();
    void multipleHardWareMedRefs();

	void noHardWare();
	void noFileType();
    void multipleInstances();

    void apiUsage();
    void threeLevelStack();
    void fullCircularapiUsage();
	void circularapiUsage();

	void sameFile();
	void sameFileSeparateExe();
	void sameFileDiffCompiler();
	void sameFileDiffFlags();

private:
    void getFile(QSharedPointer<File>& file, QSharedPointer<Component> component, QString fileName);

    QSharedPointer<Component> createSW(QString swName, QString hwInstanceName, QSharedPointer<Design> design, QString softViewName,
		 QSharedPointer<DesignConfiguration> desgconf, QSharedPointer<SWView>& softView, QString swInstanceName);

    QSharedPointer<FileSet> addFileSet(QSharedPointer<Component> component, QString fileSetName, QSharedPointer<SWView>& view);

    QSharedPointer<File> addFileToSet(QSharedPointer<FileSet> fileSet, QString fileName, QString fileType="cSource", bool isInclude=false);

    void createSWSWView(QSharedPointer<SWView> softView, QString softViewName, QSharedPointer<Component> sw);

    QSharedPointer<Component> createHW(QString hwInstanceName, QSharedPointer<Design> design, QString hardViewName,
		 QSharedPointer<DesignConfiguration> desgconf, QSharedPointer<SWView>& hardView, QString hwName="hardware");

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

tst_MakefileGenerator::tst_MakefileGenerator(): library_( this ), utilityMock_(&library_, 0, this)
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

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, outputDir_ );
    MakefileGenerator generator( parser, &utilityMock_ );
    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "_OBJ= array.c.o");
    verifyOutputContains("software_0", "OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))");
    verifyOutputContains("software_0", "ODIR= obj");
    verifyOutputContains("software_0", "ENAME= software_0");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS)");
    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "$(ENAME): $(OBJ)\n\t$(EBUILDER) -o $(ENAME) $(OBJ) $(EFLAGS)");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
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

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc -o", "cSource", "-sw", false);

    setFileBuilder(file, "python", "-l", false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );
    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw");
    verifyOutputContains("software_0", "python -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -l -sw");
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

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc -o", "cSource", "-sw", false);
    addFileSetBuilder(fileSet, "javac -beef", "cSource", "-lrt", false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw");
    verifyOutputContains("software_0", "javac -beef -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -lrt -sw");
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

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    setFileBuilder(file, "", "-u", true);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -u");
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

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);
    addFileSetBuilder(fileSet, "javac -beef", "cSource", "-lrt", true);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw");
    verifyOutputContains("software_0", "javac -beef -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -lrt");
}

// Include files shoudl not yield object files.
void tst_MakefileGenerator::includeFile()
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

    addFileToSet(fileSet, "array.c");
    addFileToSet(fileSet, "array.h", "cSource", true);

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "_OBJ= array.c.o\n");
    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
}

// Softwaree view of the software component should be able to replace those of hardware component.
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

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(sw, "someFileSet", softView);

    addFileToSet(fileSet, "array.c");

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", true);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw\n");
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

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "EBUILDER= super_asm");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -sw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -u -lrt -sw -hw");
}

// See if hardware builder works if there are no software view for the software component.
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

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QSharedPointer<FileSet> fileSet = addFileSet(sw, "someFileSet", softView);

    QSharedPointer<File> file = addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2View(hardView, "super_asm", "cSource", "-hw", false);

    setFileBuilder(file, "", "-u", false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "EBUILDER= super_asm");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -u -lrt -hw");
}

// See if hard ware's own file set references function.
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

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "EBUILDER= super_asm");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -u -lrt -hw");
}

// See if hard ware's own file set references function, while there are those of software component.
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

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "_OBJ= harray.c.o sarray.c.o");
    verifyOutputContains("software_0", "EBUILDER= super_asm");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -sw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/sarray.c.o ../../sarray.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -su -sset -sw -hw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/harray.c.o ../../harray.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hu -hset -hw");
}

// Instance specific header files must be included to the make file.
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

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);
    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");

    QString topFileSetName = "software_0_headers";
	QSharedPointer<FileSet> topFileSet = addFileSet(topComponent,topFileSetName,QSharedPointer<SWView>());
    addFileToSet(topFileSet, "array.h", "cSource", true);

    QString topFileSetName2 = "esa-pekka";
    QSharedPointer<FileSet> topFileSet2 = addFileSet(topComponent,topFileSetName2,QSharedPointer<SWView>());
    addFileToSet(topFileSet2, "joku.h", "cSource", true);

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");
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

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o");
    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
    verifyOutputContains("software_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -y -sw -hw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
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

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o");
    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
    verifyOutputContains("software_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -y -sw -hw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
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
    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -y -hw");

    verifyOutputContains("stackware_0", "EBUILDER= hopo");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -bmw");
    verifyOutputContains("stackware_0", "_OBJ= additional.c.o hiterbehn.c.o");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -hw");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -hw");
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

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("stackware_0", "EBUILDER= hopo");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -bmw");
    verifyOutputContains("stackware_0", "_OBJ= additional.c.o hiterbehn.c.o");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -hw");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -hw");

    QFile outputFile(outputDir_ + "/sw_tsydemi/whatware_0/Makefile");

    QVERIFY(!outputFile.open(QIODevice::ReadOnly));
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

    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -ahw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -ahw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -y -ahw");

    verifyOutputContains("stackware_0", "EBUILDER= juu -f");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bhw -bmw");
    verifyOutputContains("stackware_0", "_OBJ= additional.c.o hiterbehn.c.o");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -bhw");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -bhw");
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

    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -ahw");
    verifyOutputContains("crapware_0", "_OBJ= harray.c.o array.c.o support.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -ahw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -y -ahw");

    verifyOutputContains("stackware_0", "EBUILDER= juu");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bhw -bmw");
    verifyOutputContains("stackware_0", "_OBJ= additional.c.o hiterbehn.c.o");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -bhw");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -bhw");
}

// How it works without hardware. NOTICE: presently shall have no compiler!
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

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("software_0", "_OBJ= array.c.o");
    verifyOutputContains("software_0", "OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))");
    verifyOutputContains("software_0", "EBUILDER= ");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw");
}

// File with unsupported file type must not be in makefile!
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
	addFileToSet(fileSet, "array.c");
	addFileToSet(fileSet, "support.c");
	addFileToSet(fileSet, "additional.c");
	addFileToSet(fileSet, "hiterbehn.c", "topSecret");
	addFileToSet(fileSet, "array.h", "cSource", true);
	addFileToSet(fileSet, "support.h", "cSource", true);

	addCmd2View(hardView, "gcc", "cSource", "-hw", false);
	addCmd2View(softView, "gcc", "cSource", "-sw", false);

	QSharedPointer<File> file;
	getFile(file, sw, "support.c");
	setFileBuilder(file, "continental", "-y", false);

	MakefileParser parser;
	parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
	MakefileGenerator generator( parser, &utilityMock_ );

	generator.generate(outputDir_,outputDir_,"tsydemi");

	verifyOutputContains("software_0", "_OBJ= array.c.o support.c.o additional.c.o");
	verifyOutputContains("software_0", "EBUILDER= gcc");
	verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -sw");
	verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
	verifyOutputContains("software_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -y -sw -hw");
	verifyOutputContains("software_0", "gcc -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -sw -hw");
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

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("stackware_0", "EBUILDER= hopo");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -bmw");

    verifyOutputContains("stackware_1", "EBUILDER= hopo");
    verifyOutputContains("stackware_1", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw -bmw");
}

// Must include API dependencies to the make file.
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

    QList<QSharedPointer<ApiInterconnection> > deps = design->getApiConnections();
	addAPIConnection(design,"crapware_0","apina","stackware_0","banaani");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(hardView, "hopo -s", "cSource", "-hw", false);
    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );

	const QList<MakefileParser::MakeFileData> datas = parser.getParsedData();
	QCOMPARE( datas.size(), 1 );
	MakefileParser::MakeFileData data = datas.first();
	QCOMPARE( data.swObjects.size(), 6 );


    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("crapware_0", "EBUILDER= hopo -s");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o");
    verifyOutputContains("crapware_0", "hopo -s -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -y -hw");

    verifyOutputContains("crapware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -hw");
    verifyOutputContains("crapware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -bmw -hw");
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

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    addCmd2View(hardView, "hopo", "cSource", "-hw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);
    addFileSetBuilder(bfileSet,"jaska","cSource","-g",false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o ok.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -y -hw");

    verifyOutputContains("crapware_0", "jaska -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -g -hw");
    verifyOutputContains("crapware_0", "jaska -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -g -hw");

    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/ok.c.o ../../ok.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
}

// If no top level in the stack, there can be no executable!
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
	addAPI(asw, "banaani", DEPENDENCY_PROVIDER);

	QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", desgconf, gsoftView,"driver_0");
	addAPI(bsw, "stackDriver", DEPENDENCY_REQUESTER);
	addAPI(gsw, "driverStack", DEPENDENCY_PROVIDER);
	addAPI(gsw, "bottomUp", DEPENDENCY_REQUESTER);
	addAPI(asw, "upBottom", DEPENDENCY_PROVIDER);

	addAPIConnection(design, "topware_0","apina","stackware_0","banaani");
	addAPIConnection(design, "stackware_0","stackDriver","driver_0","driverStack");
	addAPIConnection(design, "topware_0","upBottom","driver_0","bottomUp");

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

    addCmd2View(hardView, "hopo", "cSource", "-hw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);
    addFileSetBuilder(bfileSet,"jaska","cSource","-g",false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    QFile outputFile(outputDir_ + "/sw_tsydemi/topware_0/Makefile");

    QVERIFY(!outputFile.open(QIODevice::ReadOnly));
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

    addCmd2View(hardView, "hopo", "cSource", "-hw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);
    addFileSetBuilder(bfileSet,"jaska","cSource","-g",false);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_,"tsydemi");

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o ok.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -y -hw");

    verifyOutputContains("crapware_0", "jaska -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -g -hw");
    verifyOutputContains("crapware_0", "jaska -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -g -hw");

    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/ok.c.o ../../ok.c $(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) -hw");
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

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c");

	addCmd2View(hardView, "hopo", "cSource", "-hw", false);
	addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

	MakefileParser parser;
	parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );

	QCOMPARE( parser.findConflicts().size(), 0 );
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

	addFileToSet(afileSet, "array.c");
	addFileToSet(afileSet, "support.c");
	addFileToSet(bfileSet, "array.c");
	addFileToSet(bfileSet, "support.c");

	addCmd2View(softView, "gcc", "cSource", "-sw", false);

	addFileSetBuilder(afileSet, "javac -beef", "cSource", "-lrt -pthread", false);
	addFileSetBuilder(bfileSet, "", "cSource", "-pthread -lrt", false);

	MakefileParser parser;
	parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );

	QVector<QSet<QSharedPointer<MakefileParser::MakeObjectData> > > conflicts = parser.findConflicts();

	QCOMPARE( conflicts.size(), 2 );

	QCOMPARE( conflicts.first().size(), 2 );
	QCOMPARE( conflicts.last().size(), 2 );
	QSharedPointer<MakefileParser::MakeObjectData> eka = *(conflicts.first().begin());
	QSharedPointer<MakefileParser::MakeObjectData> toka = *(conflicts.first().end() - 1);
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

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

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

	MakefileParser parser;
	parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );

	QVector<QSet<QSharedPointer<MakefileParser::MakeObjectData> > > conflicts = parser.findConflicts();

	QCOMPARE( conflicts.size(), 2 );

	QCOMPARE( conflicts.first().size(), 2 );
	QCOMPARE( conflicts.last().size(), 2 );
	QSharedPointer<MakefileParser::MakeObjectData> eka = *(conflicts.first().begin());
	QSharedPointer<MakefileParser::MakeObjectData> toka = *(conflicts.first().end() - 1);
	QCOMPARE( eka->path, toka->path );
	QCOMPARE( eka->fileName, toka->fileName );

	QCOMPARE( eka->fileName, QString("array.c") );

	eka = *(conflicts.last().begin());
	toka = *(conflicts.last().end() - 1);
	QCOMPARE( eka->path, toka->path );
	QCOMPARE( eka->fileName, toka->fileName );

	QCOMPARE( eka->fileName, QString("support.c") );
}

// If the files are completely same, that is, have the same flags, compiler, and include status, no conflict.
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

	QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, desgconf, softView,swInstanceName);

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

	MakefileParser parser;
	parser.parse( &library_, topComponent, desgconf, design, "tsydemi" );

	QVector<QSet<QSharedPointer<MakefileParser::MakeObjectData> > > conflicts = parser.findConflicts();

	QCOMPARE( conflicts.size(), 0 );
}

QSharedPointer<Component> tst_MakefileGenerator::createDesign(QSharedPointer<Design> &design,
    QSharedPointer<DesignConfiguration> &desgconf)
{
    VLNV dvlnv("","vendor","lib","design","1.0");
    design = QSharedPointer<Design>(new Design(dvlnv));
    VLNV dgvlnv("","vendor","lib","design-conf","1.0");
    desgconf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(dgvlnv));
    desgconf->setDesignRef(dvlnv);

    VLNV topvlvnv("","vendor","lib","master-plan","1.0");
    QSharedPointer<Component> top = QSharedPointer<Component>(new Component(topvlvnv));
    top->setImplementation(KactusAttribute::SYSTEM);
    library_.addComponent(top);
    library_.writeModelToFile("polku/master-plan",top);

    outputDir_ = QFileInfo(library_.getPath(topvlvnv)).absolutePath();

    QSharedPointer<SystemView> sw( new SystemView("sysview") );
    sw->setHierarchyRef(dgvlnv);
	QList<QSharedPointer<SystemView> > sviews;
	sviews.append(sw);
	top->setSystemViews(sviews);

    return top;
}

QSharedPointer<File> tst_MakefileGenerator::addFileToSet(QSharedPointer<FileSet> fileSet, QString fileName, QString fileType/*="cSource"*/, bool isInclude/*=false*/)
{
    QSettings settings;

    QSharedPointer<File> file;

    file = fileSet->addFile(fileName, settings);
    file->addFileType(fileType);
    file->setIncludeFile( isInclude );

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

QSharedPointer<Component> tst_MakefileGenerator::createHW(QString hwInstanceName, QSharedPointer<Design> design,
	QString hardViewName, QSharedPointer<DesignConfiguration> desgconf, QSharedPointer<SWView>& hardView,
	QString hwName/*="hardware"*/)
{
	VLNV hwvlvnv("","vendor","lib",hwName,"1.0");
	QSharedPointer<ConfigurableVLNVReference> hwvlvnv2( new ConfigurableVLNVReference( hwvlvnv ) );
    QSharedPointer<Component> hw = QSharedPointer<Component>(new Component(hwvlvnv));
    QSharedPointer<ComponentInstance> hwInstance(new ComponentInstance(
		hwInstanceName,"","esim",hwvlvnv2,QPointF(),hwInstanceName ) );

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances = design->getComponentInstances();
    instances->append(hwInstance);
    library_.addComponent(hw);
    library_.writeModelToFile("polku/" + hwName,hw);

    hardView = QSharedPointer<SWView>( new SWView );
    hardView->setName(hardViewName);
	QList<QSharedPointer<SWView> > views;
	views.append(hardView);
    hw->setSWViews(views);

	desgconf->addViewConfiguration(hwInstanceName,hardViewName);

    return hw;
}

//-----------------------------------------------------------------------------
// Function: tst_MakefileGenerator::verifyOutputContains()
//-----------------------------------------------------------------------------
void tst_MakefileGenerator::verifyOutputContains(QString instanceName, QString const& expectedOutput)
{
    QFile outputFile(outputDir_ + "/sw_tsydemi/" + instanceName + "/Makefile");

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
    QSharedPointer<QList<QSharedPointer<SWBuildCommand> > > coms = view->getSWBuildCommands();
    QSharedPointer<SWBuildCommand> cmd = QSharedPointer<SWBuildCommand>(new SWBuildCommand());
    cmd->setCommand(command);
    cmd->setFileType(fileType);
    cmd->setFlags(flags);
    cmd->setReplaceDefaultFlags(replace);
	coms->append(cmd);
}

QSharedPointer<FileSet> tst_MakefileGenerator::addFileSet(QSharedPointer<Component> component, QString fileSetName, QSharedPointer<SWView>& view)
{
    QSharedPointer<FileSet> fileSet( new FileSet(fileSetName,"sourceFiles") );

	if ( view )
	{
		QStringList fileSets = view->getFileSetRefs();
		fileSets.append(fileSetName);
		view->setFileSetRefs(fileSets);
	}
	
	QSharedPointer< QList< QSharedPointer<FileSet> > > sets = component->getFileSets();
	sets->append(fileSet);

    return fileSet;
}

void tst_MakefileGenerator::addFileSetBuilder(QSharedPointer<FileSet> fileSet, QString command, QString fileType, QString flags, bool replace)
{
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > fblist = fileSet->getDefaultFileBuilders();
    QSharedPointer<FileBuilder> fb = QSharedPointer<FileBuilder>(new FileBuilder);
    fb->setCommand( command);
    fb->setFileType(fileType);
    fb->setFlags(flags);
    fb->setReplaceDefaultFlags("false");

	if ( replace )
	{
		fb->setReplaceDefaultFlags("true");
	}

	fblist->append(fb);
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
	QSharedPointer<ActiveInterface> air1( new ActiveInterface(com1,bus1));
	QSharedPointer<ActiveInterface> air2( new ActiveInterface(com2,bus2));
	dependency1->setInterface1(air1);
	dependency1->setInterface2(air2);
	deps.append(dependency1);
	design->setApiConnections(deps);
}

QTEST_APPLESS_MAIN(tst_MakefileGenerator)

#include "tst_MakefileGenerator.moc"
