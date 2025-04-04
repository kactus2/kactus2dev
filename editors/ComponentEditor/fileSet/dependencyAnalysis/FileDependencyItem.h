//-----------------------------------------------------------------------------
// File: FileDependencyItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 13.02.2013
//
// Description:
// Base class for file dependency items.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYITEM_H
#define FILEDEPENDENCYITEM_H

#include <IPXACTmodels/common/VLNV.h>

#include <QObject>
#include <QString>
#include <QList>
#include <QSharedPointer>

class File;
class FileSet;
class Component;

//-----------------------------------------------------------------------------
//! Base class for file dependency items.
//-----------------------------------------------------------------------------
class FileDependencyItem : public QObject
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Item type enumeration.
    //-----------------------------------------------------------------------------
    enum ItemType
    {
        ITEM_TYPE_ROOT = 0,
        ITEM_TYPE_FOLDER,               //!< For packaged folders.
        ITEM_TYPE_EXTERNAL_LOCATION,    //!< For external but specified locations.
        ITEM_TYPE_UNKNOWN_LOCATION,     //!< For files whose location is unknown.
        ITEM_TYPE_FILE                  //!< For any files.
    };
    
    /*!
     *  Default constructor which creates a root item.
     */
    FileDependencyItem();

    /*!
     *  Destructor.
     */
    virtual ~FileDependencyItem();

    /*!
     *  Adds a file item.
     *
     *    @param [in] type       The item type.
     *    @param [in] component  The component being edited.
     *    @param [in] path       The file path.
     *    @param [in] fileRefs   The file references.
     */
    FileDependencyItem* addFile(QSharedPointer<Component> component, QString const& path,
                                QList<QSharedPointer<File> > const& fileRefs);

    /*!
     *  Adds a folder item.
     *
     *    @param [in] component  The component being edited.
     *    @param [in] path       The folder path. Enclose external locations with with '$' e.g. $path$.
     *
     *    @return The created item.
     */
    FileDependencyItem* addFolder(QSharedPointer<Component> component, QString const& path);

    /*!
     *  Inserts an item.
     *
     *    @param [in] item The item to insert.
     */
    void insertItem(FileDependencyItem* item);

    /*!
     *  Removes an item.
     *
     *    @param [in] item The item to remove.
     */
    void removeItem(FileDependencyItem* item);

    /*!
     *  Assigns the file into the given file sets.
     *
     *    @param [in] fileSets          The file sets where the file is assigned.
     */
    void setFileSets(QList<QSharedPointer<FileSet> > fileSets);

    /*!
     *  Refreshes the file references.
     */
    void refreshFileRefs();

    /*!
     *  Returns the child at the given index.
     *
     *    @param [in] index The index of the child to retrieve.
     */
    FileDependencyItem* getChild(int index);

    /*!
     *  Returns the child at the given index.
     *
     *    @param [in] index The index of the child to retrieve.
     */
    FileDependencyItem const* getChild(int index) const;

    /*!
     *  Returns the number of child items.
     */
    int getChildCount() const;

    /*!
     *  Returns the parent item.
     */
    FileDependencyItem* getParent();

    /*!
     *  Returns the index of this item.
     */
    int getIndex();

    /*!
     *  Returns the item type.
     */
    ItemType getType() const;

    /*!
     *  Gets the names of all the contained file sets.
     *
     *    @return The names of the contained file sets.
     */
    QStringList getFileSetNames() const;

    /*!
     *  Returns true if children have multiple (differing) filesets.
     */
    bool hasMultipleFileSets() const;

    /*!
     *  Returns the all file types the file represents.
     */
    QStringList getFileTypes() const;

    /*!
     *  Returns a shortened version of the path.
     */
    QString getDisplayPath() const;

    /*!
     *  Returns the name of the file (i.e. the packaged relative name).
     */
    QString getPath() const;

    /*!
     *  Returns true if the item is considered as external (not packaged).
     */
    bool isExternal() const;

private:
    // Disable copying.
    FileDependencyItem(FileDependencyItem const& rhs);
    FileDependencyItem& operator=(FileDependencyItem const& rhs);

    /*!
     *  Constructor which creates a file item.
     *
     *    @param [in] component  The component being edited.
     *    @param [in] path       The path of the file.
     *    @param [in] fileRefs   The file references.
     *    @param [in] parent     The parent item.
     */
    FileDependencyItem( QSharedPointer<Component> component, QString const& path,
        QList<QSharedPointer<File> > const& fileRefs, FileDependencyItem* parent);

    /*!
     *  Constructor which creates a folder item.
     *     
     *    @param [in] component  The component being edited.
     *    @param [in] path       The path of the file/folder.
     *    @param [in] parent     The parent item.
     */
    FileDependencyItem(QSharedPointer<Component> component, QString const& path, FileDependencyItem* parent);
    
    /*!
     *  Returns the file sets where the file is currently contained.
     */
    QList<QSharedPointer<FileSet> > getFileSets() const;

    /*!
     *  Returns the file sets of all the child items.
     */
    QList<QSharedPointer<FileSet> > getAllChildFileSets() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent item.
    FileDependencyItem* parent_;
    
    //! The item type.
    ItemType type_;

    //! The component being edited.
    QSharedPointer<Component> component_;

    //! The file path of the file/folder.
    QString path_;

    //! VLNV references.
    QList<VLNV> references_;

    //! File pointers.
    QList<QSharedPointer<File> >fileRefs_;

    //! The child items.
    QList<FileDependencyItem*> children_;

};

#endif // FILEDEPENDENCYITEM_H
