/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypathvalidator.h
 *		Project: Kactus 2
 */

#ifndef LIBRARYPATHVALIDATOR_H
#define LIBRARYPATHVALIDATOR_H

#include <QRegExpValidator>

/*! \brief Validator that is used when editing a save path for a new item.
 * 
 */
class LibraryPathValidator : public QRegExpValidator {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this validator.
	 *
	*/
	LibraryPathValidator(QObject *parent);
	
	//! \brief The destructor
	virtual ~LibraryPathValidator();

public slots:

	void setUnmodifiablePath(const QString& path);

private:
	
	//! \brief No copying
	LibraryPathValidator(const LibraryPathValidator& other);

	//! \brief No assignment
	LibraryPathValidator& operator=(const LibraryPathValidator& other);
};

#endif // LIBRARYPATHVALIDATOR_H
