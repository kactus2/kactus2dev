//-----------------------------------------------------------------------------
// File: ObjectSelectionDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.07.2017
//
// Description:
// Dialog for selecting the items.
//-----------------------------------------------------------------------------

#include "ObjectSelectionDialog.h"

#include "ObjectSelectionListItem.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::ObjectSelectionDialog()
//-----------------------------------------------------------------------------
ObjectSelectionDialog::ObjectSelectionDialog(QString const& okButtonText, QString const& iconPath,
    QString const& okIconPath, QString const& windowTitle, QWidget* parent, Qt::WindowFlags f):
QDialog(parent, f),
dialogIconPath_(iconPath),
okButton_(okButtonText, this),
cancelButton_(tr("Cancel"), this),
checkAllBox_(tr("Select/deselect all"), this),
itemList_(this),
busDefinitionIcon_(":icons/common/graphics/bus-def.png"),
abstractionDefinition_(":icons/common/graphics/abs-def.png"),
componentIcon_(":icons/common/graphics/hw-component.png"),
designIcon_(":icons/common/graphics/hw-design.png"),
designConfigurationIcon_(":icons/common/graphics/configuration.png"),
generatorChainIcon_(":icons/common/graphics/generatorChain.png"),
abstractorIcon_(":icons/common/graphics/connect.png"),
catalogIcon_(":icons/common/graphics/catalog.png"),
comDefinitionIcon_(":icons/common/graphics/new-com_definition.png"),
apiDefinitionIcon_(":icons/common/graphics/new-api_definition.png"),
fileIcon_(":icons/common/graphics/file.png")
{
    okButton_.setIcon(QIcon(okIconPath));
    checkAllBox_.setChecked(true);

    setWindowTitle(windowTitle);
    connectSignals();
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::~ObjectSelectionDialog()
//-----------------------------------------------------------------------------
ObjectSelectionDialog::~ObjectSelectionDialog()
{

}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::connectSignals()
//-----------------------------------------------------------------------------
void ObjectSelectionDialog::connectSignals()
{
    connect(&okButton_, SIGNAL(clicked(bool)), this, SLOT(accept()), Qt::UniqueConnection);
    connect(&cancelButton_, SIGNAL(clicked(bool)), this, SLOT(reject()), Qt::UniqueConnection);
    connect(&checkAllBox_, SIGNAL(stateChanged(int)), this, SLOT(onCheckAllChanged(int)), Qt::UniqueConnection);
    connect(&itemList_, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(onItemClicked(QListWidgetItem*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::sizeHint()
//-----------------------------------------------------------------------------
QSize ObjectSelectionDialog::sizeHint() const
{
	return QSize(700, 700);
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::createItem()
//-----------------------------------------------------------------------------
void ObjectSelectionDialog::createItem(const QString& path, const VLNV& vlnv, bool locked)
{
    if (!itemExistsInList(path, vlnv))
    {
        QIcon usedIcon = getUsedIcon(vlnv);

        ObjectSelectionListItem* newItem = new ObjectSelectionListItem(usedIcon, path, locked, vlnv);
        itemList_.addItem(newItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::itemExistsInList()
//-----------------------------------------------------------------------------
bool ObjectSelectionDialog::itemExistsInList(QString const& itemPath, VLNV const& itemVLNV) const
{
    for (int i = 0; i < itemList_.count(); ++i)
    {
        ObjectSelectionListItem* listItem = dynamic_cast<ObjectSelectionListItem*>(itemList_.item(i));
        if (listItem && listItem->getPath() == itemPath && listItem->getVLNV() == itemVLNV)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::getUsedIcon()
//-----------------------------------------------------------------------------
QIcon ObjectSelectionDialog::getUsedIcon(const VLNV& itemVLNV) const
{
    QIcon usedIcon = fileIcon_;

    if (itemVLNV.isValid())
    {
        if (itemVLNV.getType() == VLNV::BUSDEFINITION)
        {
            usedIcon = busDefinitionIcon_;
        }
        else if (itemVLNV.getType() == VLNV::ABSTRACTIONDEFINITION)
        {
            usedIcon = abstractionDefinition_;
        }
        else if (itemVLNV.getType() == VLNV::COMPONENT)
        {
            usedIcon = componentIcon_;
        }
        else if (itemVLNV.getType() == VLNV::DESIGN)
        {
            usedIcon = designIcon_;
        }
        else if (itemVLNV.getType() == VLNV::DESIGNCONFIGURATION)
        {
            usedIcon = designConfigurationIcon_;
        }
        else if (itemVLNV.getType() == VLNV::GENERATORCHAIN)
        {
            usedIcon = generatorChainIcon_;
        }
        else if (itemVLNV.getType() == VLNV::ABSTRACTOR)
        {
            usedIcon = abstractorIcon_;
        }
        else if (itemVLNV.getType() == VLNV::CATALOG)
        {
            usedIcon = catalogIcon_;
        }
        else if (itemVLNV.getType() == VLNV::COMDEFINITION)
        {
            usedIcon = comDefinitionIcon_;
        }
        else if (itemVLNV.getType() == VLNV::APIDEFINITION)
        {
            usedIcon = apiDefinitionIcon_;
        }
    }

    return usedIcon;
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::onCheckAllChanged()
//-----------------------------------------------------------------------------
void ObjectSelectionDialog::onCheckAllChanged(int checkAllState)
{
    Qt::CheckState newCheckState = Qt::Checked;
    if (checkAllState == Qt::Unchecked)
    {
        newCheckState = Qt::Unchecked;
    }

    for (int listIndex = 0; listIndex < itemList_.count(); ++listIndex)
    {
        ObjectSelectionListItem* listItem = dynamic_cast<ObjectSelectionListItem*>(itemList_.item(listIndex));
        if (listItem && !listItem->itemIsLocked())
        {
            listItem->setCheckState(newCheckState);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::onItemClicked()
//-----------------------------------------------------------------------------
void ObjectSelectionDialog::onItemClicked(QListWidgetItem* currentItem)
{
    ObjectSelectionListItem* listItem = dynamic_cast<ObjectSelectionListItem*>(currentItem);
    if (listItem && !listItem->itemIsLocked())
    {
        Qt::CheckState currentItemCheckState = listItem->checkState();
        if (currentItemCheckState == Qt::Checked)
        {
            listItem->setCheckState(Qt::Unchecked);
        }
        else
        {
            listItem->setCheckState(Qt::Checked);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::getItemsToExport()
//-----------------------------------------------------------------------------
QVector<ObjectSelectionListItem*> ObjectSelectionDialog::getSelectedItems() const
{
    QVector<ObjectSelectionListItem*> exportedItems;

    for (int listIndex = 0; listIndex < itemList_.count(); ++listIndex)
    {
        ObjectSelectionListItem* exportItem = dynamic_cast<ObjectSelectionListItem*>(itemList_.item(listIndex));
        if (exportItem && exportItem->checkState() == Qt::Checked)
        {
            exportedItems.append(exportItem);
        }
    }

    return exportedItems;
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::enableOkButton()
//-----------------------------------------------------------------------------
void ObjectSelectionDialog::enableOkButton()
{
    okButton_.setEnabled(true);
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::disableOkButton()
//-----------------------------------------------------------------------------
void ObjectSelectionDialog::disableOkButton()
{
    okButton_.setEnabled(false);
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::setupIntroWidget()
//-----------------------------------------------------------------------------
QWidget* ObjectSelectionDialog::setupIntroWidget(QString const& introName, QString const& introText) const
{
    QLabel* introLabel = new QLabel(introName);
    QLabel* introTextLabel = new QLabel(introText);
    QFont introFont = introLabel->font();
    introFont.setBold(true);
    introLabel->setFont(introFont);

    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(QPixmap(dialogIconPath_));

    QVBoxLayout* introTextLayout = new QVBoxLayout();
    introTextLayout->addWidget(introLabel);
    introTextLayout->addWidget(introTextLabel);

    QHBoxLayout* mainIntroLayout = new QHBoxLayout();
    mainIntroLayout->addLayout(introTextLayout, 1);
    mainIntroLayout->addWidget(iconLabel);

    QWidget* introWidget = new QWidget();
    introWidget->setStyleSheet("background-color: white");
    introWidget->setLayout(mainIntroLayout);

    return introWidget;
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::setupButtonLayout()
//-----------------------------------------------------------------------------
QHBoxLayout* ObjectSelectionDialog::setupButtonLayout()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(&checkAllBox_, 0, Qt::AlignLeft);
    buttonLayout->addStretch();
    buttonLayout->addWidget(&okButton_, 0, Qt::AlignRight);
    buttonLayout->addWidget(&cancelButton_, 0, Qt::AlignRight);

    return buttonLayout;
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionDialog::getItemList()
//-----------------------------------------------------------------------------
QListWidget* ObjectSelectionDialog::getItemList()
{
    return &itemList_;
}
