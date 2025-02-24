//-----------------------------------------------------------------------------
// File: FileDependencyEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 19.01.2013
//
// Description:
// File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYEDITOR_H
#define FILEDEPENDENCYEDITOR_H

#include "FileDependencyGraphWidget.h"
#include "FileDependencyInfoWidget.h"
#include "FileDependencySourceEditor.h"
#include "FileDependencyModel.h"

#include <QGroupBox>
#include <QToolBar>
#include <QTableWidget>
#include <QProgressBar>
#include <QStringList>
#include <QActionGroup>
#include <QTimer>

class Component;
class ISourceAnalyzerPlugin;
class FileSet;
class File;
class ScanProgressWidget;

//-----------------------------------------------------------------------------
//! File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------
class FileDependencyEditor : public QWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *    @param [in] component     The component being edited.
     *    @param [in] basePath      The component's base path.
     *    @param [in] parent        The parent widget.
     */
    FileDependencyEditor(QSharedPointer<Component> component, QString const& basePath,
         QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyEditor();

    /*!
     *  Sets the editor in compact mode.
     *
     *    @param [in] compact If true, the editor is set compact with only the table-graph in view.
     *                          If false, the full editor is shown.
     */
    void setCompact(bool compact);

    /*!
     *  Sets the dependencies editable/read-only.
     */
    void setDependenciesEditable(bool editable);

    /*!
     *  Returns true if the editor is scanning.
     */
    bool isScanning() const;

public slots:
    /*!
     *  Refreshes the view.
     */
    void refresh();

    /*!
     *  Scans the source directories for new files, adds them to the component
     *  file sets and runs the dependency analysis.
     */
    void scan();

signals:
    //! Emitted when a file set has been added.
    void fileSetAdded(FileSet* fileSet);

    //! Emitted when a file has been added.
    void fileAdded(File* file);

    //! Emitted when the files have been updated.
    void filesUpdated();

    //! Emitted when the dependencies have changed.
    void dependenciesChanged();

    //! Emitted when the contents have changed.
    void contentChanged();

    //! Emitted when the scan has started.
    void scanStarted();

    //! Emitted when the scan has completed.
    void scanCompleted();

private slots:

    /*!
     *  Scans the source directories.
     */
    void scanDirectories();

    /*!
     *  Updates the progress bar with the given value.
     *
     *    @param [in] value The value to set to the progress bar.
     */
    void updateProgressBar(int value);

    /*!
     *  Sends filters to the dependency graph.
     *
     *    @param [in] the filter being toggled.
     */
    void filterToggle(QAction* action);

    //! Updated the source directories for analysis.
    void onSourceDirectoriesChanged();

private:
    // Disable copying.
    FileDependencyEditor(FileDependencyEditor const& rhs);
    FileDependencyEditor& operator=(FileDependencyEditor const& rhs);

    /*!
     *  Resolves the correct file type for each extension specified in the settings.
     */
    void resolveExtensionFileTypes();
    
    /*!
     *  Finishes the scan.
     */
    void finishScan();
    
    /*!
     *  Scans recursively files in the given path and adds them to the component file sets.
     *
     *    @param [in] path The path to scan.
     */
    void scanFiles(QString const& path);

    /*!
     *  Returns true if the given file is IP-XACT.
     *
     *    @param [in] filename The name of the file to check.
     */
    bool isFileIPXact(QString const& filename) const;

    //! Setups the toolbar for file dependency analysis.
    void setupToolbar();
    
    /*
     *  Adds a filter button to the toolbar.
     *
     *    @param [in] icon        The icon for the button.
     *    @param [in] iconText    The text displayed for the icon.
     *    @param [in] filter      The filter value for the button.
     */
    void addFilterButton(QIcon const& icon, QString const& iconText, FileDependencyGraphView::DependencyFilter filter);

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Toolbar.
    QToolBar toolbar_;

    //! The progress bar.
    QProgressBar progressBar_;

    //! The dependency graph widget.
    FileDependencyGraphWidget graphWidget_;

    //! The dependency info widget.
    FileDependencyInfoWidget infoWidget_;

    FileDependencySourceEditor directoryEditor_;

    //! The component being edited.
    QSharedPointer<Component> component_;

    //! Extension fileset lookup.
    QMultiMap<QString, QString> fileTypeLookup_;

    //! Ignore list for extensions.
    QStringList ignoreExtList_;

    //! The file dependency model.
    FileDependencyModel model_;

    //! The XML base path for the component.
    QString basePath_;

    //! If true, the editor is currently scanning.
    bool scanning_;

    //! Container for the filter button actions.
    QActionGroup filterActions_;

    //! Action to start the analysis.
    QAction* runAnalysisAction_;

    //! Timer for file scanning.
    QTimer* timer_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYEDITOR_H
