//-----------------------------------------------------------------------------------
// File: InterconnectAddressHelper.h
//-----------------------------------------------------------------------------------
// Project: Kactus2
// Author: Sampo Suokuisma
// Date: 05.06.2025
//
// Description:
// Class for calculating start and range values for interconnect endpoint interfaces
//-----------------------------------------------------------------------------------

#ifndef INTERCONNECTADDRESSHELPER_H
#define INTERCONNECTADDRESSHELPER_H

#include <QHash>
#include <QString>
#include <QSharedPointer>

#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/LibraryHandler.h>
#include <KactusAPI/include/ComponentInstanceInterface.h>
#include <KactusAPI/include/MemoryMapInterface.h>
#include <KactusAPI/include/AddressBlockInterface.h>
#include <KactusAPI/include/ComponentInstanceInterface.h>
#include <KactusAPI/include/InterconnectionInterface.h>
#include <KactusAPI/include/AdHocConnectionInterface.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/RegisterInterface.h>
#include <KactusAPI/include/ResetInterface.h>
#include <KactusAPI/include/FieldInterface.h>
#include <KactusAPI/include/AccessPolicyInterface.h>
#include <KactusAPI/include/ModeReferenceInterface.h>
#include <KactusAPI/include/ParametersInterface.h>
#include <KactusAPI/include/MessageMediator.h>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>

//----------------------------------------------------------------------------------
//! Class for calculating start and range values for interconnect endpoint interfaces
//----------------------------------------------------------------------------------
class InterconnectAddressHelper {
public:

    /*!
     *  Constructor.
     *
     *  @param [in] designVLNV   The VLNV of the design component to inspect.
     *  @param [in] library      Library interface for resolving IP-XACT documents.
     *  @param [in] messager     Messaging interface for logging and user feedback.
     */
    InterconnectAddressHelper(VLNV designVLNV,
        LibraryHandler* library, MessageMediator* messager);

    /*!
     *  Destructor.
     */
    virtual ~InterconnectAddressHelper() = default;

    /*!
     *  Resolve and assign a target address range for a bus interface.
     *
     *  @param [in]  instanceName     Instance name of the component.
     *  @param [in]  interfaceName    Bus interface bound to a memory map.
     *  @param [out] outStart         Assigned start address (result).
     *  @param [out] outRange         Size of the assigned address space (result).
     *
     *  @return True if an address range could be resolved and assigned.
     */
    bool getEndpointAddressRange(const QString& instanceName,
        const QString& interfaceName,
        quint64& outStart,
        quint64& outRange);

    /*!
     *  Release an assigned address range previously allocated to a component.
     *
     *  @param [in] instanceName  The instance whose range to release.
     */
    void releaseEndpointAddress(const QString& instanceName);

    /*!
     *  Reset all allocated and free address tracking state.
     */
    void reset();

private:

    /*!
     *  Ensure all interfaces and validators are initialized for the given component.
     *
     *  @param [in] component  Component to prepare for address evaluation.
     */
    void initialize(QSharedPointer<Component> component);

    /*!
     *  Create validators used in parsing and validating address-related IP-XACT structures.
     *
     *  @param [in] component  Component to base validators on.
     */
    void createValidators(QSharedPointer<Component> component);

    /*!
     *  Create interfaces for interacting with memory maps, blocks, and parameters.
     *
     *  @param [in] component  Component to initialize interfaces for.
     */
    void createInterfaces(QSharedPointer<Component> component);

    /*!
     *  Load and return the component bound to the given instance.
     *
     *  @param [in] instanceName  Name of the instance to resolve.
     *
     *  @return The loaded component, or nullptr on failure.
     */
    QSharedPointer<Component> loadComponentForInstance(const QString& instanceName);

    /*!
     *  Find the memory map bound to a given bus interface name.
     *
     *  @param [in] component      The component to search within.
     *  @param [in] interfaceName  Name of the interface to match.
     *
     *  @return Name of the matching memory map, or empty string if none found.
     */
    QString findMatchingMemoryMap(QSharedPointer<Component> component, const QString& interfaceName);

    /*!
     *  Calculate the total range (in address units) required by all address blocks in a memory map.
     *
     *  @param [in] component  The component owning the memory map.
     *  @param [in] mapName    Name of the memory map to analyze.
     *
     *  @return Total address range in units.
     */
    quint64 calculateTotalRange(QSharedPointer<Component> component, const QString& mapName);

    /*!
     *  Assign an available address range for a component and record the assignment.
     *
     *  @param [in]  instanceName  Name of the instance being assigned.
     *  @param [in]  totalRange    Number of address units required.
     *  @param [out] outStart      Start of assigned range.
     *  @param [out] outRange      Size of assigned range.
     *
     *  @return True if assignment succeeded.
     */
    bool assignAddressRange(const QString& instanceName, quint64 totalRange,
        quint64& outStart, quint64& outRange);

