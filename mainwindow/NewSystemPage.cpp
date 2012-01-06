//-----------------------------------------------------------------------------
// File: NewSystemPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.6.2011
//
// Description:
// New system property page.
//-----------------------------------------------------------------------------

#include "NewSystemPage.h"

#include <LibraryManager/libraryitem.h>
#include <LibraryManager/libraryinterface.h>

#include <models/component.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: NewSystemPage()
//-----------------------------------------------------------------------------
NewSystemPage::NewSystemPage(LibraryInterface* libInterface, QWidget* parentDlg) : PropertyPageView(),
                                                                                   libInterface_(libInterface),
                                                                                   compTreeWidget_(0),
                                                                                   vlnvEditor_(0),
                                                                                   directoryEdit_(0),
                                                                                   viewComboBox_(0)
{
    // Create the title and description labels labels.
    QLabel* titleLabel = new QLabel(tr("New System"), this);

    QFont font = titleLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel->setFont(font);

    QLabel* descLabel = new QLabel(tr("Creates a system design for an existing component"), this);

    // Retrieve the component sub-tree item.
    LibraryItem const* root = libInterface->getTreeRoot();
    LibraryItem const* compRoot = 0;

    for (int i = 0; i < root->getNumberOfChildren(); ++i)
    {
        if (root->child(i)->getName() == VLNV::type2Print(VLNV::COMPONENT))
        {
            compRoot = root->child(i);
            break;
        }
    }

    // Tree widget label.
    QLabel* treeLabel = new QLabel(tr("Select component:"), this);

    // Create the tree widget and fill it with VLNV data.
    compTreeWidget_ = new QTreeWidget(this);
    compTreeWidget_->setColumnCount(1);
    compTreeWidget_->setSelectionBehavior(QAbstractItemView::SelectItems);
    compTreeWidget_->header()->close();

    if (compRoot != 0)
    {
        for (int i = 0; i < compRoot->getNumberOfChildren(); ++i)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, compRoot->child(i)->getName());

            // Add child items.
            addChildItems(compRoot->child(i), item);

            // Add only items that have children.
            if (item->childCount() == 0)
            {
                delete item;
                continue;
            }

            // Add the item to the tree.
            compTreeWidget_->addTopLevelItem(item);
        }

        compTreeWidget_->sortItems(0, Qt::AscendingOrder);
    }

    connect(compTreeWidget_, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(onTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    // Create the view selection combo box.
    QLabel* viewLabel = new QLabel(tr("Select configuration:"), this);
    viewComboBox_ = new QComboBox(this);

    // Create the VLNV editor.
    vlnvEditor_ = new VLNVEditor(VLNV::COMPONENT, libInterface, parentDlg, this, true);
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::KTS_SYS);
    vlnvEditor_->updateFiltering();

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

    // Create the directory line edit and label.
    QLabel *directoryLabel = new QLabel(tr("Directory:"), this);
 
    QSettings settings;
    QString defaultDir = settings.value("general/defaultDir", QCoreApplication::applicationDirPath()).toString();
    directoryEdit_ = new QLineEdit(defaultDir, this);
    connect(directoryEdit_, SIGNAL(textChanged(QString const&)), this, SIGNAL(contentChanged()));

    QPushButton* pathButton = new QPushButton("Browse...", this);
    connect(pathButton, SIGNAL(clicked()), this, SLOT(selectDirectory()));
    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(directoryLabel);
    pathLayout->addWidget(directoryEdit_, 1);
    pathLayout->addWidget(pathButton);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addWidget(descLabel);
    layout->addSpacing(12);
    layout->addWidget(treeLabel);
    layout->addWidget(compTreeWidget_);
    layout->addWidget(viewLabel);
    layout->addWidget(viewComboBox_);
    layout->addWidget(vlnvEditor_);
    layout->addSpacing(12);
    layout->addLayout(pathLayout);
    layout->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: ~NewSystemPage()
//-----------------------------------------------------------------------------
NewSystemPage::~NewSystemPage()
{

}

//-----------------------------------------------------------------------------
// Function: prevalidate()
//-----------------------------------------------------------------------------
bool NewSystemPage::prevalidate() const
{
    if (compTreeWidget_->currentItem() == 0)
    {
        return false;
    }

    QVariant data = compTreeWidget_->currentItem()->data(0, Qt::UserRole);

    return (!data.isNull() && data.value<VLNV>().isValid() &&
            vlnvEditor_->isValid() && !directoryEdit_->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewSystemPage::validate()
{
    Q_ASSERT(prevalidate());

    VLNV vlnv = vlnvEditor_->getVLNV();

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

    // Check if any of the VLNVs already exists.
    if (libInterface_->contains(vlnv))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The system cannot be created because the VLNV %1 already exists in the library.").arg(vlnv.toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    if (libInterface_->contains(designVLNV))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The system cannot be created because the VLNV %1 already exists in the library.").arg(designVLNV.toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    if (libInterface_->contains(desConfVLNV))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The system cannot be created because the VLNV %1 already exists in the library.").arg(desConfVLNV.toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }    

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewSystemPage::apply()
{
    Q_ASSERT(compTreeWidget_->currentItem() != 0);
    QVariant data = compTreeWidget_->currentItem()->data(0, Qt::UserRole);

    emit createSystem(data.value<VLNV>(), viewComboBox_->currentText(),
                      vlnvEditor_->getVLNV(), directoryEdit_->text());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewSystemPage::onPageChange()
{
    // Discard the VLNV and collapse the tree widget.
    vlnvEditor_->setVLNV(VLNV());
    compTreeWidget_->collapseAll();
    compTreeWidget_->setCurrentItem(0);
    return true;
}

//-----------------------------------------------------------------------------
// Function: selectDirectory()
//-----------------------------------------------------------------------------
void NewSystemPage::selectDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                    directoryEdit_->text());

    if (!dir.isEmpty())
    {
        directoryEdit_->setText(dir);
    }
}

//-----------------------------------------------------------------------------
// Function: onTreeItemChanged()
//-----------------------------------------------------------------------------
void NewSystemPage::onTreeItemChanged(QTreeWidgetItem* cur, QTreeWidgetItem*)
{
    if (cur != 0)
    {
        // Check if the item has a VLNV (i.e. is a leaf item).
        QVariant data = cur->data(0, Qt::UserRole);
        
        if (!data.isNull())
        {
            VLNV compVLNV = data.value<VLNV>();
            VLNV sysVLNV = vlnvEditor_->getVLNV();

            // Auto-fill the VLNV editor information (vendor and library fields).
            vlnvEditor_->setVLNV(VLNV(VLNV::DESIGN, compVLNV.getVendor(), compVLNV.getLibrary(),
                                 sysVLNV.getName(), sysVLNV.getVersion()));

            // Add all available hierarchical views to the view combo box.
            QSharedPointer<LibraryComponent> libComp = libInterface_->getModel(compVLNV);
            QSharedPointer<Component> comp = libComp.staticCast<Component>();

            viewComboBox_->clear();

            foreach (QString const& viewName, comp->getHierViews())
            {
                viewComboBox_->addItem(viewName);
            }
        }
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: addChildItems()
//-----------------------------------------------------------------------------
void NewSystemPage::addChildItems(LibraryItem const* libItem, QTreeWidgetItem* treeItem)
{
    for (int i = 0; i < libItem->getNumberOfChildren(); ++i)
    {
        // Create a child tree widget item.
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, libItem->child(i)->getName());

        VLNV* vlnv = libItem->child(i)->getVLNV();

        // Recursively add its children if this is not the leaf level.
        if (vlnv == 0)
        {
            addChildItems(libItem->child(i), item);

            // Add the item as a child of the given tree item if it has children.
            if (item->childCount() > 0)
            {
                treeItem->addChild(item);
            }
            else
            {
                delete item;
            }
        }
        else
        {
            // Only hierarchical HW components are added.
            QSharedPointer<LibraryComponent> libComp = libInterface_->getModel(*vlnv);
            QSharedPointer<Component> comp = libComp.staticCast<Component>();

            QStringList views = comp->getHierViews();
            views.removeAll("kts_sw_ref");
            views.removeAll("kts_sys_ref");

            if (comp->getComponentImplementation() != KactusAttribute::KTS_HW || views.empty())
            {
                delete item;
                continue;
            }

            // Add the VLNV to the item's data.
            item->setData(0, Qt::UserRole, QVariant::fromValue((*vlnv)));
            treeItem->addChild(item);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewSystemPage::updateDirectory()
{
    QSettings settings;
    QString dir = settings.value("general/defaultDir", QCoreApplication::applicationDirPath()).toString();

    VLNV vlnv = vlnvEditor_->getVLNV();

    if (!vlnv.getVendor().isEmpty())
    {
        dir += "/" + vlnv.getVendor();

        if (!vlnv.getLibrary().isEmpty())
        {
            dir += "/" + vlnv.getLibrary();

            if (!vlnv.getName().isEmpty())
            {
                dir += "/" + vlnv.getName();

                if (!vlnv.getVersion().isEmpty())
                {
                    dir += "/" + vlnv.getVersion();
                }
            }
        }
    }

    directoryEdit_->setText(dir);
}
