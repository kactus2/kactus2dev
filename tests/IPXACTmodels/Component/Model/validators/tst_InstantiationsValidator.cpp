//-----------------------------------------------------------------------------
// Instantiations: tst_InstantiationsValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 23.11.2015
//
// Description:
// Unit test for class InstantiationsValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <tests/MockObjects/LibraryMock.h>

#include <QtTest>

class tst_InstantiationsValidator : public QObject
{
	Q_OBJECT

public:
    tst_InstantiationsValidator();

private slots:
	void designInstantiationFail();
	void designInstantiationSuccess();
	void designConfigurationInstantiationFail();
	void designConfigurationInstantiationSuccess();
	void baseCase();
	void inexistingFileSet();
	void failFileBuilder();
	void failParameter();
	void failModuleParameter();
	void succeedEveryting();

private:

	QSharedPointer<QList<QSharedPointer<FileSet > > > fileSets_;

    QSharedPointer<ExpressionParser> expressionParser_;

    QSharedPointer<ParameterValidator2014> parameterValidator_;
};

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::tst_InstantiationsValidator()
//-----------------------------------------------------------------------------
tst_InstantiationsValidator::tst_InstantiationsValidator()
{
	fileSets_ = QSharedPointer<QList<QSharedPointer<FileSet> > >( new QList<QSharedPointer<FileSet> > );

    expressionParser_ = QSharedPointer<ExpressionParser> (new SystemVerilogExpressionParser());

    parameterValidator_ = QSharedPointer<ParameterValidator2014>
        (new ParameterValidator2014(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::designInstantiationFail()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::designInstantiationFail()
{
	QSharedPointer<DesignInstantiation> instantiation( new DesignInstantiation );
	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, 0);

	instantiation->setName(" \t \n");
	QSharedPointer<ConfigurableVLNVReference> vref( new ConfigurableVLNVReference );
	instantiation->setDesignReference(vref);

	QVector<QString> errorList;
	validator.findErrorsInDesignInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 2 );
	QVERIFY( !validator.validateDesignInstantiation(instantiation) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::designInstantiationSuccess()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::designInstantiationSuccess()
{
    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<ConfigurableVLNVReference> vref( new
        ConfigurableVLNVReference( VLNV::DESIGN, "vendori", "kurjasto", "nimi", "versio") );

    QSharedPointer<Design> testDesign (new Design(*vref.data()));
    mockLibrary->addComponent(testDesign);

	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, mockLibrary);

    QSharedPointer<DesignInstantiation> instantiation( new DesignInstantiation );
	instantiation->setName("esa");
	instantiation->setDesignReference(vref);

	QVector<QString> errorList;
	validator.findErrorsInDesignInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validateDesignInstantiation(instantiation) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::designConfigurationInstantiationFail()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::designConfigurationInstantiationFail()
{
	QSharedPointer<DesignConfigurationInstantiation> instantiation( new DesignConfigurationInstantiation );
	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, 0);

	instantiation->setName(" \t \n");
	QSharedPointer<ConfigurableVLNVReference> vref( new ConfigurableVLNVReference );
	instantiation->setDesignConfigurationReference(vref);

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{'b11, 'b00}");
	parameter->setValueId("parameterid");

	instantiation->getParameters()->append(parameter);

	QVector<QString> errorList;
	validator.findErrorsInDesignConfigurationInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 3 );
	QVERIFY( !validator.validateDesignConfigurationInstantiation(instantiation) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::designConfigurationInstantiationSuccess()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::designConfigurationInstantiationSuccess()
{
    QSharedPointer<ConfigurableVLNVReference> vref( new
        ConfigurableVLNVReference( VLNV::DESIGNCONFIGURATION, "vendori", "kurjasto", "nimi", "versio") );

    QSharedPointer<DesignConfiguration> testDesignConfiguration (new DesignConfiguration(*vref.data()));

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(testDesignConfiguration);

	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, mockLibrary);

    QSharedPointer<DesignConfigurationInstantiation> instantiation( new DesignConfigurationInstantiation );
	instantiation->setName("esa");
	instantiation->setDesignConfigurationReference(vref);

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{3'b101,3'b111}");
	parameter->setValueId("parameterid");

	instantiation->getParameters()->append(parameter);

	QVector<QString> errorList;
	validator.findErrorsInDesignConfigurationInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validateDesignConfigurationInstantiation(instantiation) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::baseCase()
{
	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, 0);

    QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation ("testInstantiation"));

    QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validateComponentInstantiation(instantiation) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::inexistingFileSet()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::inexistingFileSet()
{
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation ("testInstantiation"));
	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, 0);

	instantiation->getFileSetReferences()->append("joq");

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validateComponentInstantiation(instantiation) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::failFileBuilder()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::failFileBuilder()
{
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation ("testInstantiation"));
	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, 0);

	QSharedPointer<FileBuilder> fbuilder( new FileBuilder );
	fbuilder->setReplaceDefaultFlags("joq");
	instantiation->getDefaultFileBuilders()->append(fbuilder);

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 2 );
	QVERIFY( !validator.validateComponentInstantiation(instantiation) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::failParameter()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::failParameter()
{
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation("testInstantiation") );
	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, 0);

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{'b11, 'b00}");
	parameter->setValueId("parameterid");

	instantiation->getParameters()->append(parameter);

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validateComponentInstantiation(instantiation) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::failModuleParameter()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::failModuleParameter()
{
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation("testInstantiation") );
	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, 0);

	QSharedPointer<ModuleParameter> parameter (new ModuleParameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{'b11, 'b00}");
	parameter->setValueId("parameterid");
	parameter->setIsPresent("eeaaaa");
	parameter->setUsageType("anti-usage");

	instantiation->getModuleParameters()->append(parameter);

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 3 );
	QVERIFY( !validator.validateComponentInstantiation(instantiation) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::succeedEveryting()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::succeedEveryting()
{
	InstantiationsValidator validator(expressionParser_, fileSets_, parameterValidator_, 0);

    QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation ("testInstantiation"));
	instantiation->getFileSetReferences()->append("salaiset ansiot");
	QSharedPointer<FileSet> fileSet( new FileSet );
	fileSet->setName("salaiset ansiot");
	fileSets_->append(fileSet);

	QSharedPointer<FileBuilder> fbuilder( new FileBuilder );
	fbuilder->setReplaceDefaultFlags("0");
	fbuilder->setFileType("txt");
	instantiation->getDefaultFileBuilders()->append(fbuilder);

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{3'b101,3'b111}");
	parameter->setValueId("parameterid");

	instantiation->getParameters()->append(parameter);

	QSharedPointer<ModuleParameter> parameter2 (new ModuleParameter());
	parameter2->setName("param");
	parameter2->setType("bit");
	parameter2->setValue("{3'b101,3'b111}");
	parameter2->setValueId("parameterid");
	parameter2->setIsPresent("1");
	parameter2->setUsageType("typed");

	instantiation->getModuleParameters()->append(parameter2);

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validateComponentInstantiation(instantiation) );
}

QTEST_APPLESS_MAIN(tst_InstantiationsValidator)

#include "tst_InstantiationsValidator.moc"
