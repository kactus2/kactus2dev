//-----------------------------------------------------------------------------
// File: TabDocument.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.5.2011
//
// Description:
// Tab document base class.
//-----------------------------------------------------------------------------

#include "TabDocument.h"

#include <QMessageBox>
#include <QCoreApplication>
#include <QTimer>

//-----------------------------------------------------------------------------
// Function: TabDocument::TabDocument()
//-----------------------------------------------------------------------------
TabDocument::TabDocument(QWidget* parent, unsigned int flags, int minZoomLevel, int maxZoomLevel) : 
QWidget(parent),
supportedWindows_(OUTPUTWINDOW | PREVIEWWINDOW | LIBRARYWINDOW | CONTEXT_HELP_WINDOW),
flags_(flags), 
modified_(false),
locked_(false),
zoomLevel_(100),
maxZoomLevel_(maxZoomLevel),
minZoomLevel_(minZoomLevel),
title_(""),
docName_(""), 
previouslyUnlocked_(false),
relatedVLNVs_(),
refreshRequested_(false)
{
    connect(this, SIGNAL(contentChanged()), this, SLOT(setModified()));
}

//-----------------------------------------------------------------------------
// Function: TabDocument::~TabDocument()
//-----------------------------------------------------------------------------
TabDocument::~TabDocument()
{
}

//-----------------------------------------------------------------------------
// Function: TabDocument::applySettings()
//-----------------------------------------------------------------------------
void TabDocument::applySettings(QSettings& settings)
{

}

//-----------------------------------------------------------------------------
// Function: TabDocument::setDocumentName()
//-----------------------------------------------------------------------------
void TabDocument::setDocumentName(QString const& name)
{
    docName_ = name;
    updateTabTitle();
}

//-----------------------------------------------------------------------------
// Function: TabDocument::setDocumentType()
//-----------------------------------------------------------------------------
void TabDocument::setDocumentType(QString const& type)
{
    if (type.isEmpty())
    {
        docType_ = "";
    }
    else
    {
        docType_ = " [" + type + "]";
    }

    updateTabTitle();
}

//-----------------------------------------------------------------------------
// Function: TabDocument::fitInView()
//-----------------------------------------------------------------------------
void TabDocument::fitInView()
{

}

//-----------------------------------------------------------------------------
// Function: TabDocument::setZoomLevel()
//-----------------------------------------------------------------------------
void TabDocument::setZoomLevel(int level)
{
    level = qMin(qMax(level, minZoomLevel_), maxZoomLevel_);

    if (flags_ & DOC_ZOOM_SUPPORT)
    {
        zoomLevel_ = level;
    }
}

//-----------------------------------------------------------------------------
// Function: TabDocument::setMode()
//-----------------------------------------------------------------------------
void TabDocument::setMode(DrawMode)
{

}

//-----------------------------------------------------------------------------
// Function: TabDocument::setProtection()
//-----------------------------------------------------------------------------
void TabDocument::setProtection(bool locked)
{
    locked_ = locked;

    if (!locked_)
    {
        previouslyUnlocked_ = true;
    }
}

