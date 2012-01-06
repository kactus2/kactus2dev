//-----------------------------------------------------------------------------
// File: EndpointEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.9.2011
//
// Description:
// Endpoint editor widget for the IP-XACT component editor.
//-----------------------------------------------------------------------------

#include "EndpointEditor.h"

#include "EndpointDelegate.h"
#include "ImportEndpointsDialog.h"

#include <models/component.h>
#include <models/businterface.h>
#include <models/model.h>

#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>

EndpointEditor::EndpointEditor(LibraryInterface* lh, QSharedPointer<Component> component,
                               void* dataPointer, QWidget* parentWnd, QWidget *parent, bool showRemote)
    : ItemEditor(component, parent),
      lh_(lh),
      parentWnd_(parentWnd),
      addRowButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
      removeRowButton_(QIcon(":/icons/graphics/remove.png"), QString(), this),
      analyzeSourceButton_("Analyze Source", this),
      importEndpointsButton_("Import Endpoints", this),
      view_(this),
      model_(component, dataPointer, this)
{
    // TODO: Enable analyze source button when the analyzer is complete.
    analyzeSourceButton_.setEnabled(false);

    // Create a view for the endpoints if it does not exist.
    if (!component->hasView("kts_endpoints"))
    {
        View* view = new View("kts_endpoints");
        view->addEnvIdentifier("");

        component->addView(view);
    }

    connect(&addRowButton_, SIGNAL(clicked(bool)),
        &model_, SLOT(onAddRow()), Qt::UniqueConnection);
    connect(&removeRowButton_, SIGNAL(clicked(bool)),
        this, SLOT(onRemove()), Qt::UniqueConnection);
    connect(&analyzeSourceButton_, SIGNAL(clicked(bool)),
        this, SLOT(onAnalyzeSource()), Qt::UniqueConnection);
    connect(&importEndpointsButton_, SIGNAL(clicked(bool)),
        this, SLOT(onImportEndpoints()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    // set view to be sortable
    view_.setSortingEnabled(true);
    view_.horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    view_.setSelectionMode(QAbstractItemView::SingleSelection);
    view_.setAlternatingRowColors(true);
    //view_.setSelectionBehavior(QAbstractItemView::SelectRows);
    view_.verticalHeader()->hide();
    view_.setEditTriggers(QAbstractItemView::AllEditTriggers);
    view_.setWordWrap(true);

    // set the delegate
    view_.setItemDelegate(new EndpointDelegate(this));

    // set proxy to do the sorting automatically
    proxy_ = new QSortFilterProxyModel(this);

    // set source model for proxy
    proxy_->setSourceModel(&model_);
    // set proxy to be the source for the view
    view_.setModel(proxy_);

    // sort the view
    view_.sortByColumn(0, Qt::AscendingOrder);

    // Hide remote endpoint names if requested.
    view_.setColumnHidden(3, !showRemote);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(&addRowButton_, 0, Qt::AlignLeft);
    buttonLayout->addWidget(&removeRowButton_, 0, Qt::AlignLeft);
    buttonLayout->addWidget(&analyzeSourceButton_, 0, Qt::AlignLeft);
    buttonLayout->addWidget(&importEndpointsButton_, 0, Qt::AlignLeft);
    buttonLayout->addStretch();

    // create the layout, add widgets to it
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&view_);
    layout->addLayout(buttonLayout);

    model_.restore();
}

EndpointEditor::~EndpointEditor() {
}

void EndpointEditor::onRemove() {

    QModelIndex index = proxy_->mapToSource(view_.currentIndex());

    // if index is valid then remove the row
    if (index.isValid())
        model_.onRemoveRow(index.row());
}

bool EndpointEditor::isValid() const {
    return model_.isValid();
}

void EndpointEditor::makeChanges()
{
    model_.apply();

    // Generate the port maps automatically.
    QList< QSharedPointer<General::PortMap> > portMaps;

    QMap< QString, QSharedPointer<Port> > const& ports = component()->getModel()->getPorts();
    QMapIterator< QString, QSharedPointer<Port> > portIter(ports);

    while (portIter.hasNext())
    {
        portIter.next();
        
        QSharedPointer<General::PortMap> portMap(new General::PortMap());
        portMap->physicalPort_ = portIter.key();
        portMap->logicalPort_ = portIter.key().toUpper();
        
        portMaps.append(portMap);
    }

    BusInterface* busIf = component()->getBusInterface("app_link");
    Q_ASSERT(busIf != 0),
    
    busIf->setPortMaps(portMaps);
}

//-----------------------------------------------------------------------------
// Function: onAnalyzeSource()
//-----------------------------------------------------------------------------
void EndpointEditor::onAnalyzeSource()
{
    // TODO:
}

//-----------------------------------------------------------------------------
// Function: onImportEndpoints()
//-----------------------------------------------------------------------------
void EndpointEditor::onImportEndpoints()
{
    // Open up the import dialog.
    ImportEndpointsDialog dlg(lh_, parentWnd_, this);

    if (dlg.exec() == QDialog::Accepted)
    {
        // Retrieve the source component.
        QSharedPointer<LibraryComponent> libComp = lh_->getModel(dlg.getSourceVLNV());
        QSharedPointer<Component> sourceComp = libComp.staticCast<Component>();

        if (sourceComp == 0)
        {
            emit errorMessage(tr("The component %1 was not found in the library").arg(dlg.getSourceVLNV().getName()));
            return;
        }

        // Import the endpoints according to the selected import option.
        model_.importEndpoints(*sourceComp, dlg.getImportOption());
    }
}
