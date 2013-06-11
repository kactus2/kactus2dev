//-----------------------------------------------------------------------------
// File: MemoryDesignWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.9.2012
//
// Description:
// Implements the memory design widget class.
//-----------------------------------------------------------------------------

#include "MemoryDesignWidget.h"

#include "MemoryDesignDiagram.h"

#include <common/GenericEditProvider.h>

#include <designwidget/columnview/ColumnEditDialog.h>

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/LibraryUtils.h>

#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>

#include <mainwindow/mainwindow.h>

#include <QScrollBar>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: MemoryDesignWidget()
//-----------------------------------------------------------------------------
MemoryDesignWidget::MemoryDesignWidget(LibraryInterface* lh, MainWindow* mainWnd, QWidget* parent)
    : DesignWidget(lh, parent)
{
    setDiagram(new MemoryDesignDiagram(lh, mainWnd, *getGenericEditProvider(), this));
}

//-----------------------------------------------------------------------------
// Function: ~MemoryDesignWidget()
//-----------------------------------------------------------------------------
MemoryDesignWidget::~MemoryDesignWidget()
{
}

//-----------------------------------------------------------------------------
// Function: setDesign()
//-----------------------------------------------------------------------------
bool MemoryDesignWidget::setDesign(VLNV const& vlnv, QString const& viewName)
{
    disconnect(getDiagram(), SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    // Check if the vlnv is not valid.
    if (!vlnv.isValid() || vlnv.getType() != VLNV::COMPONENT)
    {
        return false;
    }

    // Retrieve the model.
    QSharedPointer<Component> comp = getLibraryInterface()->getModel(vlnv).staticCast<Component>();

    if (comp == 0)
    {
        return false;
    }

    // Open the design to the diagram.
    if (!setDesign(comp, viewName))
    {
        return false;
    }

    connect(getDiagram(), SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(getDiagram(), SIGNAL(modeChanged(DrawMode)),
        this, SIGNAL(modeChanged(DrawMode)), Qt::UniqueConnection);

    setDocumentName(QString("%1 (%2)").arg(getIdentifyingVLNV().getName()).arg(getIdentifyingVLNV().getVersion()));
    setDocumentType("Memory Design");

    // Open in unlocked mode by default only if the version is draft.
    setProtection(vlnv.getVersion() != "draft");

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::setDesign()
//-----------------------------------------------------------------------------
bool MemoryDesignWidget::setDesign(QSharedPointer<Component> comp, const QString& viewName)
{
    VLNV designVLNV;
    
    View* view = comp->findView(viewName);

    if (!view)
    {
        return false;
    }

    designVLNV = comp->getHierRef(viewName);

    // Check for a valid VLNV type.
    designVLNV.setType(getLibraryInterface()->getDocumentType(designVLNV));

    if (!designVLNV.isValid())
    {
        emit errorMessage(tr("Component %1 did not contain a view").arg(comp->getVlnv()->getName()));
        return false;
    }

    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> designConf;

    // if the component contains a direct reference to a design
    if (designVLNV.getType() == VLNV::DESIGN)
    {
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(designVLNV);	
        design = libComp.staticCast<Design>();
    }
    // if component had reference to a design configuration
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(designVLNV);
        designConf = libComp.staticCast<DesignConfiguration>();

        designVLNV = designConf->getDesignRef();

        if (designVLNV.isValid())
        {
            QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(designVLNV);	
            design = libComp.staticCast<Design>();
        }

        // if design configuration did not contain a reference to a design.
        if (!design)
        {
            emit errorMessage(tr("Component %1 did not contain a view").arg(
                comp->getVlnv()->getName()));
            return false;
        }
    }

    if (!getDiagram()->setDesign(comp, design, designConf))
    {
        return false;
    }

    return DesignWidget::setDesign(comp, viewName);
}

//-----------------------------------------------------------------------------
// Function: getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int MemoryDesignWidget::getSupportedDrawModes() const
{
    if (getView()->isInteractive())
    {
        return (MODE_SELECT | MODE_DRAFT);
    }
    else
    {
        return MODE_SELECT;
    }
}

//-----------------------------------------------------------------------------
// Function: keyPressEvent()
//-----------------------------------------------------------------------------
void MemoryDesignWidget::keyPressEvent(QKeyEvent* event)
{
    // If the document is protected, skip all delete events.
    if (isProtected())
    {
        return;
    }

    if (event->key() == Qt::Key_Delete)
    {
        if (getDiagram()->selectedItems().empty())
        {
            return;
        }

        //QGraphicsItem* selected = getDiagram()->selectedItems().first();
    }
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void MemoryDesignWidget::addColumn()
{
    ColumnEditDialog dialog(this, false);
    dialog.hideContentSettings();

    if (dialog.exec() == QDialog::Accepted)
    {
        getDiagram()->addColumn(ColumnDesc(dialog.getName(), COLUMN_CONTENT_COMPONENTS, 0,
                                           MemoryDesignDiagram::COLUMN_WIDTH));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignWidget::getImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation MemoryDesignWidget::getImplementation() const
{
    return KactusAttribute::KTS_HW;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignWidget::saveAs()
//-----------------------------------------------------------------------------
bool MemoryDesignWidget::saveAs()
{
    return false;
}
