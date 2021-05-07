//-----------------------------------------------------------------------------
// File: CPUSelectionDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2021
//
// Description:
// Dialog for selecting CPUs from a connectivity graph.
//-----------------------------------------------------------------------------

#ifndef CPUSELECTIONDIALOG_H
#define CPUSELECTIONDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPlainTextEdit>

#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>

#include <ConnectivityGraphUtilities.h>

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>

class Component;
class LibraryInterface;
class Cpu;

//-----------------------------------------------------------------------------
//! Dialog for selecting CPUs from a connectivity graph.
//-----------------------------------------------------------------------------
class CPUSelectionDialog : public QDialog
{
    Q_OBJECT

public:

	/*!
     *  The constructor.
     *
	 *      @param [in] topComponent    Top component of the selected design.
	 *      @param [in] library         Interface for accessing the library.
	 *      @param [in] viewsNames      Names of the available views.
     *      @param [in] fileSetNames    Names of the available file sets.
	 *      @param [in] parent          Parent widget.
	 */
	CPUSelectionDialog(QSharedPointer<Component> topComponent, LibraryInterface* library,
        QStringList const& viewNames, QStringList const& fileSetNames, QWidget *parent);

	/*!
     *  The destructor.
     */
    virtual ~CPUSelectionDialog() = default;

    /*!
     *  Get the list of the selected CPUs.
     *
     *      @return List of the CPUs and their connected paths.
     */
    QVector<ConnectivityGraphUtilities::interfaceRoutes> getSelectedCPUs();

    /*!
     *  Check if the peripherals for SVD generation are address blocks.
     *
     *      @return True, if the peripherals are address blocks, false otherwise.
     */
    bool peripheralsAreBlocks() const;

    /*!
     *  Check if the peripherals for SVD generation are memory maps.
     *
     *      @return True, if the peripherals are memory maps, false otherwise.
     */
    bool peripheralsAreMaps() const;

    /*!
     *  Check if the files should be saved to a file set.
     *
     *      @return True, if the files should be saved to a file set, false otherwise.
     */
    bool saveToFileSet() const;

    /*!
     *  Get the name of the file set to save the files to.
     *
     *      @return Name of the target file set.
     */
    QString getTargetFileSet() const;

private slots:

    /*!
     *  Handle the view selection.
     */
    void onViewChanged();

    /*!
     *  Change peripherals to be constructed from address blocks.
     *
     *      @param [in] blockPeripherals    Value for address block peripherals.
     */
    void onBlockPeripherals(bool blockPeripherals);

    /*!
     *  Change peripherals to be constructed from memory maps.
     *
     *      @param [in] mapPeripherals  Value for memory map peripherals.
     */
    void onMapPeripherals(bool mapPeripherals);

private:

    // Disable copying.
    CPUSelectionDialog(CPUSelectionDialog const& rhs);
    CPUSelectionDialog& operator=(CPUSelectionDialog const& rhs);

    //! Container for CPU, check box for the CPU and the routes starting from the CPU.
    struct CPUCheckInterface
    {
        //! Check box for the CPU.
        QCheckBox* cpuCheck_;

        //! Interface containing the CPU.
        QSharedPointer<const ConnectivityInterface> cpuInterface_;

        //! Routes from the CPU.
        QVector<QVector<QSharedPointer<const ConnectivityInterface> > > cpuRoutes_;
    };

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    /*!
     *  Setup the CPU selection layout.
     */
    void setupCPUSelection();

    /*!
     *  Check if a check box exists for the selected interface.
     *
     *      @param [in] master  The selected interface.
     *
     *      @return True, if a check box exists, false otherwise.
     */
    bool checkBoxExistsForInterface(QSharedPointer<const ConnectivityInterface> master);

    /*!
     *  Get the CPU check box container of the selected interface.
     *
     *      @param [in] master  The selected interface.
     *
     *      @return CPU check box container matching the selected interface.
     */
    QSharedPointer<CPUCheckInterface> getMatchingCheckInterface(QSharedPointer<const ConnectivityInterface> master);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! Widget for selecting view.
    QComboBox* viewSelection_;

    //! Widget for selecting target file set.
    QComboBox* fileSetSelection_;

    //! Group box for containing the file set widget.
    QGroupBox* fileSetBox_;

    //! Containers for the selected CPUs.
    QVector<QSharedPointer<CPUCheckInterface>> cpuSelection_;

    //! Layout containing the CPU check boxes.
    QVBoxLayout* cpuLayout_;

    //! Interface for accessing the library.
    LibraryInterface* library_;

    //! Top component of the design.
    QSharedPointer<Component> component_;

    //! Graph factory for creating the design connections.
    ConnectivityGraphFactory graphFactory_;

    //! Check box for creating peripherals from address blocks.
    QCheckBox* blockPeripherals_;

    //! Check box for creating peripherals from memory maps.
    QCheckBox* mapPeripherals_;
};

#endif //CPUSELECTIONDIALOG_H