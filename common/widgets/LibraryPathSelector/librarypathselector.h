/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypathselector.h
 *		Project: Kactus 2
 */

#ifndef LIBRARYPATHSELECTOR_H
#define LIBRARYPATHSELECTOR_H

#include <common/widgets/LibraryPathEditor/librarypatheditor.h>
#include <common/validators/LibraryPathValidator/librarypathvalidator.h>

#include <QComboBox>
#include <QSettings>
#include <QStringList>

/*! \brief Combo box to select one of the previously set library locations.
 *
 */
class LibraryPathSelector : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this combo box.
	 *
	*/
	LibraryPathSelector(QWidget *parent);
	
	//! \brief The destructor
	virtual ~LibraryPathSelector();

	/*! \brief Get the currently selected base location.
	 *
	 * \return QString containing the selected library location.
	*/
	QString currentLocation() const;

private slots:

	/*! \brief Handler for index changes on combo box.
	 *
	*/
	void onIndexChanged();

private:
	
	//! \brief No copying
	LibraryPathSelector(const LibraryPathSelector& other);

	//! \brief No assignment
	LibraryPathSelector& operator=(const LibraryPathSelector& other);

	//! \brief Contains the Kactus2 settings.
	QSettings settings_;

	//! \brief Validator used for the editable combo box.
	LibraryPathValidator* validator_;

	//! \brief the library locations from the settings.
	QStringList libraryLocations_;

	//! \brief The editor to display and edit the path.
	LibraryPathEditor* pathEditor_;
};

#endif // LIBRARYPATHSELECTOR_H
