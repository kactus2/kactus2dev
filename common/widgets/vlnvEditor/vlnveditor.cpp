//-----------------------------------------------------------------------------
// File: vlnveditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.2.2011
//
// Description:
// VLNV editor group box.
//-----------------------------------------------------------------------------

#include "vlnveditor.h"

#include <common/widgets/assistedLineEdit/AssistedLineEdit.h>
#include <common/validators/nameValidator/namevalidator.h>
#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/libraryitem.h>

#include "VLNVContentMatcher.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: VLNVEditor()
//-----------------------------------------------------------------------------
VLNVEditor::VLNVEditor(VLNV::IPXactType type, LibraryInterface* libHandler, QWidget* parentWnd,
					   QWidget* parent, bool compact) : 
QGroupBox(parent), m_type(type),
m_rootItem(0), m_vendorEdit(0),
m_vendorMatcher(), m_libraryEdit(0),
m_libraryMatcher(), m_nameEdit(0),
m_nameMatcher(), m_versionEdit(0),
m_versionMatcher(),
handler_(libHandler)
{
    Q_ASSERT(libHandler != 0);
    Q_ASSERT(m_type != VLNV::INVALID);

    // Retrieve the correct sub-tree from the library handler based on the IP-XACT type.
    LibraryItem const* root = libHandler->getTreeRoot();

    for (int i = 0; i < root->getNumberOfChildren(); ++i)
    {
        if (root->child(i)->getName() == VLNV::type2Print(m_type))
        {
            m_rootItem = root->child(i);
            break;
        }
    }
    
    // Set group box settings.
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setTitle(tr("VLNV"));

    // Initialize the widgets and the connections between widgets.
    initWidgets(parentWnd, compact);
    initConnections();

	// accept drops from drag & drop
	setAcceptDrops(true);
}

//-----------------------------------------------------------------------------
// Function: ~VLNVEditor()
//-----------------------------------------------------------------------------
VLNVEditor::~VLNVEditor()
{
}

//-----------------------------------------------------------------------------
// Function: setVLNV()
//-----------------------------------------------------------------------------
void VLNVEditor::setVLNV(VLNV const* vlnv)
{
    // Set the fields according to the VLNV.
    m_vendorEdit->setText(vlnv->getVendor());
    m_libraryEdit->setText(vlnv->getLibrary());
    m_nameEdit->setText(vlnv->getName());
    m_versionEdit->setText(vlnv->getVersion());

    // Update the matcher items.
    updateLibraryMatcherItem();
}

void VLNVEditor::setVLNV( const VLNV& vlnv ) {
	setVLNV(&vlnv);
}

