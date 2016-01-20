//-----------------------------------------------------------------------------
// File: ComponentInstanceValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.01.2016
//
// Description:
// Validator for the ipxact:componentInstance.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEVALIDATOR_H
#define COMPONENTINSTANCEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class ComponentInstance;

class LibraryInterface;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! Validator for the ipxact:componentInstance.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentInstanceValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parser      The used expression parser.
     *      @param [in] library     The used library interface.
     */
    ComponentInstanceValidator(QSharedPointer<ExpressionParser> parser, LibraryInterface* library);

	//! The destructor.
	~ComponentInstanceValidator();
    
    /*!
     *  Validates the given component instance.
     *
     *      @param [in] instance    The selected component instance.
     *
     *      @return True, if the component instance is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] instance    The selected component instance.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Check if the isPresent is valid.
     *
     *      @param [in] instance    The selected component instance.
     *
     *      @return True, if the isPresent is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Check if the component reference is valid.
     *
     *      @param [in] instance    The selected component instance.
     *
     *      @return True, if the component reference is valid, otherwise false.
     */
    bool hasValidComponentReference(QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Locate errors within a component instance.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] instance    The selected component instance.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<ComponentInstance> instance, QString const& context)
        const;

private:

	// Disable copying.
	ComponentInstanceValidator(ComponentInstanceValidator const& rhs);
	ComponentInstanceValidator& operator=(ComponentInstanceValidator const& rhs);

    /*!
     *  Find errors in name.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] instance    The selected component instance.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<ComponentInstance> instance,
        QString const& context) const;

    /*!
     *  Find errors in isPresent.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] instance    The selected component instance.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<ComponentInstance> instance,
        QString const& context) const;

    /*!
     *  Find errors in component reference.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] instance    The selected component instance.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInComponentReference(QVector<QString>& errors, QSharedPointer<ComponentInstance> instance,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used expression parser.
    QSharedPointer<ExpressionParser> parser_;

    //! The used library interface.
    LibraryInterface* libraryHandler_;
};

#endif // COMPONENTINSTANCEVALIDATOR_H
