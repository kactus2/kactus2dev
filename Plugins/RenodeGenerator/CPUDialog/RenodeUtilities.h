//-----------------------------------------------------------------------------
// File: RenodeUtilities.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.03.2023
//
// Description:
// Utility functions for Renode generator.
//-----------------------------------------------------------------------------

#ifndef RENODEUTILITIES_H
#define RENODEUTILITIES_H

#include <Plugins/RenodeGenerator/CPUDialog/RenodeCpuRoutesContainer.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QString>
#include <QJsonObject>

//-----------------------------------------------------------------------------
//! Utility functions for Renode generator.
//-----------------------------------------------------------------------------
namespace RenodeUtilities
{
    /*!
     *  Get CPU detail routes for Renode generator.
     *
     *      @param [in] configurationObject     JSON object containing the editor configuration.
     *      @param [in] library                 Interface for accessing the library.
     *      @param [in] component               Top component of the design.
     *      @param [in] viewName                The currently active view.
     *      @param [in] templates               List of available peripheral templates.
     *
     *      @return List of CPU detail routes for Renode generator.
     */
    QVector<QSharedPointer<RenodeCpuRoutesContainer> > getRenodeCpuRoutes(
        QJsonObject const& configurationObject,
        LibraryInterface* library,
        QSharedPointer<Component> component,
        QString const& viewName,
        QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > templates);

    /*!
     *  Get the available peripheral template names.
     *	
     *      @param [in] templates   List of available peripheral templates.
     *
     *      @return List of peripheral template names.
     */
    QStringList getTemplateNames(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > templates);

    /*!
     *  Get the selected peripheral template from the selected list.
     *	
     *      @param [in] templateName    Name of the selected peripheral template.
     *      @param [in] templates       List of available peripheral templates.
     *
     *      @return The selected peripheral template.
     */
    QSharedPointer<RenodeStructs::peripheralTemplate> getTemplateFromList(QString const& templateName, QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > templates);
};

#endif //RENODEUTILITIES_H