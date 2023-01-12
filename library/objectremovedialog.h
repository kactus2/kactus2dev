//-----------------------------------------------------------------------------
// File: ObjectRemoveDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 08.08.2011
//
// Description:
// Dialog for selecting items to be removed.
//-----------------------------------------------------------------------------

#ifndef OBJECTREMOVEDIALOG_H
#define OBJECTREMOVEDIALOG_H

#include "ObjectSelectionDialog.h"

//-----------------------------------------------------------------------------
//! Dialog for selecting items to be removed.
//-----------------------------------------------------------------------------
class ObjectRemoveDialog : public ObjectSelectionDialog
{
    Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this dialog.
	 *      @param [in] f       Widget flags for the dialog.
	 *
	 */
	ObjectRemoveDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
	
	/*!
     *  The destructor
     */
	virtual ~ObjectRemoveDialog();

protected:

    /*!
     *  Setup the layout for the dialog.
     */
    virtual void setupLayout();

private:

	//! No copying. No assignment.
	ObjectRemoveDialog(const ObjectRemoveDialog& other);
	ObjectRemoveDialog& operator=(const ObjectRemoveDialog& other);
};

#endif // OBJECTREMOVEDIALOG_H
