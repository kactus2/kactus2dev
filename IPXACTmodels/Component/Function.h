//-----------------------------------------------------------------------------
// File: Function.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.09.2015
//
// Description:
// Describes the ipxact:function element.
//-----------------------------------------------------------------------------

#ifndef FUNCTION_H
#define FUNCTION_H

#include <IPXACTmodels/common/NameValuePair.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Describes the ipxact:function element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Function
{

public:

	/*! 
     *  Equals the ipxact:sourceFile element in IP-Xact specification.
	 */
	struct SourceFile
    {
		//! Contains absolute or relative path to a file name or directory
		QString sourceName_;

		//! Specifies the file type known by IP-Xact.
		QString fileType_;
		
		/*!
		 *  The constructor.
		 *
		 *      @param [in] sourceName  The name of the source file.
		 *      @param [in] fileType    The file type.
		 */
		SourceFile(QString const& sourceName = QString(), QString const& fileType = QString());

        //! Copy constructor.
        SourceFile(const SourceFile& other);

        /*!
         *  Gets the source name.
         *
         *      @return The name of the source.
         */
        QString getSourceName() const;

        /*!
         *  Set the source name.
         *
         *      @param [in] newSourceName   The new name for the source.
         */
        void setSourceName(QString const& newSourceName);

        /*!
         *  Get the file type of the source.
         *
         *      @return The source file type.
         */
        QString getFileType() const;

        /*!
         *  Set the file type for the source.
         *
         *      @param [in] newFileType     The new source file type.
         */
        void setFileType(QString const& newFileType);
	};

    /*!
     *  The constructor.
     *
     *      @param [in] fileReference   The referenced file.
     */
    Function(QString const& fileReference = QString());

	// Copy constructor
	 Function(const Function &other);

	// Assignment operator
	 Function& operator=(const Function& other);

	/*!
     *  The destructor.
	 */
	~Function();

	/*! 
     *  Get the value of the replicate-attribute.
	 *
	 *      @return True for a replicated function, false otherwise.
	 */
	bool getReplicate() const;
    
	/*! 
     *  Set the replicate value.
	 *
	 *      @param [in] replicate   The new replicate value.
	 */
	void setReplicate(bool replicate);
    
	/*! 
     *  Get entry point of the function.
	 *
	 *      @return     The entry point.
	 */
	QString getEntryPoint() const;
    
	/*!
     *  Set the entry point for the function.
	 *
	 *      @param [in] entryPoint  The new entry point.
	 */
	void setEntryPoint(QString const& entryPoint);
    
	/*!
     *  Get the file reference.
	 *
	 *      @return The file reference.
	 */
	QString getFileRef() const;
    
	/*! 
     *  Set the file reference.
	 *
	 *      @param [in] fileRef     The reference to the new file.
	 */
	void setFileRef(QString const& fileRef);
    
	/*!
     *  Get the return type of the function.
	 *
	 *      @return The return type.
	 */
	QString getReturnType() const;
    
	/*!
     *  Set the return type of the function.
	 *
	 *      @param [in] returnType  The new return type. Only void and int are appected.
	 */
	void setReturnType(QString const& returnType);

	/*! 
     *  Get the list of function arguments.
	 *
	 *      @return Pointer to a list containing the arguments.
	 */
    QSharedPointer<QList<QSharedPointer<NameValuePair> > > getArguments() const;
    
	/*!
     *  Set arguments for the function.
	 *
	 *      @param [in] arguments   Pointer to a list containing the new arguments.
	 */
	void setArguments(QSharedPointer<QList<QSharedPointer<NameValuePair> > > arguments);

	/*!
     *  Get the disabled value
	 *
	 *      @return The expression for the disabled value.
	 */
	QString getDisabled() const;

	/*! 
     *  Set the disabled value
	 *
	 *      @param [in] disabled    The new disabled value.
	 */
	void setDisabled(QString const& newDisabled);
    
	/*!
     *  Get the attributes of the disabled-element.
	 *
	 *      @return QMap containing the attributes for disabled value.
	 */
	QMap<QString, QString> getDisabledAttributes() const;

	/*! 
     *  Set the attributes for the disabled-element.
	 *
	 *      @param [in] disabledAttributes  The new attributes for the disabled value.
	 */
	void setDisabledAttributes(QMap<QString, QString>& disabledAttributes);

	/*!
     *  Get list of the source files for the function.
	 *
	 *      @return Pointer to a list containing the source files.
	 */
    QSharedPointer<QList<QSharedPointer<SourceFile> > > getSourceFiles() const;

	/*! 
     *  Set the source files for the function.
	 *
	 *      @param [in] sourceFiles     Pointer to a list containing the new source files.
	 */
    void setSourceFiles(QSharedPointer<QList<QSharedPointer<SourceFile> > > newSourceFiles);

private:

    /*!
     *  Copy the arguments of a function.
     *
     *      @param [in] other   The function being copied.
     */
    void copyArguments(const Function& other);

    /*!
     *  Copy the source files of a function.
     *
     *      @param [in] other   The function being copied.
     */
    void copySourceFiles(const Function& other);

	 //! When true, the generator compiles a separate object module for each instance of the component.
	 bool replicate_;

	 //! The entry point name for the function or subroutine.
	 QString entryPoint_;

	 //! Reference to the file that contains the entry point for the function.
	 QString fileRef_;

	 //! Indicates the return type of the function.
	 QString returnType_;

	 //! Contains arguments passed when this function is called.
     QSharedPointer<QList<QSharedPointer<NameValuePair> > > arguments_;

	 //! Holds whether the software function is disabled or not.
     QString disabled_;

	 //! Contains attributes for the disabled element.
	 QMap<QString, QString> disabledAttributes_;

	 //!  References any source files. The order of the source files must be maintained.
     QSharedPointer<QList<QSharedPointer<SourceFile> > > sourceFiles_;
};

#endif // FUNCTION_H
