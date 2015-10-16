//-----------------------------------------------------------------------------
// File: targetnameedit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 8.2.2011
//
// Description:
// TargetNameEdit is a line-edit to select a file path for target file.
//-----------------------------------------------------------------------------

#include "targetnameedit.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/Component/Component.h>

#include <QFileDialog>
#include <QMouseEvent>

//-----------------------------------------------------------------------------
// Function: targetnameedit::TargetNameEdit()
//-----------------------------------------------------------------------------
TargetNameEdit::TargetNameEdit(QWidget *parent, LibraryInterface* handler, QSharedPointer<Component> component,
    const QString contents): 
QLineEdit(contents, parent), 
handler_(handler),
component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: targetnameedit::~TargetNameEdit()
//-----------------------------------------------------------------------------
TargetNameEdit::~TargetNameEdit()
{

}

//-----------------------------------------------------------------------------
// Function: targetnameedit::mousePressEvent()
//-----------------------------------------------------------------------------
void TargetNameEdit::mousePressEvent( QMouseEvent * e )
{
	// event is handled only if the left mouse button was pressed
	if (e->button() != Qt::LeftButton)
    {
		e->ignore();
		return;
	}

	// accept the event so it wont be passed to parent
	e->accept();

	const QString baseLocation = handler_->getPath(component_->getVlnv());

	QString file = QFileDialog::getSaveFileName(this, tr("Set a target file"), baseLocation);

	if (file.isEmpty())
    {
		return;
	}

	QString relPath = General::getRelativeSavePath(baseLocation, file);

	// set the relative path to the lineEdit
	setText(relPath);
	emit contentChanged();
}
