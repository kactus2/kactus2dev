//-----------------------------------------------------------------------------
// File: RenodePeripheralsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.02.2023
//
// Description:
// Editor for CPU peripheral details in renode generation.
//-----------------------------------------------------------------------------

#include "RenodePeripheralsEditor.h"

#include <Plugins/RenodeGenerator/CPUDialog/RenodePeripheralsDelegate.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodePeripheralsModel.h>
#include <Plugins/RenodeGenerator/CPUDialog/TemplateEditor/PeripheralTemplatesDialog.h>
#include <Plugins/RenodeGenerator/CPUDialog/TemplateEditor/PeripheralTemplateConfigurer.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsEditor::RenodePeripheralsEditor()
//-----------------------------------------------------------------------------
RenodePeripheralsEditor::RenodePeripheralsEditor(PeripheralTemplateConfigurer* templateConfigurer, QWidget* parent):
QWidget(parent),
view_(new QTableView(this)),
model_(new RenodePeripheralsModel(this)),
peripheralDelegate_(new RenodePeripheralsDelegate(parent)),
templateConfigurer_(templateConfigurer)
{
    setupPeripheralTemplates(templateConfigurer_->getTemplates());

    auto proxy(new QSortFilterProxyModel(this));
    proxy->setSourceModel(model_);

    view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->hide();
    view_->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

    view_->setModel(proxy);
    view_->setItemDelegate(peripheralDelegate_);

	auto templatesButton(new QPushButton("Python peripheral templates", this));
	connect(templatesButton, SIGNAL(clicked()), this, SLOT(openTemplatesDialog()), Qt::UniqueConnection);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(view_, 1);
    layout->addWidget(templatesButton, 0, Qt::AlignLeft);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsEditor::setupPeripherals()
//-----------------------------------------------------------------------------
void RenodePeripheralsEditor::setupPeripherals(QVector<QSharedPointer<RenodeStructs::cpuPeripheral>> peripherals)
{
    model_->setupPeripherals(peripherals);
    view_->resizeColumnsToContents();
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsEditor::setupFolderPath()
//-----------------------------------------------------------------------------
void RenodePeripheralsEditor::setupFolderPath(QString const& newFolderPath)
{
    generationFolder_ = newFolderPath;
    peripheralDelegate_->onFolderChanged(newFolderPath);
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsEditor::openTemplatesDialog()
//-----------------------------------------------------------------------------
void RenodePeripheralsEditor::openTemplatesDialog()
{
    PeripheralTemplatesDialog templateDialog(generationFolder_, this);
    templateDialog.setupTemplates(templateConfigurer_->getTemplates());

    if (templateDialog.exec() == QDialog::Accepted)
    {
        QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplates = templateDialog.getTemplates();
        templateConfigurer_->setTemplates(newTemplates);
        setupPeripheralTemplates(newTemplates);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsEditor::setupPeripheralTemplates()
//-----------------------------------------------------------------------------
void RenodePeripheralsEditor::setupPeripheralTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate>> templates)
{
    model_->setupTemplates(templates);
    peripheralDelegate_->setupTemplates(templates);
}
