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
	
    /*!
     *  Validates the given instantiation.
     *
	 *      @param [in] designInstantiation          The instantiation to validate.
     *
     *      @return True, if the instantiations is valid IP-XACT, otherwise false.
     */
	bool validateDesignInstantiation(QSharedPointer<DesignInstantiation> designInstantiation) const;
	
    /*!
     *  Finds possible errors in a instantiation and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] designInstantiation   The instantiation whose errors to find.
	 *      @param [in] context     Context to help locate the errors.
     */
	void findErrorsInDesignInstantiation(QVector<QString>& errors,
		QSharedPointer<DesignInstantiation> componentInstantiation, QString const& contex) const;
	
    /*!
     *  Validates the given instantiation.
     *
	 *      @param [in] designConfigurationInstantiation          The instantiation to validate.
     *
     *      @return True, if the instantiations is valid IP-XACT, otherwise false.
     */
	bool validateDesignConfigurationInstantiation(QSharedPointer<DesignConfigurationInstantiation>
		designConfigurationInstantiation) const;
	
    /*!
     *  Finds possible errors in a instantiation and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] designConfigurationInstantiation   The instantiation whose errors to find.
	 *      @param [in] context     Context to help locate the errors.
     */
	void findErrorsInDesignConfigurationInstantiation(QVector<QString>& errors,
		QSharedPointer<DesignConfigurationInstantiation> designConfigurationInstantiation, QString const& contex) const;

    /*!
     *  Validates the given instantiation.
     *
	 *      @param [in] componentInstantiation          The instantiation to validate.
	 *      @param [in] fileSets						Should contain file sets referred by the instantiation.
     *
     *      @return True, if the instantiations is valid IP-XACT, otherwise false.
     */
    virtual bool validateComponentInstantiation(QSharedPointer<ComponentInstantiation> componentInstantiation,
		QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets) const;

    /*!
     *  Finds possible errors in a instantiation and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] componentInstantiation   The instantiation whose errors to find.
	 *      @param [in] context     Context to help locate the errors.
	 *      @param [in] fileSets						Should contain file sets referred by the instantiation.
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
	
    /*!
     *  Checks if the there is any file sets to begin with.
     *
	 *      @param [in] fileSets    The list of potentially referenced file sets
	 *      @param [in] componentInstantiation    The componentInstantiation potentially containing file set references.
     *
     *      @return False, if there are file set references but no file sets, otherwise true.
     */
	bool inexistingReferenceList(QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets,
		QSharedPointer<ComponentInstantiation> componentInstantiation) const;
	
    /*!
     *  Checks if file set referenced by parameter fileSetRef exist in file sets.
     *
	 *      @param [in] fileSets    The list of potentially referenced file sets
	 *      @param [in] fileSetRef    The searched file set reference.
     *
     *      @return False, if there are file set references but no file sets, otherwise true.
     */
	bool existingReference(QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets, QString fileSetRef) const;

    /*!
     *  Check if the usage type of a parameter is valid.
     *
     *      @param [in] parameter    The parameter to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
	bool isValidUsageType(QSharedPointer<ModuleParameter> parameter) const;

	/*!
	 *  Checks if isPresent element is valid.
	 *
	 *      @param [in] parameter		The element containing isPresent.
	 *      @param [in] parser			The parser used to check the validity.
	 *
	 *      @return True, if the presence is valid expression, otherwise false.
	 */
	bool isValidPresence(QSharedPointer<ModuleParameter> parameter, QSharedPointer<ExpressionParser> parser) const;

	// Disable copying.
	InstantiationsValidator(InstantiationsValidator const& rhs);
	InstantiationsValidator& operator=(InstantiationsValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
