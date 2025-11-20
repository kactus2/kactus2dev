//-----------------------------------------------------------------------------
// File: InterconnectGeneratorDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Sampo Suokuisma
// Date: 05.06.2025
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

class ExpressionEditor;
class InstanceInterfacesEditor;

namespace InterconnectGeneration
{

    //-----------------------------------------------------------------------------
    //! Class for configuring generated interconnect component
    //-----------------------------------------------------------------------------
    class Dialog : public QDialog
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
        Dialog(DesignWidget* designWidget, LibraryHandler* library,
            MessageMediator* messager, QWidget* parent = nullptr);

        /*!
         *  The destructor.
         */
        ~Dialog();

        /*!
         *  Get config to generate interconnect.
         *
         *  @return Struct containing interconnect configurations.
         */
        ConfigStruct* getConfig() const;

        /*!
         *  Get selected starting point interfaces.
         *
         *  @return QHash containing instance name as key and bus interfaces as values.
         */
        QHash<QString, QList<QSharedPointer<BusInterface>>> getSelectedStartingPoints() const;

        /*!
         *  Get selected endpoint interfaces.
         *
         *  @return QHash containing instance name as key and bus interfaces as values.
         */
        QHash<QString, QList<QSharedPointer<EndpointData>>> getSelectedEndpoints() const;

        /*!
         *	Checks if RTL generation is enabled or not.
         *
         *    @return True, if generator should generate RTL, otherwise false.
         */
        bool rtlGenerationSelected();

    protected:

        /*!
         *  Overrides QDialog accept.
         */
        void accept() override;

    private slots:

        /*!
         *	Handle change in selected abstraction definition (abstraction type) for interconnect component.
         *
         *    @param [in] currentIndex     Index of currently selected abstraction definition
         */
        void onAbsDefChanged(int currentIndex);

        /*!
         *  Handle logic triggered when a new instance is selected from the list.
         *
         *  @param [in] newInstanceName     Name of the newly selected instance.
         *  @param [in] oldInstanceName     Name of the previously selected instance.
         */
        void onInstanceSelected(QString const& newInstanceName, QString const& oldInstanceName);

        /*!
         *	Handle removing instance and its selected interfaces.
         */
        void onInstanceRemoved();

        /*!
         *	Handler for adding a new instance.
         */
        void addNewInstance();

        /*!
         *	Handle toggling of interconnect mode selector. Called for each radio button in selector group box.
         *
         *    @param [in] checked     Flag indicating if the calling radio button is checked or not.
         */
        void onInterconnectSelectorToggled(bool checked);

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
        QGroupBox* createInstancesSection();

        /*!
         *  Create the standard button box for dialog actions (OK/Cancel).
         *
         *  @return Pointer to the created QDialogButtonBox.
         */
        QDialogButtonBox* createButtonBox();

        /*!
         *  Create a widget that allows the user to select the interconnect mode.
         *
         *  @return Pointer to the created QWidget.
         */
        QWidget* createInterconnectModeSelector();

        /*!
         *  Creates the checkbox which is used to enable RTL generation.
         *
         *  @return Pointer to the created QWidget.
         */
        QWidget* createGenerateRtlCheckbox();

        /*!
         *	Checks the selected abstraction definition for RTL generation compatibility. Compatible VLNVs are
         *  defined in rtlCompatibleAbsDefs_
         *
         *    @return True, if RTL can be generated, otherwise false.
         */
        bool rtlCanBeGenerated();

        /*!
         *  Set up the full layout of the interconnect generator dialog.
         */
        void setUpLayout();

        /*!
         *  Populate the parameter editor with a copy of abstraction definition parameters.
         */
        void populateParameters();

        /*!
         *  Clear all entries from both the starting and endpoint lists.
         */
        void clearInstanceSelections();

        /*!
         *  Collect instance configurations for interconnect generation.
         */
        void collectSelectedInterfaces();

        /*!
         *  Get the currently selected interconnect type.
         *
         *  @return The selected interconnect type (Bridge or Channel).
         */
        InterconnectGeneration::InterconnectType getSelectedInterconnectType() const;

        void filterAvailableInstancesByAbsDef();

        /*!
         *	Update the available instances for all instance interface editors
         */
        void updateAvailableInstances();

        /*!
         *	Set the available interfaces for selected instance interface editor.
         *
         *    @param [in] interfacesEditor     The selected editor.
         *    @param [in] selectedInstance     The currently selected instance whose interfaces will be set.
         */
        void updateAvailableInstanceInterfaces(InstanceInterfacesEditor* interfacesEditor, QString const& selectedInstance);

        /*!
         *	Disable or enable add instances button based on available instances.
         */
        void updateAddInstanceButtonStatus();

        /*!
         *	Checks if any instances have been added.
         *
         *    @return True, if at least one instance has been added. Otherwise false.
         */
        bool hasSelectedInstances();

        /*!
         *	Get the allowed interface modes that can be selected for a given instance.
         *
         *    @param [in] selectedInstance     The selected instance.
         *
         *    @return A set of allowed interface modes.
         */
        QSet<General::InterfaceMode> getAllowedInterfaceModes(QString const& selectedInstance);

