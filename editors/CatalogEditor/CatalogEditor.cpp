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

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/validators/CatalogValidator.h>

#include <common/views/EditableTableView/editabletableview.h>
#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <QGridLayout>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: CatalogEditor::CatalogEditor()
//-----------------------------------------------------------------------------
CatalogEditor::CatalogEditor(LibraryInterface* library, QSharedPointer<Catalog> catalog, QWidget* parent):
TabDocument(parent, DOC_PROTECTION_SUPPORT),
      library_(library),
      catalog_(catalog),
      vlnvDisplay_(new VLNVDisplayer(this, catalog->getVlnv())),
      descriptionEditor_(new QTextEdit(this)),
      fileModel_(new CatalogFileModel(library, this)),
      fileView_(new CatalogFileView(this))
{
    // Set the document name and type.
    VLNV vlnv = catalog_->getVlnv();
    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
    setDocumentType(tr("Catalog"));

    vlnvDisplay_->setFlat(false);
    vlnvDisplay_->setTitle(tr("Catalog VLNV and location"));
    vlnvDisplay_->setPath(library_->getPath(vlnv));
   
    CatalogFileFilter* proxy = new CatalogFileFilter(this);   
    proxy->setSourceModel(fileModel_);

    fileView_->setModel(proxy);
    fileView_->setItemDelegate(new CatalogFileDelegate(library_, this));
    
    refresh();

    setupLayout();

    connect(descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

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
    connect(fileModel_, SIGNAL(openBus(VLNV const&, VLNV const&)),
        this, SIGNAL(openBus(VLNV const&, VLNV const&)), Qt::UniqueConnection);
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
    descriptionEditor_->setDisabled(locked);
    fileModel_->setProtection(locked);
    
    TabDocument::setProtection(locked);
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::refresh()
//-----------------------------------------------------------------------------
void CatalogEditor::refresh()
{
    VLNV vlnv = catalog_->getVlnv();
    catalog_ = library_->getModel(vlnv).staticCast<Catalog>();

    // Update the editors.
    vlnvDisplay_->setVLNV(vlnv);
    descriptionEditor_->setPlainText(catalog_->getDescription());
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
    library_->writeModelToFile(catalog_);

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

    if (!NewObjectDialog::saveAsDialog(parentWidget(), library_, catalog_->getVlnv(), vlnv, directory))
    {
        return false;
    }

    // Create a copy of the object and update its VLNV.
    catalog_ = QSharedPointer<Catalog>(new Catalog(*catalog_));

    vlnv.setType(VLNV::CATALOG);
    catalog_->setVlnv(vlnv);

    if (library_->writeModelToFile(directory, catalog_))
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
// Function: CatalogEditor::showEvent()
//-----------------------------------------------------------------------------
void CatalogEditor::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);
    emit helpUrlRequested("definitions/catalog.html");
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::onDescriptionChanged()
//-----------------------------------------------------------------------------
void CatalogEditor::onDescriptionChanged()
{
    catalog_->setDescription(descriptionEditor_->toPlainText());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: CatalogEditor::setupLayout()
//-----------------------------------------------------------------------------
void CatalogEditor::setupLayout()
{
    QGroupBox* descriptionBox = new QGroupBox(tr("Description"), this);

    QVBoxLayout* descriptionLayout = new QVBoxLayout(descriptionBox);
    descriptionLayout->addWidget(descriptionEditor_);

    QGroupBox* filesBox = new QGroupBox(tr("Catalog files"), this);

    QVBoxLayout* filesLayout = new QVBoxLayout(filesBox);
    filesLayout->addWidget(fileView_);

    QGridLayout* topLayout = new QGridLayout(this);
    topLayout->addWidget(vlnvDisplay_, 0, 0, 1, 1);
    topLayout->addWidget(descriptionBox, 0, 1, 1, 1);
    topLayout->addWidget(filesBox, 1, 0, 1, 2);

    topLayout->setColumnStretch(0, 1);
    topLayout->setColumnStretch(1, 1);
    topLayout->setRowStretch(0, 1);
    topLayout->setRowStretch(1, 10);
}
