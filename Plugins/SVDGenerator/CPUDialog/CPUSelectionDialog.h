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
#include <QLineEdit>

#include <Plugins/SVDGenerator/ConnectivityGraphUtilities.h>

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>

class Component;
class LibraryInterface;
class Cpu;
class SVDCPUEditor;

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
     *      @return List of the CPU data and their connected paths.
     */
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > getSelectedCPUs();

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

    /*!
     *  Get the target folder for the SVD files.
     *
     *      @return Path to the target folder of the selected SVD files.
     */
    QString getTargetFolder() const;

private slots:

    /*!
     *  Handle the view selection.
     */
    void onViewChanged();

    /*!
     *  Handle the change of target folder.
     */
    void onChangeTargetFolder();

private:

    // Disable copying.
    CPUSelectionDialog(CPUSelectionDialog const& rhs);
    CPUSelectionDialog& operator=(CPUSelectionDialog const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    /*!
     *  Setup the CPU selection layout.
     */
    void setupCPUSelection();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! Widget for selecting view.
    QComboBox* viewSelection_;

    //! Widget for selecting target file set.
    QComboBox* fileSetSelection_;

    //! Group box for containing the file set widget.
    QGroupBox* fileSetBox_;

    //! Containers for data of the selected CPUs.
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > cpuSelection_;

    //! Interface for accessing the library.
    LibraryInterface* library_;

    //! Top component of the design.
    QSharedPointer<Component> component_;

    //! Graph factory for creating the design connections.
    ConnectivityGraphFactory graphFactory_;

    //! Editor for CPU details.
    SVDCPUEditor* cpuDetailEditor_;

    //! Editor for folder path.
    QLineEdit* folderLine_;
};

#endif //CPUSELECTIONDIALOG_H