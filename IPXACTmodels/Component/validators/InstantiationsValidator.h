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

#include <QSharedPointer>
#include <QString>
#include <QVector>

class LibraryInterface;
class ExpressionParser;

class ComponentInstantiation;
class DesignInstantiation;
class DesignConfigurationInstantiation;

class FileBuilder;
class FileSet;
class Parameter;
class ModuleParameter;
class ParameterValidator2014;
//-----------------------------------------------------------------------------
//! Validator for ipxact:Instantiations.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InstantiationsValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser    The used expression parser.
     *      @param [in] fileSets            The available file sets.
     *      @param [in] parameterValidator  The used parameter validator.
     *      @param [in] libraryHandler      The used library interface.
     */
    InstantiationsValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets,
        QSharedPointer<ParameterValidator2014> parameterValidator, LibraryInterface* libraryHandler);

	//! The destructor.
	virtual ~InstantiationsValidator();
	
    /*!
     *  Change the available file sets.
     *
     *      @param [in] newFileSets     The new file sets.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<FileSet> > > newFileSets);

    /*!
     *  Validates the given design instantiation.
     *
	 *      @param [in] designInstantiation     The instantiation to validate.
     *
     *      @return True, if the design instantiation is valid IP-XACT, otherwise false.
     */
	bool validateDesignInstantiation(QSharedPointer<DesignInstantiation> designInstantiation) const;
	
    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Check if the design reference is valid.
     *
     *      @param [in] designInstantiation     The selected design instantiation.
     *
     *      @return True, if the reference is valid, otherwise false.
     */
    bool hasValidDesignReference(QSharedPointer<DesignInstantiation> designInstantiation) const;

    /*!
     *  Finds possible errors in design instantiation and creates a list of them.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] designInstantiation     The design instantiation whose errors to find.
	 *      @param [in] context                 Context to help locate the errors.
     */
	void findErrorsInDesignInstantiation(QVector<QString>& errors,
		QSharedPointer<DesignInstantiation> designInstantiation, QString const& context) const;
	
    /*!
     *  Validates the given design configuration instantiation.
     *
	 *      @param [in] instantiation   The design configuration instantiation to validate.
     *
     *      @return True, if the design configuration instantiation is valid IP-XACT, otherwise false.
     */
	bool validateDesignConfigurationInstantiation(QSharedPointer<DesignConfigurationInstantiation> instantiation)
        const;
	
    /*!
     *  Check if the design configuration reference is valid.
     *
     *      @param [in] instantiation   The selected design configuration instantiation.
     *
     *      @return True, if the design configuration reference is valid, otherwise false.
     */
    bool hasValidDesignConfigurationReference(QSharedPointer<DesignConfigurationInstantiation> instantiation)
        const;

    /*!
     *  Finds possible errors in a design configuration instantiation and creates a list of them.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] instantiation   The design configuration instantiation whose errors to find.
	 *      @param [in] context         Context to help locate the errors.
     */
	void findErrorsInDesignConfigurationInstantiation(QVector<QString>& errors,
		QSharedPointer<DesignConfigurationInstantiation> instantiation, QString const& contex) const;

    /*!
     *  Validates the given component instantiation.
     *
	 *      @param [in] instantiation   The component instantiation to validate.
     *
     *      @return True, if the component instantiation is valid IP-XACT, otherwise false.
     */
    bool validateComponentInstantiation(QSharedPointer<ComponentInstantiation> instantiation) const;

    /*!
     *  Check if the file builders are valid.
     *
     *      @param [in] instantiation   The selected component instantiation.
     *
     *      @return True, if the file builders are valid, otherwise false.
     */
    bool componentInstantiationFileBuildersAreValid(QSharedPointer<ComponentInstantiation> instantiation) const;

    /*!
     *  Check if the file builder replace default flags value is valid.
     *
     *      @param [in] fileBuilder     The selected file builder.
     *
     *      @return True, if the replace default flags value is valid, otherwise false.
     */
    bool fileBuilderReplaceDefaultFlagsIsValid(QSharedPointer<FileBuilder> fileBuilder) const;

    /*!
     *  Check if the file set references are valid.
     *
     *      @param [in] instantiation   The selected component instantiation.
     *
     *      @return True, if the file set references are valid, otherwise false.
     */
    bool componentInstantiationFileSetReferencesAreValid(QSharedPointer<ComponentInstantiation> instantiation)
        const;

    /*!
     *  Checks if file set referenced by parameter fileSetRef exist in file sets.
     *
	 *      @param [in] fileSetRef  The selected file set reference.
     *
     *      @return True, if the file set reference is valid, otherwise false.
     */
	bool fileSetReferenceIsValid(QString const& fileSetRef) const;

    /*!
     *  Finds possible errors in a instantiation and creates a list of them.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] instantiation   The instantiation whose errors to find.
	 *      @param [in] context         Context to help locate the errors.
     */
    virtual void findErrorsInComponentInstantiation(QVector<QString>& errors,
        QSharedPointer<ComponentInstantiation> instantiation, QString const& context) const;

private:

    /*!
     *  Check if the contained parameters are valid.
     *
     *      @param [in] availableParameters     The contained parameters.
     *
     *      @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > availableParameters) const;

    /*!
     *  Check if the contained module parameters are valid.
     *
     *      @param [in] instantiation   The selected component instantiation.
     *
     *      @return True, if the module parameters are valid, otherwise false.
     */
    bool hasValidModuleParameters(QSharedPointer<ComponentInstantiation> instantiation) const;

    /*!
     *  Check if the usage type of a model parameter is valid.
     *
     *      @param [in] parameter    The model parameter to be evaluated.
     *
     *      @return True, if the usage type is valid, otherwise false.
     */
	bool moduleParameterHasValidUsageType(QSharedPointer<ModuleParameter> parameter) const;

	/*!
	 *  Checks if isPresent element is valid.
	 *
	 *      @param [in] parameter		The selected module parameter.
	 *
	 *      @return True, if the presence is a valid expression, otherwise false.
	 */
	bool moduleParameterHasValidPresence(QSharedPointer<ModuleParameter> parameter) const;

	// Disable copying.
	InstantiationsValidator(InstantiationsValidator const& rhs);
	InstantiationsValidator& operator=(InstantiationsValidator const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The available file sets.
    QSharedPointer<QList<QSharedPointer<FileSet> > > availableFileSets_;

    //! The validator used for parameters.
    QSharedPointer<ParameterValidator2014> parameterValidator_;

    //! The used library interface.
    LibraryInterface* libraryHandler_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
