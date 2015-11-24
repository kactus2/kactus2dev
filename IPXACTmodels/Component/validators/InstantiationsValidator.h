//-----------------------------------------------------------------------------
// Instantiations: InstantiationsValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 23.11.2015
//
// Description:
// Validator for ipxact:Instantiations.
//-----------------------------------------------------------------------------

#ifndef InstantiationsVALIDATOR_H
#define InstantiationsVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QString>
#include <QVector>

//-----------------------------------------------------------------------------
//! Validator for ipxact:Instantiations.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InstantiationsValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    InstantiationsValidator();

	//! The destructor.
	~InstantiationsValidator();

	bool validateDesignInstantiation(QSharedPointer<DesignInstantiation> designInstantiation) const;

	void findErrorsInDesignInstantiation(QVector<QString>& errors,
		QSharedPointer<DesignInstantiation> componentInstantiation, QString const& contex) const;

	bool validateDesignConfigurationInstantiation(QSharedPointer<DesignConfigurationInstantiation> designInstantiation) const;

	void findErrorsInDesignConfigurationInstantiation(QVector<QString>& errors,
		QSharedPointer<DesignConfigurationInstantiation> componentInstantiation, QString const& contex) const;

    /*!
     *  Validates the given Instantiations.
     *
     *      @param [in] Instantiations           The Instantiations to validate.
     *
     *      @return True, if the Instantiations is valid IP-XACT, otherwise false.
     */
    virtual bool validateComponentInstantiation(QSharedPointer<ComponentInstantiation> componentInstantiation,
		QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets) const;

	bool inexistingReferenceList(QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets,
		QSharedPointer<ComponentInstantiation> componentInstantiation) const;

	bool existingReference(QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets, QString fileSetRef) const;

	bool isValidUsageType(QSharedPointer<ModuleParameter> parameter) const;

	bool isValidPresence(QSharedPointer<ModuleParameter> parameter, QSharedPointer<ExpressionParser> parser) const;

    /*!
     *  Finds possible errors in a Instantiations and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] Instantiations   The Instantiations whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsInComponentInstantiation(QVector<QString>& errors,
		QSharedPointer<ComponentInstantiation> componentInstantiation,
		QString const& contex, QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets) const;

private:

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

	// Disable copying.
	InstantiationsValidator(InstantiationsValidator const& rhs);
	InstantiationsValidator& operator=(InstantiationsValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
