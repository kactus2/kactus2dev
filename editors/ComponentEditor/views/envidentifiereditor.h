//-----------------------------------------------------------------------------
// File: envidentifiereditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.04.2011
//
// Description:
// Widget to edit the ipxact:envIdentifiers element within view.
//-----------------------------------------------------------------------------

#ifndef ENVIDENTIFIEREDITOR_H
#define ENVIDENTIFIEREDITOR_H

#include "envidentifiersmodel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QString>
#include <QSortFilterProxyModel>
#include <QSharedPointer>

class View;

//-----------------------------------------------------------------------------
//! Widget to edit the spirit:envIdentifiers element within view.
//-----------------------------------------------------------------------------
class EnvIdentifierEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] view    The view being edited.
	 *      @param [in] parent  The owner of this widget.
	 */
	EnvIdentifierEditor(QSharedPointer<View> view, QWidget *parent);
	
	//! The destructor.
	virtual ~EnvIdentifierEditor();

	/*!
     *  Restore the changes made in the editor back to ones in the model.
	 */
	virtual void refresh();

	/*!
     *  Check for the validity of the edited parameters.
	 *
	 *      @return True if all parameters are in valid state.
	 */
	virtual bool isValid() const;

signals:

	//! Signal to inform user that one element has been modified.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:

	//! No copying.
	EnvIdentifierEditor(const EnvIdentifierEditor& other);

	//! No assignment
	EnvIdentifierEditor& operator=(const EnvIdentifierEditor& other);

	//! The view that displays the parameters.
	EditableTableView view_;

	//! The model that holds the data to be displayed to the user.
	EnvIdentifiersModel model_;

	//! The proxy that is used to sort the view.
	QSortFilterProxyModel proxy_;
};

#endif // ENVIDENTIFIEREDITOR_H
