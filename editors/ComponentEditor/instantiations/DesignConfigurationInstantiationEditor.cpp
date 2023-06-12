//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiationEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 26.01.2016
//
// Description:
// Editor for design configuration instantiations.
//-----------------------------------------------------------------------------

#include "DesignConfigurationInstantiationEditor.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ListParameterFinder.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/ConfigurableElementFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/InstantiationConfigurableElementEditor.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Component/Component.h>

#include <QApplication>
#include <QMainWindow>
#include <QScrollArea>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::DesignConfigurationInstantiationEditor()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiationEditor::DesignConfigurationInstantiationEditor(QSharedPointer<Component> component,
    QSharedPointer<DesignConfigurationInstantiation> instantiation,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    LibraryInterface* libHandler, QWidget* parent):
ParameterItemEditor(component, libHandler, parent), 
instantiation_(instantiation),
nameGroupEditor_(new NameGroupEditor(instantiation, component->getRevision(), this,
    tr("Design configuration instance name and description"))),
designConfigurationEditor_(0),
parameters_(instantiation->getParameters(), component->getChoices(), parameterFinder, expressionFormatter, this),
elementEditor_(0),
designConfigurationParameterFinder_(new ListParameterFinder())
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

    createConfigurableElementEditor(parameterFinder);

    designConfigurationEditor_ = new VLNVEditor(VLNV::DESIGNCONFIGURATION, libHandler, parentWindow, this);
    designConfigurationEditor_->setTitle(tr("Design configuration reference"));
    designConfigurationEditor_->setMandatory(true);
    designConfigurationEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(&parameters_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    connect(&parameters_,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        this,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        Qt::UniqueConnection);

    connect(&parameters_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(nameGroupEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(designConfigurationEditor_, SIGNAL(vlnvEdited()), this, SLOT(onHierRefChange()), Qt::UniqueConnection);
   
    connect(elementEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(elementEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(elementEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    refresh();

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::createConfigurableElementEditor()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::createConfigurableElementEditor(
    QSharedPointer<ParameterFinder> parameterFinder)
{
    QSharedPointer<ConfigurableElementFinder> elementFinder(new ConfigurableElementFinder());

    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    multiFinder->addFinder(elementFinder);
    multiFinder->addFinder(parameterFinder);

    QSharedPointer<IPXactSystemVerilogParser> multiParser(new IPXactSystemVerilogParser(multiFinder));
    QSharedPointer<IPXactSystemVerilogParser> configurationParameterParser(
        new IPXactSystemVerilogParser(designConfigurationParameterFinder_));

    QSharedPointer<IPXactSystemVerilogParser> referenceParser(new IPXactSystemVerilogParser(parameterFinder));
    ComponentParameterModel* completionModel = new ComponentParameterModel(parameterFinder, this);
    completionModel->setExpressionParser(referenceParser);

    elementEditor_ = new InstantiationConfigurableElementEditor(elementFinder, multiFinder,
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(multiFinder)),
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(designConfigurationParameterFinder_)),
        multiParser, configurationParameterParser, completionModel, this);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::refresh()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::refresh()
{
	nameGroupEditor_->refresh();
    designConfigurationEditor_->setVLNV(*instantiation_->getDesignConfigurationReference());
    parameters_.refresh();

    setupParametersAsConfigurableElements();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::onHierRefChange()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::onHierRefChange()
{
	instantiation_->getDesignConfigurationReference()->setVLNV(designConfigurationEditor_->getVLNV());
    setupParametersAsConfigurableElements();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::setupParametersAsConfigurableElements()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::setupParametersAsConfigurableElements()
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters;
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newElements;

    QSharedPointer<const Document> configurationDocument = handler()->getModelReadOnly(
        designConfigurationEditor_->getVLNV());
    QSharedPointer<const DesignConfiguration> designConfiguration(0);
    if (configurationDocument)
    {
        designConfiguration = configurationDocument.staticCast<const DesignConfiguration>();
        if (designConfiguration)
        {
            newParameters = designConfiguration->getParameters();
        }
    }

    if (instantiation_->getDesignConfigurationReference())
    {
        newElements = instantiation_->getDesignConfigurationReference()->getConfigurableElementValues();
    }

    designConfigurationParameterFinder_->setParameterList(newParameters);

    if (designConfiguration && instantiation_->getDesignConfigurationReference())
    {
        elementEditor_->setParameters(designConfiguration->getVlnv().toString(), newParameters, newElements);
    }
    else
    {
        elementEditor_->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::showEvent()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/designConfigurationInstantiation.html");
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::setupLayout()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::setupLayout()
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
    topLayout->addWidget(designConfigurationEditor_, 0, 1, 1, 1, Qt::AlignTop);
    topLayout->addWidget(elementEditor_, 1, 0, 1, 1);
    topLayout->addWidget(&parameters_, 2, 0, 2, 2);
    topLayout->setRowStretch(2, 3);
}
