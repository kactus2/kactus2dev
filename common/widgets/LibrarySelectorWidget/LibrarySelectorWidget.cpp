//-----------------------------------------------------------------------------
// File: LibrarySelectorWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.06.2013
//
// Description:
// Widget for selecting a path under library locations.
//-----------------------------------------------------------------------------

#include "LibrarySelectorWidget.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>

#include <common/widgets/LibrarySelectorWidget/LibraryPathSelector/librarypathselector.h>
#include <common/dialogs/LibrarySettingsDialog/LibrarySettingsDialog.h>

//-----------------------------------------------------------------------------
// Function: LibrarySelectorWidget::LibrarySelectorWidget()
//-----------------------------------------------------------------------------
LibrarySelectorWidget::LibrarySelectorWidget(QWidget* parent):
    QWidget(parent),
    librarySelector_(new LibraryPathSelector(this)),
    browseButton_(new QPushButton(tr("Browse"),this)),
    directorySet_(false)
{
    connect(browseButton_, SIGNAL(clicked()),this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(librarySelector_, SIGNAL(contentChanged()), 
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(librarySelector_, SIGNAL(editTextChanged(QString const&)), 
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: LibrarySelectorWidget::~LibrarySelectorWidget()
//-----------------------------------------------------------------------------
LibrarySelectorWidget::~LibrarySelectorWidget()
{
}

//-----------------------------------------------------------------------------
// Function: LibrarySelectorWidget::getPath()
//-----------------------------------------------------------------------------
QString LibrarySelectorWidget::getPath() const
{
    return librarySelector_->currentText();
}

//-----------------------------------------------------------------------------
// Function: LibrarySelectorWidget::reset()
//-----------------------------------------------------------------------------
void LibrarySelectorWidget::reset()
{
    directorySet_ = false;
    librarySelector_->refresh();
}

//-----------------------------------------------------------------------------
// Function: LibrarySelectorWidget::isValid()
//-----------------------------------------------------------------------------
bool LibrarySelectorWidget::isValid() const
{
    return librarySelector_->isValid();
}

//-----------------------------------------------------------------------------
// Function: LibrarySelectorWidget::onBrowse()
//-----------------------------------------------------------------------------
void LibrarySelectorWidget::onBrowse()
{
    QString baseDirectory = QFileInfo(librarySelector_->currentText()).filePath();
    if (baseDirectory.size() < 1)
    {
        baseDirectory = librarySelector_->currentLocation();
    }

    QString targetDirectory = QFileDialog::getExistingDirectory(this, tr("Choose target directory"),
        baseDirectory);

    if (targetDirectory.size() < 1)
    {
        return;
    }

    targetDirectory = QFileInfo(targetDirectory).filePath();

    if (targetDirectory.size() < 1)
    {
        targetDirectory = ".";
    }

    QSettings settings;
    while (!librarySelector_->hasIndexFor(targetDirectory))
    {
        QMessageBox warningDialog(QMessageBox::Warning, "Warning", 
            "Chosen path is not in any active library. Do you want to configure libraries?",
            QMessageBox::Yes | QMessageBox::Cancel, this);

        warningDialog.setDetailedText(targetDirectory + "\nis not in any of the active libraries:\n" + 
            librarySelector_->getLibraryLocations().join("\n"));

        if (warningDialog.exec() == QMessageBox::Yes)
        {
            LibrarySettingsDialog dialog(settings, this);
            dialog.exec();
            librarySelector_->refresh();
        }
        else //if( msg == QMessageBox::Cancel )
        {
            return;
        }
    }
    
    librarySelector_->setIndexFor(targetDirectory);
    directorySet_ = true;
}

//-----------------------------------------------------------------------------
// Function: LibrarySelectorWidget::updateDirectory()
//-----------------------------------------------------------------------------
void LibrarySelectorWidget::updatePath(QString const& path)
{
    if (!directorySet_)
    {
        QString directory = librarySelector_->currentLocation();

        librarySelector_->setEditText(directory + path);
    }
}

//-----------------------------------------------------------------------------
// Function: LibrarySelectorWidget::setupLayout()
//-----------------------------------------------------------------------------
void LibrarySelectorWidget::setupLayout()
{
    QHBoxLayout *widgetLayout = new QHBoxLayout(this);
    widgetLayout->addWidget(new QLabel(tr("Directory:"), this));
    widgetLayout->addWidget(librarySelector_, 1);
    widgetLayout->addWidget(browseButton_);

    widgetLayout->setContentsMargins(0, 0, 0, 0);
}
