//-----------------------------------------------------------------------------
// File: SystemViewEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 22.5.2012
//
// Description:
// System view editor.
//-----------------------------------------------------------------------------

#include "SystemViewEditor.h"

#include <common/widgets/viewSelector/viewselector.h>

#include <IPXACTmodels/kactusExtensions/SystemView.h>

#include <QApplication>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::SystemViewEditor()
//-----------------------------------------------------------------------------
SystemViewEditor::SystemViewEditor(FileSetInterface* fileSetInterface, QSharedPointer<Component> component,
    QSharedPointer<SystemView> systemView, LibraryInterface* libHandler, QWidget* parent):
ItemEditor(component, libHandler, parent), 
    view_(systemView.data()),
    nameEditor_(systemView, component->getRevision(), this, tr("Name and description")),
    hierRefEditor_(NULL),
    HWViewRefEditor_(NULL),
    fileSetRefEditor_(NULL)
{
    // find the main window for VLNV editor
    QWidget* parentWindow = nullptr;
    for (QWidget * widget : QApplication::topLevelWidgets())
    {
        QMainWindow* mainWnd = dynamic_cast<QMainWindow*>(widget);
        if (mainWnd)
        {
            parentWindow = mainWnd;
            break;
        }
    }

	hierRefEditor_ = new VLNVEditor(VLNV::DESIGNCONFIGURATION, libHandler, parentWindow, this);
	hierRefEditor_->setTitle(tr("Hierarchy reference"));
    hierRefEditor_->setRevisionFilter(true, component->getRevision());

    fileSetRefEditor_ = new FileSetRefEditor(fileSetInterface, tr("File set references"), this);
    fileSetRefEditor_->initialize();

	HWViewRefEditor_ = new ViewSelector(ViewSelector::BOTH_HW_VIEWS, component, this);	

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(hierRefEditor_, SIGNAL(vlnvEdited()), this, SLOT(onHierRefChange()), Qt::UniqueConnection);
    connect(fileSetRefEditor_, SIGNAL(contentChanged()), this, SLOT(onFileSetRefChange()), Qt::UniqueConnection);
    connect(HWViewRefEditor_, SIGNAL(viewSelected(QString const&)),
        this, SLOT(onHWViewChange(QString const&)), Qt::UniqueConnection);
     
     refresh();

     setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::~SystemViewEditor()
//-----------------------------------------------------------------------------
SystemViewEditor::~SystemViewEditor()
{
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::isValid()
//-----------------------------------------------------------------------------
bool SystemViewEditor::isValid() const
{
    if (!nameEditor_.isValid() || !hierRefEditor_->isValid())
    {
        return false;
    }

    // check the file set references that they are to valid file sets.
    for (QString const& ref : fileSetRefEditor_->items())
    {
        // if the component does not contain the referenced file set.
        if (!component()->hasFileSet(ref))
        {
            return false;
        }
    }

    // if there is a HW view selected
    QString selectedHWView = HWViewRefEditor_->currentText();
    if (!selectedHWView.isEmpty())
    {
        // if the component does not contain the referenced view.
        if (!component()->hasView(selectedHWView))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::refresh()
//-----------------------------------------------------------------------------
void SystemViewEditor::refresh()
{
    nameEditor_.refresh();
    hierRefEditor_->setVLNV(view_->getHierarchyRef());
    HWViewRefEditor_->refresh();
    HWViewRefEditor_->selectView(view_->getHWViewRef());
    fileSetRefEditor_->setItems(view_->getFileSetRefs());
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::onHierRefChange()
//-----------------------------------------------------------------------------
void SystemViewEditor::onHierRefChange()
{
    view_->setHierarchyRef(hierRefEditor_->getVLNV());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::showEvent()
//-----------------------------------------------------------------------------
void SystemViewEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/systemview.html");
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::onFileSetRefChange()
//-----------------------------------------------------------------------------
void SystemViewEditor::onFileSetRefChange()
{
	view_->setFileSetRefs(fileSetRefEditor_->items());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::onHWViewChange()
//-----------------------------------------------------------------------------
void SystemViewEditor::onHWViewChange(QString const& /*viewName*/)
{
	view_->setHWViewRef(HWViewRefEditor_->currentText());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::setupLayout()
//-----------------------------------------------------------------------------
void SystemViewEditor::setupLayout()
{
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    auto HWViewGroup = new QGroupBox(tr("HW View"));
    auto HWViewLabel = new QLabel(tr("Used HW view"), HWViewGroup);
    HWViewGroup->setFlat(true);

    auto groupLayout = new QHBoxLayout(HWViewGroup);
    groupLayout->addWidget(HWViewLabel, 0, Qt::AlignTop);
    groupLayout->addWidget(HWViewRefEditor_, 0, Qt::AlignTop);
    groupLayout->addStretch();
    hierRefEditor_->setFlat(true);

    auto topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    auto topLayout = new QGridLayout(topWidget);
    topLayout->addWidget(&nameEditor_, 0, 0, 2, 1);
    topLayout->addWidget(hierRefEditor_, 0, 1, 1, 1, Qt::AlignTop);
    topLayout->addWidget(HWViewGroup, 1, 1, 1, 1);
    topLayout->addWidget(fileSetRefEditor_, 2, 0, 1, 1);
    topLayout->setContentsMargins(0, 0, 0, 0);
}
