//-----------------------------------------------------------------------------
// File: DesignConfigurationValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.01.2016
//
// Description:
// Validator for the ipxact:designConfiguration.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONVALIDATOR_H
#define DESIGNCONFIGURATIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class DesignConfiguration;

class LibraryInterface;
class ExpressionParser;

class ParameterValidator2014;
class AssertionValidator;
class InterconnectionConfigurationValidator;
class ViewConfigurationValidator;

//-----------------------------------------------------------------------------
//! Validator for the ipxact:designConfiguration.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignConfigurationValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parser      The used expression parser.
     *      @param [in] library     The used library interface.
     */
    DesignConfigurationValidator(QSharedPointer<ExpressionParser> parser, LibraryInterface* library);

	//! The destructor.
	virtual ~DesignConfigurationValidator();
    
    /*!
     *  Validates the given design configuration.
     *
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return True, if the design configuration is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<DesignConfiguration> designConfiguration);

    /*!
     *  Check if the design configuration vlnv is valid.
     *
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return True, if the vlnv is valid, otherwise false.
     */
    bool hasValidVLNV(QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Check if the design reference is valid.
     *
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return True, if the design reference is valid, otherwise false.
     */
    bool hasValidDesignReference(QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Check if the generator chain configurations are valid.
     *
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return True, if the generator chain configurations are valid, otherwise false.
     */
    bool hasValidGeneratorChainConfigurations(QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Check if the interconnection configurations are valid.
     *
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return True, if the interconnection configurations are valid, otherwise false.
     */
    bool hasValidInterconnectionConfigurations(QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Check if the view configurations are valid.
     *
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return True, if the view configurations are valid, otherwise false.
     */
    virtual bool hasValidViewConfigurations(QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Check if the parameters are valid.
     *
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Check if the assertions are valid.
     *
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return True, if the assertions are valid, otherwise false.
     */
    bool hasValidAssertions(QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Locate errors within a design configuration.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<DesignConfiguration> designConfiguration);

protected:

    /*!
     *  Get the view configuration validator.
     *
     *      @return The used view configuration validator.
     */
    QSharedPointer<ViewConfigurationValidator> getViewConfigurationValidator() const;

    /*!
     *  Get the library interface.
     *
     *      @return The used library interface.
     */
    LibraryInterface* getLibraryHandler() const;

private:

	// Disable copying.
	DesignConfigurationValidator(DesignConfigurationValidator const& rhs);
	DesignConfigurationValidator& operator=(DesignConfigurationValidator const& rhs);

    /*!
     *  Find errors in design configuration VLNV.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void findErrorsInVLNV(QVector<QString>& errors, QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Find errors in design reference.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designConfiguration     The selected design configuration.
     *      @param [in] context                 Context to help locate the errors.
     */
    void findErrorsInDesignReference(QVector<QString>& errors,
        QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const;

    /*!
     *  Find errors in generator chain configurations.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designConfiguration     The selected design configuration.
     *      @param [in] context                 Context to help locate the errors.
     */
    void findErrorsInGeneratorChainConfigurations(QVector<QString>& errors,
        QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const;

    /*!
     *  Find errors in interconnection configurations.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designConfiguration     The selected design configuration.
     *      @param [in] context                 Context to help locate the errors.
     */
    void findErrorsInInterconnectionConfigurations(QVector<QString>& errors,
        QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const;

    /*!
     *  Find errors in view configurations.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designConfiguration     The selected design configuration.
     *      @param [in] context                 Context to help locate the errors.
     */
    virtual void findErrorsInViewConfigurations(QVector<QString>& errors,
        QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const;

    /*!
     *  Find errors in parameters.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designConfiguration     The selected design configuration.
     *      @param [in] context                 Context to help locate the errors.
     */
    void findErrorsInParameters(QVector<QString>& errors, QSharedPointer<DesignConfiguration> designConfiguration,
        QString const& context) const;

    /*!
     *  Find errors in assertions.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designConfiguration     The selected design configuration.
     *      @param [in] context                 Context to help locate the errors.
     */
    void findErrorsInAssertions(QVector<QString>& errors, QSharedPointer<DesignConfiguration> designConfiguration,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used library interface.
    LibraryInterface* libraryHandler_;

    //! The used parameter validator.
    QSharedPointer<ParameterValidator2014> parameterValidator_;

    //! The used assertion validator.
    QSharedPointer<AssertionValidator> assertionValidator_;

    //! The used interconnection configuration validator.
    QSharedPointer<InterconnectionConfigurationValidator> interconnectionValidator_;

    //! The used view configuration validator.
    QSharedPointer<ViewConfigurationValidator> viewConfigurationValidator_;
};

#endif // DESIGNCONFIGURATIONVALIDATOR_H
