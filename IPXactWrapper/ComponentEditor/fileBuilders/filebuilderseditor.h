/* 
 *  	Created on: 31.5.2012
 *      Author: Antti Kamppi
 * 		filename: filebuilderseditor.h
 *		Project: Kactus 2
 */

#ifndef FILEBUILDERSEDITOR_H
#define FILEBUILDERSEDITOR_H

#include "filebuildersmodel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QTableView>

/*! \brief Editor to edit file builders.
 *
 */
class FileBuildersEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param fileBuilders QList that contains the file builders to edit.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	FileBuildersEditor(QList<QSharedPointer<FileBuilder> >& fileBuilders,
		QWidget* parent,
		const QString& title = tr("Default file build commands"));

	//! \brief the destructor
	virtual ~FileBuildersEditor();

	/*! \brief Check for the validity of the edited file builders.
	*
	* \return True if all file builders are in valid state.
	*/
	virtual bool isValid() const;

signals:

	//! \brief Emit an error message to user.
	void errorMessage(const QString& msg);

	//! \brief Emit a notice to user.
	void noticeMessage(const QString& msg);

	//! \brief Inform that contents of this editor have changed.
	void contentChanged();

private:

	//! \brief No copying
	FileBuildersEditor(const FileBuildersEditor& other);

	//! No assignment
	FileBuildersEditor& operator=(const FileBuildersEditor& other);

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	FileBuildersModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel proxy_;
};

#endif // FILEBUILDERSEDITOR_H
