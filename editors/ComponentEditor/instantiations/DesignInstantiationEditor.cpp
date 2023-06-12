//-----------------------------------------------------------------------------
// File: DesignInstantiationEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 26.01.2016
//
// Description:
// Editor for design configuration instantiation.
//-----------------------------------------------------------------------------

#include "DesignInstantiationEditor.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <editors/ComponentEditor/common/InstantiationConfigurableElementEditor.h>
#include <editors/ComponentEditor/common/ConfigurableElementFinder.h>
#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ListParameterFinder.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>

#include <QApplication>
#include <QMainWindow>
#include <QScrollArea>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::DesignInstantiationEditor()
//-----------------------------------------------------------------------------
DesignInstantiationEditor::DesignInstantiationEditor(QSharedPointer<Component> component,
    QSharedPointer<DesignInstantiation> instantiation, LibraryInterface* libHandler,
    QSharedPointer<ParameterFinder> componentParameterFinder, QWidget* parent):
ItemEditor(component, libHandler, parent),
instantiation_(instantiation),
nameGroupEditor_(new NameGroupEditor(instantiation, component->getRevision(), this, tr("Design instance name and description"))),
designEditor_(0),
elementEditor_(0),
designParameterFinder_(new ListParameterFinder())
{
    // find the main window for VLNV editor
    QWidget* parentWindow = nullptr;
    foreach(QWidget * widget, QApplication::topLevelWidgets())
    {
        QMainWindow* mainWnd = dynamic_cast<QMainWindow*>(widget);
        if (mainWnd)
        {
            parentWindow = mainWnd;
            break;
        }
    }

    QSharedPointer<ConfigurableElementFinder> elementFinder(new ConfigurableElementFinder());

    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    multiFinder->addFinder(componentParameterFinder);
    multiFinder->addFinder(elementFinder);

    QSharedPointer<IPXactSystemVerilogParser> multiParser(new IPXactSystemVerilogParser(multiFinder));
    QSharedPointer<IPXactSystemVerilogParser> designParameterParser(
        new IPXactSystemVerilogParser(designParameterFinder_));

    ComponentParameterModel* designParametersModel = new ComponentParameterModel(componentParameterFinder, this);
    designParametersModel->setExpressionParser(designParameterParser);

    elementEditor_ = new InstantiationConfigurableElementEditor(elementFinder, multiFinder,
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(multiFinder)),
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(designParameterFinder_)), multiParser,
        designParameterParser, designParametersModel, this);

    designEditor_ = new VLNVEditor(VLNV::DESIGN, libHandler, parentWindow, this);
    designEditor_->setTitle(tr("Design reference"));
    designEditor_->setMandatory(true);
    designEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(nameGroupEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(designEditor_, SIGNAL(vlnvEdited()), this, SLOT(onHierRefChange()), Qt::UniqueConnection);
   
    connect(elementEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(elementEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(elementEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    refresh();
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::~DesignInstantiationEditor()
//-----------------------------------------------------------------------------
DesignInstantiationEditor::~DesignInstantiationEditor()
{
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::refresh()
//-----------------------------------------------------------------------------
void DesignInstantiationEditor::refresh()
{
	nameGroupEditor_->refresh();
	designEditor_->setVLNV(*instantiation_->getDesignReference());
    setupParametersAsConfigurableElements();
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::onHierRefChange()
//-----------------------------------------------------------------------------
void DesignInstantiationEditor::onHierRefChange()
{
	instantiation_->getDesignReference()->setVLNV(designEditor_->getVLNV());
    setupParametersAsConfigurableElements();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::setupParametersAsConfigurableElements()
//-----------------------------------------------------------------------------
void DesignInstantiationEditor::setupParametersAsConfigurableElements()
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters;
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newElements;

    QSharedPointer<const Design> currentDesign;
    QSharedPointer<const Document> designDocument = handler()->getModel(designEditor_->getVLNV());
    if (designDocument)
    {
        currentDesign = designDocument.staticCast<const Design>();
        if (currentDesign)
        {
            newParameters = currentDesign->getParameters();
        }
    }
    if (instantiation_->getDesignReference())
    {
        newElements = instantiation_->getDesignReference()->getConfigurableElementValues();
    }

    designParameterFinder_->setParameterList(newParameters);

    if (currentDesign && instantiation_->getDesignReference())
    {
        elementEditor_->setParameters(currentDesign->getVlnv().toString(), newParameters, newElements);
    }
    else
    {
        elementEditor_->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::showEvent()
//-----------------------------------------------------------------------------
void DesignInstantiationEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/designInstantiation.html");
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::setupLayout()
//-----------------------------------------------------------------------------
void DesignInstantiationEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    QGridLayout* topLayout = new QGridLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addWidget(nameGroupEditor_, 0, 0, 1, 1, Qt::AlignTop);
    topLayout->addWidget(designEditor_, 0, 1, 1, 1, Qt::AlignTop);
    topLayout->addWidget(elementEditor_, 1, 0, 2, 1);
    topLayout->setRowStretch(1, 3);
}
