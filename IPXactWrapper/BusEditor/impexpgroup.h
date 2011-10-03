/* 
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: impexpgroup.h
 */

#ifndef IMPEXPGROUP_H
#define IMPEXPGROUP_H

#include <QGroupBox>
#include <QPushButton>

/*! \brief ImpExpGroup contains buttons to import/export a csv file for signals.
 *
 */
class ImpExpGroup : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	ImpExpGroup(QWidget *parent);
	
	//! \brief The destructor
	virtual ~ImpExpGroup();

signals:

	//! \brief Emitted when user clicks "import csv"
	void importCSV();

	//! \brief Emitted when user clicks "export csv"
	void exportCSV();

private:
	//! \brief No copying
	ImpExpGroup(const ImpExpGroup& other);

	//! \brief No assignment
	ImpExpGroup& operator=(const ImpExpGroup& other);	

	//! \brief Button to import a csv-file
	QPushButton importButton_;

	//! \brief Button to export a csv-file
	QPushButton exportButton_;
};

#endif // IMPEXPGROUP_H
