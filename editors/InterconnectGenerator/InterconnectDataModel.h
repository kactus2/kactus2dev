//---------------------------------------------------------------------------
// File: InterconnectDataModel.h
//---------------------------------------------------------------------------
// Project: Kactus2
// Author: Sampo Suokuisma
// Date: 05.06.2025
//
// Description:
// Model class for collecting and validating interconnect data from a design.
//---------------------------------------------------------------------------


#ifndef INTERCONNECTDATAMODEL_H
#define INTERCONNECTDATAMODEL_H

#include <QHash>
#include <QSet>
#include <QList>
#include <QSharedPointer>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <KactusAPI/include/LibraryHandler.h>
#include <KactusAPI/include/InterconnectGenerator.h>
#include <editors/common/DesignWidget.h>

/*!
 *  Model class for collecting and validating interconnect data from a design.
 *  Used to determine valid connection rules between bus interfaces in a component hierarchy.
 */
class InterconnectDataModel
{
public:
    /*!
     *  Types of connection entities in the design.
     */
    enum class EntityType {
        Instance,       /*!< A sub-component instance within the design. */
        TopComponent    /*!< The top-level component of the design. */
    };

    /*!
     *  Supported types of interconnect behavior.
     */
    enum class InterconnectType {
        Bridge,         /*!< Transparent bridge with no arbitration. */
        Channel         /*!< Channel with arbitration and address mapping. */
    };

    /*!
     *  Key for identifying valid connection rule sources.
     */
    struct ConnectionKey {
        EntityType sourceEntity;             /*!< Source entity type (instance or top). */
        General::InterfaceMode sourceMode;   /*!< Mode of the source interface. */

        bool operator==(const ConnectionKey& other) const;
        bool operator!=(const ConnectionKey& other) const;
    };

    struct ConnectionKeyNew
    {
        InterconnectType icType;
        EntityType entityType;

        bool operator==(const ConnectionKeyNew& other) const { return other.icType == icType && other.entityType == entityType; }
        bool operator!=(const ConnectionKeyNew& other) const { return !operator==(other); }
    };

    /*!
     *  Describes a valid target connection rule.
     */
    struct ConnectionRule {
        EntityType targetEntity;             /*!< Target entity type (instance or top). */
        General::InterfaceMode targetMode;   /*!< Interface mode for the target. */
        InterconnectType interconnect;       /*!< Compatible interconnect type. */
    };

public:
    /*!
     *  Constructor.
     *
     *  @param [in] designWidget  Design editor widget (provides model access).
     *  @param [in] library       Library handler for VLNV models.
     *  @param [in] messager      Mediator for messaging and errors.
     */
    InterconnectDataModel(DesignWidget* designWidget, LibraryHandler* library, MessageMediator* messager);

    /*!
     *  Destructor.
     */
    virtual ~InterconnectDataModel() = default;

    /*!
     *  Gathers all bus interfaces and abstraction definitions from the design and its instances.
     */
    void gatherBusAndAbstractionData();

    /*!
     *  Returns abstraction references validated as compatible with existing buses.
     *
     *  @return Set of compatible abstraction references.
     */
    QSet<QSharedPointer<ConfigurableVLNVReference>> getValidAbstractionRefs() const;

    /*!
     *  Returns all abstraction references found in the design, regardless of validation.
     *
     *  @return Set of all abstraction references.
     */
    QSet<QSharedPointer<ConfigurableVLNVReference>> getAllAbstractionRefs() const;

    /*!
     *  Returns map of instances and their associated bus interfaces.
     *
     *  @return Hash of instance names to bus interface sets.
     */
    QHash<QString, QSet<QSharedPointer<BusInterface>>> getInstanceBusMap() const;

    /*!
     *  Returns hash of bus interfaces to abstraction names they implement.
     *
     *  @return Interface-to-abstraction-name hash.
     */
    QHash<QSharedPointer<BusInterface>, QSet<QString>> getInterfaceAbstractionHash() const;

