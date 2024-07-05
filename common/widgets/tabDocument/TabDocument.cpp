//-----------------------------------------------------------------------------
// File: TabDocument.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 4.5.2011
//
// Description:
// Tab document base class.
//-----------------------------------------------------------------------------

#include "TabDocument.h"

#include <QApplication>
#include <QMessageBox>
#include <QCoreApplication>
#include <QTimer>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: TabDocument::TabDocument()
//-----------------------------------------------------------------------------
TabDocument::TabDocument(QWidget* parent, unsigned int flags, int minZoomLevel, int maxZoomLevel) : 
    QWidget(parent),
    supportedWindows_(OUTPUTWINDOW | LIBRARYWINDOW | CONTEXT_HELP_WINDOW ),
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
    refreshRequested_(false),
    docType_(DocumentTypes::EMPTY)
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
    if (isProtected() && !settings.value("General/EnableLocking").toBool())
    {
        setProtection(false);
    }
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
void TabDocument::setDocumentType(DocumentType const& type)
{
    docType_ = type;
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

    modified_ = modified;
    updateTabTitle();

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
// Function: TabDocument::getZoomLevel()
//-----------------------------------------------------------------------------
int TabDocument::getZoomLevel() const
{
    return zoomLevel_;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int TabDocument::getSupportedDrawModes() const
{
    return MODE_NONE;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::isProtected()
//-----------------------------------------------------------------------------
bool TabDocument::isProtected() const
{
    return locked_;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getFlags()
//-----------------------------------------------------------------------------
unsigned int TabDocument::getFlags() const
{
    return flags_;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::isModified()
//-----------------------------------------------------------------------------
bool TabDocument::isModified() const
{
    return modified_;
}


//-----------------------------------------------------------------------------
// Function: TabDocument::validate()
//-----------------------------------------------------------------------------
bool TabDocument::validate(QVector<QString>&)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::save()
//-----------------------------------------------------------------------------
bool TabDocument::save()
{
    setModified(false);
    emit documentSaved(this);
    return true;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::saveAs()
//-----------------------------------------------------------------------------
bool TabDocument::saveAs()
{
	setModified(false);
	return true;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::print()
//-----------------------------------------------------------------------------
void TabDocument::print()
{
}

//-----------------------------------------------------------------------------
// Function: TabDocument::exportImage()
//-----------------------------------------------------------------------------
bool TabDocument::exportImage()
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getMaxZoomLevel()
//-----------------------------------------------------------------------------
int TabDocument::getMaxZoomLevel() const
{
    return maxZoomLevel_;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getMinZoomLevel()
//-----------------------------------------------------------------------------
int TabDocument::getMinZoomLevel() const
{
    return minZoomLevel_;
}

//-----------------------------------------------------------------------------
// Function: TabDocument::askSaveFile()
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
// Function: TabDocument::updateTabTitle()
//-----------------------------------------------------------------------------
void TabDocument::updateTabTitle()
{
    if (isModified())
    {
        if (docType_!=DocumentTypes::EMPTY)
        {
            setTabTitle(docName_ + " [" + docType_.ToString() + "]" + "*");
        }
        else 
        {
            setTabTitle(docName_ + "*");
        }
    }
    else
    {
        if (docType_ != DocumentTypes::EMPTY)
        {
            setTabTitle(docName_ + " [" + docType_.ToString() + "]");
        }
        else
        {
            setTabTitle(docName_);
        }        
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

//-----------------------------------------------------------------------------
// Function: TabDocument::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV TabDocument::getDocumentVLNV() const 
{
	return VLNV();
}

//-----------------------------------------------------------------------------
// Function: TabDocument::getEditProvider()
//-----------------------------------------------------------------------------
QSharedPointer<IEditProvider> TabDocument::getEditProvider() const
{
    return QSharedPointer<IEditProvider>();
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
// Function: TabDocument::getDocType()
//-----------------------------------------------------------------------------
TabDocument::DocumentType TabDocument::getDocType() const
{
    return docType_;
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

    if (count == 1)
    {
        relatedVLNVs_.remove(vlnv);
    }
    else if (count > 0)
    {
        relatedVLNVs_.insert(vlnv, count - 1);
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
        refreshRequested_ = false;
        QTimer::singleShot(20, this, SLOT(handleRefreshRequest()));
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
