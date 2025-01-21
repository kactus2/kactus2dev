//-----------------------------------------------------------------------------
// File: ImportInstancesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.04.2019
//
// Description:
// Used to generate IP-XACT component instances from parsed source files.
//-----------------------------------------------------------------------------

#ifndef IMPORTINSTANCESEDITOR_H
#define IMPORTINSTANCESEDITOR_H

#include <wizards/ComponentWizard/InstanceData.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QTableWidget>
#include <QContextMenuEvent>
#include <QAction>
#include <QSettings>

class LibraryInterface;
class Component;
class File;
class ImportInstancesDelegate;

//-----------------------------------------------------------------------------
//! Used to generate IP-XACT component instances from parsed source files.
//-----------------------------------------------------------------------------
class ImportInstancesEditor : public QTableWidget
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] workingComponent    The importedComponent.
	 *    @param [in] libraryAccess       The instance which manages the library.
	 *    @param [in] parent              The owner of this widget.
	 */
    ImportInstancesEditor(QSharedPointer<Component> workingComponent, LibraryInterface* libraryAccess,
        QWidget* parent);

	/*!
     *  The destructor.
     */
    ~ImportInstancesEditor() = default;

    /*!
     *  Setup the imported component instances.
     *
     *    @param [in] newInstances    The new imported instances.
     */
    void setupInstances(QVector<InstanceData::instanceData> const& newInstances);

    //! No copying. No assignment.
    ImportInstancesEditor(const ImportInstancesEditor& other) = delete;
    ImportInstancesEditor& operator=(const ImportInstancesEditor& other) = delete;

public slots:

    /*!
     *  Change the status of the indexed item.
     *
     *    @param [in] index       Index of the selected item.
     *    @param [in] isEditable  New status for the selected item.
     */
    void changeItemEditableStatus(QModelIndex const& index, bool isEditable);

    /*!
     *  Check if the selected path is in a new library location.
     *
     *    @param [in] newPath     The selected path.
     */
    void onCheckForNewLibraryLocation(QString const& newPath);

    /*!
     *  Handle the tooltip change in the selected item.
     *
     *    @param [in] index       Index of the selected item.
     *    @param [in] newText     New text for the selected item.
     */
    void onChangeIndexItemTooltip(QModelIndex const& index, QString const& newText);

protected:

    /*!
     *  The context menu handler.
     *
     *    @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:

    /*!
     *  Handle the browsing of an imported file.
     */
    void onBrowseImportFile();

    /*!
     *  Handle the browsing of an imported XML file.
     */
    void onBrowseXMLFile();

private:
	
    /*!
     *  Create a table item for the instance name.
     *
     *    @param [in] instanceName    Name of the instance.
     *    @param [in] instanceRow     Row of the instance.
     */
    void createInstanceNameItem(QString const& instanceName, int const& instanceRow);

    /*!
     *  Create table items for the import files of the instances.
     *
     *    @param [in] instances   List of imported instances.
     */
    void createInstancePathItems(QVector<InstanceData::instanceData> const& instances);

    /*!
     *  Get the possible file paths for the imported instances.
     *
     *    @param [in] instances   List of imported instances.
     *
     *    @return List of instance name file path pairs.
     */
    QMultiMap<QString, QString> getPossibleFilePaths(QVector<InstanceData::instanceData> const& instances) const;

    /*!
     *  Get the importable files of the selected component.
     *
     *    @param [in] component   The selected component.
     *
     *    @return List importable component files.
     */
    QVector<QSharedPointer<File> > getCompatibleComponentFiles(QSharedPointer<Component> component) const;

    /*!
     *  Read the selected file.
     *
     *    @param [in] relativePath    Relative path of the selected file.
     *    @param [in] basePath        File path to the containing component.
     *
     *    @return The contents of the selected file.
     */
    QString readInputFile(QString const& relativePath, QString const& basePath) const;

    /*!
     *  Check if the selected file is a verilog file.
     *
     *    @param [in] file    The selected file.
     *
     *    @return True, if the selected file is verilog, false otherwise.
     */
    bool fileIsVerilogFile(QSharedPointer<File> file) const;

    /*!
     *  Create VLNV items for the selected component instances.
     *
     *    @param [in] instances   List of the selected component instances.
     */
    void createInstanceVLNVItems(QVector<InstanceData::instanceData> const& instances);

    /*!
     *  Check if the selected instance data matches ports of the selected component.
     *
     *    @param [in] instance    The selected instance data.
     *    @param [in] component   The compared component.
     *
     *    @return True, if the instance data matches component ports, false otherwise.
     */
    bool instanceDataMatchesComponentPorts(InstanceData::instanceData const& instance,
        QSharedPointer<const Component> component) const;

    /*!
     *  Get the component from the selected VLNV.
     *
     *    @param [in] componentVLNV   The selected VLNV.
     *
     *    @return Component matching the selected VLNV.
     */
    QSharedPointer<const Component> getComponentFromVLNV(VLNV const& componentVLNV) const;

    /*!
     *  Set the active library locations.
     *
     *    @param [in] settings    The settings.
     */
    void setActiveLibraryLocations(QSettings const& settings);

    /*!
     *  Check if the selected path exists in the active libraries.
     *
     *    @param [in] path    The selected path.
     *
     *    @return True, if the path exists in the active libraries, false otherwise.
     */
    bool pathExistsInActiveLibraries(QString const& path) const;

    /*!
     *  Get the path of the currently selected item.
     *
     *    @return The path of the currently selected item.
     */
    QString getCurrentPath() const;

    /*!
     *  Set a new path for the currently selected item.
     *
     *    @param [in] targetPath  The new path.
     */
    void setTargetPath(QString const& targetPath);

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

    //! The imported component.
    QSharedPointer<Component> workingComponent_;

    //! Default library path.
    QString defaultPath_;

    //! Delegate for the editors.
    ImportInstancesDelegate* instanceDelegate_;

    //! Interface to the library.
    LibraryInterface* libraryAccess_;

    //! Action for browsing XML files.
    QAction browseXMLFileAction_;

    //! Action for browsing import files.
    QAction browseImportFileAction_;

    //! The currently selected item.
    QTableWidgetItem* selectedItem_;

    //! Active library locations.
    QStringList libraryLocations_;
};

#endif // IMPORTINSTANCESEDITOR_H
