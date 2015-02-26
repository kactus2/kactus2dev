//-----------------------------------------------------------------------------
// File: tst_ComponentParameterModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.01.2015
//
// Description:
// Unit test for class ComponentParameterModel.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QModelIndex>
#include <QVariant>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/parameters/ComponentParameterColumns.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/parameter.h>

class tst_ComponentParameterModel : public QObject
{
    Q_OBJECT

public:
    tst_ComponentParameterModel();

private slots:
    void testHasRowForEachTopLevelParameter();
    void testHasRowForEachTopLevelParameter_data();     
    void testHasColumnForNameValueAndId();
    void testFunctionAsValue();

private:
    ComponentParameterModel* make(QSharedPointer<Component> component);
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentParameterModel::()
//-----------------------------------------------------------------------------
tst_ComponentParameterModel::tst_ComponentParameterModel()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentParameterModel::testHasRowForEachTopLevelParameter()
//-----------------------------------------------------------------------------
void tst_ComponentParameterModel::testHasRowForEachTopLevelParameter()
{
    QFETCH(int, parameterCount);

    QList<QSharedPointer<Parameter> > parameters;

    for (int i = 0; i < parameterCount; i++)
    {
        parameters.append(QSharedPointer<Parameter>(new Parameter()));
    }

    QSharedPointer<Component> component(new Component());
    component->getParameters()->append(parameters);

    ComponentParameterModel* model = make(component);

    QCOMPARE(model->rowCount(), parameterCount);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentParameterModel::testHasRowForEachTopLevelParameter_data()
//-----------------------------------------------------------------------------
void tst_ComponentParameterModel::testHasRowForEachTopLevelParameter_data()
{
    QTest::addColumn<int>("parameterCount");

    QTest::newRow("no parameters") << 0;
    QTest::newRow("single parameter") << 1;
    QTest::newRow("multiple parameters") << 4;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentParameterModel::testHasColumnForNameValueAndId()
//-----------------------------------------------------------------------------
void tst_ComponentParameterModel::testHasColumnForNameValueAndId()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("testId");
    testParameter->setValue("1");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);
   
    QSharedPointer<Component> component(new Component());
    component->getParameters()->append(parameters);

    ComponentParameterModel* model = make(component);

    QCOMPARE(model->columnCount(), 3);
    QCOMPARE(model->data(model->index(0, ComponentParameterColumns::NAME)).toString(), QString("testParameter"));
    QCOMPARE(model->data(model->index(0, ComponentParameterColumns::VALUE)).toString(), QString("1"));
    QCOMPARE(model->data(model->index(0, ComponentParameterColumns::ID)).toString(), QString("testId"));

    QCOMPARE(model->data(QModelIndex()).toString(), QString(""));
    QCOMPARE(model->data(model->index(42, ComponentParameterColumns::NAME)).toString(), QString(""));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentParameterModel::testFunctionAsValue()
//-----------------------------------------------------------------------------
void tst_ComponentParameterModel::testFunctionAsValue()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("testId");
    testParameter->setValue("1+1");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> component(new Component());
    component->getParameters()->append(parameters);

    ComponentParameterModel* model = make(component);

    QCOMPARE(model->data(model->index(0, ComponentParameterColumns::VALUE)).toString(), QString("2"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentParameterModel::make()
//-----------------------------------------------------------------------------
ComponentParameterModel* tst_ComponentParameterModel::make(QSharedPointer<Component> component)
{
    QSharedPointer<ParameterFinder> parameterFinder(new ComponentParameterFinder(component));

    ComponentParameterModel* model = new ComponentParameterModel(this, parameterFinder);
    
    QSharedPointer<ExpressionParser> parser(new IPXactSystemVerilogParser(parameterFinder));
    model->setExpressionParser(parser);
    
    return model;
}

QTEST_APPLESS_MAIN(tst_ComponentParameterModel)

#include "tst_ComponentParameterModel.moc"
