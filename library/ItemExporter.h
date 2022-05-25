//-----------------------------------------------------------------------------
// File: ItemExporter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.03.2019
//
// Description:
// Exports the selected library items.
//-----------------------------------------------------------------------------

#ifndef ITEMEXPORTER_H
#define ITEMEXPORTER_H

#include "ObjectSelectionListItem.h"

#include <IPXACTmodels/common/VLNV.h>

#include <QDir>
#include <QFileInfo>
#include <QObject>
#include <QString>
#include <QWidget>

using FileList = QVector<QFileInfo>;

class LibraryInterface;
class Component;
class File;
class FileSet;

//-----------------------------------------------------------------------------
//! Exports the selected library items.
//-----------------------------------------------------------------------------
class ItemExporter : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] libraryAccess       Access to the library.
     *      @param [in] dialogParentWidget  Parent widget for the export dialog.
     *      @param [in] parent              The parent object.
     */
    ItemExporter(LibraryInterface* libraryAccess, 
        QWidget* dialogParentWidget, QObject* parent = nullptr);

    /*!
     *  The destructor.
     */
    virtual ~ItemExporter() = default;

    //! No copying. No assignment.
    ItemExporter(const ItemExporter &other) = delete;
    ItemExporter &operator=(const ItemExporter &other) = delete;

public slots:

    /*!
     *  Export a library item and it's subitems to a new location.
     *
     *      @param [in] vlnv    VLNV tag that identifies the top item to export.
     */
    void onExportItem(VLNV const& vlnv);

    /*!
     *  Export a group of items to a new location
     *
     *      @param [in] vlnvs   QList containing the VLNVs that are to be exported.
     */
    void onExportItems(const QList<VLNV> vlnvs);

signals:

    //! Emit a notice message to be printed to user.
    void noticeMessage(const QString& message);

    //! Emit an error message to be printed to user.
    void errorMessage(const QString& message);

private:

    //! Struct for collecting document statistics e.g. in export.
    struct DocumentStatistics
    {
        int fileCount = 0;
        int documentCount = 0;
    };

    struct InputSelection
    {
        bool yesToAll = false;
        bool noToAll = false;
    };

    /*!
     *  Export the selected objects.
     *
     *      @param [in] exportedItems       The selected items to be exported.
     *      @param [in] destinationPath     The export destination path.
     *
     *      @return The count of the exported items in a pair of 1. VLNVs and 2. files.
     */
    DocumentStatistics exportSelectedObjects(QVector<ObjectSelectionListItem*> const& exportedItems,
        QString const& destinationPath);

    /*!
     *  Get the selected exported files of the current component.
     *
     *      @param [in] componentIndex  Index of the selected component.
     *      @param [in] exportedItems   List of all the selected export items.
     *
     *      @return List of the exported files of the current component.
     */
    QVector<ObjectSelectionListItem const*> getExportedFilesForComponent(int const& componentIndex,
        QVector<ObjectSelectionListItem*> const& exportedItems) const;

    /*!
     *  Export the selected component document.
     *
     *      @param [in] component               The selected component.
     *      @param [in] exportedComponentFiles  Selected files to be exported with the component.
     *      @param [in] destinationFolder       Destination folder for the export.
     *      @param [in] handledFiles            List of exported files.
     *      @param [in] selections              Selections for overwriting existing files.
     *      @param [in] exportStatistics        Export statistics.
     *
     *      @return True, if the export was a success, false otherwise.
     */
    bool exportComponent(QSharedPointer<Component> component,
        QVector<ObjectSelectionListItem const*> exportedComponentFiles, QDir const& destinationFolder,
        FileList& handledFiles, InputSelection & selections, DocumentStatistics& exportStatistics);

    /*!
     *  Change the path of the selected file.
     *
     *      @param [in] file                    The selected file.
     *      @param [in] containingFileSet       File set containing the file.
     *      @param [in] originalComponent       The original component.
     *      @param [in] originalComponentPath   Path to the original component.
     *      @param [in] destinationFolder       Destination folder for the export.
     */
    void changeFilePath(QSharedPointer<File> file, QSharedPointer<FileSet> containingFileSet,
        QSharedPointer<Component> originalComponent, QString const& originalComponentPath,
        QDir const& destinationFolder);

    /*!
     *  Export the selected VLNV object.
     *
     *      @param [in] destinationFolder   Destination folder for the export.
     *      @param [in] vlnv                VLNV of the exported item.
     *      @param [in] handledFiles        List of the files that have been handled.
     *      @param [in] selections          Container for yes/no to all status.
     *
     *      @return True, if the selected VLNV object was exported, false otherwise.
     */
    bool exportObject(QDir const& destinationFolder, VLNV const& vlnv, FileList& handledFiles,
        InputSelection &selections);
    
    /*!
     *  Copy a single file.
     *
     *      @param [in] itemType                Type of the copied file.
     *      @param [in] source                  The source file to be copied
     *      @param [in] containingVLNV          VLNV of the containing document.
     *      @param [in/out] targetDirectory     The directory where the file is copied to.
     *      @param [in/out] handledFiles        List of files that have been copied
     *      @param [in/out] yesToAll            Info if user has selected "yes to all" to overwrite
     *      @param [in/out] noToAll             Info is user has selected "No to all" to overwrite
     *
     *      @return True, if the file was copied, false otherwise.
     */
    bool copyFile(ObjectSelectionListItem::ItemType itemType, QFileInfo const& source, VLNV const& containingVLNV,
        QDir& targetDirectory, FileList& handledFiles, InputSelection& selections);

    /*!
     *  Set the action for overwriting an existing file.
     *
     *      @param [in] targetFileName  Name of the selected file.
     *      @param [in] selections      Selections made for overwriting an existing file.
     *
     *      @return True, if the existing file is to be overwritten, false otherwise.
     */
    bool setActionForExistingFile(QString const& targetFileName, InputSelection& selections) const;

    /*!
     *  Create the export message.
     *
     *      @param [in] vlnvCount           The amount of exported VLNV items.
     *      @param [in] fileCount           The amount of exported files.
     *      @param [in] destinationPath     The export destination.
     *
     *      @return The constructed export message.
     */
    QString createExportMessage(DocumentStatistics const& exportStatistics, QString const& destinationPath) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    
    //! Access to the library.
    LibraryInterface* libraryAccess_;

    //! Parent widget for the export dialog.
    QWidget* dialogParentWidget_;
};

#endif // ITEMEXPORTER_H
