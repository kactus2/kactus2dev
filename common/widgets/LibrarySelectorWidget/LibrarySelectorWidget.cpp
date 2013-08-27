//-----------------------------------------------------------------------------
// File: LibrarySelectorWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.06.2013
//
// Description:
// Widget for viewing and parsing a vhdl file.
//-----------------------------------------------------------------------------

#include "LibrarySelectorWidget.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>

#include <common/widgets/LibrarySelectorWidget/LibraryPathSelector/librarypathselector.h>
#include <common/dialogs/LibrarySettingsDialog/LibrarySettingsDialog.h>

//-----------------------------------------------------------------------------
// Function: LibrarySelectorWidget()
//-----------------------------------------------------------------------------
LibrarySelectorWidget::LibrarySelectorWidget(QWidget* parent):
    QWidget(parent),
    librarySelector_(new LibraryPathSelector(this)),
    browseButton_(new QPushButton(tr("Browse"),this)),
    directorySet_(false)
{

    // Pushbutton connections.
    connect(browseButton_, SIGNAL(clicked()),this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(librarySelector_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(librarySelector_, SIGNAL(editTextChanged(QString const&)), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ~LibrarySelectorWidget()
//-----------------------------------------------------------------------------
LibrarySelectorWidget::~LibrarySelectorWidget() {
}

QString LibrarySelectorWidget::getPath() const
{
    return librarySelector_->currentText();
}

//-----------------------------------------------------------------------------
// Function: ~isValid()
//-----------------------------------------------------------------------------
void LibrarySelectorWidget::reset()
{
    directorySet_ = false;
    librarySelector_->refresh();
}

//-----------------------------------------------------------------------------
// Function: ~isValid()
//-----------------------------------------------------------------------------
bool LibrarySelectorWidget::isValid() const
{
    return librarySelector_->isValid();
}

//-----------------------------------------------------------------------------
// Function: onBrowse()
//-----------------------------------------------------------------------------
void LibrarySelectorWidget::onBrowse()
{
    QString baseDirectory = QFileInfo(librarySelector_->currentText()).filePath();
    if ( baseDirectory.size() < 1 )
    {
        baseDirectory = librarySelector_->currentLocation();
    }

    QString targetDirectory = QFileDialog::getExistingDirectory(this, tr("Choose Target Directory"),
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
        QMessageBox warningDialog(QMessageBox::Warning, 
            "Warning", "Chosen path is not in any active library. Do you want to configure libraries?"
            , QMessageBox::Yes | QMessageBox::Cancel, this);

        warningDialog.setDetailedText(targetDirectory + 
            "\nis not in any of the active libraries:\n" + 
            librarySelector_->getLibraryLocations().join("\n"));

        warningDialog.exec();

        QMessageBox::StandardButton clicked = warningDialog.standardButton(warningDialog.clickedButton());

        if (clicked == QMessageBox::Yes) {
            LibrarySettingsDialog dialog(settings, this);
            dialog.exec();
            librarySelector_->refresh();
        }
        else //if( msg == QMessageBox::Cancel ){
        {
            return;
        }
    }
    
    librarySelector_->setIndexFor(targetDirectory);
    directorySet_ = true;
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void LibrarySelectorWidget::updatePath(QString const& path)
{
    if ( !directorySet_ )
    {
        QString dir = librarySelector_->currentLocation();

        librarySelector_->setEditText(dir + path);
    }
}

//-----------------------------------------------------------------------------
// Function: setupLayout()
//-----------------------------------------------------------------------------
void LibrarySelectorWidget::setupLayout()
{
    // Create the directory line edit and label.
    QLabel *directoryLabel = new QLabel(tr("Directory:"), this);

    QHBoxLayout *widgetLayout = new QHBoxLayout(this);
    widgetLayout->addWidget(directoryLabel);
    widgetLayout->addWidget(librarySelector_, 1);
    widgetLayout->addWidget(browseButton_);
}