//-----------------------------------------------------------------------------
// File: CompileConflictDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.10.2015
//
// Description:
// Dialog for presenting files that conflict in compilation.
//-----------------------------------------------------------------------------

#ifndef CompileConflictDIALOG_H
#define CompileConflictDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QLabel>

#include "MakefileParser.h"
#include "QTableWidget"

//-----------------------------------------------------------------------------
//! CompileConflictDialog class.
//-----------------------------------------------------------------------------
class CompileConflictDialog : public QDialog
{
public:
    /*!
     *  Constructor.
     */
    CompileConflictDialog(QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~CompileConflictDialog();

private:
    // Disable copying.
    CompileConflictDialog(CompileConflictDialog const& rhs);
    CompileConflictDialog& operator=(CompileConflictDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The description label.
    QLabel descLabel_;

	//! Table used to display all detected conflicts and their participants.
	QTableWidget conflictTable_;
};

//-----------------------------------------------------------------------------

#endif // CompileConflictDIALOG_H
