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

#include <Plugins/common/ConnectivityGraphUtilities.h>

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>

class Component;
class LibraryInterface;
class Cpu;
class CPUEditor;
class CpuRoutesContainer;

#include <QJsonDocument>

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
	 *      @param [in] topComponent            Top component of the selected design.
	 *      @param [in] library                 Interface for accessing the library.
	 *      @param [in] viewsNames              Names of the available views.
     *      @param [in] fileSetNames            Names of the available file sets.
     *      @param [in] cpuEditor               Editor for CPU details.
     *      @param [in] dialogType              The type text for dialog.
     *      @param [in] extraEditor             An extra editor.
	 *      @param [in] parent                  Parent widget.
     *      @param [in] configurationFolder     Folder path for the generator.
     *      @param [in] saveToFileSet           Flag for save to file set.
     *      @param [in] configurationFileSet    Name of the target file set.
     *      @param [in] configurationView       Name of the view to use in the generator.
     */
	CPUSelectionDialog(QSharedPointer<Component> topComponent,
        LibraryInterface* library,
        QStringList const& viewNames,
        QStringList const& fileSetNames,
        CPUEditor* cpuEditor,
        QString const& dialogType,
        QWidget* extraEditor,
        QWidget *parent,
        QString const& configurationFolder = "",
        bool saveToFileSet = true,
        QString const& configurationFileSet = "",
        QString const& configurationView = "");

	/*!
     *  The destructor.
     */
    virtual ~CPUSelectionDialog() = default;

    /*!
     *  Get the list of the selected CPUs.
     *
     *      @return List of the CPU data and their connected paths.
     */
    QVector<QSharedPointer<CpuRoutesContainer> > getSelectedCPUs();

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

    /*!
     *  Get the active view.
     *
     *      @return Name of the active view.
     */
    QString getView() const;

private slots:

    /*!
     *  Handle the view selection.
     */
    void onViewChanged();

    /*!
     *  Handle the change of target folder.
     */
    void onChangeTargetFolder();

signals:

    /*!
     *  Signal informing of a change in the destination path.
     *
     *      @param [in] newFolderLocation   The new destination.
     */
    void changeInSelectedPath(QString const& newFolderLocation);

private:

    // Disable copying.
    CPUSelectionDialog(CPUSelectionDialog const& rhs);
    CPUSelectionDialog& operator=(CPUSelectionDialog const& rhs);

    /*!
     *  Setup the dialog.
     *
     *      @param [in] configurationFolder     Folder path for the generated files.
     *      @param [in] saveToFileSet           Flag for saving to a file set.
     *      @param [in] configurationFileSet    Name of the selected file set.
     *      @param [in] configurationView       The currently active view.
     */
    void setupConfiguration(QString const& configurationFolder, bool saveToFileSet, QString const& configurationFileSet, QString const& configurationView);

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

    //! Interface for accessing the library.
    LibraryInterface* library_;

    //! Top component of the design.
    QSharedPointer<Component> component_;

    //! Graph factory for creating the design connections.
    ConnectivityGraphFactory graphFactory_;

    //! Editor for CPU details.
    CPUEditor* cpuDetailEditor_;

    //! Editor for folder path.
    QLineEdit* folderLine_;

    //! An extra editor.
    QWidget* extraEditor_;
};

#endif //CPUSELECTIONDIALOG_H