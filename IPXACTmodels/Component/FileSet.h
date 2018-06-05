//-----------------------------------------------------------------------------
// File: FileSet.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.09.2015
//
// Description:
// Describes the ipxact:fileSet element.
//-----------------------------------------------------------------------------

#ifndef FILESET_H
#define FILESET_H

#include "Function.h"
#include "File.h"
#include <IPXACTmodels/common/FileBuilder.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>
#include <QStringList>
#include <QSharedPointer>
#include <QSettings>

//-----------------------------------------------------------------------------
//! Describes the ipxact:fileSet element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FileSet : public NameGroup, public Extendable
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name    Name of the file set.
     *      @param [in] group   Group where this file set belongs to.
     */
    FileSet(const QString& name = QString(), const QString& group = QString());

	//! Copy constructor
	FileSet(const FileSet& other);

	//! Assignment operator
	FileSet &operator=(const FileSet& other);

	/*!
     *  The destructor
	 */
	~FileSet();

	/*! 
     *  Get the groups of this file set
	 *
	 *      @reutnr The groups.
	 */
    QSharedPointer<QStringList> getGroups() const;
    
	/*!
     *  Set the groups for this file  set
	 *
	 *      @param [in] groups  The new group names.
	 */
    void setGroups (QSharedPointer<QStringList> newGroups);

	/*! Clear the previous groups and add a new group identifier.
	 *
	 *      @param [in] groupName   The group identifier which is set after clearing.
	 */
	void setGroups(const QString& groupName);
    
	/*! 
     *  Get the files associated with this file set
	 *
	 *      @return Pointer to a list containing the files associated with this file set.
	 */
    QSharedPointer<QList<QSharedPointer<File> > > getFiles() const;
    
	/*! 
     *  Get the files that match the specified file types.
	 *
	 *      @param [in] fileTypes   Contains the file types that are being searched.
	 *
	 *      @return The relative file paths to the matched files.
	 */
	QStringList getFileNames(const QStringList& fileTypes) const;

	/*! 
     *  Set the files for this file set
	 *
	 *      @param [in] files   Pointer to a list containing the new files.
	 */
    void setFiles(QSharedPointer<QList<QSharedPointer<File> > > newFiles);


	/*! 
     *  Get the default file builders for this file set
	 *
	 *      @return Pointer to a list containing the file builders.
	 */
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > getDefaultFileBuilders()  const;
    
	/*! 
     *  Set the default file builders for this file set.
	 *
	 *      @param [in] defaultFileBuilders     Pointer to a list containing the new default file builders.
	 */
    void setDefaultFileBuilders(QSharedPointer<QList<QSharedPointer<FileBuilder> > > newDefaultFileBuilders);
    
	/*! 
     *  Get the dependencies for this file set
	 *
	 *      @return Paths for the directories containing files on which this file set depends.
	 */
    QSharedPointer<QStringList> getDependencies() const;

	/*! 
     *  Set the dependencies for this file set
	 *
	 *      @param [in] dependencies    The new dependencies.
	 */
    void setDependencies(QSharedPointer<QStringList> newDependencies);

	/*! Add a new dependent directory to the file set.
	 *
	 *      @param [in] path    Relative path to the dependent directory to add.
	 *
	 */
	void addDependency(const QString& path);

	/*! Get list of the functions for this file set
	 *
	 *      @return Pointer to a list containing the functions.
	 */
    QSharedPointer<QList<QSharedPointer<Function> > > getFunctions() const;
    
	/*! 
     *  Set the functions for this file set
	 *
	 *      @param [in] functions Pointer to a list containing the new functions.
	 */
    void setFunctions(QSharedPointer<QList<QSharedPointer<Function> > > newFunctions);

	/*! 
     *  Get the names of the vhdl libraries on this file set.
	 *
	 *      @return The library names.
	 */
	QStringList getVhdlLibraryNames() const;
    
	/*! 
     *  Get the file paths of the files stored in the file set.
	 *
	 *      @return The file paths.
	 */
    QStringList getFilePaths() const;
        
	/*! 
     *  Add a new file to the file set.
	 *
	 *      @param [in] file Pointer to the new file to be added.
	 */
	void addFile(QSharedPointer<File> file);

	/*! 
     *  Add a new file to the file set.
	 *
	 *      @param [in] filePath    Path to the file to add (this should be relative path).
	 *      @param [in] settings    Contains the file type settings for Kactus2.
	 *
     *      @return Pointer to the new file.
	 */
	QSharedPointer<File> addFile(const QString& filePath, QSettings& settings);

	/*! 
     *  Get pointer to the file with given logical name.
	 *
	 *      @param [in] logicalName     The logical name of the wanted file. The first one found is returned.
	 *
	 *      @return Pointer to the File instance, null pointer if none is found.
	 */
	QSharedPointer<File> getFile(const QString logicalName) const;

	/*! 
     *  Get the name elements of the files stored within this file set.
	 *
	 *      @return The file names.
	 */
	QStringList getFileNames() const;

	/*! 
     *  Find the files that match the specified file type.
	 *
	 *      @param [in] fileType    The file type to search within files.
	 *
	 *      @return The file paths to files that match the file type.
     */
	QStringList findFilesByFileType(const QString& fileType) const;

	/*!
     *  Remove the specified file from the file set.
	 *
	 *      @param [in] fileName    The name of the file to be removed
	 */
	void removeFile(const QString& fileName);

	/*! 
     * Remove the specified file from the file set.
	 *
	 *      @param [in] file    Pointer to the file that is to be removed
	 */
    void removeFile(QSharedPointer<File> file);

	/*!
     *  Create a new file to the file set.
	 * 
	 *      @return Pointer to the file that was created.
	 */
	QSharedPointer<File> createFile();

	/*!
     *  Get the default build command's command part for given file.
	 *
	 *      @param [in] file    Pointer to the file that's command is wanted.
	 *
	 *      @return Command of the selected file.
	 */
    QString getDefaultCommand(QSharedPointer<File> file) const;

	/*! 
     *  Get the default build command's flags for given file.
	 *
	 *      @param [in] file    Pointer to the file that's default flags are wanted.
	 *
	 *      @return The default flags of selected file.
	 */
    QString getDefaultFlags(QSharedPointer<File> file) const;

	/*!
     *  Get list of the RTL files contained in this file set.
	 *
	 *      @return Pointers to the files.
	 */
	QList<QSharedPointer<File> > getRTLFiles() const;

	/*! 
     *  Does the file set contain file with given name.
	 *
	 *      @param [in] fileName    The name of the file searched for.
	 *
	 *      @return True if file is found, false otherwise.
	 */
	bool contains(const QString& fileName) const;

	/*! 
     *  Change the name of a file within this file set.
	 * 
	 *      @param [in] from    The current file name.
	 *      @param [in] to      The file name to set.
	 */
	void changeFileName(const QString& from, const QString& to);

	/*!
     *  Sort the files in the file set to given order.
	 *
	 *      @param [in] fileNames   Names of files in the order to sort them into.
	 */
	void sortFiles(const QStringList& fileNames);

	/*!
     *  Remove the files listed in the file set.
	 */
	void clearFiles();

	/*! 
     *  Get id of the file set.
	 *
	 *      @return The file set id.
	 */
	QString getFileSetId() const;

	/*!
     *  Set the file set id.
	 *
	 *      @param [in] id  The id to be set.
	 */
	void setFileSetId(const QString& newId);

	/*! 
     *  Sorts files by file name.
	 */
	void sortFiles();

private:

    /*!
     *  Copies the files.
     *
     *      @param [in] other   The file set from which to copy the files.
     */
    void copyFiles(const FileSet& other);

    /*!
     *  Copies the default file builders.
     *
     *      @param [in] other   The file set from which to copy the default file builders.
     */
    void copyDefaultFileBuilders(const FileSet& other);

    /*!
     *  Copies the functions.
     *
     *      @param [in] other   The file set from which to copy the functions.
     */
    void copyFunctions(const FileSet& other);

    /*!
     *  Copies the groups and dependencies.
     *
     *      @param [in] other   The file set from which to copy the groups and dependencies.
     */
    void copyStringLists(const FileSet& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Describes the function or purpose of the file set.
	QSharedPointer<QStringList> groups_;

	//! The contained files.
	QSharedPointer<QList<QSharedPointer<File> > > files_;

	//! The contained default file builders.
	QSharedPointer<QList<QSharedPointer<FileBuilder> > > defaultFileBuilders_;

	//! Contains paths to directories containing dependency files
	QSharedPointer<QStringList> dependencies_;

	//! The contained functions.
	QSharedPointer<QList<QSharedPointer<Function> > > functions_;
};

#endif // FILESET_H
