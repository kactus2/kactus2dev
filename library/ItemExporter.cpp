//-----------------------------------------------------------------------------
// File: ItemExporter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.03.2019
//
// Description:
// Exports the selected library items.
//-----------------------------------------------------------------------------

#include "ItemExporter.h"

#include "MessageMediator.h"
#include "ObjectExportDialog.h"

#include "LibraryItemSelectionFactory.h"

#include <KactusAPI/include/DocumentFileAccess.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>

#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: ItemExporter::ItemExporter()
//-----------------------------------------------------------------------------
ItemExporter::ItemExporter(LibraryInterface* libraryAccess,
    QWidget* dialogParentWidget, QObject* parent):
QObject(parent),
libraryAccess_(libraryAccess),
dialogParentWidget_(dialogParentWidget)
{

}

//-----------------------------------------------------------------------------
// Function: ItemExporter::onExportItem()
//-----------------------------------------------------------------------------
void ItemExporter::onExportItem(VLNV const& vlnv)
{
    onExportItems(QList<VLNV>() << vlnv);
}

//-----------------------------------------------------------------------------
// Function: ItemExporter::onExportItems()
//-----------------------------------------------------------------------------
void ItemExporter::onExportItems(const QList<VLNV> vlnvs)
{
    if (vlnvs.isEmpty())
    {
        return;
    }

    // get the current working directory and save it to be restored in the end of the function
    QDir savedWorkingDirectory = QDir::current();

    ObjectExportDialog* exportDialog = new ObjectExportDialog(dialogParentWidget_);
    LibraryItemSelectionFactory::constructItemsForSelectionDialog(libraryAccess_, exportDialog, vlnvs);

    if (exportDialog->exec() == QDialog::Rejected)
    {
        return;
    }

    QString destinationPath = exportDialog->getTargetDirectory();

    DocumentStatistics exportStatistics = exportSelectedObjects(exportDialog->getSelectedItems(), destinationPath);

    emit noticeMessage(createExportMessage(exportStatistics, destinationPath));

    QDir::setCurrent(savedWorkingDirectory.absolutePath());
}

//-----------------------------------------------------------------------------
// Function: ItemExporter::exportSelectedObjects()
//-----------------------------------------------------------------------------
ItemExporter::DocumentStatistics ItemExporter::exportSelectedObjects(
    QVector<ObjectSelectionListItem*> const& exportedItems, QString const& destinationPath)
{
    FileList handledFiles;
    QDir destinationFolder(destinationPath);

    InputSelection userSelections;
    DocumentStatistics exportStatistics;

    bool exportSuccess = false;

    for (int i = 0; i < exportedItems.count(); i++)
    {
        ObjectSelectionListItem const* exportedItem = exportedItems.at(i);
        exportSuccess = false;
        if (exportedItem->getType() == ObjectSelectionListItem::VLNVOJBECT)
        {
            QSharedPointer<Document> vlnvDocument = libraryAccess_->getModel(exportedItem->getVLNV());
            QSharedPointer<Component> component = vlnvDocument.dynamicCast<Component>();
            if (component)
            {
                QVector<ObjectSelectionListItem const*> exportedComponentFiles =
                    getExportedFilesForComponent(i, exportedItems);

                exportSuccess = exportComponent(component, exportedComponentFiles, destinationFolder, handledFiles,
                    userSelections, exportStatistics);
            }
            else
            {
                exportSuccess =
                    exportObject(destinationFolder, exportedItem->getVLNV(), handledFiles, userSelections);
            }

            if (exportSuccess)
            {
                exportStatistics.documentCount++;
            }
        }
    }

    return exportStatistics;
}

//-----------------------------------------------------------------------------
// Function: ItemExporter::getExportedFilesForComponent()
//-----------------------------------------------------------------------------
QVector<ObjectSelectionListItem const*> ItemExporter::getExportedFilesForComponent(int const& componentIndex,
    QVector<ObjectSelectionListItem *> const& exportedItems) const
{
    QVector<ObjectSelectionListItem const*> exportedFiles;

    for (int i = componentIndex + 1; i < exportedItems.count(); ++i)
    {
        ObjectSelectionListItem const* exportedItem = exportedItems.at(i);
        if (exportedItem->getType() == ObjectSelectionListItem::FILE)
        {
            exportedFiles.append(exportedItem);
        }
        else if (exportedItem->getType() == ObjectSelectionListItem::VLNVOJBECT)
        {
            QSharedPointer<const Document> vlnvDocument =
                libraryAccess_->getModelReadOnly(exportedItem->getVLNV());
            if (!vlnvDocument.dynamicCast<const Component>().isNull())
            {
                break;
            }
        }
    }

    return exportedFiles;
}

