//-----------------------------------------------------------------------------
// File: SystemViewConfigurationValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.02.2016
//
// Description:
// Validator for the kactus2:SystemViewConfiguration.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVIEWCONFIGURATIONVALIDATOR_H
#define SYSTEMVIEWCONFIGURATIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/designConfiguration/validators/ViewConfigurationValidator.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class SystemView;
class SWInstance;

class ExpressionParser;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Validator for the kactus2:SystemViewConfiguration.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SystemViewConfigurationValidator: public ViewConfigurationValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] library     The used library interface.
     *      @param [in] parser      The used expression parser.
     */
    SystemViewConfigurationValidator(LibraryInterface* library, QSharedPointer<ExpressionParser> parser);

	//! The destructor.
	~SystemViewConfigurationValidator();
    
    /*!
     *  Change the currently available sw component instances.
     *
     *      @param [in] newInstances    The list of new sw component instances.
     */
    virtual void changeComponentInstances(QList<QSharedPointer<SWInstance> > newInstances);

    /*!
     *  Check if the view reference is valid.
     *
     *      @param [in] configuration   The selected view configuration.
     *
     *      @return True, if the view reference is valid, otherwise false.
     */
    virtual bool hasValidViewReference(QSharedPointer<ViewConfiguration> configuration);

private:

	// Disable copying.
	SystemViewConfigurationValidator(SystemViewConfigurationValidator const& rhs);
	SystemViewConfigurationValidator& operator=(SystemViewConfigurationValidator const& rhs);

    /*!
     *  Change the currently available views.
     *
     *      @param [in] configuration   The selected view configuration.
     */
    void changeAvailableSystemViews(QSharedPointer<ViewConfiguration> configuration);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The currently available system views.
    QStringList availableViews_;
};

#endif // SYSTEMVIEWCONFIGURATIONVALIDATOR_H
