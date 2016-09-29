//-----------------------------------------------------------------------------
// File: tst_ComponentInstanceVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Unit test for class ComponentInstanceVerilogWriter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/common/PortSorter/InterfaceDirectionNameSorter.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <QSharedPointer>


class tst_ComponentInstanceVerilogWriter : public QObject
{
    Q_OBJECT

public:
    tst_ComponentInstanceVerilogWriter();

private slots:
    void init();
    void cleanup();

    // Test cases:
    void testNullComponentAsConstructorParameter();
    void testNamedInstance();
    void testNamedInstance_data();
    void testUnconnectedInstancePorts();
    void testFullyConnectedPorts();
    void testPartiallyConnectedPorts();
    void testDefaultPortValueIsUsedForUnconnectedInputPort();
    void testInstanceParametersAreAssigned();

    void testModelParametersInComponentAreAssigned();

private:

    /*!
     *  Create an expression formatter using a component.
     *
     *      @param [in] component   The component, whose parameters are used.
     *
     *      @return Expression formatter, that uses the selected component.
     */
    QSharedPointer<ExpressionFormatter> createExpressionFormatterFromComponent(
        QSharedPointer <Component> component);

    /*!
     *  Create a module parameter.
     *
     *      @param [in] name        The name of the parameter.
     *      @param [in] value       The value of the parameter.
     *      @param [in] valueID     The unique ID of the parameter.
     */
    QSharedPointer<ModuleParameter> createTestModuleParameter(QString const& name, QString const& value,
        QString const& valueID);

    //! The writer output.
    QString output_;

    //! The output stream for the writer.
    QTextStream outputStream_;

