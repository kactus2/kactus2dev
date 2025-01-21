//-----------------------------------------------------------------------------
// Port: PortTypeValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.01.2018
//
// Description:
// Validator for port types and their associated elements.
//-----------------------------------------------------------------------------

#ifndef PORTTYPEVALIDATOR_H
#define PORTTYPEVALIDATOR_H

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/WireTypeDef.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Validator for port types and their associated elements.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortTypeValidator
{
public:

	/*!
	 *  The constructor.
	 *
     *    @param [in] views   The available views.
	 */
    PortTypeValidator(QSharedPointer<QList<QSharedPointer<View> > > views);

	/*!
     *  The destructor.
     */
    ~PortTypeValidator() = default;

    // No copying. No assignment.
    PortTypeValidator(PortTypeValidator const& rhs) = delete;
    PortTypeValidator& operator=(PortTypeValidator const& rhs) = delete;

    /*!
     *  Change the available views.
     *
     *    @param [in] newViews    The new views.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<View> > > newViews);

    /*!
     *  Validates the given port type definition.
     *
     *    @param [in] typeDefinition  The selected type definition.
     *    @param [in] definitionList  List containing all the port type definitions.
     *
     *    @return True, if the type definition is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<WireTypeDef> typeDefinition,
        QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList) const;

    /*!
     *  Checks if the selected type definition has a valid type name.
     *
     *    @param [in] typeDefinition  The selected type definition.
     *
     *    @return True, if the type name is valid IP-XACT, otherwise false.
     */
    bool hasValidtype(QSharedPointer<WireTypeDef> typeDefinition) const;

    /*!
     *  Checks if the selected type definition has valid view references.
     *
     *    @param [in] typeDefinition  The selected type definition.
     *    @param [in] definitionList  List containing all the type definitions of the containing port.
     *
     *    @return True, if the view references are valid IP-XACT, otherwise false.
     */
    bool hasValidViewReferences(QSharedPointer<WireTypeDef> typeDefinition,
        QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList) const;

    /*!
     *  Checks if the selected view reference is valid.
     *
     *    @param [in] view                The selected view reference.
     *    @param [in] typeDefinitionList  List containing all the type definitions of the containing port.
     *
     *    @return True, if the view reference is valid IP-XACT, otherwise false.
     */
    bool hasValidView(QString const& view, QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitionList)
        const;

    /*!
     *  Check if the referenced view exists within the containing component.
     *
	 *    @param [in] viewRef     The selected view reference.
     *
     *    @return True, if the view reference exists, otherwise false.
     */
    bool referencedViewExists(QString const& viewRef) const;

    /*!
     *  Checks if the selected view is referenced only once in all of the type definitions.
     *
     *    @param [in] referencedView      The selected view reference.
     *    @param [in] typeDefinitionList  List containing all the type definitions of the containing port.
     *
     *    @return True, if the view reference is used only once in all of the type definitions, otherwise false.
     */
    bool viewIsReferencedOnce(QString const& referencedView,
        QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitionList) const;

    /*!
     *  Find possible errors in a type definition and create a list of them.
     *
     *    @param [in/out] errors      List of found errors.
     *    @param [in] context         Context to help locate the errors.
     *    @param [in] typeDefinition  The selected type definition.
     *    @param [in] definitionList  List of type definitions contained within the containing port.
     */
    void findErrorsIn(QVector<QString>& errors, QString const& context,
        QSharedPointer<WireTypeDef> typeDefinition,
        QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList) const;

private:

    /*!
     *  Find errors in the view references of the selected type definition.
     *
     *    @param [in/out] errors          List containing the found errors.
     *    @param [in] context             Context to help locate the errors.
     *    @param [in] typeDefinition      The selected type definition.
     *    @param [in] definitionList      List of type definitions contained within the containing port.
     */
    void findErrorsInViewReferences(QVector<QString>& errors, QString const& context,
        QSharedPointer<WireTypeDef> typeDefinition,
        QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The currently available views.
    QSharedPointer<QList<QSharedPointer<View> > > availableViews_;
};

#endif // PORTTYPEVALIDATOR_H
