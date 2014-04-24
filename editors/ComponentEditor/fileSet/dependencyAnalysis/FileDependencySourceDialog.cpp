//-----------------------------------------------------------------------------
// File: FileDependencySourceDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Tommi Korhonen
// Date: 22.01.2013
//
// Description:
// Dialog for choosing the file dependency source directories.
//-----------------------------------------------------------------------------

#include "FileDependencySourceDialog.h"
#include <IPXACTmodels/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::FileDependencySourceDialog()
//-----------------------------------------------------------------------------
FileDependencySourceDialog::FileDependencySourceDialog(QString const& basePath,
                                                       QStringList const& sourceDirs,
                                                       QWidget* parent)
    : QDialog(parent),
      editor_(basePath, sourceDirs, this)
      //mainGroupBox_(tr("Directories"), this)
{
    setWindowTitle(tr("Import Sources"));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&editor_);
    layout->addWidget(buttonBox);

    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()), Qt::UniqueConnection);

    resize(600, sizeHint().height());
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::~FileDependencySourceDialog()
//-----------------------------------------------------------------------------
FileDependencySourceDialog::~FileDependencySourceDialog()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::getSourceDirectories()
//-----------------------------------------------------------------------------
QStringList FileDependencySourceDialog::getSourceDirectories() const
{
    return editor_.getSourceDirectories();
}
