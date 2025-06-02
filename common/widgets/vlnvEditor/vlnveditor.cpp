//-----------------------------------------------------------------------------
// File: vlnveditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 7.2.2011
//
// Description:
// VLNV editor group box.
//-----------------------------------------------------------------------------

#include "vlnveditor.h"

#include <common/widgets/assistedLineEdit/AssistedLineEdit.h>

#include <KactusAPI/include/LibraryInterface.h>
#include <KactusAPI/include/LibraryItem.h>
#include <KactusAPI/include/LibraryHandler.h>

#include <IPXACTmodels/common/validators/namevalidator.h>
#include <IPXACTmodels/common/validators/NMTokenValidator.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Design/Design.h>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMimeData>
#include <QSharedPointer>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: VLNVEditor::VLNVEditor()
//-----------------------------------------------------------------------------
VLNVEditor::VLNVEditor(VLNV::IPXactType type, LibraryInterface* libHandler, QWidget* parentWnd, QWidget* parent):
QGroupBox(parent),
type_(type),
contentTypes_(),
dirty_(false),
dataTree_(),
vendorEdit_(0),
vendorMatcher_(),
libraryEdit_(0),
libraryMatcher_(),
nameEdit_(0),
nameExtensionLabel_(this),
nameMatcher_(),
versionEdit_(0),
versionMatcher_(),
handler_(libHandler),
implementationFilter_(KactusAttribute::HW)
{
    Q_ASSERT(libHandler != 0);
    Q_ASSERT(type_ != VLNV::INVALID);

    nameExtensionLabel_.setVisible(false);

    // Set group box settings.
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setTitle(tr("VLNV"));

    // Initialize the widgets and the connections between widgets.
    initWidgets(parentWnd);
    initConnections();

	// accept drops from drag & drop
	setAcceptDrops(true);

    // By default, add the VLNV type to edit to the content types.
    addContentType(type);

    vendorMatcher_.setDataNode(&dataTree_);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::~VLNVEditor()
//-----------------------------------------------------------------------------
VLNVEditor::~VLNVEditor()
{
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::addContentType()
//-----------------------------------------------------------------------------
void VLNVEditor::addContentType(VLNV::IPXactType type)
{
    contentTypes_.append(type);
    dirty_ = true;
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setVLNV()
//-----------------------------------------------------------------------------
void VLNVEditor::setVLNV(const VLNV& vlnv)
{
    if (dirty_)
    {
        updateFiltering();
    }

    // Set the fields according to the VLNV.
    vendorEdit_->setText(vlnv.getVendor());
    libraryEdit_->setText(vlnv.getLibrary());
    nameEdit_->setText(vlnv.getName());
    versionEdit_->setText(vlnv.getVersion());
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::getVLNV()
//-----------------------------------------------------------------------------
VLNV VLNVEditor::getVLNV() const
{
    return VLNV(type_, vendorEdit_->text(), libraryEdit_->text(), nameEdit_->text(), versionEdit_->text());
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::isValid()
//-----------------------------------------------------------------------------
bool VLNVEditor::isValid() const
{
    return (!vendorEdit_->text().isEmpty() && !libraryEdit_->text().isEmpty() &&
            !nameEdit_->text().isEmpty() && !versionEdit_->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::updateMatcherItems()
//-----------------------------------------------------------------------------
void VLNVEditor::updateMatcherItems()
{
    updateLibraryMatcherItem();
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::updateLibraryMatcherItem()
//-----------------------------------------------------------------------------
void VLNVEditor::updateLibraryMatcherItem()
{
    VLNVDataNode const* foundItem = dataTree_.findChild(vendorEdit_->text());
    libraryMatcher_.setDataNode(foundItem);

    // Update "recursively" also the name matcher item.
    updateNameMatcherItem();
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::updateNameMatcherItem()
//-----------------------------------------------------------------------------
void VLNVEditor::updateNameMatcherItem()
{
    VLNVDataNode const* libItem = libraryMatcher_.getDataNode();
    VLNVDataNode const* foundItem = 0;

    if (libItem != 0)
    {
        foundItem = libItem->findChild(libraryEdit_->text());
    }

    nameMatcher_.setDataNode(foundItem);

    // Update "recursively" also the version matcher item.
    updateVersionMatcherItem();
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::updateVersionMatcherItem()
//-----------------------------------------------------------------------------
void VLNVEditor::updateVersionMatcherItem()
{
    VLNVDataNode const* nameItem = nameMatcher_.getDataNode();
    VLNVDataNode const* foundItem = 0;

    if (nameItem != 0)
    {
        foundItem = nameItem->findChild(nameEdit_->text());
    }

    versionMatcher_.setDataNode(foundItem);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::initWidgets()
//-----------------------------------------------------------------------------
void VLNVEditor::initWidgets(QWidget* parentWnd)
{
    // Create the labels.
    QLabel* vendorLabel = new QLabel(tr("Vendor:"), this);
    QLabel* libraryLabel = new QLabel(tr("Library:"), this);
    QLabel* nameLabel = new QLabel(tr("Name:"), this);
    QLabel* versionLabel = new QLabel(tr("Version:"), this);

    // Create the line edits.
    vendorEdit_ = new AssistedLineEdit(parentWnd, this);
    vendorEdit_->setContentMatcher(&vendorMatcher_);
    vendorEdit_->setValidator(new NameValidator(this));
	vendorEdit_->setProperty("mandatoryField", true);

    libraryEdit_ = new AssistedLineEdit(parentWnd, this);
    libraryEdit_->setContentMatcher(&libraryMatcher_);
    libraryEdit_->setValidator(new NameValidator(this));
	libraryEdit_->setProperty("mandatoryField", true);

    nameEdit_ = new AssistedLineEdit(parentWnd, this);
    nameEdit_->setContentMatcher(&nameMatcher_);
	nameEdit_->setValidator(new NMTokenValidator(this));
	nameEdit_->setProperty("mandatoryField", true);

    versionEdit_ = new AssistedLineEdit(parentWnd, this);
    versionEdit_->setContentMatcher(&versionMatcher_);
	versionEdit_->setValidator(new NMTokenValidator(this));
	versionEdit_->setProperty("mandatoryField", true);

    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(nameEdit_, 1);
    nameLayout->addWidget(&nameExtensionLabel_);
    
    // Create the layout and add the widgets to it.
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(vendorLabel, 0, 0, 1, 1);
    layout->addWidget(vendorEdit_, 0, 1, 1, 1);
    layout->addWidget(libraryLabel, 1, 0, 1, 1);
    layout->addWidget(libraryEdit_, 1, 1, 1, 1);
    layout->addWidget(nameLabel, 2, 0, 1, 1);
    layout->addLayout(nameLayout, 2, 1, 1, 1);
    layout->addWidget(versionLabel, 3, 0, 1, 1);
    layout->addWidget(versionEdit_, 3, 1, 1, 1);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::initConnections()
//-----------------------------------------------------------------------------
void VLNVEditor::initConnections()
{
    // Connect the signals informing of changes in items.
    connect(vendorEdit_, SIGNAL(textChanged(QString const&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(libraryEdit_, SIGNAL(textChanged(QString const&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(nameEdit_, SIGNAL(textChanged(QString const&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(versionEdit_, SIGNAL(textChanged(QString const&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    // Connect the signals informing the editing of items.
    connect(vendorEdit_, SIGNAL(editingFinished()), this, SIGNAL(vlnvEdited()), Qt::UniqueConnection);
    connect(libraryEdit_, SIGNAL(editingFinished()), this, SIGNAL(vlnvEdited()), Qt::UniqueConnection);
    connect(nameEdit_, SIGNAL(editingFinished()), this, SIGNAL(vlnvEdited()), Qt::UniqueConnection);
    connect(versionEdit_, SIGNAL(editingFinished()), this, SIGNAL(vlnvEdited()), Qt::UniqueConnection);

    // Connect the matcher update slots to the textChanged signals.
    connect(vendorEdit_, SIGNAL(textChanged(QString const&)),
        this, SLOT(updateLibraryMatcherItem()), Qt::UniqueConnection);
    connect(libraryEdit_, SIGNAL(textChanged(QString const&)),
        this, SLOT(updateNameMatcherItem()), Qt::UniqueConnection);
    connect(nameEdit_, SIGNAL(textChanged(QString const&)),
        this, SLOT(updateVersionMatcherItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::isEmpty()
//-----------------------------------------------------------------------------
bool VLNVEditor::isEmpty() const
{
	return (vendorEdit_->text().isEmpty() && libraryEdit_->text().isEmpty() &&
		nameEdit_->text().isEmpty() && versionEdit_->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setVendor()
//-----------------------------------------------------------------------------
void VLNVEditor::setVendor(QString const& vendor)
{
	vendorEdit_->setText(vendor);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setLibrary()
//-----------------------------------------------------------------------------
void VLNVEditor::setLibrary(QString const& library)
{
	libraryEdit_->setText(library);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setName()
//-----------------------------------------------------------------------------
void VLNVEditor::setName(QString const& name)
{
	nameEdit_->setText(name);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setVersion()
//-----------------------------------------------------------------------------
void VLNVEditor::setVersion(QString const& version)
{
	versionEdit_->setText(version);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::dropEvent()
//-----------------------------------------------------------------------------
void VLNVEditor::dropEvent(QDropEvent* event)
{
	// Retrieve the vlnv.
	QVariant data = event->mimeData()->imageData();
	if (!data.canConvert<VLNV>())
    {
		return;
	}

	VLNV vlnv = data.value<VLNV>();

    // If filtering is enabled and a design/design configuration is dropped, check the implementation.
    VLNV::IPXactType type = handler_->getDocumentType(vlnv);
    if (implementationFilterEnabled_)
    {
        if (type == VLNV::DESIGN)
        {
            if (handler_->getDesign(vlnv)->getDesignImplementation() != implementationFilter_)
            {
                return;
            }

        }
        else if (type == VLNV::DESIGNCONFIGURATION)
        {
            QSharedPointer<Document> libComp = handler_->getModel(vlnv);
            QSharedPointer<DesignConfiguration> designConf = libComp.staticCast<DesignConfiguration>();
            if (designConf->getDesignConfigImplementation() != implementationFilter_)
            {
                return;
            }
        }    
    }

    // Check for revision mismatch on drop, if revision filtering is enabled.
    if (auto document = handler_->getModelReadOnly(vlnv); 
        revisionFilterEnabled_ && document->getRevision() != revisionFilter_)
    {
        QMessageBox::warning(this, QCoreApplication::applicationName(),
            tr("Dropped item cannot use different IP-XACT standard revision than the item being edited."),
            QMessageBox::Close, QMessageBox::Close);
        return;
    }

	setVLNV(vlnv);
	event->acceptProposedAction();

    // for abs def and bus def there is additional option to set the paired vlnv editor
    if (type == VLNV::BUSDEFINITION)
    {
        // if there is only one abs def for the dropped bus def
        QList<VLNV> absDefVLNVs;
        if (handler_->getChildren(absDefVLNVs, vlnv) == 1)
        {
            emit setAbsDef(absDefVLNVs.first());
        }

        // if there are more than one abs defs for the bus def
        else if (absDefVLNVs.size() > 1)
        {
            // if the signal is connected then inform user that the abs def must be
            // selected manually
            if (receivers(SIGNAL(setAbsDef(const VLNV&))) > 0)
            {
                QMessageBox::information(this, QCoreApplication::applicationName(),
                    tr("More than one abstraction definitions exist for the dropped"
                    " bus definition. Select one manually from the library."),
                    QMessageBox::Close, QMessageBox::Close);
            }
        }

    }
    else if(type == VLNV::ABSTRACTIONDEFINITION)
    {
        QSharedPointer<Document> libComp = handler_->getModel(vlnv);
        QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();
        Q_ASSERT(absDef);

        VLNV busDefVLNV = absDef->getBusType();
        emit setBusDef(busDefVLNV);
    }

    emit vlnvEdited();
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::dragEnterEvent()
//-----------------------------------------------------------------------------
void VLNVEditor::dragEnterEvent( QDragEnterEvent* event )
{
	if (event->mimeData()->hasImage())
    {
		// Retrieve the vlnv.
		QVariant data = event->mimeData()->imageData();
		if (!data.canConvert<VLNV>())
        {
			return;
		}

		VLNV vlnv = data.value<VLNV>();
		// if the vlnv is of correct type
		if (contentTypes_.contains(handler_->getDocumentType(vlnv)))
        {
			event->acceptProposedAction();
		}
	}
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setFirmnessFilter()
//-----------------------------------------------------------------------------
void VLNVEditor::setFirmnessFilter(bool on, KactusAttribute::Firmness firmness /*= KTS_TEMPLATE*/)
{
    dataTree_.setFirmnessFilter(on, firmness);
    dirty_ = true;
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setHierarchyFilter()
//-----------------------------------------------------------------------------
void VLNVEditor::setHierarchyFilter(bool on, KactusAttribute::ProductHierarchy productHier /*= KactusAttribute::KTS_IP*/)
{
    dataTree_.setHierarchyFilter(on, productHier);
    dirty_ = true;
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setImplementationFilter()
//-----------------------------------------------------------------------------
void VLNVEditor::setImplementationFilter(bool on, KactusAttribute::Implementation implementation /*= KTS_HW*/)
{
    dataTree_.setImplementationFilter(on, implementation);
    dirty_ = true;
    implementationFilterEnabled_ = on;
    implementationFilter_ = implementation;
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setRevisionFilter()
//-----------------------------------------------------------------------------
void VLNVEditor::setRevisionFilter(bool on, Document::Revision revision /*= Document::Revision::Std22*/)
{
    dataTree_.setRevisionFilter(on, revision);
    dirty_ = true;
    revisionFilterEnabled_ = on;
    revisionFilter_ = revision;
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::updateFiltering()
//-----------------------------------------------------------------------------
void VLNVEditor::updateFiltering()
{
    dataTree_.clear();
    dataTree_.parse(handler_, contentTypes_);
    dirty_ = false;
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::setMandatory()
//-----------------------------------------------------------------------------
void VLNVEditor::setMandatory(bool mandatory)
{
	vendorEdit_->setProperty("mandatoryField", mandatory);
	libraryEdit_->setProperty("mandatoryField", mandatory);
	nameEdit_->setProperty("mandatoryField", mandatory);
	versionEdit_->setProperty("mandatoryField", mandatory);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::addNameExtension()
//-----------------------------------------------------------------------------
void VLNVEditor::addNameExtension(QString const& extension)
{
    if (nameExtensionLabel_.text().isEmpty())
    {
        nameExtensionLabel_.setText(extension);
    }
    else
    {
        nameExtensionLabel_.setText(nameExtensionLabel_.text() + "/" + extension);
    }

    nameExtensionLabel_.setVisible(true);

    dataTree_.addExtensionFilter(extension);
    dirty_ = true;
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::showEvent()
//-----------------------------------------------------------------------------
void VLNVEditor::showEvent(QShowEvent* event)
{
    if (dirty_)
    {
        refresh();
    }

    QGroupBox::showEvent(event);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::focusNextPrevChild()
//-----------------------------------------------------------------------------
bool VLNVEditor::focusNextPrevChild(bool next)
{
    // Function overridden to fix tab and shift+tab behavior when created by delegate of EditableTableView.

    if (next)
    {
        if (vendorEdit_->hasFocus())
        {
            libraryEdit_->setFocus();
            return true;
        }
        else if (libraryEdit_->hasFocus())
        {
            nameEdit_->setFocus();
            return true;
        }
        else if (nameEdit_->hasFocus())
        {
            versionEdit_->setFocus();
            return true;
        }
    }
    else
    {
        if (libraryEdit_->hasFocus())
        {
            vendorEdit_->setFocus();
            return true;
        }
        else if (nameEdit_->hasFocus())
        {
            libraryEdit_->setFocus();
            return true;
        }
        else if (versionEdit_->hasFocus())
        {
            nameEdit_->setFocus();
            return true;
        }
    }

    return QWidget::focusNextPrevChild(next);
}

//-----------------------------------------------------------------------------
// Function: VLNVEditor::refresh()
//-----------------------------------------------------------------------------
void VLNVEditor::refresh()
{
    updateFiltering();
    updateMatcherItems();
}
