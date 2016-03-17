//-----------------------------------------------------------------------------
// File: FileValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 20.11.2015
//
// Description:
// Validator for ipxact:File.
//-----------------------------------------------------------------------------

#ifndef FileVALIDATOR_H
#define FileVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class ExpressionParser;
class File;
class BuildCommand;

//-----------------------------------------------------------------------------
//! Validator for ipxact:File.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FileValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    FileValidator(QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	virtual ~FileValidator();
    
    /*!
     *  Validates the given File.
     *
     *      @param [in] File           The File to validate.
     *
     *      @return True, if the File is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<File> file) const;

    /*!
     *  Check if a build command has a valid replace default flags value.
     *
     *      @param [in] buildCommand    The selected build command.
     *
     *      @return True, if the build command replace default flags is valid, false otherwise.
     */
    bool buildCommandHasValidReplaceDefaultFlags(QSharedPointer<BuildCommand> buildCommand) const;

    /*!
     *  Finds possible errors in a File and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] File   The File whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<File> file,
		QString const& context) const;

private:
    
	// Disable copying.
	FileValidator(FileValidator const& rhs);
	FileValidator& operator=(FileValidator const& rhs);

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
