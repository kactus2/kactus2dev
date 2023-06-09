//-----------------------------------------------------------------------------
// File: FileBuilder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Implementation for ipxact:fileBuilder element.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDER_H
#define FILEBUILDER_H

#include <IPXACTmodels/common/BuildModel.h>

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/FileType.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>
#include <QStringList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Contains flags or commands for building the containing source file types.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FileBuilder : public BuildModel, public Extendable
{

public:

	/*!
	 *  The default constructor.
	 */
	FileBuilder();

	/*!
	 *  The constructor.
	 *
	 *      @param [in] fileType    The type of the file.
	 */
	FileBuilder(QString const& fileType);

	/*!
	 *  Copy constructor.
	 */
	FileBuilder(FileBuilder const& other);

	/*!
	 *  Assignment operator.
	 */
	FileBuilder &operator=(const FileBuilder &other);

	/*!
	 *  The destructor.
	 */
	virtual ~FileBuilder();

	/*!
	 *  Get the type of the file.
	 *
     *      @return The type of the file.
	 */
	FileType getFileType() const;

	/*!
	 *  Set a file type.
	 *
	 *      @param [in] newFileType     The new file type.
	 */
	void setFileType(QString const& newFileType, QString const& libext = QString());

	/*!
	 *  Check if the file builde is meant for a given file type.
	 *
	 *      @param [in] fileType    The file type to be searched for.
     *
     *      @return True, if the file type is contained within the file builder, false otherwise.
	 */
	bool hasFileType(QString const& fileType) const;

private:

    //! The file type.
    FileType fileType_;
};
#endif // FILEBUILDER_H
