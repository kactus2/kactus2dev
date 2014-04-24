/* 
 *	Created on:	11.6.2013
 *	Author:		Antti Kamppi
 *	File name:	fileselector.h
 *	Project:		Kactus 2
*/

#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <IPXACTmodels/component.h>

#include <QComboBox>
#include <QSharedPointer>
#include <QStringList>

/*! \brief Used to select one file from the component's file sets.
 *
 */
class KACTUS2_API FileSelector : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * Method: 		FileSelector
	 * Full name:	FileSelector::FileSelector
	 * Access:		private 
	 *
	 * \param component Pointer to the component which's files are selected.
	 * \param parent Pointer to the owner of the combo box.
	 *
	*/
	FileSelector(QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~FileSelector();

	/*! \brief Add a file suffix to be used as filter when selecting files to display.
	 *
	 * Method: 		addFilter
	 * Full name:	FileSelector::addFilter
	 * Access:		public 
	 *
	 * \param suffix The file suffix to use as filter.
	 *
	*/
	void addFilter(const QString& suffix);

public slots:

	//! \brief Refresh the items to be selected in the combo box.
	void refresh();

	/*! \brief Set the specified file as selected.
	 *
	 * \param filePath The file path of the file to select.
	 *
	*/
	void selectFile(const QString& filePath);

signals:

	//! \brief Emitted when a file is selected.
	void fileSelected(const QString& filePath);

private slots:

	//! \brief Handler for selected index changes.
	void onIndexChange(int newIndex);

private:
	
	//! \brief No copying
	FileSelector(const FileSelector& other);

	//! \brief No assignment
	FileSelector& operator=(const FileSelector& other);

	//! \brief Pointer to the component containing the files.
	QSharedPointer<Component> component_;

	//! \brief Contains the file suffixes used to filter file types.
	QStringList filters_;
};

#endif // FILESELECTOR_H