//-----------------------------------------------------------------------------
// Function: TabDocument::setModified()
//-----------------------------------------------------------------------------
void TabDocument::setModified(bool modified)
{
	if (modified == modified_)
    {
		return;
    }

    if (modified)
    {
        setTabTitle(docName_ + docType_ + "*");
    }
    else
    {
        setTabTitle(docName_ + docType_);
    }

    modified_ = modified;
	emit modifiedChanged(modified_);
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getDocumentName()
//-----------------------------------------------------------------------------
QString TabDocument::getDocumentName() const
{
    return docName_;
}

//-----------------------------------------------------------------------------
// Function: getZoomLevel()
//-----------------------------------------------------------------------------
int TabDocument::getZoomLevel() const
{
    return zoomLevel_;
}

//-----------------------------------------------------------------------------
// Function: getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int TabDocument::getSupportedDrawModes() const
{
    return MODE_NONE;
}

//-----------------------------------------------------------------------------
// Function: isProtected()
//-----------------------------------------------------------------------------
bool TabDocument::isProtected() const
{
    return locked_;
}

//-----------------------------------------------------------------------------
// Function: getFlags()
//-----------------------------------------------------------------------------
unsigned int TabDocument::getFlags() const
{
    return flags_;
}

//-----------------------------------------------------------------------------
// Function: isModified()
//-----------------------------------------------------------------------------
bool TabDocument::isModified() const
{
    return modified_;
}


//-----------------------------------------------------------------------------
// Function: TabDocument::validate()
//-----------------------------------------------------------------------------
bool TabDocument::validate(QStringList&)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: save()
//-----------------------------------------------------------------------------
bool TabDocument::save()
{
    setModified(false);
    emit documentSaved(this);
    return true;
}

//-----------------------------------------------------------------------------
// Function: saveAs()
//-----------------------------------------------------------------------------
bool TabDocument::saveAs()
{
	setModified(false);
	return true;
}

//-----------------------------------------------------------------------------
// Function: print()
//-----------------------------------------------------------------------------
void TabDocument::print()
{
}

//-----------------------------------------------------------------------------
// Function: getMaxZoomLevel()
//-----------------------------------------------------------------------------
int TabDocument::getMaxZoomLevel() const
{
    return maxZoomLevel_;
}

//-----------------------------------------------------------------------------
// Function: getMinZoomLevel()
//-----------------------------------------------------------------------------
int TabDocument::getMinZoomLevel() const
{
    return minZoomLevel_;
}

//-----------------------------------------------------------------------------
// Function: askSaveFile()
//-----------------------------------------------------------------------------
bool TabDocument::askSaveFile() const {

	QMessageBox::StandardButton button = QMessageBox::question(parentWidget(), 
		tr("Save changes to file?"), 
		tr("Would you like to save the changes you have made to the file?"),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	// return true if user clicked yes
	return QMessageBox::Yes == button;
}

//-----------------------------------------------------------------------------
// Function: updateTabTitle()
//-----------------------------------------------------------------------------
void TabDocument::updateTabTitle()
{
    // Update also the title.
    if (isModified())
    {
        setTabTitle(docName_ + docType_ + "*");
    }
    else
    {
        setTabTitle(docName_ + docType_);
    }
}

//-----------------------------------------------------------------------------
// Function: TabDocument::setTabTitle()
//-----------------------------------------------------------------------------
void TabDocument::setTabTitle(QString const& title)
{
    emit titleChanged(this, title);
    title_ = title;
}

VLNV TabDocument::getDocumentVLNV() const 
{
	return VLNV();
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getEditProvider()
//-----------------------------------------------------------------------------
IEditProvider* TabDocument::getEditProvider()
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::refresh()
//-----------------------------------------------------------------------------
void TabDocument::refresh()
{
	emit refreshed();
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getSupportedWindows()
//-----------------------------------------------------------------------------
unsigned int TabDocument::getSupportedWindows() const 
{
	return supportedWindows_;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::setPreviouslyUnlocked()
//-----------------------------------------------------------------------------
void TabDocument::setPreviouslyUnlocked()
{
    previouslyUnlocked_ = true;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::isPreviouslyUnlocked()
//-----------------------------------------------------------------------------
bool TabDocument::isPreviouslyUnlocked() const
{
    return previouslyUnlocked_;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::setVisibilityControlState()
//-----------------------------------------------------------------------------
void TabDocument::setVisibilityControlState(QString const& name, bool state)
{
    if (visibilityControls_.contains(name))
    {
        visibilityControls_.insert(name, state);
    }
}

//-----------------------------------------------------------------------------
// Function: TabDocument::addVisibilityControl()
//-----------------------------------------------------------------------------
void TabDocument::addVisibilityControl(QString const& name, bool state)
{
    visibilityControls_.insert(name, state);
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getVisibilityControls()
//-----------------------------------------------------------------------------
QMap<QString, bool> const& TabDocument::getVisibilityControls() const
{
    return visibilityControls_;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::addRelatedVLNV()
//-----------------------------------------------------------------------------
void TabDocument::addRelatedVLNV(VLNV const& vlnv)
{
    unsigned int count = relatedVLNVs_.value(vlnv, 0);
    relatedVLNVs_.insert(vlnv, count + 1);
}

//-----------------------------------------------------------------------------
// Function: TabDocument::removeRelatedVLNV()
//-----------------------------------------------------------------------------
void TabDocument::removeRelatedVLNV(VLNV const& vlnv)
{
    unsigned int count = relatedVLNVs_.value(vlnv, 0);

    if (count > 0)
    {
        if (count == 1)
        {
            relatedVLNVs_.remove(vlnv);
        }
        else
        {
            relatedVLNVs_.insert(vlnv, count - 1);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getRelatedVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> TabDocument::getRelatedVLNVs() const
{
    return relatedVLNVs_.keys();
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getTitle()
//-----------------------------------------------------------------------------
QString TabDocument::getTitle() const
{
    return title_;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::requestRefresh()
//-----------------------------------------------------------------------------
void TabDocument::requestRefresh()
{
    refreshRequested_ = true;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::showEvent()
//-----------------------------------------------------------------------------
void TabDocument::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (refreshRequested_)
    {
        QTimer::singleShot(20, this, SLOT(handleRefreshRequest()));
        refreshRequested_ = false;
    }
}

//-----------------------------------------------------------------------------
// Function: TabDocument::clearRelatedVLNVs()
//-----------------------------------------------------------------------------
void TabDocument::clearRelatedVLNVs()
{
    relatedVLNVs_.clear();
}

//-----------------------------------------------------------------------------
// Function: TabDocument::handleRefreshRequest()
//-----------------------------------------------------------------------------
void TabDocument::handleRefreshRequest()
{
    if (isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("Related documents contain modifications that affect this document."
               "Save changes and refresh?"),
            QMessageBox::Yes | QMessageBox::No, this);

        if (msgBox.exec() == QMessageBox::Yes)
        {
            save();
            refresh();
        }
    }
    else
    {
        refresh();
    }
}

//-----------------------------------------------------------------------------
// Function: TabDocument::onErrorDialog()
//-----------------------------------------------------------------------------
void TabDocument::onErrorDialog(const QString& message)
{
	QMessageBox::warning(this, tr("Component editor"), message);
}