    /*!
     *  Merge overlapping or adjacent free address blocks into a compact list.
     */
    void mergeFreeSpaces();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for displaying messages, warnings, or errors to the user.
    MessageMediator* messager_;

    //! Parses expressions (e.g., widths or addresses) into evaluated forms.
    QSharedPointer<ExpressionParser> expressionParser_ = nullptr;

    //! Formats expressions into human-readable strings.
    QSharedPointer<ExpressionFormatter> expressionFormatter_ = nullptr;

    //! Finds parameter values within a component, supports expression evaluation.
    QSharedPointer<ListParameterFinder> listParameterFinder_ = nullptr;

    //!Parameter finder for abstraction definition expressions.
    QSharedPointer<ListParameterFinder> absDefFinder_ = nullptr;

    //! Parser for abstraction definition expressions.
    QSharedPointer<ExpressionParser> absDefParser_ = nullptr;

    //! Access to the IP-XACT library and all registered documents.
    LibraryHandler* library_ = nullptr;

    //! Validates parameter expressions and types against IP-XACT constraints.
    QSharedPointer<ParameterValidator> parameterValidator_ = nullptr;

    //! Validates enumerated values within fields and registers.
    QSharedPointer<EnumeratedValueValidator> enumValidator_ = nullptr;

    //! Validates individual fields within a register.
    QSharedPointer<FieldValidator> fieldValidator_ = nullptr;

    //! Validates complete registers, including fields and constraints.
    QSharedPointer<RegisterValidator> registerValidator_ = nullptr;

    //! Validates register files, including nested structures and register sets.
    QSharedPointer<RegisterFileValidator> registerFileValidator_ = nullptr;

    //! Validates address blocks, their layout, and contents.
    QSharedPointer<AddressBlockValidator> addressBlockValidator_ = nullptr;

    //! Validates subspace maps and address translations.
    QSharedPointer<SubspaceMapValidator> subspaceValidator_ = nullptr;

    //! Validates memory maps and their address layout.
    QSharedPointer<MemoryMapValidator> memoryMapValidator_ = nullptr;

    //! Validates port map bindings and directionality.
    QSharedPointer<PortMapValidator> portMapValidator_ = nullptr;

    //! Validates bus interfaces including ports, views, and connections.
    QSharedPointer<BusInterfaceValidator> busValidator_ = nullptr;

    //! Interface for interacting with memory maps.
    QSharedPointer<MemoryMapInterface> memoryMapInterface_ = nullptr;

    //! Interface for interacting with address blocks.
    QSharedPointer<AddressBlockInterface> addressBlockInterface_ = nullptr;

    //! Interface for handling reset types and default values.
    ResetInterface* resetInterface_ = nullptr;

    //! Interface for accessing and editing register fields.
    FieldInterface* fieldInterface_ = nullptr;

    //! Interface for accessing component mode definitions.
    ModeReferenceInterface* modeRefInterface_ = nullptr;

    //! Interface for resolving access policies based on active modes.
    AccessPolicyInterface* accessPolicyInterface_ = nullptr;

    //! Interface for resolving bus interface structure and metadata.
    BusInterfaceInterface* busInfInterface_ = nullptr;

    //! Interface for interacting with parameters in a component.
    QSharedPointer<ParametersInterface> parameterInterface_ = nullptr;

    //! Interface for accessing registers and register fields.
    RegisterInterface* registerInterface_ = nullptr;

    //! Interface for reading and writing system-level interconnect data.
    InterconnectionInterface* connectionInterface_{ new InterconnectionInterface() };

    //! Interface for accessing ad hoc (non-bus) connections in the design.
    AdHocConnectionInterface* adhocConnectionInterface_{ new AdHocConnectionInterface() };

    //! Interface for enumerating and accessing component instances.
    ComponentInstanceInterface* instanceInterface_{ new ComponentInstanceInterface(connectionInterface_, adhocConnectionInterface_) };

    //! The currently loaded component for address evaluation.
    QSharedPointer<Component> currentComponent_ = nullptr;

    //! Stores address ranges assigned to each component instance.
    QHash<QString, QPair<quint64, quint64>> usedAddressRanges_;

    //! Monotonically increasing next free address.
    quint64 nextAvailableAddress_ = 0;

    //! List of reusable free address regions that can be assigned again.
    QList<QPair<quint64, quint64>> freeAddressSpaces_;
};

#endif // INTERCONNECTADDRESSHELPER_H