//-----------------------------------------------------------------------------
// Function: ItemExporter::exportComponent()
//-----------------------------------------------------------------------------
bool ItemExporter::exportComponent(QSharedPointer<Component> component,
    QVector<ObjectSelectionListItem const *> exportedComponentFiles, QDir const& destinationFolder,
    FileList& handledFiles, InputSelection & selections, DocumentStatistics& exportStatistics)
{
    QString documentPath = libraryAccess_->getPath(component->getVlnv());
    QFileInfo documentFileInfo(documentPath);
    if (handledFiles.contains(documentFileInfo))
    {
        return false;
    }
    else
    {
        handledFiles.append(documentFileInfo);
    }

    QSharedPointer<Document> cloneDocument = component->clone();
    if (!cloneDocument)
    {
        return false;
    }

    QSharedPointer<Component> cloneComponent = cloneDocument.dynamicCast<Component>();
    if (!cloneComponent)
    {
        return false;
    }

    const QDir savedWorkingDir = QDir::current();

    QDir::setCurrent(destinationFolder.absolutePath());

    QDir vlnvTargetDirectory;
    QString directoryPath = component->getVlnv().toString("/");
    if (!vlnvTargetDirectory.mkpath(directoryPath))
    {
        emit errorMessage(tr("Could not create directory structure, aborting."));
        QDir::setCurrent(savedWorkingDir.absolutePath());
        return false;
    }

    vlnvTargetDirectory.setPath(directoryPath);

    QString targetDirectoryPath = vlnvTargetDirectory.absolutePath();
    QDir::setCurrent(targetDirectoryPath);

    QString cloneFileName =
        cloneComponent->getVlnv().getName() + '.' + cloneComponent->getVlnv().getVersion() + ".xml";

    QFileInfo cloneComponentInfo(cloneFileName);
    bool createCloneComponent = true;
    if (QFile::exists(cloneFileName) && !setActionForExistingFile(cloneFileName, selections))
    {
        createCloneComponent = false;
    }

    //! Copy selected files.
    for (auto file : exportedComponentFiles)
    {
        if (copyFile(ObjectSelectionListItem::FILE, QFileInfo(file->getPath()), component->getVlnv(),
            vlnvTargetDirectory, handledFiles, selections))
        {
            exportStatistics.fileCount++;
        }
    }

    //! Change the path of the contained, unselected files.
    QString originalDirectoryPath = libraryAccess_->getDirectoryPath(component->getVlnv());
    for (auto fileSet : *cloneComponent->getFileSets())
    {
        for (auto currentFile : *fileSet->getFiles())
        {
            if (!QFile::exists(currentFile->name()))
            {
                QFileInfo currentFileInfo(currentFile->name());
                if (QFile::exists(currentFileInfo.fileName()))
                {
                    currentFile->setName(currentFileInfo.fileName());
                }
                else
                {
                    changeFilePath(currentFile, fileSet, component, originalDirectoryPath, vlnvTargetDirectory);
                }
            }
        }
    }

    if (createCloneComponent)
    {
        // Bypass library since exported items are not stored in library.
        DocumentFileAccess::writeDocument(cloneComponent, cloneFileName);
    }

    QDir::setCurrent(savedWorkingDir.absolutePath());
    return createCloneComponent;
}

