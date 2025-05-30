//-----------------------------------------------------------------------------
// File: InterconnectGeneratorDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Sampo Suokuisma
// Date: xx.xx.2025
//
// Description:
// Class for configuring generated interconnect component
//-----------------------------------------------------------------------------

#ifndef INTERCONNECTGENERATORDIALOG_H
#define INTERCONNECTGENERATORDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QString>
#include <QScrollArea>
#include <QFrame>
#include <QSet>
#include <QHash>
#include <QList>
#include <QRadioButton>

#include <common/widgets/ParameterGroupBox/parametergroupbox.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/EnumCollectionEditor/InterfaceEnumEditor.h>
#include <common/widgets/instanceWidgets/InstanceWidgets.h>
#include <editors/common/DesignWidget.h>
#include <editors/InterconnectGenerator/InterconnectAddressHelper.h>
#include <editors/InterconnectGenerator/InterconnectDataModel.h>
#include <common/graphicsItems/ComponentItem.h>

#include <KactusAPI/include/ExpressionParser.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <KactusAPI/include/ConfigJsonParser.h>
#include <KactusAPI/include/LibraryHandler.h>
#include <KactusAPI/include/InterconnectGenerator.h>
#include <KactusAPI/include/MessageMediator.h>

//-----------------------------------------------------------------------------
//! Class for configuring generated interconnect component
//-----------------------------------------------------------------------------
class InterconnectGeneratorDialog : public QDialog
{
    Q_OBJECT
public:
    /*!
     *  The constructor.
     * 
     *  @param [in] designWidget            class for showing design diagrams.
     *  @param [in] library                 Interface for accessing IP-XACT-library.
     *  @param [in] messager                Interface for outputting messages.
     *  @param [in] parent                  The parent widget.
     */
    InterconnectGeneratorDialog(DesignWidget* designWidget, LibraryHandler* library,
        MessageMediator* messager, QWidget* parent = nullptr);

    /*!
     *  The destructor.
     */
    ~InterconnectGeneratorDialog() = default;

    /*!
     *  Get config to generate interconnect.
     * 
     *  @return Struct containing interconnect configurations.
     */
    ConfigStruct* getConfig();

    /*!
     *  Get selected starting point interfaces.
     *
     *  @return QHash containing instance name as key and bus interfaces as values.
     */
    QHash<QString, QList<QSharedPointer<BusInterface>>> getSelectedStartingPoints();

    /*!
     *  Get selected endpoint interfaces.
     *
     *  @return QHash containing instance name as key and bus interfaces as values.
     */
    QHash<QString, QList<QSharedPointer<EndpointData>>> getSelectedEndpoints();

protected:

    /*!
     *  Overrides QDialog accept.
     */
    void accept() override;

private:
    
    /*!
     *  Create the top section of the configuration UI containing global settings.
     *
     *  @return Pointer to the created QWidget.
     */
    QWidget* createTopConfigSection();

    /*!
     *  Create the UI section for listing and managing starting point interfaces.
     *
     *  @return Pointer to the created QGroupBox containing starting point controls.
     */
    QGroupBox* createStartingPointsSection();

    /*!
     *  Create the UI section for listing and managing endpoint interfaces.
     *
     *  @return Pointer to the created QGroupBox containing endpoint controls.
     */
    QGroupBox* createEndpointsSection();

    /*!
     *  Create the standard button box for dialog actions (OK/Cancel).
     *
     *  @return Pointer to the created QDialogButtonBox.
     */
    QDialogButtonBox* createButtonBox();

    /*!
     *  Create a layout combining the interconnect name editor and enumeration controls.
     *
     *  @param [in] type              Type of interface being configured.
     *  @param [in] interfaceEditor   Editor for selecting bus interfaces to connect.
     *  @param [in] instanceFrame     Frame containing the instance list.
     *
     *  @return Pointer to the created vertical layout.
     */
    QVBoxLayout* createNameAndEnumerationEditorLayout(const QString& type, InterfaceEnumEditor* interfaceEditor, QFrame* instanceFrame);

    /*!
     *  Create the UI frame for selecting and managing instances of a specific type.
     *
     *  @param [in] type  Type of instance (e.g. initiator, target).
     *
     *  @return Pointer to the created QFrame.
     */
    QFrame* createInstanceEditorFrame(const QString& type);

    /*!
     *  Create a widget that allows the user to select the interconnect mode.
     *
     *  @return Pointer to the created QWidget.
     */
    QWidget* createInterconnectModeSelector();

    /*!
     *  Set up the full layout of the interconnect generator dialog.
     */
    void setUpLayout();

    /*!
     *  Populate the parameter editor with a copy of abstraction definition parameters.
     */
    void populateParameters();

    /*!
     *  Update the instance name combo box with available instance names.
     *
     *  @param [in,out]  nameCombo            Combo box to update.
     *  @param [in]      instanceType         Type of instance (starting point or endpoint).
     *  @param [in,out]  availableInstances   List of instances to populate into the combo box.
     */
    void updateNameCombo(QComboBox* nameCombo, const QString& instanceType, QStringList& availableInstances);

