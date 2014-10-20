//-----------------------------------------------------------------------------
// File: IPXactDiff.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Class for capturing changes in IP-XACT elements.
//-----------------------------------------------------------------------------

#ifndef IPXACTDIFF_H
#define IPXACTDIFF_H

#include <QString>
#include <QList>

//-----------------------------------------------------------------------------
//! Class for capturing changes in IP-XACT elements.
//-----------------------------------------------------------------------------
class IPXactDiff
{
public:
    
    //! The different change types.
    enum DiffType 
    {
        ADD = 0,        //<! Element added.
        REMOVE,         //<! Element removed.
        MODIFICATION,   //<! Element modified.
        NO_CHANGE       //<! Element unchanged.
    };
    
    //! Struct for storing modifications.
    struct Modification
    {
        //! The name of the modified element e.g. default value.
        QString modifiedElement;

        //! The previous value of the element.
        QString previousValue;

        //! The modified value of the element.
        QString newValue;
    };

    /*!
     *  The constructor.
     *
     *      @param [in] element     The identifier for the IP-XACT element.
     *      @param [in] name        The name of the element.
     */
    IPXactDiff(QString const& element, QString const& name = "");

    //! The destructor.
    ~IPXactDiff();
    
    /*!
     *  Returns the identifier for the element.
     *
     *      @return The identifier for the element.
     */
    QString element() const;

    /*!
     *  Returns the name of the element.
     *
     *      @return The name of the element.
     */
    QString name() const;

    /*!
     *  Returns the change type.
     *
     *      @return The change type.
     */
    IPXactDiff::DiffType changeType() const;

    /*!
     *  Sets the change type.
     *
     *      @param [in] type   The type to set.
     */
    void setChangeType(DiffType type);

    /*!
     *  Gets the modifications in the contained elements.
     *
     *      @return The modifications to contained elements.
     */
    QList<Modification> getChangeList() const;

    /*!
     *  Checks if the given element has changed. If the element has changed, stores the modification.
     *
     *      @param [in] element         The name of the contained element.
     *      @param [in] referenceValue  The value in the reference element.
     *      @param [in] subjectValue    The value in the subject element.
     */
    void checkForChange(QString const& element, QString const& referenceValue, QString const& subjectValue);

private:

    //! Disable copying.
    IPXactDiff(IPXactDiff const& rhs);
    IPXactDiff& operator=(IPXactDiff const& rhs);

    //! The element identifier e.g. port.
    QString element_;

    //! The name of the element e.g. clk.
    QString name_;

    //! The type of the change.
    IPXactDiff::DiffType changeType_;

    //! The modifications to the element.
    QList<Modification> modifications_;
};

#endif // IPXACTDIFF_H