    /*!
     *  Constructs a lookup table for [instance][interface name] -> BusInterface.
     *
     *  @return Nested hash lookup for quick bus interface resolution.
     */
    QHash<QString, QHash<QString, QSharedPointer<BusInterface>>> createInstanceBusesLookup() const;

    /*!
     *  Normalizes a legacy interface mode to IP-XACT 2022 standard.
     *
     *  @param [in] mode  Interface mode to normalize.
     *  @return Standardized interface mode.
     */
    static General::InterfaceMode normalizeTo2022(General::InterfaceMode mode);

    /*!
     *  Returns valid connection targets for a given source configuration.
     *
     *  @param [in] sourceEntity         Type of the source entity.
     *  @param [in] sourceMode           Interface mode of the source.
     *  @param [in] currentInterconnect  Selected interconnect type (bridge or channel).
     *  @return List of valid connection rules.
     */
    QList<ConnectionRule> getValidConnectionTargets(
        EntityType sourceEntity,
        General::InterfaceMode sourceMode,
        InterconnectType currentInterconnect) const;

    QSet<General::InterfaceMode> getConnectableInterfaceTypes(ConnectionKeyNew const& connectionKey) const;

    /*!
     *  Verifies whether the provided interconnect mode is valid for all
     *  current starting point and endpoint combinations.
     *
     *  @param [in] startingPoints   Map of instance names to source interfaces.
     *  @param [in] endpoints        Map of instance names to endpoint data.
     *  @param [in] mode             Interconnect type to validate.
     *  @return True if valid for all combinations; otherwise false.
     */
    bool isModeValidForAllConnections(
        const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints,
        const QHash<QString, QList<QSharedPointer<EndpointData>>>& endpoints,
        InterconnectType mode) const;

    /*!
     *  Returns true if the given source bus can connect to any target under any interconnect type.
     *
     *  @param [in] sourceEntity     Type of the source entity (Instance or TopComponent).
     *  @param [in] sourceMode       Normalized interface mode of the source.
     *  @return True if connectable under any interconnect type.
     */
    bool hasAnyValidConnection(EntityType sourceEntity, General::InterfaceMode sourceMode) const;

    /*!
     *  Returns invalid connection string to show on the dialog.
     *  
     *  @return True if valid for all combinations; otherwise false.
     */
    QString getLastInvalidConnectionMessage() const;

private:
    /*!
     *  Initialize all valid connection rules.
     */
    void initConnectionRules();
    void initConnectionRulesNew();
    /*!
     *  Collect bus interfaces from all component instances in the design.
     */
    void getBusesFromInstances();

    /*!
     *  Collect bus interfaces from the top-level component.
     */
    void getBusesFromTopComponent();

    /*!
     *  Extract and record abstraction definitions used by the given bus.
     *
     *  @param [in] bus  Bus interface to analyze.
     */
    void getAbstractionDefinitions(QSharedPointer<BusInterface> bus);

    /*!
     *  Filters abstraction references to determine which are connectable
     *  based on current design topology and connection rules.
     */
    void filterValidAbstractionReferences();

private:

    /*!
     *  Builds a lookup map of abstraction names to the bus interfaces
     *  that declare them.
     *
     *  @return Mapping from abstraction name to set of associated BusInterfaces.
     */
    QHash<QString, QSet<QSharedPointer<BusInterface>>> buildAbstractionToBusMap() const;

    /*!
     *  Constructs a multimap grouping bus interfaces by their mode
     *  and whether they belong to an instance or the top component.
     *
     *  @param [in] instanceBusesHash_   Map of instance names to sets of bus interfaces.
     *  @param [in] designWidget_        Design context used to resolve top component name.
     *
     *  @return Multi-hash mapping (interface mode, entity type) to BusInterfaces.
     */
    QMultiHash<QPair<General::InterfaceMode, EntityType>, QSharedPointer<BusInterface>>
        buildModeEntityToBusMap() const;

