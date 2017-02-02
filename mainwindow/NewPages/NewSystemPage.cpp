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

#include <library/LibraryManager/libraryitem.h>
#include <library/LibraryManager/libraryinterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: NewSystemPage::NewSystemPage()
//-----------------------------------------------------------------------------
NewSystemPage::NewSystemPage(LibraryInterface* libInterface, QWidget* parent):
NewPage(libInterface, VLNV::COMPONENT, tr("New System"), tr("Creates SW architecture that can be mapped to HW"), parent),
    optionGroupBox_(new QGroupBox(tr("Mapping options"), this)),
    unmapRadioButton_(new QRadioButton(tr("Leave initially unmapped"), this)),
    mapRadioButton_(new QRadioButton(tr("Map to specific HW now"), this)),
    compTreeWidget_(new QTreeWidget(this)),             
    viewComboBox_(new QComboBox(this))
{
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::SYSTEM);
    vlnvEditor_->addNameExtension(".sysdesign");
    vlnvEditor_->addNameExtension(".sysdesigncfg");

    QButtonGroup* actionGroup(new QButtonGroup(this));
    actionGroup->addButton(unmapRadioButton_);
    actionGroup->addButton(mapRadioButton_);

    QFont buttonFont = unmapRadioButton_->font();
    buttonFont.setBold(true);    
    unmapRadioButton_->setFont(buttonFont);
    mapRadioButton_->setFont(buttonFont);

    QString buttonStyle = QStringLiteral("QRadioButton::indicator { width: 15px; height: 15px; }");
    unmapRadioButton_->setStyleSheet(buttonStyle);
    mapRadioButton_->setStyleSheet(buttonStyle);

    initializeHWComponentSelectionWidget(libInterface);

    unmapRadioButton_->setChecked(true);

    setupLayout();

    connect(actionGroup, SIGNAL(buttonClicked(QAbstractButton*)),
        this, SLOT(actionChanged(QAbstractButton*)), Qt::UniqueConnection);

    connect(compTreeWidget_, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
        this, SLOT(onTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::~NewSystemPage()
//-----------------------------------------------------------------------------
NewSystemPage::~NewSystemPage()
{

}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::prevalidate()
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

        return (!data.isNull() && data.value<VLNV>().isValid() && NewPage::prevalidate());
    }
    else
    {
        return NewPage::prevalidate();
    }
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::validate()
//-----------------------------------------------------------------------------
bool NewSystemPage::validate()
{
    Q_ASSERT(prevalidate());

    VLNV vlnv = vlnvEditor_->getVLNV();
    bool validVLNV = NewPage::validate();

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".sysdesign", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".sysdesigncfg", vlnv.getVersion());

    // Check if any of the VLNVs already exists.
    if (unmapRadioButton_->isChecked() && !validVLNV)
    {
        showErrorForReservedVLVN(vlnv);
        return false;
    }

    if (!isUnusedVLNV(designVLNV))
    {
        showErrorForReservedVLVN(designVLNV);
        return false;
    }

    if (!isUnusedVLNV(desConfVLNV))
    {
        showErrorForReservedVLVN(desConfVLNV);        
        return false;
    }    

    return true;
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::apply()
//-----------------------------------------------------------------------------
void NewSystemPage::apply()
{
    // Create the system in a different way based on the mapping option selection.
    if (mapRadioButton_->isChecked())
    {
        Q_ASSERT(compTreeWidget_->currentItem() != 0);
        QVariant data = compTreeWidget_->currentItem()->data(0, Qt::UserRole);

        emit createSystem(data.value<VLNV>(), viewComboBox_->currentText(),
            vlnvEditor_->getVLNV(), selectedPath());
    }
    else
    {
        emit createSystem(VLNV(), QString(), vlnvEditor_->getVLNV(), selectedPath());
    }
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::onPageChange()
//-----------------------------------------------------------------------------
bool NewSystemPage::onPageChange()
{
    // Discard the VLNV and collapse the tree widget.
    compTreeWidget_->collapseAll();
    compTreeWidget_->setCurrentItem(0);

    return NewPage::onPageChange();
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::onTreeItemChanged()
//-----------------------------------------------------------------------------
void NewSystemPage::onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem*)
{
    if (current != 0)
    {
        // Check if the item has a VLNV (i.e. is a leaf item).
        QVariant data = current->data(0, Qt::UserRole);

        if (!data.isNull())
        {
            VLNV vlnv = data.value<VLNV>();

            // Add all available hierarchical views to the view combo box.
            QSharedPointer<Component> component = libInterface_->getModel(vlnv).staticCast<Component>();

            viewComboBox_->clear();
            viewComboBox_->addItems(component->getHierViews());
        }
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::actionChanged()
//-----------------------------------------------------------------------------
void NewSystemPage::actionChanged(QAbstractButton* button)
{
    if (button == unmapRadioButton_)
    {
        compTreeWidget_->setCurrentItem(0);
        viewComboBox_->clear();
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::initializeHWComponentSelectionWidget()
//-----------------------------------------------------------------------------
void NewSystemPage::initializeHWComponentSelectionWidget(LibraryInterface* libInterface)
{
    compTreeWidget_->setColumnCount(1);
    compTreeWidget_->setSelectionBehavior(QAbstractItemView::SelectItems);
    compTreeWidget_->header()->close();
    compTreeWidget_->setIconSize(QSize(20, 20));

    LibraryItem const* rootItem = libInterface->getTreeRoot();

    const int itemCount = rootItem->getNumberOfChildren();
    for (int i = 0; i < itemCount; i++)
    {
        QTreeWidgetItem* child = createChildRecursively(rootItem->child(i));
        if (child)
        {
            compTreeWidget_->addTopLevelItem(child);
        }
    }

    compTreeWidget_->sortItems(0, Qt::AscendingOrder);
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::createChildRecursively()
//-----------------------------------------------------------------------------
QTreeWidgetItem* NewSystemPage::createChildRecursively(LibraryItem const* libraryItem) const
{
    // Only hierarchical HW components are added.
    if (!libraryItem->hasChildren())
    {
        VLNV vlnv = libraryItem->getVLNV();

        if (vlnv.isValid() && vlnv.getType() == VLNV::COMPONENT)
        {
            QSharedPointer<Component> component = libInterface_->getModel(vlnv).staticCast<Component>();

            if (component->getImplementation() == KactusAttribute::HW && !component->getHierViews().isEmpty())
            {
                QTreeWidgetItem* leafItem = new QTreeWidgetItem();
                leafItem->setText(0, libraryItem->name());

                leafItem->setData(0, Qt::UserRole, QVariant::fromValue(vlnv));
                leafItem->setIcon(0, QIcon(":icons/common/graphics/hier-hw-component.png"));

                return leafItem;
            }
        }
    }

    // Recursively add its children if this is not the leaf level.
    else
    {
        QTreeWidgetItem* intermediateItem = new QTreeWidgetItem();
        intermediateItem->setText(0, libraryItem->name());

        const int childCount = libraryItem->getNumberOfChildren();
        for (int i = 0; i < childCount; i++)
        {
            QTreeWidgetItem* childItem = createChildRecursively(libraryItem->child(i));  
            if (childItem)
            {
                intermediateItem->addChild(childItem);
            }
        }
        
        // Discard items with no content to show.
        if (intermediateItem->childCount() == 0)
        {
            delete intermediateItem;
            intermediateItem = 0;
        }

        return intermediateItem;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::setupLayout()
//-----------------------------------------------------------------------------
void NewSystemPage::setupLayout()
{
    QString labelStyle = QStringLiteral("QLabel { padding-left: 19px; }");

    QLabel* unmappedLabel = new QLabel(tr("Creates an unmapped SW architecture. "
        "A new system component will be created."), this);
    unmappedLabel->setStyleSheet(labelStyle);
    unmappedLabel->setWordWrap(true);

    QLabel* mappedLabel = new QLabel(tr("Creates a SW architecture and maps it to selected top-level HW. "
        "A system view will be created for the HW component to reference the new system design."), this);
    mappedLabel->setStyleSheet(labelStyle);
    mappedLabel->setWordWrap(true);

    QLabel* treeLabel = new QLabel(tr("Select top-level HW component:"), this);

    QVBoxLayout* optionsLayout = new QVBoxLayout(optionGroupBox_);
    optionsLayout->addWidget(unmapRadioButton_);
    optionsLayout->addWidget(unmappedLabel);
    optionsLayout->addWidget(mapRadioButton_);
    optionsLayout->addWidget(mappedLabel);
    optionsLayout->addWidget(treeLabel);
    optionsLayout->addWidget(compTreeWidget_, 1);

    QHBoxLayout* viewLayout = new QHBoxLayout();
    QLabel* viewLabel = new QLabel(tr("Select view of top-level HW component:"), this);
    viewLayout->addWidget(viewLabel);
    viewLayout->addWidget(viewComboBox_, 1);
    
    optionsLayout->addLayout(viewLayout);

    // Finalize inherited NewPage layout.
    QVBoxLayout* topLayout = dynamic_cast<QVBoxLayout*>(layout());
    topLayout->addWidget(optionGroupBox_);

    treeLabel->setDisabled(true);
    compTreeWidget_->setDisabled(true);
    viewLabel->setDisabled(true);
    viewComboBox_->setDisabled(true);

    connect(mapRadioButton_, SIGNAL(toggled(bool)), treeLabel, SLOT(setEnabled(bool)), Qt::UniqueConnection);
    connect(mapRadioButton_, SIGNAL(toggled(bool)), compTreeWidget_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
    connect(mapRadioButton_, SIGNAL(toggled(bool)), viewLabel, SLOT(setEnabled(bool)), Qt::UniqueConnection);
    connect(mapRadioButton_, SIGNAL(toggled(bool)), viewComboBox_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
}
