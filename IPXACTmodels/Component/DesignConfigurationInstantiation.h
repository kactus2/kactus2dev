//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiation.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Describes the ipxact:designConfigurationInstantiation element.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONINSTANTIATION_H
#define DESIGNCONFIGURATIONINSTANTIATION_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Describes the ipxact:designConfigurationInstantiation element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignConfigurationInstantiation : public NameGroup, public Extendable
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            Name of the instantiation.
     *      @param [in] displayName     Display name of the instantiation.
     *      @param [in] description     Description of the instantiation.
     */
    DesignConfigurationInstantiation(QString const& name = QString(), QString const& displayName = QString(),
        QString const& description = QString());

    /*!
     *  The copy constructor.
     */
    DesignConfigurationInstantiation(const DesignConfigurationInstantiation& other);

    /*!
     *  The assignment operator.
     */
    DesignConfigurationInstantiation& operator=(const DesignConfigurationInstantiation& other);

    /*!
     *  The destructor.
     */
    ~DesignConfigurationInstantiation();

    /*!
     *  Get the HDL used for instantiation.
     *
     *      @return The used HDL.
     */
    QString getLanguage() const;

    /*!
     *  Set the HDL for this instantiation.
     *
     *      @param [in] newLanguage     The new HDL.
     */
    void setLanguage(QString const& newLanguage);

    /*!
     *  Check if the used language shall be strictly enforced.
     *
     *      @return True, if the language shall be strictly enforced, false otherwise.
     */
    bool isLangugageStrict() const;

    /*!
     *  Set the strictness of the language.
     *
     *      @param [in] strictness  If true, the language shall be strictly enforced.
     */
    void setLanguageStrict(bool strictness);

    /*!
     *  Get the design configuration reference.
     *
     *      @return Pointer to the design configuration referenced by the instantiation.
     */
    QSharedPointer<ConfigurableVLNVReference> getDesignConfigurationReference() const;

    /*!
     *  Set the design configuration reference.
     *
     *      @param [in] newDesignReference  The referenced design configuration.
     */
    void setDesignConfigurationReference(QSharedPointer<ConfigurableVLNVReference> newDesignConfigurationReference);

    /*!
     *  Get a list of parameters used in this instantiation.
     *
     *      @return A list of parameters.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Set a list of parameters.
     *
     *      @param [in] newParameters   The new list of parameters.
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

private:

    /*!
     *  Copy parameters from another design configuration instantiation.
     *
     *      @param [in] other   The location of the copied parameters.
     */
    void copyParameters(DesignConfigurationInstantiation const& other) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used HDL.
    QString language_;

    //! Determines whether the language shall be strictly enforced or not.
    bool languageStrictness_;

    //! Design configuration reference.
    QSharedPointer<ConfigurableVLNVReference> designConfigurationRef_;

    //! A list of instantiation parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

#endif // DESIGNINSTANTIATION_H