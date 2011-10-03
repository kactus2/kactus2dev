/* 
 *
 *  Created on: 15.2.2011
 *      Author: Antti Kamppi
 * 		filename: filedefineview.h
 */

#ifndef FILEDEFINEVIEW_H
#define FILEDEFINEVIEW_H

#include <QTableView>

/*! \brief Table view to display the define-elements for the file.
 *
 */
class FileDefineView : public QTableView{
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FileDefineView(QWidget* parent);

	//! \brief The destructor
	virtual ~FileDefineView();

private:

	//! \brief No copying
	FileDefineView(const FileDefineView& other);

	//! No assignment
	FileDefineView& operator=(const FileDefineView& other);
	
};

#endif // FILEDEFINEVIEW_H
