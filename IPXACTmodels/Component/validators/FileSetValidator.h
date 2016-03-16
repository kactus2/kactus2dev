//-----------------------------------------------------------------------------
// FileSet: FileSetValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 20.11.2015
//
// Description:
// Validator for ipxact:FileSet.
//-----------------------------------------------------------------------------

#ifndef FileSetVALIDATOR_H
#define FileSetVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/FileSet.h>

#include <QString>
#include <QVector>

class ExpressionParser;
class FileValidator;
class FileBuilder;

//-----------------------------------------------------------------------------
//! Validator for ipxact:FileSet.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FileSetValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    FileSetValidator(QSharedPointer<FileValidator> fileValidator,
        QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	virtual ~FileSetValidator();
    
    /*!
     *  Validates the given FileSet.
     *
     *      @param [in] FileSet           The FileSet to validate.
     *
     *      @return True, if the FileSet is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<FileSet> FileSet) const;
    
    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Check if a file builder has a valid replace default flags value.
     *
     *      @param [in] builder     The selected file builder.
     *
     *      @return True, if the replace default flags valie is valid, false otherwise.
     */
    bool fileBuilderHasValidReplaceDefaultFlags(QSharedPointer<FileBuilder> builder) const;

    /*!
     *  Finds possible errors in a FileSet and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] FileSet   The FileSet whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<FileSet> FileSet,
		QString const& context) const;

private:

    // Disable copying.
    FileSetValidator(FileSetValidator const& rhs);
    FileSetValidator& operator=(FileSetValidator const& rhs);

    //! The used file validator.
    QSharedPointer<FileValidator> fileValidator_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