//-----------------------------------------------------------------------------
// Function: getVLNV()
//-----------------------------------------------------------------------------
VLNV VLNVEditor::getVLNV() const
{
    return VLNV(m_type, m_vendorEdit->text(), m_libraryEdit->text(),
                m_nameEdit->text(), m_versionEdit->text());
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool VLNVEditor::isValid() const
{
    return (!m_vendorEdit->text().isEmpty() && !m_libraryEdit->text().isEmpty() &&
            !m_nameEdit->text().isEmpty() && !m_versionEdit->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: updateLibraryMatcherItem()
//-----------------------------------------------------------------------------
void VLNVEditor::updateLibraryMatcherItem()
{
    LibraryItem const* foundItem = 0;
  
    if (m_rootItem != 0)
    {
        for (int i = 0; i < m_rootItem->getNumberOfChildren(); ++i)
        {
            LibraryItem const* child = m_rootItem->child(i);

            if (child->getName() == m_vendorEdit->text())
            {
                foundItem = child;
                break;
            }
        }
    }

    m_libraryMatcher->setLibraryItem(foundItem);

    // Update "recursively" also the name matcher item.
    updateNameMatcherItem();
}

//-----------------------------------------------------------------------------
// Function: updateNameMatcherItem()
//-----------------------------------------------------------------------------
void VLNVEditor::updateNameMatcherItem()
{
    LibraryItem const* libItem = m_libraryMatcher->getLibraryItem();
    LibraryItem const* foundItem = 0;

    if (libItem != 0)
    {
        for (int i = 0; i < libItem->getNumberOfChildren(); ++i)
        {
            LibraryItem const* child = libItem->child(i);

            if (child->getName() == m_libraryEdit->text())
            {
                foundItem = child;
                break;
            }
        }
    }

    m_nameMatcher->setLibraryItem(foundItem);

    // Update "recursively" also the version matcher item.
    updateVersionMatcherItem();
}

//-----------------------------------------------------------------------------
// Function: updateVersionMatcherItem()
//-----------------------------------------------------------------------------
void VLNVEditor::updateVersionMatcherItem()
{
    LibraryItem const* nameItem = m_nameMatcher->getLibraryItem();
    LibraryItem const* foundItem = 0;

    if (nameItem != 0)
    {
        for (int i = 0; i < nameItem->getNumberOfChildren(); ++i)
        {
            LibraryItem const* child = nameItem->child(i);

            if (child->getName() == m_nameEdit->text())
            {
                foundItem = child;
                break;
            }
        }
    }

    m_versionMatcher->setLibraryItem(foundItem);
}

//-----------------------------------------------------------------------------
// Function: initWidgets()
//-----------------------------------------------------------------------------
void VLNVEditor::initWidgets(QWidget* parentWnd, bool compact)
{
    // Create the labels.
    QLabel* vendorLabel = new QLabel(tr("Vendor:"), this);
    QLabel* libraryLabel = new QLabel(tr("Library:"), this);
    QLabel* nameLabel = new QLabel(tr("Name:"), this);
    QLabel* versionLabel = new QLabel(tr("Version:"), this);

    // Create the matchers.
    m_vendorMatcher = QSharedPointer<VLNVContentMatcher>(new VLNVContentMatcher(m_rootItem));
    m_libraryMatcher = QSharedPointer<VLNVContentMatcher>(new VLNVContentMatcher(0));
    m_nameMatcher = QSharedPointer<VLNVContentMatcher>(new VLNVContentMatcher(0));
    m_versionMatcher = QSharedPointer<VLNVContentMatcher>(new VLNVContentMatcher(0));
    
    // Create the line edits.
    m_vendorEdit = new AssistedLineEdit(m_vendorMatcher, parentWnd, this);
    m_vendorEdit->setValidator(new NameValidator(this));
	m_vendorEdit->setProperty("mandatoryField", true);

    m_libraryEdit = new AssistedLineEdit(m_libraryMatcher, parentWnd, this);
    m_libraryEdit->setValidator(new NameValidator(this));
	m_libraryEdit->setProperty("mandatoryField", true);

    m_nameEdit = new AssistedLineEdit(m_nameMatcher, parentWnd, this);
	m_nameEdit->setValidator(new NameValidator(this, true));
	m_nameEdit->setProperty("mandatoryField", true);

    m_versionEdit = new AssistedLineEdit(m_versionMatcher, parentWnd, this);
	m_versionEdit->setValidator(new NameValidator(this, true));
	m_versionEdit->setProperty("mandatoryField", true);
    
    // Create the layout and add the widgets to it.
    if (compact)
    {
        QGridLayout* layout = new QGridLayout(this);
        layout->addWidget(vendorLabel, 0, 0, 1, 1);
        layout->addWidget(m_vendorEdit, 0, 1, 1, 1);
        layout->addWidget(libraryLabel, 1, 0, 1, 1);
        layout->addWidget(m_libraryEdit, 1, 1, 1, 1);
        layout->addWidget(nameLabel, 2, 0, 1, 1);
        layout->addWidget(m_nameEdit, 2, 1, 1, 1);
        layout->addWidget(versionLabel, 3, 0, 1, 1);
        layout->addWidget(m_versionEdit, 3, 1, 1, 1);
    }
    else
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(vendorLabel);
        layout->addWidget(m_vendorEdit);
        layout->addWidget(libraryLabel);
        layout->addWidget(m_libraryEdit);
        layout->addWidget(nameLabel);
        layout->addWidget(m_nameEdit);
        layout->addWidget(versionLabel);
        layout->addWidget(m_versionEdit);
    }
}

//-----------------------------------------------------------------------------
// Function: initConnections()
//-----------------------------------------------------------------------------
void VLNVEditor::initConnections()
{
    // Connect the signals informing of changes in items.
    connect(m_vendorEdit, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(m_libraryEdit, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(m_nameEdit, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(m_versionEdit, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    // Connect the matcher update slots to the textChanged signals.
    connect(m_vendorEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateLibraryMatcherItem()), Qt::UniqueConnection);
    connect(m_libraryEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateNameMatcherItem()), Qt::UniqueConnection);
    connect(m_nameEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateVersionMatcherItem()), Qt::UniqueConnection);
}

bool VLNVEditor::isEmpty() const {
	return (m_vendorEdit->text().isEmpty() && m_libraryEdit->text().isEmpty() &&
		m_nameEdit->text().isEmpty() && m_versionEdit->text().isEmpty());
}

void VLNVEditor::setVendor( const QString& vendor ) {
	m_vendorEdit->setText(vendor);
}

void VLNVEditor::setLibrary( const QString& library ) {
	m_libraryEdit->setText(library);
}

void VLNVEditor::setName( const QString& name ) {
	m_nameEdit->setText(name);
}

void VLNVEditor::setVersion( const QString& version ) {
	m_versionEdit->setText(version);
}

void VLNVEditor::dropEvent( QDropEvent* event ) {
	// Retrieve the vlnv.
	VLNV *vlnv;
	memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));
	setVLNV(vlnv);
	event->acceptProposedAction();
}

void VLNVEditor::dragEnterEvent( QDragEnterEvent* event ) {
	if (event->mimeData()->hasFormat("data/vlnvptr")) {
		
		// Retrieve the vlnv.
		VLNV *vlnv;
		memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));

		// if the vlnv is of correct type
		if (handler_->getDocumentType(*vlnv) == m_type)
			event->acceptProposedAction();
	}
}