    //! The sorter for the instance ports.
    QSharedPointer<PortSorter> defaultSorter_;
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::tst_ComponentInstanceVerilogWriter()
//-----------------------------------------------------------------------------
tst_ComponentInstanceVerilogWriter::tst_ComponentInstanceVerilogWriter() : output_(), outputStream_(), 
    defaultSorter_(new InterfaceDirectionNameSorter())
{
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::init()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::init()
{    
    outputStream_.setString(&output_);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::cleanup()
{
    output_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testNullComponentAsConstructorParameter()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testNullComponentAsConstructorParameter()
{
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0"));

    QSharedPointer<ComponentInstance> instance(new ComponentInstance("compInstance", instanceVLNV));

    ComponentInstanceVerilogWriter writer(instance, QSharedPointer<Component>(0), QSharedPointer<PortSorter>(0),
        createExpressionFormatterFromComponent(QSharedPointer<Component>(0)));
    writer.write(outputStream_);

    QCOMPARE(output_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testNamedInstance()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testNamedInstance()
{
    QFETCH(QString, referenceName);
    QFETCH(QString, instanceName);
    QFETCH(QString, expectedOutput);

    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", referenceName, "1.0"));
    QSharedPointer<Component> component (new Component(*instanceVLNV.data()));
    QSharedPointer<ComponentInstance> instance(new ComponentInstance(instanceName, instanceVLNV));

    QSharedPointer<ExpressionFormatter> expressionFormatter = createExpressionFormatterFromComponent(component);

    ComponentInstanceVerilogWriter writer(instance, component, QSharedPointer<PortSorter>(0), expressionFormatter);
    writer.write(outputStream_);

    QCOMPARE(output_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testNamedInstance_data()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testNamedInstance_data()
{
    QTest::addColumn<QString>("referenceName");
    QTest::addColumn<QString>("instanceName");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("empty") << "" << "" << "     ();\n";
    QTest::newRow("empty reference") << "" << "instance" <<      "     instance();\n";
    QTest::newRow("empty instance name") << "chip" << "" <<      "    chip ();\n";
    QTest::newRow("common") << "TestComponent" << "instance1" << "    TestComponent instance1();\n";
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testUnconnectedInstancePorts()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testUnconnectedInstancePorts()
{
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0"));
    QSharedPointer<Component> refComponent(new Component(*instanceVLNV.data()));

    refComponent->getPorts()->append(QSharedPointer<Port>(new Port("a_in", DirectionTypes::IN)));
    refComponent->getPorts()->append(QSharedPointer<Port>(new Port("b_in", DirectionTypes::IN)));
    refComponent->getPorts()->append(QSharedPointer<Port>(new Port("c_out", DirectionTypes::OUT)));

    QSharedPointer<ComponentInstance> instance (new ComponentInstance("instance1", instanceVLNV));

    QSharedPointer<ExpressionFormatter> expressionFormatter = createExpressionFormatterFromComponent(refComponent);

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_, expressionFormatter);
    writer.write(outputStream_);

    QCOMPARE(output_, QString(
        "    TestComponent instance1(\n"
        "        // These ports are not in any interface\n"    
        "        .a_in                ( ),\n"
        "        .b_in                ( ),\n"
        "        .c_out               ( ));\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testFullyConnectedPorts()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testFullyConnectedPorts()
{
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV (
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0"));
    QSharedPointer<Component> refComponent(new Component(*instanceVLNV.data()));

    refComponent->getPorts()->append(QSharedPointer<Port>(new Port("clk", DirectionTypes::IN)));
    refComponent->getPorts()->append(QSharedPointer<Port>(new Port("rst_n", DirectionTypes::IN)));

    QSharedPointer<ComponentInstance> instance(new ComponentInstance("instance1", instanceVLNV));

    QSharedPointer<ExpressionFormatter> expressionFormatter = createExpressionFormatterFromComponent(refComponent);

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_, expressionFormatter);
    writer.assignPortForFullWidth("clk", "top_clk");
    writer.assignPortForFullWidth("rst_n", "rst");

    writer.write(outputStream_);

    QCOMPARE(output_, QString(
        "    TestComponent instance1(\n"
        "        // These ports are not in any interface\n"  
        "        .clk                 (top_clk),\n"
        "        .rst_n               (rst));\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testPartiallyConnectedPorts()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testPartiallyConnectedPorts()
{
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0"));
    QSharedPointer<Component> refComponent(new Component(*instanceVLNV.data()));

    refComponent->getPorts()->append(QSharedPointer<Port>(new Port("chip_select", DirectionTypes::IN)));
    refComponent->getPorts()->append(QSharedPointer<Port>(new Port("data", DirectionTypes::IN)));

    QSharedPointer<ComponentInstance> instance(new ComponentInstance("instance1", instanceVLNV));

    QSharedPointer<ExpressionFormatter> expressionFormatter = createExpressionFormatterFromComponent(refComponent);

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_, expressionFormatter);
    writer.assignPortForRange("chip_select", "top_select", 0, 0);
    writer.assignPortForRange("data", "top_data", 7, 0);    

    writer.write(outputStream_);

    QCOMPARE(output_, QString(
        "    TestComponent instance1(\n"
        "        // These ports are not in any interface\n"  
        "        .chip_select         (top_select[0]),\n"
        "        .data                (top_data[7:0]));\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testDefaultPortValueIsUsedForUnconnectedInputPort()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testDefaultPortValueIsUsedForUnconnectedInputPort()
{
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0"));
    QSharedPointer<Component> refComponent(new Component(*instanceVLNV.data()));

    QSharedPointer<Port> firstPort (new Port("a_in", DirectionTypes::IN));
    firstPort->setDefaultValue("'b0");
    firstPort->setAllLogicalDirectionsAllowed(true);
    refComponent->getPorts()->append(firstPort);

    QSharedPointer<Port> secondPort (new Port("b_out", DirectionTypes::OUT));
    secondPort->setLeftBound("7");
    secondPort->setDefaultValue("bDefault");
    refComponent->getPorts()->append(secondPort);

    QSharedPointer<Port> thirdPort (new Port("c_inout", DirectionTypes::INOUT));
    thirdPort->setDefaultValue("'cDefault");
    refComponent->getPorts()->append(thirdPort);

    QSharedPointer<ComponentInstance> instance(new ComponentInstance("instance1", instanceVLNV));

    QSharedPointer<ExpressionFormatter> expressionFormatter = createExpressionFormatterFromComponent(refComponent);

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_, expressionFormatter); 
    writer.write(outputStream_);

    QCOMPARE(output_, QString(
        "    TestComponent instance1(\n"
        "        // These ports are not in any interface\n"  
        "        .a_in                ('b0),\n"
        "        .b_out               ( ),\n"
        "        .c_inout             ( ));\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testInstanceParametersAreAssigned()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testInstanceParametersAreAssigned()
{
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0"));
    QSharedPointer<Component> refComponent(new Component(*instanceVLNV.data()));

    QSharedPointer<ComponentInstance> instance(new ComponentInstance("instance1", instanceVLNV));

    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > parameters(
        new QList<QSharedPointer<ConfigurableElementValue> >());
    parameters->append(QSharedPointer<ConfigurableElementValue>(new ConfigurableElementValue("1", "id")));
    parameters->append(QSharedPointer<ConfigurableElementValue>(new ConfigurableElementValue("tester", "name")));
    parameters->append(QSharedPointer<ConfigurableElementValue>(new ConfigurableElementValue("16", "numElements")));

    instance->setConfigurableElementValues(parameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createExpressionFormatterFromComponent(refComponent);

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_, expressionFormatter); 
    writer.write(outputStream_);

    QCOMPARE(output_, QString(
        "    TestComponent #(\n"
        "        .id                  (1),\n"
        "        .name                (tester),\n"
        "        .numElements         (16))\n"
        "    instance1();\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testModelParametersInComponentAreAssigned()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testModelParametersInComponentAreAssigned()
{
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV (
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0"));
    QSharedPointer<Component> refComponent(new Component(*instanceVLNV.data()));

    QSharedPointer<ModuleParameter> modelRef = createTestModuleParameter("modelRef", "15", "A-model");
    QSharedPointer<ModuleParameter> referringModelRef =
        createTestModuleParameter("referer", modelRef->getValueId(), "B-model");

    QSharedPointer<ComponentInstantiation> testInstantiation (new ComponentInstantiation("testInstantiation"));
    testInstantiation->getModuleParameters()->append(modelRef);
    testInstantiation->getModuleParameters()->append(referringModelRef);

    refComponent->getComponentInstantiations()->append(testInstantiation);

    QSharedPointer<View> testView (new View("testView"));
    testView->setComponentInstantiationRef(testInstantiation->name());
    refComponent->getViews()->append(testView);

    QSharedPointer<ComponentInstance> instance (new ComponentInstance("instance1", instanceVLNV));

    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > parameters(
        new QList<QSharedPointer<ConfigurableElementValue> >());
    parameters->append(QSharedPointer<ConfigurableElementValue>(
        new ConfigurableElementValue("10", modelRef->getValueId())));
    parameters->append(QSharedPointer<ConfigurableElementValue>(
        new ConfigurableElementValue(referringModelRef->getValue(), referringModelRef->getValueId())));

    instance->setConfigurableElementValues(parameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createExpressionFormatterFromComponent(refComponent);

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_, expressionFormatter);
    writer.write(outputStream_);

    QCOMPARE(output_, QString(
        "    TestComponent #(\n"
        "        .modelRef            (10),\n"
        "        .referer             (modelRef))\n"
        "    instance1();\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::createExpressionFormatterFromComponent()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionFormatter> tst_ComponentInstanceVerilogWriter::createExpressionFormatterFromComponent(
    QSharedPointer<Component> component)
{
    QSharedPointer<ParameterFinder> parameterFinder (new ComponentParameterFinder(component));
    QSharedPointer<ExpressionFormatter> expressionFormatter (new ExpressionFormatter(parameterFinder));

    return expressionFormatter;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::createTestModuleParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModuleParameter> tst_ComponentInstanceVerilogWriter::createTestModuleParameter(QString const& name,
    QString const& value, QString const& valueID)
{
    QSharedPointer<ModuleParameter> referencingModuleParameter(new ModuleParameter());
    referencingModuleParameter->setName(name);
    referencingModuleParameter->setValue(value);
    referencingModuleParameter->setValueId(valueID);

    return referencingModuleParameter;
}

QTEST_APPLESS_MAIN(tst_ComponentInstanceVerilogWriter)

#include "tst_ComponentInstanceVerilogWriter.moc"
