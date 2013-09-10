/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypathselector.h
 *		Project: Kactus 2
 */

#ifndef LIBRARYPATHSELECTOR_H
#define LIBRARYPATHSELECTOR_H

#include <common/widgets/LibrarySelectorWidget/LibraryPathEditor/librarypatheditor.h>
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

    bool isValid();

	/*! \brief Get the currently selected base location.
	 *
	 * \return QString containing the selected library location.
	*/
    QString currentLocation() const;

    /*!
     *  Searches the active library locations.
     */
    void refresh();

    /*!
     *  Gets the active library paths.
     *
     *      @return     The active library locations. 
     */
    QStringList getLibraryLocations();

    /*!
     *  Checks if there is a library for a given path.
     *
     *      @param [in] path   The path to search for.
     *
     *      @return     True, if the path is under any of the active libraries, otherwise false.
     */
    bool hasIndexFor(QString const& path);

    /*!
     *  Sets the index for the corresponding library for a given path.
     *
     *      @param [in] path   The path under a library.
     *
     *      @return     True, if the path is under any of the active libraries and can be set, otherwise false.
     */
    bool setIndexFor(QString const& path);

signals:

    //! \brief Emitted when the library path changes.
    void contentChanged();

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
