//-----------------------------------------------------------------------------
// File: RenodeCpuEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.03.2023
//
// Description:
// Editor for CPU details in renode generation.
//-----------------------------------------------------------------------------

#ifndef RENODECPUEDITOR_H
#define RENODECPUEDITOR_H

#include <QWidget>
#include <QObject>
#include <QTableView>

#include <Plugins/common/ConnectivityGraphUtilities.h>
#include <Plugins/common/CPUDialog/CPUEditor.h>

class CpuRoutesContainer;
class LibraryInterface;
class Component;
class RenodeCpuRoutesContainer;
class RenodePeripheralsEditor;
class RenodeMemoriesEditor;
class IPluginUtility;

#include <QComboBox>
#include <QLineEdit>
#include <QJsonObject>

//-----------------------------------------------------------------------------
//! Editor for CPU details in renode generation.
//-----------------------------------------------------------------------------
class RenodeCpuEditor : public CPUEditor
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] utility                 Utility for plugins.
     *      @param [in] configurationObject     JSON object containing the editor configuration.
     *      @param [in] parent                  Pointer to the owner of this widget.
     */
    RenodeCpuEditor(IPluginUtility* utility, QJsonObject const& configurationObject, QWidget *parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~RenodeCpuEditor() = default;

    /*!
     *  Setup the selected CPUs.
     *
     *      @param [in] library     Interface for accessing the library.
     *      @param [in] component   Top component of the selected design.
     *      @param [in] activeView  Active view of the design.
     */
    virtual void setupCPUDetails(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView) override final;

    /*!
     *  Get the selected CPUs.
     *
     *      @return List of the selected CPUs.
     */
    virtual QVector<QSharedPointer<CpuRoutesContainer> > getSelectedCPUs() const override final;

    /*!
     *  Get the name of the currently active CPU.
     *
     *      @return Name of the currently active CPU.
     */
    QString getSelectedCpuName();

    //! No copying. No assignment.
    RenodeCpuEditor(const RenodeCpuEditor& other) = delete;
    RenodeCpuEditor& operator=(const RenodeCpuEditor& other) = delete;

public slots:

	/*!
	 *  Setup the selected folder path.
	 *	
	 *      @param [in] newFolderPath   The new folder path.
	 */
	virtual void setupFolderPath(QString const& newFolderPath) override final;

signals:

    /*!
     *  Inform of a change in CPU.
     *
     *      @param [in] newCpuName  Name of the new CPU.
     */
    void cpuChanged(QString const& newCpuName);

private slots:

    /*!
     *  Handle the change in CPU selection.
     *
     *      @param [in] newCPU  Name of the new CPU.
     */
    void onHandleCpuChange(QString const& newCPU);

    /*!
     *  Handle the changes in the CPU class.
     *
     *      @param [in] newClass    The new class.
     */
    void onHandleClassChange(QString const& newClass);

    /*!
     *  Handle the change in cpu type.
     *
     *      @param [in] newType     The new type.
     */
    void onHandleTypeChange(QString const& newType);

    /*!
     *  Handle the change in time provider.
     *
     *      @param [in] newTimeProvider     The new time provider.
     */
    void onHandleTimeProviderChange(QString const& newTimeProvider);

private:

    /*!
     *  Check the starting routes for errors.
     */
    void checkStartingInterfacesForErrors();

    /*!
     *  Set up the currently active CPU container.
     */
    void setupActiveCpuContainer();

    /*!
     *  Set up the editors for the currently active CPU.
     */
    void setupEditorsForCpu();

    /*!
     *  Setup the items for the CPU class selector.
     */
    void setupCpuClassEditor();

    /*!
     *  Get the CPU container for the selected CPU.
     *
     *      @param [in] cpuText     Identifier of the selected CPU.
     *
     *      @return CPU container for the selected CPU.
     */
    QSharedPointer<RenodeCpuRoutesContainer> getContainerForCpu(QString const& cpuText) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin utility interface.
    IPluginUtility* utility_;

    //! List of available cpu containers.
    QVector<QSharedPointer<RenodeCpuRoutesContainer> > availableCpuContainers_;

    //! The selected CPU (work in progress, should be multiple cpus).
    QSharedPointer<RenodeCpuRoutesContainer> renodeCPU_ = nullptr;

    //! Editor for peripherals.
    RenodePeripheralsEditor* peripheralEditor_;

    //! Editor for memory.
    RenodeMemoriesEditor* memoryEditor_;

    //! Selector for the currently active CPU.
    QComboBox* cpuSelector_;

    //! Editor for CPU class.
    QComboBox* cpuClassCombo_;

    //! Editor for CPU type.
    QLineEdit* cpuTypeEditor_;

    //! Editor for CPU time provider.
    QLineEdit* cpuTimeProviderEditor_;

    //! JSON object containing the editor configuration.
    QJsonObject configurationObject_;
};

#endif // RENODECPUEDITOR_H
