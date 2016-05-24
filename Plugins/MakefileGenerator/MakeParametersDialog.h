//-----------------------------------------------------------------------------
// File: MakeParametersDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.10.2015
//
// Description:
// Dialog for presenting parameters, which may be chosen for makefile generation.
//-----------------------------------------------------------------------------

#ifndef MAKEPARAMETERSDIALOG_H
#define MAKEPARAMETERSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QLabel>

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

#include "MakefileParser.h"
#include "QTableWidget"

//-----------------------------------------------------------------------------
//! CompileConflictDialog class.
//-----------------------------------------------------------------------------
class MakeParametersDialog : public QDialog
{
public:
    /*!
     *  Constructor.
     */
    MakeParametersDialog(QStringList replacedFiles,
		QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~MakeParametersDialog();

private:
    // Disable copying.
    MakeParametersDialog(MakeParametersDialog const& rhs);
    MakeParametersDialog& operator=(MakeParametersDialog const& rhs);
};

//-----------------------------------------------------------------------------

#endif // MAKEPARAMETERSDIALOG_H
