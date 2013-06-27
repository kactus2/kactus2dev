//-----------------------------------------------------------------------------
// File: ComponentWizardDependencyPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Dependency analysis page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardDependencyPage.h"

#include "ComponentWizard.h"

#include <IPXactWrapper/ComponentEditor/fileSet/filesetsdelegate.h>

#include <common/KactusAttribute.h>
#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: ComponentWizardDependencyPage::ComponentWizardDependencyPage()
//-----------------------------------------------------------------------------
ComponentWizardDependencyPage::ComponentWizardDependencyPage(PluginManager& pluginMgr, ComponentWizard* parent)
    : QWizardPage(parent),
      parent_(parent),
      splitter_(Qt::Vertical, this),
      view_(&splitter_),
      model_(parent->getComponent(), this),
      proxy_(this),
      editor_(parent->getComponent(), parent->getBasePath(), pluginMgr, &splitter_)
{
    setTitle(tr("Dependency Analysis & File Sets"));
    setSubTitle(tr("Check for missing files with dependency analysis and create file sets."));
    setFinalPage(true);

    editor_.setCompact(true);
    editor_.setDependenciesEditable(false);

    splitter_.addWidget(&view_);
    splitter_.addWidget(&editor_);
    splitter_.setStretchFactor(1, 1);

    proxy_.setSourceModel(&model_);

    QString defPath = parent->getBasePath() + "/fileSetList.csv";

    view_.setAllowImportExport(true);
    view_.setDefaultImportExportPath(defPath);
    view_.setModel(&proxy_);
    view_.setItemDelegate(new FileSetsDelegate(this));
    view_.setItemsDraggable(false);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&splitter_);

    connect(&editor_, SIGNAL(fileSetAdded(FileSet*)),
            &model_, SLOT(onFileSetAdded(FileSet*)), Qt::UniqueConnection);

    connect(&model_, SIGNAL(contentChanged()),
            &editor_, SLOT(refresh()), Qt::UniqueConnection);

    connect(&editor_, SIGNAL(scanStarted()),
            this, SIGNAL(completeChanged()), Qt::UniqueConnection);
    connect(&editor_, SIGNAL(scanCompleted()),
            this, SIGNAL(completeChanged()), Qt::UniqueConnection);

    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
            &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
            &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
    //connect(&editor_, SIGNAL(fileSetsUpdated()), this, SLOT(updateFileSetView()));

    QList<int> sizes;
    sizes << 150 << 300;
    splitter_.setSizes(sizes);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardDependencyPage::~ComponentWizardDependencyPage()
//-----------------------------------------------------------------------------
ComponentWizardDependencyPage::~ComponentWizardDependencyPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardDependencyPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardDependencyPage::nextId() const
{
     if (parent_->getComponent()->getComponentImplementation() == KactusAttribute::KTS_HW)
    {
        return ComponentWizard::PAGE_VHDL_IMPORT;
    }
    else
    {
        return ComponentWizard::PAGE_CONCLUSION;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardDependencyPage::initializePage()
//-----------------------------------------------------------------------------
void ComponentWizardDependencyPage::initializePage()
{
    // Clear file sets.
    QList< QSharedPointer<FileSet> > fileSets;
    parent_->getComponent()->setFileSets(fileSets);
    model_.refresh();

    // Start the scan.
    editor_.scan();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardDependencyPage::isComplete()
//-----------------------------------------------------------------------------
bool ComponentWizardDependencyPage::isComplete() const
{
    return (!editor_.isScanning());
}
