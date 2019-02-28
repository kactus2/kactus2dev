/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypathvalidator.h
 *		Project: Kactus 2
 */

#ifndef LIBRARYPATHVALIDATOR_H
#define LIBRARYPATHVALIDATOR_H

#include <QRegularExpressionValidator>

/*! \brief Validator that is used when editing a save path for a new item.
 * 
 */
class LibraryPathValidator : public QRegularExpressionValidator {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this validator.
	 *
	*/
	LibraryPathValidator(QObject *parent);
	
	//! \brief The destructor
	virtual ~LibraryPathValidator() = default;

    //! \brief No copying
    LibraryPathValidator(const LibraryPathValidator& other) = delete;

    //! \brief No assignment
    LibraryPathValidator& operator=(const LibraryPathValidator& other) = delete;

	/*! \brief Validates a given input.
	 *
	 * \param input Input to validate.
	 * \param pos   Length set to input, if input is not matched.
	 *
     * \return Acceptable, if input is valid, otherwise Invalid. Intermediate is never returned.
	*/
    virtual QValidator::State validate(QString& input, int& pos) const;

public slots:

	void setUnmodifiablePath(const QString& path);

};

#endif // LIBRARYPATHVALIDATOR_H