        /*!
         *	Checks if gathered interface selections satisfy the requirements for generating an interconnect component.
         *
         *    @param [in/out] errorMsg     Error message to create if selections are invalid.
         *
         *    @return True, if the selections are valid, otherwise false.
         */
        bool interfaceSelectionsAreValid(QString& errorMsg);

        /*!
         *	Checks if the given instance is the top component of the open design.
         *
         *    @param [in] instanceName     Name of the instance to check.
         *
         *    @return  True, if the instance is top component, otherwise false.
         */
        bool instanceIsTopComponent(QString const& instanceName);

        /*!
         *	Find parameters that define address and data width. Finds ports in abs def with address and data qualifers and gets their widths.
         *  If width is parameterized, addr/data width is set as that parameter. Otherwise hard-coded parameter is created with found width.
         *  If no widths are found, a default value of ????? (32 or non-numeric?) is used.
         */
        void findWidthParameters();

        ConfigurableVLNVReference* getCurrentlySelectedAbsDef();

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

        //! TODO delete if addressing auto population isn't implemented
        //! Helper for generating and analyzing address mappings in interconnects.
        //QSharedPointer<InterconnectAddressHelper> addressHelper_;

        //! Parameters specific to the generated interconnect component.
        QSharedPointer<QList<QSharedPointer<Parameter>>> interconnectParams_;

        QSharedPointer<ExpressionParser> expressionParser_{ new SystemVerilogExpressionParser() };

        //! Formatter for expressions.
        QSharedPointer<ExpressionFormatter> expressionFormatter_;

        //! Current working interconnect configuration.
        ConfigStruct* config_ = nullptr;

        //! Nested hash lookup for quick bus interface resolution.
        QHash<QString, QHash<QString, QSharedPointer<BusInterface>>> busLookUp_;

        //! VLNV identifier of the design currently being edited.
        QString designVLNV_;

        //! Editor widget for editing the VLNV of the interconnect.
        VLNVEditor* vlnvEditor_;

        //! Parameter editor group box for the interconnect component.
        ParameterGroupBox* parameterGroupBox_;

        //! Combo box for selecting a bus interface.
        InstanceComboBox* absDefCombo_;

        //! Combo box for selecting a clock interface.
        InstanceComboBox* clockCombo_;

        //! Combo box for selecting a reset interface.
        InstanceComboBox* resetCombo_;

        //! Checkbox to enable or disable clock signal configuration.
        QCheckBox* clockCheckBox_;

        //! Checkbox to enable or disable reset signal configuration.
        QCheckBox* resetCheckBox_;

        //! Checkbox to select if RTL code should be generated if compatible abs def is selected.
        QCheckBox* generateRTLCheckBox_;

        //! Radio button to choose channel-based interconnect generation.
        QRadioButton* channelButton_;

        //! Radio button to choose bridge-based interconnect generation.
        QRadioButton* bridgeButton_;

        //! Layout container for instance and interface selection.
        QVBoxLayout* instancesLayout_;

        //! Button for adding new instances to select interfaces from.
        QPushButton* addInstanceButton_ = new QPushButton(QIcon(":/icons/common/graphics/add.png"), tr("Add New Instance"));

        //! Previous index selected in UI (used for change tracking).
        int previousAbsDefIndex_ = -1;

        //! Internal flag to suppress change-triggered logic during setup.
        bool ignoreChange_ = false;

        //! Indicates whether the interconnect is being generated as a channel.
        bool isChannel_ = false;

        //! Set containing names of available instances to select.
        QSet<QString> availableInstances_;

        //! Set of filtered abstraction references compatible with the selected bus.
        QSet<QSharedPointer<ConfigurableVLNVReference>> filteredAbsRefs_;

        //! Set of all abstraction references for the selected design.
        QSet<QSharedPointer<ConfigurableVLNVReference>> allAbsRefs_;

        //! Map of instance names to their associated bus interfaces.
        QHash<QString, QSet<QSharedPointer<BusInterface>>> instanceBusesHash_;

        //! Map of bus interfaces to abstraction definitions used by them.
        QHash<QSharedPointer<BusInterface>, QSet<QString>> interfaceAbsDefsHash_;

        //! Parameter finder for editors needing access to interconnect component parameters.
        QSharedPointer<ParameterFinder> parameterFinder_;

        //! Parameter completer model for expression editors in interface selection editors.
        ComponentParameterModel* parameterCompleterModel_;

        //! Map of selected initiator interfaces, grouped by instance name.
        QHash<QString, QList<QSharedPointer<BusInterface>>> selectedStartingPoints_;

        //! Map of selected target interfaces and related metadata, grouped by instance name.
        QHash<QString, QList<QSharedPointer<EndpointData>>> selectedEndpoints_;

        //! Set containing all editors for added instances
        QSet<InstanceInterfacesEditor*> instanceEditors_;

        QString addrWidthParamName_;
        QString dataWidthParamName_;

        QString addrWidthValue_;
        QString dataWidthValue_;

        //! RTL generation -compatible VLNV bases (version doesn't matter)
        const QStringList rtlCompatibleAbsDefs_ = {
            "tuni.fi:interface:AXI4.absDef:version",
            "tuni.fi:interface:AXI4LITE.absDef:version",
            "tuni.fi:interface:OBI.absDef:version",
        };
    };
}

#endif // INTERCONNECTGENERATORDIALOG_H
