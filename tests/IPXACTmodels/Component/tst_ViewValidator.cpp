//-----------------------------------------------------------------------------
// View: tst_ViewValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 24.11.2015
//
// Description:
// Unit test for class ViewValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/validators/ViewValidator.h>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Model.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QtTest>

class tst_ViewValidator : public QObject
{
	Q_OBJECT

public:
    tst_ViewValidator();

private slots:
	void baseCase();
	void nameFail();
	void presenceFail();
	void envId();
	void instantiationsNotExist();
	void instantiationsExist();

private:
	
    QSharedPointer<Model> model_;

    QSharedPointer<ExpressionParser> parser_;
};

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::tst_ViewValidator()
//-----------------------------------------------------------------------------
tst_ViewValidator::tst_ViewValidator()
{
	model_ = QSharedPointer<Model>( new Model );

    parser_ = QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser());
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_ViewValidator::baseCase()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator(parser_, model_);

	view->setName("joq");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validate(view) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::nameFail()
//-----------------------------------------------------------------------------
void tst_ViewValidator::nameFail()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator(parser_, model_);

	view->setName(" \t");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(view) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::presenceFail()
//-----------------------------------------------------------------------------
void tst_ViewValidator::presenceFail()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator(parser_, model_);

	view->setName("esa");
	view->setIsPresent("foo");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(view) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::envId()
//-----------------------------------------------------------------------------
void tst_ViewValidator::envId()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator(parser_, model_);

	view->setName("esa");

	QStringList envIds;
	QSharedPointer<View::EnvironmentIdentifier> identifier1( new View::EnvironmentIdentifier );
	identifier1->language = "1";
	identifier1->tool = "2";
	identifier1->vendorSpecific = "3";
	view->addEnvIdentifier(identifier1);
	QSharedPointer<View::EnvironmentIdentifier> identifier2( new View::EnvironmentIdentifier );
	identifier2->language = "[]";
	identifier2->tool = "2";
	identifier2->vendorSpecific = "3";
	view->addEnvIdentifier(identifier2);
	QSharedPointer<View::EnvironmentIdentifier> identifier3( new View::EnvironmentIdentifier );
	identifier3->language = "%";
	view->addEnvIdentifier(identifier3);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validate(view) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::instantiationsNotExist()
//-----------------------------------------------------------------------------
void tst_ViewValidator::instantiationsNotExist()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator(parser_, model_);

	view->setName("esa");

	view->setComponentInstantiationRef("bogus");
	view->setDesignInstantiationRef("inexisting");
	view->setDesignConfigurationInstantiationRef("nil");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test");

	QCOMPARE( errorList.size(), 3 );
	QVERIFY( !validator.validate(view) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::instantiationsExist()
//-----------------------------------------------------------------------------
void tst_ViewValidator::instantiationsExist()
{
	QSharedPointer<ComponentInstantiation> cimp( new ComponentInstantiation("eka") );
	model_->getComponentInstantiations()->append( cimp );

	QSharedPointer<DesignInstantiation> di( new DesignInstantiation("toka") );
	model_->getDesignInstantiations()->append(di);

	QSharedPointer<DesignConfigurationInstantiation> dci( new DesignConfigurationInstantiation("kolmas") );
	model_->getDesignConfigurationInstantiations()->append(dci);

    QSharedPointer<View> view( new View );
    view->setName("esa");
    view->setComponentInstantiationRef(cimp->name());
    view->setDesignInstantiationRef(di->name());
    view->setDesignConfigurationInstantiationRef(dci->name());

    ViewValidator validator(parser_, model_);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validate(view) );
}

QTEST_APPLESS_MAIN(tst_ViewValidator)

#include "tst_ViewValidator.moc"
