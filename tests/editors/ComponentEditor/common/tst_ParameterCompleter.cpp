//-----------------------------------------------------------------------------
// File: tst_ParameterCompleter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.01.2015
//
// Description:
// Unit test for class ParameterCompleter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/component.h>


class tst_ParameterCompleter : public QObject
{
    Q_OBJECT

public:
    tst_ParameterCompleter();

private slots:
    void testNameIsFoundAsCompletion();
};

//-----------------------------------------------------------------------------
// Function: tst_ParameterCompleter::()
//-----------------------------------------------------------------------------
tst_ParameterCompleter::tst_ParameterCompleter()
{
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterCompleter::testNameIsFoundAsCompletion()
//-----------------------------------------------------------------------------
void tst_ParameterCompleter::testNameIsFoundAsCompletion()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("param");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->setParameters(parameters);

    QSharedPointer<ParameterFinder> parameterFinder(new ComponentParameterFinder(targetComponent));

    ComponentParameterModel* model = new ComponentParameterModel(this, parameterFinder);

    QCompleter* completer = new ParameterCompleter(this);
    completer->setModel(model);

    QCOMPARE(completer->completionCount(), 1);
    QCOMPARE(completer->currentCompletion(), QString("param"));
}

QTEST_MAIN(tst_ParameterCompleter)

#include "tst_ParameterCompleter.moc"
