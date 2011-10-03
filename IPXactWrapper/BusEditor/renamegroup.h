/*
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: renamegroup.h
 */

#ifndef RENAMEGROUP_H
#define RENAMEGROUP_H

#include <QGroupBox>
#include <QPushButton>

/*! \brief Rename group contains buttons to rename signals
 *
 */
class RenameGroup : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	RenameGroup(QWidget *parent);
	
	//! \brief The destructor
	virtual ~RenameGroup();

signals:

	//! \brief Emitted when user clicks "Rename signal group"
	void renameSignalGroup();

	//! \brief Emitted when user clicks "Add new keep column"
	void addNewKeepColumn();

	//! \brief Emitted when user clicks "Add new change column"
	void addNewChangeColumn();

private:
	//! \brief No copying
	RenameGroup(const RenameGroup& other);

	//! \brief No assignment
	RenameGroup& operator=(const RenameGroup& other);

	//! \brief Button to rename signal groups
	QPushButton renameButton_;

	//! \brief Button to add new but keep the column
	QPushButton addKeepButton_;

	//! \brief Button to add new but change column
	QPushButton addChangeButton_;
};

#endif // RENAMEGROUP_H
