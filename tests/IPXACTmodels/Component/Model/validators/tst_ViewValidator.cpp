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
};

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::tst_ViewValidator()
//-----------------------------------------------------------------------------
tst_ViewValidator::tst_ViewValidator()
{
	model_ = QSharedPointer<Model>( new Model );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_ViewValidator::baseCase()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator;

	view->setName("joq");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test", model_);

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validate(view, model_) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::nameFail()
//-----------------------------------------------------------------------------
void tst_ViewValidator::nameFail()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator;

	view->setName(" \t");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test", model_);

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(view, model_) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::presenceFail()
//-----------------------------------------------------------------------------
void tst_ViewValidator::presenceFail()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator;

	view->setName("esa");
	view->setIsPresent("foo");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test", model_);

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(view, model_) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::envId()
//-----------------------------------------------------------------------------
void tst_ViewValidator::envId()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator;

	view->setName("esa");

	QStringList envIds;
	envIds.append("1:2:3");
	envIds.append("joq");
	envIds.append("ega");
	view->setEnvIdentifiers(envIds);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test", model_);

	QCOMPARE( errorList.size(), 2 );
	QVERIFY( !validator.validate(view, model_) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::instantiationsNotExist()
//-----------------------------------------------------------------------------
void tst_ViewValidator::instantiationsNotExist()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator;

	view->setName("esa");

	view->setComponentInstantiationRef("bogus");
	view->setDesignInstantiationRef("inexisting");
	view->setDesignConfigurationInstantiationRef("nil");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test", model_);

	QCOMPARE( errorList.size(), 3 );
	QVERIFY( !validator.validate(view, model_) );
}

//-----------------------------------------------------------------------------
// Function: tst_ViewValidator::instantiationsExist()
//-----------------------------------------------------------------------------
void tst_ViewValidator::instantiationsExist()
{
	QSharedPointer<View> view( new View );
	ViewValidator validator;

	view->setName("esa");

	QSharedPointer<ComponentInstantiation> cimp( new ComponentInstantiation("eka") );
	model_->getComponentInstantiations()->append( cimp );
	view->setComponentInstantiationRef(cimp->name());

	QSharedPointer<DesignInstantiation> di( new DesignInstantiation("toka") );
	model_->getDesignInstantiations()->append(di);
	view->setDesignInstantiationRef(di->name());

	QSharedPointer<DesignConfigurationInstantiation> dci( new DesignConfigurationInstantiation("kolmas") );
	model_->getDesignConfigurationInstantiations()->append(dci);
	view->setDesignConfigurationInstantiationRef(dci->name());

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, view, "test", model_);

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validate(view, model_) );
}

QTEST_APPLESS_MAIN(tst_ViewValidator)

#include "tst_ViewValidator.moc"
