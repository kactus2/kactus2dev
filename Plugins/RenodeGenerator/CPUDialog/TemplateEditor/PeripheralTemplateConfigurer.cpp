//-----------------------------------------------------------------------------
// File: PeripheralTemplateConfigurer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.06.2023
//
// Description:
// Configures renode generator peripheral templates.
//-----------------------------------------------------------------------------

#include "PeripheralTemplateConfigurer.h"

#include <Plugins/RenodeGenerator/RenodeSettingsModel.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeConstants.h>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::RenodeSettingsModel()
//-----------------------------------------------------------------------------
PeripheralTemplateConfigurer::PeripheralTemplateConfigurer(RenodeSettingsModel* settingsModel) :
settingsModel_(settingsModel)
{
    initializeTemplates();
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateConfigurer::getTemplates()
//-----------------------------------------------------------------------------
void PeripheralTemplateConfigurer::initializeTemplates()
{
    if (QFile configurationFile(settingsModel_->getFilePath()); configurationFile.open(QFile::ReadOnly))
    {
        if (QJsonDocument configurationDocument = QJsonDocument::fromJson(configurationFile.readAll()); configurationDocument.isObject())
        {
            QJsonObject configurationObject = configurationDocument.object();
            if (QJsonValue templatesValue = configurationObject.value(RenodeTemplateConstants::TEMPLATEARRAYKEY); templatesValue.isArray())
            {
                QJsonArray templatesArray = templatesValue.toArray();
                for (auto templateValue : templatesArray)
                {
                    if (templateValue.isObject())
                    {
                        QJsonObject templateObject = templateValue.toObject();

                        QSharedPointer<RenodeStructs::peripheralTemplate> newTemplate(new RenodeStructs::peripheralTemplate());
                        newTemplate->identifier_ = templateObject.value(RenodeTemplateConstants::TEMPLATENAME).toString(QString(""));
                        newTemplate->template_ = templateObject.value(RenodeTemplateConstants::TEMPLATEFILE).toString(QString(""));
                        newTemplate->path_ = templateObject.value(RenodeTemplateConstants::TARGETPATH).toString(QString(""));

                        templates_.append(newTemplate);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateConfigurer::getTemplates()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > PeripheralTemplateConfigurer::getTemplates() const
{
    return templates_;
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateConfigurer::setTemplates()
//-----------------------------------------------------------------------------
void PeripheralTemplateConfigurer::setTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate>> newTemplates)
{
    templates_ = newTemplates;

    QFile configurationFile(settingsModel_->getFilePath());
    if (!configurationFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonObject configurationObject;
    if (!templates_.isEmpty())
    {
        QJsonArray templateArray;

        for (auto currentTemplate : templates_)
        {
            QJsonObject templateObject;
            templateObject.insert(RenodeTemplateConstants::TEMPLATENAME, currentTemplate->identifier_);
            templateObject.insert(RenodeTemplateConstants::TEMPLATEFILE, currentTemplate->template_);
            templateObject.insert(RenodeTemplateConstants::TARGETPATH, currentTemplate->path_);

            templateArray.push_back(templateObject);
        }

        configurationObject.insert(RenodeTemplateConstants::TEMPLATEARRAYKEY, templateArray);
    }

    QJsonDocument document(configurationObject);
    configurationFile.write(document.toJson());
}
