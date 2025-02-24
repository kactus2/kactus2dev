//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.05.2018
//
// Description:
// Dialog for setting linux device tree generation options.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREEDIALOG_H
#define LINUXDEVICETREEDIALOG_H

class FileSet;
class View;
class VLNV;
class Component;
class Design;
class LinuxDeviceTreeCPUEditor;

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>

#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCPUDetails.h>

#include <QDialog>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QSharedPointer>
#include <QCheckBox>

class LinuxDeviceTreeCpuRoutesContainer;

//-----------------------------------------------------------------------------
//! Dialog for setting linux device tree generation options.
//-----------------------------------------------------------------------------
class LinuxDeviceTreeDialog: public QDialog
{
    Q_OBJECT
public:
    
    /*!
     *	The constructor.
     *
     *		@param [in]	defaultPath     The default path for the Linux Device Tree file.
     *		@param [in]	component       The top component of the selected design.
     *		@param [in]	design          The selected design.
     *    @param [in] library         The library interface.
     *		@param [in]	parent          Parent item for the dialog.
     */
    LinuxDeviceTreeDialog(QString const& defaultPath, QSharedPointer<Component> component,
        QSharedPointer<Design> design, LibraryInterface* library, QWidget* parent);

	/*!
     *  The destructor.
     */
    virtual ~LinuxDeviceTreeDialog();

    /*!
     *	Get the output path for the device tree file.
     *
     *		@return	The output path of the device tree file.
     */
    QString getOutputPath() const;

    /*!
     *  Get the top component view from which the device tree file is constructed from.
     *
     *    @return	The name of the selected view.
     */
    QString getSelectedView() const;

    /*!
     *  Check if the file should be saved to a file set.
     *
     *    @return	True, if the file should be saved to a file set, false otherwise.
     */
    bool saveFileToFileSet() const;

    /*!
     *  Get the name of the file set where to save the file to.
     *
     *    @return	The name of the selected file set.
     */
    QString getTargetFileSet() const;

    /*!
     *  Check if address blocks should be written.
     *
     *    @return True, if address blocks should be written, false otherwise.
     */
    bool allowAddressBlocks() const;

    /*!
     *  Get the list of the selected CPU containers.
     *
     *    @return List of the selected CPU containers.
     */
    QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > getAcceptedContainers() const;

public slots:

    /*!
     *	Handles the accepting of the dialog.
     */
    void accept();

private slots:

    /*!
     *	Handles the target file browsing.
     */
    void onBrowse();

    /*!
     *  Setup the CPU editor.
     *
     *    @param [in] view    Name of the active view.
     */
    void setupCPUEditor(QString const& view);

private:

	// Disable copying. Disable assignment.
    LinuxDeviceTreeDialog(LinuxDeviceTreeDialog const& rhs);
    LinuxDeviceTreeDialog& operator=(LinuxDeviceTreeDialog const& rhs);

    /*!
     *	Setup the view selector for the selected component. Set the current view according to the selected design.
     *
     *		@param [in]	component   The selected component.
     *		@param [in]	design      The selected design.
     */
    void setupViewSelector(QSharedPointer<Component> component, QSharedPointer<Design> design);

    /*!
     *	Setup the file set selector for the selected component.
     *
     *		@param [in]	componentFileSets   File sets of the selected component.
     */
    void setupFileSetSelector(QSharedPointer<QList<QSharedPointer<FileSet> > > componentFileSets);

    /*!
     *  Get the name of the design referenced by the currently active view.
     *
     *
     *    @return 
     */
    QString getDesignName();

    /*!
     *	Setup the dialog layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* library_;

    //! The component from which the linux device tree is created from.
    QSharedPointer<Component> topComponent_;

    //! Selects the target view.
    QComboBox* viewSelector_;

    //! Groups the desired items for file set selection.
    QGroupBox* fileSetGroup_;

    //! Selects the target file set.
    QComboBox* fileSetSelector_;

    //! Editor for selecting the path for the device tree file.
    QLineEdit* fileEditor_;

    //! Check box for writing address blocks.
    QCheckBox* writeBlocks_;

    //! Editor for CPU details.
    LinuxDeviceTreeCPUEditor* cpuEditor_;

    //! Factory for creating connectivity graphs.
    ConnectivityGraphFactory graphFactory_;

    //! Master slave search algorithm for connectivity graphs.
    MasterSlavePathSearch searchAlgorithm_;
};

#endif // LINUXDEVICETREEDIALOG_H
