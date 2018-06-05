//-----------------------------------------------------------------------------
// File: WireTypeDef.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// Describes the ipxact:wireTypeDef element.
//-----------------------------------------------------------------------------

#ifndef WIRETYPEDEF_H
#define WIRETYPEDEF_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QStringList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Describes the ipxact:wireTypeDef element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT WireTypeDef
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] typeName        The type name for the wire type definition.
     *      @param [in] viewNameRef     The name of the referenced view.
     */
    WireTypeDef(const QString& typeName = QString(), const QString& viewNameRef = QString());

    /*!
     *  The copy constructor.
     */
    WireTypeDef(const WireTypeDef& other);

    /*!
     *  Assignment operator.
     */
    WireTypeDef& operator=(const WireTypeDef& other);

    /*!
     *  The destructor.
     */
    ~WireTypeDef();

    /*!
     *  Check if a wire type definition has been determined for a given view.
     *
     *      @param [in] viewName    The name of the view to search.
     *
     *      @return True, if the view is found within a view name reference.
     */
    bool hasView(QString const& viewName);

    /*!
     *  Get the type name.
     *
     *      @return The type name.
     */
    QString getTypeName() const;

    /*!
     *  Set the type name.
     *
     *      @param [in] newTypeName     The new type name.
     */
    void setTypeName(QString const& newTypeName);

    /*!
     *  Check if the number of bits in the type declaration is fixed.
     *
     *      @return True, if the number of bits is fixed, false otherwise.
     */
    bool isConstrained() const;

    /*!
     *  Set the number of bits in the type declaration to be fixed / not fixed.
     *
     *      @param [in] constrainedStatus   The new status.
     */
    void setConstrained(bool constrainedStatus);

    /*!
     *  Get the type definitions.
     *
     *      @return A list of type definitions.
     */
    QSharedPointer<QStringList> getTypeDefinitions() const;

    /*!
     *  Set the type definitions.
     *
     *      @param [in] newTypeDefinitions  A list of new type definitions.
     */
    void setTypeDefinitions(QSharedPointer<QStringList> newTypeDefinitions);

    /*!
     *  Get the view references.
     *
     *      @return A list of view references.
     */
    QSharedPointer<QStringList> getViewRefs() const;

    /*!
     *  Set the view references.
     *
     *      @param [in] newViewRefs     A new list of view references.
     */
    void setViewRefs(QSharedPointer<QStringList> newViewRefs);

private:

    /*!
     *  Copy the type definitions.
     *
     *      @param [in] newTypeDefinitions  List of new type definitions.
     */
    void copyTypeDefinitions(QSharedPointer<QStringList> newTypeDefinitions);

    /*!
     *  Copy the view references.
     *
     *      @param [in] newViewReferences   List of new view references.
     */
    void copyViewNameReferences(QSharedPointer<QStringList> newViewReferences);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Defines the name of the type for the port
    QString typeName_;

    //! Indicates whether or not the number of bits in the type declaration is fixed or not.
    bool constrained_;

    //! Contains language specific reference to where given type is actually defined.
    QSharedPointer<QStringList> typeDefinitions_;

    //! Indicates the view or views in which this type definition applies.
    QSharedPointer<QStringList> viewNameRefs_;
};

#endif // WIRETYPEDEF_H
