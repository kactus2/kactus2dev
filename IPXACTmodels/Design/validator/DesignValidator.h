//-----------------------------------------------------------------------------
// File: DesignValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.01.2016
//
// Description:
// Validator for the ipxact:design.
//-----------------------------------------------------------------------------

#ifndef DESIGNVALIDATOR_H
#define DESIGNVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class Design;

class LibraryInterface;
class ExpressionParser;

class ComponentInstanceValidator;
class InterconnectionValidator;
class AdHocConnectionValidator;
class ParameterValidator2014;
class AssertionValidator;

//-----------------------------------------------------------------------------
//! Validator for the ipxact:design.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parser      The used expression parser.
     *      @param [in] library     The used library interface.
     */
    DesignValidator(QSharedPointer<ExpressionParser> parser, LibraryInterface* library);

	//! The destructor.
	virtual ~DesignValidator();
    
    /*!
     *  Validates the given design.
     *
     *      @param [in] design  The selected design.
     *
     *      @return True, if the design is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<Design> design) const;

    /*!
     *  Check if the design vlnv is valid.
     *
     *      @param [in] design  The selected design.
     *
     *      @return True, if the vlnv is valid, otherwise false.
     */
    bool hasValidVLNV(QSharedPointer<Design> design) const;

    /*!
     *  Check if the component instances are valid.
     *
     *      @param [in] design  The selected design.
     *
     *      @return True, if the component instances are valid, otherwise false.
     */
    bool hasValidComponentInstances(QSharedPointer<Design> design) const;

    /*!
     *  Check if the interconnections are valid.
     *
     *      @param [in] design  The selected design.
     *
     *      @return True, if the interconnections are valid, otherwise false.
     */
    bool hasValidInterconnections(QSharedPointer<Design> design) const;

    /*!
     *  Check if the monitor interconnections are valid.
     *
     *      @param [in] design  The selected design.
     *
     *      @return True, if the monitor interconnections are valid, otherwise false.
     */
    bool hasValidMonitorInterconnections(QSharedPointer<Design> design) const;

    /*!
     *  Check if the ad hoc connections are valid.
     *
     *      @param [in] design  The selected design.
     *
     *      @return True, if the ad hoc connections are valid, otherwise false.
     */
    bool hasValidAdHocConnections(QSharedPointer<Design> design) const;

    /*!
     *  Check if the parameters are valid.
     *
     *      @param [in] design  The selected design.
     *
     *      @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<Design> design) const;

    /*!
     *  Check if the assertions are valid.
     *
     *      @param [in] design  The selected design.
     *
     *      @return True, if the assertions are valid, otherwise false.
     */
    bool hasValidAssertions(QSharedPointer<Design> design) const;

    /*!
     *  Locate errors within a design.
     *
     *      @param [in] errors  List of found errors.
     *      @param [in] design  The selected design.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<Design> design) const;

private:

	// Disable copying.
	DesignValidator(DesignValidator const& rhs);
	DesignValidator& operator=(DesignValidator const& rhs);

    /*!
     *  Find errors in design VLNV.
     *
     *      @param [in] errors  List of found errors.
     *      @param [in] design  The selected design.
     */
    void findErrorsInVLNV(QVector<QString>& errors, QSharedPointer<Design> design) const;

    /*!
     *  Find errors in component instances.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] design      The selected design.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInComponentInstances(QVector<QString>& errors, QSharedPointer<Design> design,
        QString const& context) const;

    /*!
     *  Find errors in interconnections.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] design      The selected design.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInInterconnections(QVector<QString>& errors, QSharedPointer<Design> design,
        QString const& context) const;

    /*!
     *  Find errors in monitor interconnections.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] design      The selected design.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInMonitorInterconnections(QVector<QString>& errors, QSharedPointer<Design> design,
        QString const& context) const;

    /*!
     *  Find errors in ad hoc connections.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] design      The selected design.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInAdHocConnections(QVector<QString>& errors, QSharedPointer<Design> design,
        QString const& context) const;

    /*!
     *  Find errors in parameters.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] design      The selected design.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInParameters(QVector<QString>& errors, QSharedPointer<Design> design, QString const& context)
        const;

    /*!
     *  Find errors in assertions.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] design      The selected design.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInAssertions(QVector<QString>& errors, QSharedPointer<Design> design, QString const& context)
        const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used component instance validator.
    QSharedPointer<ComponentInstanceValidator> componentInstanceValidator_;

    //! The used interconnection validator.
    QSharedPointer<InterconnectionValidator> interconnectionValidator_;

    //! The used ad hoc connection validator.
    QSharedPointer<AdHocConnectionValidator> adHocConnectionValidator_;

    //! The used parameter validator.
    QSharedPointer<ParameterValidator2014> parameterValidator_;

    //! The used assertion validator.
    QSharedPointer<AssertionValidator> assertionValidator_;
};

#endif // DESIGNVALIDATOR_H
