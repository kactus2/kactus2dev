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

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

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
};

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::tst_InstantiationsValidator()
//-----------------------------------------------------------------------------
tst_InstantiationsValidator::tst_InstantiationsValidator()
{
	fileSets_ = QSharedPointer<QList<QSharedPointer<FileSet> > >( new QList<QSharedPointer<FileSet> > );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::designInstantiationFail()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::designInstantiationFail()
{
	QSharedPointer<DesignInstantiation> instantiation( new DesignInstantiation );
	InstantiationsValidator validator;

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
	QSharedPointer<DesignInstantiation> instantiation( new DesignInstantiation );
	InstantiationsValidator validator;

	instantiation->setName("esa");
	QSharedPointer<ConfigurableVLNVReference> vref( new
		ConfigurableVLNVReference( VLNV::DESIGN, "vendori", "kurjasto", "nimi", "versio") );
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
	InstantiationsValidator validator;

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
	QSharedPointer<DesignConfigurationInstantiation> instantiation( new DesignConfigurationInstantiation );
	InstantiationsValidator validator;

	instantiation->setName("esa");
	QSharedPointer<ConfigurableVLNVReference> vref( new
		ConfigurableVLNVReference( VLNV::DESIGN, "vendori", "kurjasto", "nimi", "versio") );
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
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation );
	InstantiationsValidator validator;

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test", fileSets_);

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validateComponentInstantiation(instantiation, fileSets_) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::inexistingFileSet()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::inexistingFileSet()
{
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation );
	InstantiationsValidator validator;

	instantiation->getFileSetReferences()->append("joq");

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test", fileSets_);

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validateComponentInstantiation(instantiation, fileSets_) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::failFileBuilder()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::failFileBuilder()
{
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation );
	InstantiationsValidator validator;

	QSharedPointer<FileBuilder> fbuilder( new FileBuilder );
	fbuilder->setReplaceDefaultFlags("joq");
	instantiation->getDefaultFileBuilders()->append(fbuilder);

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test", fileSets_);

	QCOMPARE( errorList.size(), 2 );
	QVERIFY( !validator.validateComponentInstantiation(instantiation, fileSets_) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::failParameter()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::failParameter()
{
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation );
	InstantiationsValidator validator;

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{'b11, 'b00}");
	parameter->setValueId("parameterid");

	instantiation->getParameters()->append(parameter);

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test", fileSets_);

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validateComponentInstantiation(instantiation, fileSets_) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::failModuleParameter()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::failModuleParameter()
{
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation );
	InstantiationsValidator validator;

	QSharedPointer<ModuleParameter> parameter (new ModuleParameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{'b11, 'b00}");
	parameter->setValueId("parameterid");
	parameter->setIsPresent("eeaaaa");
	parameter->setUsageType("anti-usage");

	instantiation->getModuleParameters()->append(parameter);

	QVector<QString> errorList;
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test", fileSets_);

	QCOMPARE( errorList.size(), 3 );
	QVERIFY( !validator.validateComponentInstantiation(instantiation, fileSets_) );
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsValidator::succeedEveryting()
//-----------------------------------------------------------------------------
void tst_InstantiationsValidator::succeedEveryting()
{
	QSharedPointer<ComponentInstantiation> instantiation( new ComponentInstantiation );
	InstantiationsValidator validator;

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
	validator.findErrorsInComponentInstantiation(errorList, instantiation, "test", fileSets_);

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validateComponentInstantiation(instantiation, fileSets_) );
}

QTEST_APPLESS_MAIN(tst_InstantiationsValidator)

#include "tst_InstantiationsValidator.moc"
