//-----------------------------------------------------------------------------
// File: CatalogEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.02.2017
//
// Description:
// Editor for editing Catalogs.
//-----------------------------------------------------------------------------

#include "CatalogEditor.h"

#include "CatalogFileColumns.h"
#include "CatalogFileDelegate.h"
#include "CatalogFileFilter.h"
#include "CatalogFileModel.h"
#include "CatalogFileView.h"

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/validators/CatalogValidator.h>

#include <common/views/EditableTableView/editabletableview.h>
#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <QCoreApplication>
#include <QMessageBox>
#include <QGridLayout>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: CatalogEditor::CatalogEditor()
//-----------------------------------------------------------------------------
CatalogEditor::CatalogEditor(LibraryInterface* libHandler, QSharedPointer<Catalog> catalog, QWidget* parent):
TabDocument(parent, libHandler, DOC_PROTECTION_SUPPORT),
    catalog_(catalog),
    documentNameGroupEditor_(new DocumentNameGroupEditor(this)),
    fileModel_(new CatalogFileModel(libHandler, this)),
    fileView_(new CatalogFileView(this))
{
    // Set the document name and type.
    VLNV vlnv = catalog_->getVlnv();
    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
    setDocumentType(DocumentType(DocumentTypes::CATALOG));

    documentNameGroupEditor_->setDocumentNameGroup(catalog_, libHandler->getPath(vlnv));
    documentNameGroupEditor_->setTitle(tr("Catalog"));
   
    CatalogFileFilter* proxy = new CatalogFileFilter(catalog_->getRevision(), this);
    proxy->setSourceModel(fileModel_);

    fileView_->setModel(proxy);
    fileView_->setItemDelegate(new CatalogFileDelegate(libHandler, this));
    
    refresh();

    setupLayout();

    connect(documentNameGroupEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(fileModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(fileView_, SIGNAL(addItem(QModelIndex const&)),
        proxy, SLOT(onAddItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(proxy, SIGNAL(addItem(QModelIndex const&)),
        fileModel_, SLOT(onAddItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(fileView_, SIGNAL(removeItem(QModelIndex const&)),
        proxy, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(proxy, SIGNAL(removeItem(QModelIndex const&)),
        fileModel_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(fileView_, SIGNAL(openItem(QModelIndex const&)),
        proxy, SLOT(onOpenItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(proxy, SIGNAL(openItem(QModelIndex const&)),
        fileModel_, SLOT(onOpenItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(fileModel_, SIGNAL(openCatalog(VLNV const&)),
        this, SIGNAL(openCatalog(VLNV const&)), Qt::UniqueConnection);
    connect(fileModel_, SIGNAL(openComponent(VLNV const&)),
        this, SIGNAL(openComponent(VLNV const&)), Qt::UniqueConnection);
    connect(fileModel_, SIGNAL(openBus(VLNV const&)),
        this, SIGNAL(openBus(VLNV const&)), Qt::UniqueConnection);
    connect(fileModel_, SIGNAL(stdRevisionMismatch()), 
        this, SLOT(showStdRevisionMismatchWarning()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::~CatalogEditor()
//-----------------------------------------------------------------------------
CatalogEditor::~CatalogEditor()
{
}


//-----------------------------------------------------------------------------
// Function: CatalogEditor::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV CatalogEditor::getDocumentVLNV() const
{
    return catalog_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV CatalogEditor::getIdentifyingVLNV() const
{
    return getDocumentVLNV();
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::setProtection()
//-----------------------------------------------------------------------------
void CatalogEditor::setProtection(bool locked)
{
    documentNameGroupEditor_->setDisabled(locked);
    fileModel_->setProtection(locked);
    fileView_->setDisabled(locked);
    
    TabDocument::setProtection(locked);
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::refresh()
//-----------------------------------------------------------------------------
void CatalogEditor::refresh()
{
    VLNV vlnv = catalog_->getVlnv();
    catalog_ = getLibHandler()->getModel(vlnv).staticCast<Catalog>();

    // Update the editors.
    documentNameGroupEditor_->setDocumentNameGroup(catalog_, getLibHandler()->getPath(vlnv));
    fileModel_->refresh(catalog_);

    for (int row = 0; row <= CatalogFileColumns::CATEGORY_COUNT; row++)
    {
        fileView_->setFirstColumnSpanned(row, QModelIndex(), true);
    }

    fileView_->expandAll();

    for (int column = 0; column <= CatalogFileColumns::COLUMN_COUNT; column++)
    {
        fileView_->resizeColumnToContents(column);
    }

    setModified(false);
    TabDocument::refresh();
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::validate()
//-----------------------------------------------------------------------------
bool CatalogEditor::validate(QVector<QString>& errorList)
{
	CatalogValidator validator;
    validator.findErrorsIn(errorList, catalog_);

    return errorList.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::save()
//-----------------------------------------------------------------------------
bool CatalogEditor::save()
{
    getLibHandler()->writeModelToFile(catalog_);

    return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::saveAs()
//-----------------------------------------------------------------------------
bool CatalogEditor::saveAs()
{
    // Ask the user for a new VLNV and directory.
    VLNV vlnv;
    QString directory;

    if (!NewObjectDialog::saveAsDialog(parentWidget(), getLibHandler(), catalog_->getVlnv(), vlnv, directory))
    {
        return false;
    }

    // Create a copy of the object and update its VLNV.
    catalog_ = QSharedPointer<Catalog>(new Catalog(*catalog_));

    vlnv.setType(VLNV::CATALOG);
    catalog_->setVlnv(vlnv);

    if (getLibHandler()->writeModelToFile(directory, catalog_))
    {
        setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
        return TabDocument::saveAs();
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::showStdRevisionMismatchWarning()
//-----------------------------------------------------------------------------
void CatalogEditor::showStdRevisionMismatchWarning()
{
    QMessageBox::warning(this, QCoreApplication::applicationName(),
        tr("Dropped item cannot use different IP-XACT standard revision than the item being edited."),
        QMessageBox::Close, QMessageBox::Close);
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::showEvent()
//-----------------------------------------------------------------------------
void CatalogEditor::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);
    if (catalog_->getRevision() == Document::Revision::Std14)
    {
        emit helpUrlRequested("definitions/catalog.html");
    }
    else if (catalog_->getRevision() == Document::Revision::Std22)
    {
        emit helpUrlRequested("definitions/catalog2022.html");
    }
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::setupLayout()
//-----------------------------------------------------------------------------
void CatalogEditor::setupLayout()
{
    QGroupBox* filesBox = new QGroupBox(tr("Catalog files"), this);

    QVBoxLayout* filesLayout = new QVBoxLayout(filesBox);
    filesLayout->addWidget(fileView_);

    QHBoxLayout* docNameGroupAndFilesLayout = new QHBoxLayout(this);
    docNameGroupAndFilesLayout->addWidget(documentNameGroupEditor_);
    docNameGroupAndFilesLayout->addWidget(filesBox);

    QWidget* docNameGroupAndFiles = new QWidget(this);
    docNameGroupAndFiles->setLayout(docNameGroupAndFilesLayout);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(new SummaryLabel(tr("Catalog"), this), 0, Qt::AlignCenter);
    topLayout->addWidget(docNameGroupAndFiles);
}
