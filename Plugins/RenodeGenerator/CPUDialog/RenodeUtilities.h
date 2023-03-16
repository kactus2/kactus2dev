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

#include <Plugins/RenodeGenerator/CPUDialog/RenodeCPUDetailRoutes.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Utility functions for Renode generator.
//-----------------------------------------------------------------------------
namespace RenodeUtilities
{
    /*!
     *  Get CPU detail routes for Renode generator.
     *
     *      @param [in] library     Interface for accessing the library.
     *      @param [in] component   Top component of the design.
     *      @param [in] viewName    The currently active view.
     *
     *      @return List of cpu detail routes for Renode generator.
     */
    QVector<QSharedPointer<RenodeCPUDetailRoutes> > getRenodeCpuRoutes(LibraryInterface* library, QSharedPointer<Component> component, QString const& viewName);
};

#endif //RENODEUTILITIES_H