    /*!
     *  Handle logic triggered when a new instance is selected from the list.
     *
     *  @param [in] instanceName     Name of the selected instance.
     *  @param [in] type             Type of instance (starting point or endpoint).
     *  @param [in] interfaceEditor  Editor to update with instance-specific data.
     */
    void onInstanceSelected(const QString& instanceName, const QString& type, InterfaceEnumEditor* interfaceEditor);

    /*!
     *  Clear all entries from both the starting and endpoint lists.
     */
    void clearConnectionsLists();

    /*!
     *  Collect instance configurations for interconnect generation.
     *
     *  @param [out] config  Struct to be filled with configuration data.
     *
     *  @return True if collection was successful, false otherwise.
     */
    bool collectInstances(ConfigStruct* config);

    /*!
     *  Collect initiator interface selections from the UI and populate the config.
     *
     *  @param [in] instanceBusesLookup  Lookup table mapping instances to their bus interfaces.
     */
    void collectStartingPoints(const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup);

    /*!
     *  Collect target interface selections from the UI and populate the config.
     *
     *  @param [in] instanceBusesLookup  Lookup table mapping instances to their bus interfaces.
     */
    void collectEndpoints(const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup);

    /*!
     *  Add a new instance of the given type to the instance list.
     *
     *  @param [in] type  Type of instance to add (starting point or endpoint).
     */
    void addNewInstance(const QString& type);

    /*!
     *  Add a new starting point interface to the list.
     */
    void addNewStartingPoint();

    /*!
     *  Add a new endpoint interface to the list.
     */
    void addNewEndpoint();

    InterconnectDataModel::InterconnectType getSelectedInterconnectType() const;

    void updateInterconnectModeOptions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Reference to the design widget used to query the edited component.
    DesignWidget* designWidget_;

    //! Interface for accessing IP-XACT library models.
    LibraryHandler* library_;

    //! Message output interface for displaying user-facing notifications or errors.
    MessageMediator* messager_;

    //! Shared data model that provides abstraction and bus/interface information.
    QSharedPointer<InterconnectDataModel> dataModel_;

    //! Helper for generating and analyzing address mappings in interconnects.
    QSharedPointer<InterconnectAddressHelper> addressHelper_;

    //! Parameters specific to the generated interconnect component.
    QSharedPointer<QList<QSharedPointer<Parameter>>> interconnectParams_;

    //! Current working interconnect configuration.
    ConfigStruct* config_ = nullptr;

    //! VLNV identifier of the design currently being edited.
    QString designVLNV_;

    //! Editor widget for editing the VLNV of the interconnect.
    VLNVEditor* vlnvEditor_;

    //! Parameter editor group box for the interconnect component.
    ParameterGroupBox* parameterGroupBox_;

    //! Combo box for selecting a component instance.
    InstanceComboBox* componentInstances_;

    //! Combo box for selecting a bus interface.
    InstanceComboBox* busInterfaceCombo_;

    //! Combo box for selecting a clock interface.
    InstanceComboBox* clockCombo_;

    //! Combo box for selecting a reset interface.
    InstanceComboBox* resetCombo_;

    //! Line edit for specifying data width.
    QLineEdit* dataWidthEdit_;

    //! Checkbox to enable or disable clock signal configuration.
    QCheckBox* clockCheckBox_;

    //! Checkbox to enable or disable reset signal configuration.
    QCheckBox* resetCheckBox_;

    //! Radio button to choose channel-based interconnect generation.
    QRadioButton* channelButton_;

    //! Radio button to choose bridge-based interconnect generation.
    QRadioButton* bridgeButton_;

    //! Layout container for initiator interface selection.
    QVBoxLayout* startingPointsLayout_;

    //! Layout container for target interface selection.
    QVBoxLayout* endpointsLayout_;

    //! Previous index selected in UI (used for change tracking).
    int previousIndex_ = -1;

    //! Internal flag to suppress change-triggered logic during setup.
    bool ignoreChange_ = false;

    //! Indicates whether the interconnect is being generated as a channel.
    bool isChannel_ = false;

    //! Set of filtered abstraction references compatible with the selected bus.
    QSet<QSharedPointer<ConfigurableVLNVReference>> filteredAbsRefs_;

    //! Set of all abstraction references for the selected design.
    QSet<QSharedPointer<ConfigurableVLNVReference>> allAbsRefs_;

    //! Map of instance names to their associated bus interfaces.
    QHash<QString, QSet<QSharedPointer<BusInterface>>> instanceBusesHash_;

    //! Map of bus interfaces to abstraction definitions used by them.
    QHash<QSharedPointer<BusInterface>, QSet<QString>> interfaceAbsDefsHash_;

    //! Set of already added initiator interface keys (to avoid duplicates).
    QSet<QString> addedStartingPoints_;

    //! Set of already added target interface keys (to avoid duplicates).
    QSet<QString> addedEndpoints_;

    //! Map of selected initiator interfaces, grouped by instance name.
    QHash<QString, QList<QSharedPointer<BusInterface>>> selectedStartingPoints_;

    //! Map of selected target interfaces and related metadata, grouped by instance name.
    QHash<QString, QList<QSharedPointer<EndpointData>>> selectedEndpoints_;
};

#endif // INTERCONNECTGENERATORDIALOG_H
