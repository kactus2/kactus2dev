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
#include "ComponentWizardPages.h"

#include <editors/ComponentEditor/fileSet/filesetsdelegate.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/component.h>

#include <QLabel>

//-----------------------------------------------------------------------------
// Function: ComponentWizardDependencyPage::ComponentWizardDependencyPage()
//-----------------------------------------------------------------------------
ComponentWizardDependencyPage::ComponentWizardDependencyPage(PluginManager const& pluginMgr, ComponentWizard* parent)
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

    // Set file set view and editor settings.
    proxy_.setSourceModel(&model_);

    view_.setModel(&proxy_);
    view_.setItemDelegate(new FileSetsDelegate(this));
    view_.setItemsDraggable(false);
    view_.setAllowImportExport(false);

    editor_.setCompact(true);
    editor_.setDependenciesEditable(false);

    // Create a container widget for the file set view and its label.
    QWidget* container = new QWidget(this);
    
    QVBoxLayout* containerLayout = new QVBoxLayout(container);
    containerLayout->addWidget(new QLabel(tr("File sets:"), container));
    containerLayout->addWidget(&view_, 1);
    containerLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* container2 = new QWidget(this);

    QVBoxLayout* containerLayout2 = new QVBoxLayout(container2);
    containerLayout2->addWidget(new QLabel(tr("Dependency analysis:"), container2));
    containerLayout2->addWidget(&editor_, 1);
    containerLayout2->setContentsMargins(0, 0, 0, 0);

    // Configure the splitter.
    splitter_.addWidget(container);
    splitter_.addWidget(container2);
    splitter_.setStretchFactor(1, 1);

    QList<int> sizes;
    sizes << 150 << 300;
    splitter_.setSizes(sizes);

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
        return ComponentWizardPages::IMPORT;
    }
    else
    {
        return ComponentWizardPages::CONCLUSION;
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
