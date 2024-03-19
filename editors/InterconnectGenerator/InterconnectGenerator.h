#ifndef INTERCONNECTGENERATOR_H
#define INTERCONNECTGENERATOR_H

#include <IPXACTmodels/Component/validators/PortValidator.h>


#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/LibraryInterface.h>
#include <KactusAPI/include/ComponentAndInstantiationsParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/PortsInterface.h>
#include <KactusAPI/include/ComponentInstanceInterface.h>
#include <KactusAPI/include/InterconnectionInterface.h>
#include <KactusAPI/include/AdHocConnectionInterface.h>
#include <KactusAPI/include/MessageMediator.h>
#include <KactusAPI/include/AbstractionTypeInterface.h>

class InterconnectGenerator
{
public:
    //! The constructor
    InterconnectGenerator(LibraryInterface* library,  MessageMediator* messager);

    //! The destructor
    ~InterconnectGenerator() = default;

    VLNV generate(std::string sDesignVLNV, std::string sInterconVLNV);

    void openDesign(VLNV designVLNV);

    void createInterconComponent(VLNV VLNV);

    void findUnconnectedInterface();

    void createBusInterface(std::string busName, std::string modeString, int index);

    void createPortMaps(std::string modeString, QSharedPointer<BusInterface> busInf);

    void createPhysPorts(QSharedPointer<Component> comp, QString busName);

    //! No copying. No assignment.
    InterconnectGenerator(const InterconnectGenerator& other) = delete;
    InterconnectGenerator& operator=(const InterconnectGenerator& other) = delete;

private:

    //! Message handler.
    MessageMediator* messager_{ nullptr };

    LibraryInterface* library_{ nullptr };

    QSharedPointer<Component> interconComponent_{ nullptr };

    //! Currently active design.
    QSharedPointer<Design> design_{ nullptr };

    //! Component parameter finder.
    QSharedPointer<ComponentParameterFinder> parameterFinder_{ new ComponentAndInstantiationsParameterFinder(nullptr) };

    //! Parser for expressions.
    QSharedPointer<ExpressionParser> expressionParser_{ new IPXactSystemVerilogParser(parameterFinder_) };

    //! Formatter for expressions.
    QSharedPointer<ExpressionFormatter>expressionFormatter_{ new ExpressionFormatter(parameterFinder_) };

    //! Validator for ports.
    QSharedPointer<PortValidator> portValidator_{ new PortValidator(expressionParser_,
        QSharedPointer<QList<QSharedPointer<View> > >()) };

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInfInterface_{ nullptr };

    //! Interface for accessing the component ports.
    PortsInterface* portsInterface_{ new PortsInterface(portValidator_, expressionParser_, expressionFormatter_) };

    //! Interface for accessing interconnections.
    InterconnectionInterface* connectionInterface_{ new InterconnectionInterface() };

    //! Interface for accessing ad hoc connections.
    AdHocConnectionInterface* adhocConnectionInterface_{ new AdHocConnectionInterface() };

    //! Interface for accessing component instances.
    ComponentInstanceInterface* instanceInterface_{ new ComponentInstanceInterface(connectionInterface_,
        adhocConnectionInterface_) };

    AbstractionTypeInterface* absTypeInf_{ nullptr };

    std::string prefix_;

    VLNV busDefVLNV_ = VLNV(VLNV::BUSDEFINITION, "pulp-platform.org:interface:amba_AXI4:1.0");

};

#endif // INTERCONNECTGENERATOR_H
