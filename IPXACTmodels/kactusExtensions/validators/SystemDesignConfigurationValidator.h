//-----------------------------------------------------------------------------
// File: SystemDesignConfigurationValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.02.2016
//
// Description:
// Validator for the kactus2:systemDesignConfiguration.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDESIGNCONFIGURATIONVALIDATOR_H
#define SYSTEMDESIGNCONFIGURATIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/designConfiguration/validators/DesignConfigurationValidator.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class DesignConfiguration;
class Design;
class ComponentInstance;
class ViewConfiguration;

class LibraryInterface;
class ExpressionParser;

class SystemViewConfigurationValidator;

//-----------------------------------------------------------------------------
//! Validator for the kactus2:systemDesignConfiguration.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SystemDesignConfigurationValidator : public DesignConfigurationValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parser      The used expression parser.
     *      @param [in] library     The used library interface.
     */
    SystemDesignConfigurationValidator(QSharedPointer<ExpressionParser> parser, LibraryInterface* library);

	//! The destructor.
	~SystemDesignConfigurationValidator();
    
    /*!
     *  Check if the view configurations are valid.
     *
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return True, if the view configurations are valid, otherwise false.
     */
    virtual bool hasValidViewConfigurations(QSharedPointer<DesignConfiguration> designConfiguration) const;

private:

	// Disable copying.
	SystemDesignConfigurationValidator(SystemDesignConfigurationValidator const& rhs);
	SystemDesignConfigurationValidator& operator=(SystemDesignConfigurationValidator const& rhs);

    /*!
     *  Check if the view configuration references a hw component instances.
     *
     *      @param [in] viewConfiguration   The view configuration to check.
     *      @param [in] hwInstances         The currently available hw component instances.
     */
    bool viewConfigurationReferencesHWInstance(QSharedPointer<ViewConfiguration> viewConfiguration,
        QSharedPointer<QList<QSharedPointer<ComponentInstance> > > hwInstances) const;

    /*!
     *  Find errors in view configurations.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designConfiguration     The selected design configuration.
     *      @param [in] context                 Context to help locate the errors.
     */
    virtual void findErrorsInViewConfigurations(QVector<QString>& errors,
        QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used system view configuration validator.
    QSharedPointer<SystemViewConfigurationValidator> systemViewConfigurationValidator_;
};

#endif // SYSTEMDESIGNCONFIGURATIONVALIDATOR_H
