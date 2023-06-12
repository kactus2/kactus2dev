//-----------------------------------------------------------------------------
// File: PeripheralTemplateConfigurer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.06.2023
//
// Description:
// Configures renode generator peripheral templates.
//-----------------------------------------------------------------------------

#ifndef PERIPHERALTEMPLATECONFIGURER_H
#define PERIPHERALTEMPLATECONFIGURER_H

#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

#include <QSettings>

class RenodeSettingsModel;

//-----------------------------------------------------------------------------
//! Configures renode generator peripheral templates.
//-----------------------------------------------------------------------------
class PeripheralTemplateConfigurer
{
public:

    /*!
     *  Constructor.
     * 
     *      @param [in] settingsModel   Container for Renode generator settings.
     */
    PeripheralTemplateConfigurer(RenodeSettingsModel* settingsModel);

    /*!
     *  Destructor.
     */
    virtual ~PeripheralTemplateConfigurer() = default;

    /*!
     *  Get the list of available peripheral templates.
     *	
     *      @return List of available peripheral templates.
     */
    QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > getTemplates() const;

    /*!
     *  Set a new list of peripheral templates.
     *	
     *      @param [in] newTemplates    The new peripheral templates.
     */
    void setTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplates);

private:

    /*!
     *  Initialize templates from the currently active settings model.
     */
    void initializeTemplates();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of available peripheral templates.
    QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > templates_;

    // Container for Renode generator settings.
    RenodeSettingsModel* settingsModel_;
};

//-----------------------------------------------------------------------------

#endif // PERIPHERALTEMPLATECONFIGURER_H