    /*!
     *  Checks if a given abstraction reference can be used to connect
     *  at least one valid pair of interfaces based on defined rules.
     *
     *  @param [in] absRef             Abstraction reference to validate.
     *  @param [in] absToBuses         Lookup from abstraction name to BusInterfaces.
     *  @param [in] modeEntityToBuses  Lookup from (mode, entity) to BusInterfaces.
     * 
     *  @return True if the abstraction is connectable; otherwise false.
     */
    bool isAbstractionConnectable(
        QSharedPointer<ConfigurableVLNVReference> absRef,
        const QHash<QString, QSet<QSharedPointer<BusInterface>>>& absToBuses,
        const QMultiHash<QPair<General::InterfaceMode, EntityType>, 
        QSharedPointer<BusInterface>>& modeEntityToBuses) const;

    /*!
     *  Determines the entity type (Instance or TopComponent) of a given bus interface.
     *
     *  @param [in] bus  The BusInterface whose owning entity is to be identified.
     *  @return EntityType indicating if it's part of the top-level component or an instance.
     */
    EntityType getEntityTypeForBus(const QSharedPointer<BusInterface>& bus) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the design widget (for accessing model and instances).
    DesignWidget* designWidget_;

    //! Handler to access library models.
    LibraryHandler* library_;

    //! Interface to show messages or warnings to the user.
    MessageMediator* messager_;

    //! Component currently edited in the design view.
    QSharedPointer<Component> currentComponent_;

    //! Set of all abstraction definitions referenced by the design.
    QSet<QSharedPointer<ConfigurableVLNVReference>> allAbsRefs_;

    //! Set of filtered abstraction references compatible with the selected bus.
    QSet<QSharedPointer<ConfigurableVLNVReference>> validAbsRefs_;

    //! Hash mapping instance names to their associated bus interfaces.
    QHash<QString, QSet<QSharedPointer<BusInterface>>> instanceBusesHash_;

    //! Hash of bus interface to the abstraction names it supports.
    QHash<QSharedPointer<BusInterface>, QSet<QString>> interfaceAbsDefsHash_;

    //! Reverse mapping: abstraction name -> set of buses that implement it.
    //QHash<QString, QSet<QSharedPointer<BusInterface>>> absToBuses;

    //! Multi-map to track all available bus interfaces by mode and entity type.
    //QMultiHash<QPair<General::InterfaceMode, EntityType>, QSharedPointer<BusInterface>> modeEntityToBuses;

    //! Lookup of all valid connection rules for each source configuration.
    QHash<ConnectionKey, QList<ConnectionRule>> connectionRules_;

    QHash<ConnectionKeyNew, QSet<General::InterfaceMode> > connectionRulesNew_;

    //! Used to store invalid connection message to display on dialog.
    mutable QString lastInvalidConnectionMessage_;
};


/*!
*  Hash function specialization for ConnectionKey, used in QHash containers.
*
*  @param [in] key   The connection key to hash.
*  @param [in] seed  Optional seed value.
*  @return Hashed value.
*/
inline uint qHash(InterconnectDataModel::EntityType key, uint seed = 0); // added by Teemu Hanhisuanto 10.7.2025

/*!
*  Hash function specialization for InterconnectType, used in QHash containers.
*
*  @param [in] key   The Interconnect Type to hash.
*  @param [in] seed  Optional seed value.
*  @return Hashed value.
*/
inline uint qHash(InterconnectDataModel::InterconnectType key, uint seed = 0); // added by Teemu Hanhisuanto 10.7.2025

/*!
*  Hash function specialization for ConnectionKey, used in QHash containers.
*
*  @param [in] key   The connection key to hash.
*  @param [in] seed  Optional seed value.
*  @return Hashed value.
*/
inline uint qHash(const InterconnectDataModel::ConnectionKey& key, uint seed = 0);

inline uint qHash(const InterconnectDataModel::ConnectionKeyNew& key, uint seed = 0);


#endif // INTERCONNECTDATAMODEL_H