//-----------------------------------------------------------------------------
// Function: ItemExporter::changeFilePath()
//-----------------------------------------------------------------------------
void ItemExporter::changeFilePath(QSharedPointer<File> file, QSharedPointer<FileSet> containingFileSet,
    QSharedPointer<Component> originalComponent, QString const& originalComponentPath,
    QDir const& destinationFolder)
{
    for (auto fileSet : *originalComponent->getFileSets())
    {
        if (fileSet->name() == containingFileSet->name())
        {
            for (auto originalFile : *fileSet->getFiles())
            {
                if (file->name() == originalFile->name())
                {
                    QString originalPath = originalComponentPath + '/' + originalFile->name();
                    QString directoryRelativePath = destinationFolder.relativeFilePath(originalPath);

                    file->setName(directoryRelativePath);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ItemExporter::exportObject()
//-----------------------------------------------------------------------------
bool ItemExporter::exportObject(QDir const& destinationFolder, VLNV const& vlnv, FileList& handledFiles,
    InputSelection &selections)
{
    if (libraryAccess_->contains(vlnv) == false)
    {
        return false;
    }

    const QDir savedWorkingDir = QDir::current();

    QDir::setCurrent(destinationFolder.absolutePath());

    QDir vlnvTargetDirectory;
    QString directoryPath = vlnv.toString("/");
    if (!vlnvTargetDirectory.mkpath(directoryPath))
    {
        emit errorMessage(tr("Could not create directory structure, aborting."));
        QDir::setCurrent(savedWorkingDir.absolutePath());
        return false;
    }

    vlnvTargetDirectory.setPath(directoryPath);

    QString documentPath = libraryAccess_->getPath(vlnv);
    QFileInfo documentFileInfo(documentPath);

    bool fileWasExported = copyFile(ObjectSelectionListItem::VLNVOJBECT, documentFileInfo, vlnv,
        vlnvTargetDirectory, handledFiles, selections);

    QDir::setCurrent(savedWorkingDir.absolutePath());
    return fileWasExported;
}

//-----------------------------------------------------------------------------
// Function: ItemExporter::copyFile()
//-----------------------------------------------------------------------------
bool ItemExporter::copyFile(ObjectSelectionListItem::ItemType itemType, QFileInfo const& source,
    VLNV const& containingVLNV, QDir& targetDirectory, FileList& handledFiles, InputSelection& selections)
{
    if (handledFiles.contains(source))
    {
        return false;
    }

    if (source.exists() == false)
    {
        QString message(tr("Could not find file: %1").arg(source.fileName()));
        if (itemType == ObjectSelectionListItem::FILE)
        {
            message += QStringLiteral(" in component ") + containingVLNV.toString();
        }

        emit errorMessage(message);
        return false;
    }

    // Add file to handledFiles list to indicate that we don't need to check later in possible recursive calls.
    handledFiles.append(source);

    // save the current directory to be restored at the end of the function
    QDir savedCurrentDir = QDir::current();

    QString targetFileName = source.fileName();
    QDir::setCurrent(targetDirectory.absolutePath());
    if (QFile::exists(targetFileName))
    {
        if (!setActionForExistingFile(targetFileName, selections))
        {
            QDir::setCurrent(savedCurrentDir.absolutePath());
            return false;
        }
    }

    QFile sourceFile(source.filePath());

    if (!sourceFile.copy(targetFileName))
    {
        emit errorMessage(tr("File %1 couldn't be copied").arg(source.fileName()));
    }

    // restore the current directory to the state it was before this function
    QDir::setCurrent(savedCurrentDir.absolutePath());
    return true;
}

//-----------------------------------------------------------------------------
// Function: ItemExporter::setActionForExistingFile()
//-----------------------------------------------------------------------------
bool ItemExporter::setActionForExistingFile(QString const& targetFileName, InputSelection& selections) const
{
    QMessageBox::StandardButton answer = QMessageBox::No;

    if (selections.noToAll)
    {
        answer = QMessageBox::No;
    }
    else if (selections.yesToAll)
    {
        answer = QMessageBox::Yes;
    }
    else
    {
        // If "yes to all" or "no to all" has not been clicked, ask user what to do.
        answer = QMessageBox::question(dialogParentWidget_, tr("Overwrite file?"),
            tr("The file %1 already exists, would you like to overwrite the file?").arg(targetFileName),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll,
            QMessageBox::No);

        if (answer == QMessageBox::YesToAll)
        {
            selections.yesToAll = true;
        }
        else if (answer == QMessageBox::NoToAll)
        {
            selections.noToAll = true;
        }
    }

    if (answer == QMessageBox::Yes || answer == QMessageBox::YesToAll)
    {
        QFile fileToOverwrite(targetFileName);
        fileToOverwrite.remove();
    }
    else if (answer == QMessageBox::No || answer == QMessageBox::NoToAll)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ItemExporter::createExportMessage()
//-----------------------------------------------------------------------------
QString ItemExporter::createExportMessage(DocumentStatistics const& exportStatistics,
    QString const& destinationPath) const
{
    QString exportMessage = "Exported ";

    if (exportStatistics.documentCount > 0)
    {
        exportMessage += QString("%1 VLNV item(s)").arg(QString::number(exportStatistics.documentCount));

        if (exportStatistics.fileCount > 0)
        {
            exportMessage += QString(" and %1 file(s)").arg(QString::number(exportStatistics.fileCount));
        }
    }
    else if (exportStatistics.fileCount > 0)
    {
        exportMessage += QString("%1 file(s)").arg(QString::number(exportStatistics.fileCount));
    }
    else
    {
        exportMessage += "0 items";
    }

    exportMessage += QString(" to %1.").arg(destinationPath);

    return exportMessage;
}
