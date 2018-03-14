//-----------------------------------------------------------------------------
// File: DocumentValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.02.2018
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef DOCUMENTVALIDATOR_H
#define DOCUMENTVALIDATOR_H

#include <QSharedPointer>
#include <QString>
#include <QVector>

#include <IPXACTmodels/Component/validators/ComponentValidator.h>
#include <IPXACTmodels/Design/validator/DesignValidator.h>
#include <IPXACTmodels/designConfiguration/validators/DesignConfigurationValidator.h>
#include <IPXACTmodels/kactusExtensions/validators/SystemDesignConfigurationValidator.h>

class Document;
class AbstractionDefinition;
class BusDefinition;
class Catalog;
class Component;
class ComponentParameterFinder;
class Design;
class DesignConfiguration;
class LibraryInterface;
class ListParameterFinder;

class DocumentValidator 
{
public:

    //! The constructor.
    DocumentValidator(LibraryInterface* library);

    //! The destructor.
    ~DocumentValidator();

    bool validate(QSharedPointer<Document> document);

    void findErrorsIn(QSharedPointer<Document> document, QVector<QString>& errorList);

   
private:

    // Disable copying.
    DocumentValidator(DocumentValidator const& rhs);
    DocumentValidator& operator=(DocumentValidator const& rhs);
     
    /*!
     *  Change the component used in the component validator parameter finder.
     *
     *      @param [in] targetComponent     The new component.
     */
    void changeComponentValidatorParameterFinder(QSharedPointer<Component> targetComponent);


  /*!
     *  Finds any errors within a given bus definition document.
     *
     *      @param [in]  busDefinition   The bus definition to search in.
     *      @param [out] errorList      The list of errors to add any found errors.
     */
    void findErrorsInBusDefinition(QSharedPointer<BusDefinition> busDefinition, QVector<QString>& errorList);
    
    /*!
     *  Finds any errors within a given abstraction definition document.
     *
     *      @param [in]  abstraction    The bus definition to search in.
     *      @param [out] errorList      The list of errors to add any found errors.
     */
    void findErrorsInAbstractionDefinition(QSharedPointer<AbstractionDefinition> abstraction, QVector<QString>& errorList);

    /*!
     *  Finds any errors within a given component.
     *
     *      @param [in] component   The selected component.
     *      @param [in] errorList   The list of errors.
     */
    void findErrorsInComponent(QSharedPointer<Component> component, QVector<QString>& errorList);

    /*!
     *  Finds any errors within a given design.
     *
     *      @param [in] design      The selected design.
     *      @param [in] errorList   The list of errors.
     */
    void findErrorsInDesign(QSharedPointer<Design> design, QVector<QString>& errorList);

    /*!
     *  Finds any errors within a given design configuration.
     *
     *      @param [in] configuration   The selected design configuration.
     *      @param [in] errorList       The list of errors.
     */
    void findErrorsInDesignConfiguration(QSharedPointer<DesignConfiguration> configuration,
        QVector<QString>& errorList);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

     LibraryInterface* library_;

     //! The parameter finder used in the component validator.
     QSharedPointer<ComponentParameterFinder> componentValidatorFinder_;

     //! The used component validator.
     ComponentValidator componentValidator_;

     //! The parameter finder used in the design validator.
     QSharedPointer<ListParameterFinder> designValidatorFinder_;

     //! The used design validator.
     DesignValidator designValidator_;

     //! The used design configuration validator.
     DesignConfigurationValidator designConfigurationValidator_;

     //! The used system design configuration validator.
     SystemDesignConfigurationValidator systemDesignConfigurationValidator_;
};

#endif // DOCUMENTVALIDATOR_H
