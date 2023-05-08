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

#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

class CPUDetailRoutes;
class LibraryInterface;
class Component;
class RenodeCPUDetailRoutes;
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
    virtual QVector<QSharedPointer<CPUDetailRoutes> > getSelectedCPUs() const override final;

    /*!
     *  Setup the folder path.
     *
     *      @param [in] newFolderPath   The new folder path.
     */
    virtual void setupFolderPath(QString const& newFolderPath) override final;

    //! No copying. No assignment.
    RenodeCpuEditor(const RenodeCpuEditor& other) = delete;
    RenodeCpuEditor& operator=(const RenodeCpuEditor& other) = delete;

private slots:

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
    void checkStartingInterfacesForErrores();

    /*!
     *  Setup the items for the CPU class selector.
     */
    void setupCpuClassEditor();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin utility interface.
    IPluginUtility* utility_;

    //! The selected CPU (work in progress, should be multiple cpus).
    QSharedPointer<RenodeCPUDetailRoutes> renodeCPU_;

    //! Editor for peripherals.
    RenodePeripheralsEditor* peripheralEditor_;

    //! Editor for memory.
    RenodeMemoriesEditor* memoryEditor_;

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
