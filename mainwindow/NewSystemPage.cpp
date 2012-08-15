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

//-----------------------------------------------------------------------------
// Function: NewSystemPage()
//-----------------------------------------------------------------------------
NewSystemPage::NewSystemPage(LibraryInterface* libInterface, QWidget* parentDlg)
    : PropertyPageView(),
      libInterface_(libInterface),
      actionGroupBox_(new QGroupBox(tr("Mapping Options"), this)),
      actionGroup_(new QButtonGroup(this)),
      emptyRadioButton_(new QRadioButton(tr("Leave initially unmapped\nCreates an unmapped "
                                            "SW architecture. A new system component will be created."), this)),
      mapRadioButton_(new QRadioButton(tr("Map to specific HW now\nCreates a SW architecture "
                                          "and maps it to selected top-level HW. A system view will\n"
                                          "be created for the HW component to reference the new system design."), this)),
      treeLabel_(new QLabel(tr("Select top-level HW component:"), this)),
      compTreeWidget_(new QTreeWidget(this)),
      vlnvEditor_(new VLNVEditor(VLNV::COMPONENT, libInterface, parentDlg, this, true)),
      directoryEdit_(new LibraryPathSelector(this)),
      viewLabel_(new QLabel(tr("Select view of top-level HW component:"), this)),
      viewComboBox_(new QComboBox(this)),
      layout_(new QVBoxLayout(this))
{
    // Create the title and description labels labels.
    QLabel* titleLabel = new QLabel(tr("New System"), this);

    QFont font = titleLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel->setFont(font);

    QLabel* descLabel = new QLabel(tr("Creates SW architecture that can be mapped to HW"), this);

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

    actionGroup_->addButton(emptyRadioButton_);
    actionGroup_->addButton(mapRadioButton_);

    // Create the tree widget and fill it with VLNV data.
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

    // Create the VLNV editor.
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::KTS_SYS);
    vlnvEditor_->setNameExtension(".sysdesign/.sysdesigncfg");
    vlnvEditor_->updateFiltering();
    vlnvEditor_->setTitle(tr("VLNV of the new system design"));

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

    // Create the directory line edit and label.
    QLabel *directoryLabel = new QLabel(tr("Directory:"), this);
 	connect(directoryEdit_, SIGNAL(editTextChanged(QString const&)), this, SIGNAL(contentChanged()));

    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(directoryLabel);
    pathLayout->addWidget(directoryEdit_, 1);

    QVBoxLayout* groupLayout = new QVBoxLayout(actionGroupBox_);
    groupLayout->addWidget(emptyRadioButton_);
    groupLayout->addWidget(mapRadioButton_);

    // Setup the layout.
    layout_->addWidget(titleLabel);
    layout_->addWidget(descLabel);
    layout_->addSpacing(12);
    layout_->addWidget(actionGroupBox_);
    layout_->addWidget(treeLabel_);
    layout_->addWidget(compTreeWidget_);
    layout_->addWidget(viewLabel_);
    layout_->addWidget(viewComboBox_);
    layout_->addWidget(vlnvEditor_);
    layout_->addSpacing(12);
    layout_->addLayout(pathLayout);
    layout_->addStretch(1);

    connect(actionGroup_, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(actionChanged(QAbstractButton*)), Qt::UniqueConnection);

    emptyRadioButton_->setChecked(true);
    actionChanged(emptyRadioButton_);
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
    if (mapRadioButton_->isChecked())
    {
        if (compTreeWidget_->currentItem() == 0)
        {
            return false;
        }

        QVariant data = compTreeWidget_->currentItem()->data(0, Qt::UserRole);

        return (!data.isNull() && data.value<VLNV>().isValid() &&
                vlnvEditor_->isValid() && !directoryEdit_->currentText().isEmpty());
    }
    else
    {
        return (vlnvEditor_->isValid() && !directoryEdit_->currentText().isEmpty());
    }
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewSystemPage::validate()
{
    Q_ASSERT(prevalidate());

    VLNV vlnv = vlnvEditor_->getVLNV();

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".sysdesign", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".sysdesigncfg", vlnv.getVersion());

    // Check if any of the VLNVs already exists.
    if (emptyRadioButton_->isChecked() && libInterface_->contains(vlnv))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("VLNV %1 already exists in the library.").arg(vlnv.toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    if (libInterface_->contains(designVLNV))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("VLNV %1 already exists in the library.").arg(designVLNV.toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    if (libInterface_->contains(desConfVLNV))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("VLNV %1 already exists in the library.").arg(desConfVLNV.toString()),
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
    // Create the system in a different way based on the mapping option selection.
    if (mapRadioButton_->isChecked())
    {
        Q_ASSERT(compTreeWidget_->currentItem() != 0);
        QVariant data = compTreeWidget_->currentItem()->data(0, Qt::UserRole);

        emit createSystem(data.value<VLNV>(), viewComboBox_->currentText(),
            vlnvEditor_->getVLNV(), directoryEdit_->currentText());
    }
    else
    {
        emit createSystem(VLNV(), "", vlnvEditor_->getVLNV(), directoryEdit_->currentText());
    }
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

        VLNV vlnv = libItem->child(i)->getVLNV();

        // Recursively add its children if this is not the leaf level.
        if (!vlnv.isValid())
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
            QSharedPointer<LibraryComponent> libComp = libInterface_->getModel(vlnv);
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
            item->setData(0, Qt::UserRole, QVariant::fromValue((vlnv)));
            treeItem->addChild(item);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewSystemPage::updateDirectory()
{
    QString dir = directoryEdit_->currentLocation();

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

    directoryEdit_->setEditText(dir);
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::actionChanged()
//-----------------------------------------------------------------------------
void NewSystemPage::actionChanged(QAbstractButton* button)
{
    if (button == emptyRadioButton_)
    {
        compTreeWidget_->collapseAll();
        compTreeWidget_->setCurrentItem(0);
        viewComboBox_->clear();
    }

    treeLabel_->setEnabled(button == mapRadioButton_);
    compTreeWidget_->setEnabled(button == mapRadioButton_);

    viewLabel_->setEnabled(button == mapRadioButton_);
    viewComboBox_->setEnabled(button == mapRadioButton_);
    
//     layout_->activate();
    emit contentChanged();
}
