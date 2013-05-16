//-----------------------------------------------------------------------------
// File: FileDependencyEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYEDITOR_H
#define FILEDEPENDENCYEDITOR_H

#include "FileDependencyGraphWidget.h"
#include "FileDependencyInfoWidget.h"
#include "FileDependencyModel.h"

#include <QGroupBox>
#include <QToolBar>
#include <QTableWidget>
#include <QProgressBar>
#include <QStringList>
#include <QActionGroup>
#include <QTimer>

class Component;
class PluginManager;
class ISourceAnalyzerPlugin;
class LibraryInterface;
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
     *      @param [in] component     The component being edited.
     *      @param [in] libInterface  The library interface.
     *      @param [in] pluginMgr     The plugin manager for accessing plugins.
     *      @param [in] parent        The parent widget.
     */
    FileDependencyEditor(QSharedPointer<Component> component,
                         LibraryInterface* libInterface,
                         PluginManager& pluginMgr, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyEditor();

signals:
    //! Emitted when a file set has been added.
    void fileSetAdded(FileSet* fileSet);

    //! Emitted when a file has been added.
    void fileAdded(File* file);

    //! Emitted when the file sets have been updated.
    void fileSetsUpdated();

    //! Emitted when the files have been updated.
    void filesUpdated();

    //! Emitted when the contents have changed.
    void contentChanged();

private slots:
    /*!
     *  Opens up the source selection dialog.
     */
    void openSourceDialog();

    /*!
     *  Scans the source directories for new files, adds them to the component
     *  file sets and runs the dependency analysis.
     */
    void scan();

    /*!
     *  Scans the source directories.
     */
    void scanDirectories();

    /*!
     *  Updates the progress bar with the given value.
     *
     *      @param [in] value The value to set to the progress bar.
     */
    void updateProgressBar(int value);

    /*!
     *  Sends filters to the dependency graph.
     *
     *      @param [in] the filter being toggled.
     */
    void filterToggle(QAction* action);

private:
    // Disable copying.
    FileDependencyEditor(FileDependencyEditor const& rhs);
    FileDependencyEditor& operator=(FileDependencyEditor const& rhs);

    /*!
     *  Resolves the correct file type for each extension specified in the settings.
     */
    void resolveExtensionFileTypes();

    /*!
     *  Scans recursively files in the given path and adds them to the component file sets.
     *
     *      @param [in] path The path to scan.
     */
    void scanFiles(QString const& path);

    /*
     *  Adds a filter button to the toolbar.
     *
     *      @param [in] icon        The icon for the button.
     *      @param [in] iconText    The text displayed for the icon.
     *      @param [in] filter      The filter value for the button.
     */
    void addFilterButton(QIcon icon, QString iconText, FileDependencyGraphView::DependencyFilter filter);
    
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

    //! The component being edited.
    QSharedPointer<Component> component_;

    //! The library interface.
    LibraryInterface* libInterface_;

    //! Extension fileset lookup.
    QMap<QString, QString> fileTypeLookup_;

    //! Ignore list for extensions.
    QStringList ignoreExtList_;

    //! The file dependency model.
    FileDependencyModel model_;

    //! The XML base path for the component.
    QString xmlPath_;

    //! If true, the editor is currently scanning.
    bool scanning_;

    //! Container for the filter button actions.
    QActionGroup filterActions_;

    QAction* runAnalysisAction_;

    QTimer* timer_;
    ScanProgressWidget* progWidget_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYEDITOR_H
