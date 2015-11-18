//-----------------------------------------------------------------------------
// File: SystemView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// System view class for making VLNV references to System designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVIEW_H
#define SYSTEMVIEW_H

#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QDomNode>
#include <QString>
#include <QStringList>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! System view class for making VLNV references to system designs.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SystemView : public NameGroup, public VendorExtension
{
public:
    /*!
     *  Constructor.
	 *
	 *      @param [in] viewNode A reference to a QDomNode to parse the information from.
	 */
	SystemView(QDomNode &viewNode);

	/*!
     *  The constructor
     *
	 *      @param [in] name The name of the view.
	 */
	SystemView(QString const& name);

	/*!
     *  Default constructor.
	 */
	SystemView();

	//! Copy constructor.
	SystemView(const SystemView &other);

	//! Assignment operator.
	SystemView &operator=(const SystemView &other);

	/*!
     *  Destructor.
	 */
	~SystemView();

    /*!
     *  Clone this system view.
     *
     *      @return The cloned system view.
     */
    virtual SystemView* clone() const;

    /*!
     *  Get the type of this extension.
     *
     *      @return The type of this extension.
     */
    virtual QString type() const;

	/*! 
     *  Writes the contents of the class using the writer.
	 *
	 *  Uses the specified writer to write the class contents into file as valid IP-Xact.
	 *
	 *      @param [in] writer A QXmlStreamWriter instance that is used to write the document into file.
	 */
	virtual void write(QXmlStreamWriter& writer) const;

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 *		@param [in] fileSetNames	 File set names of the containing component.
	 *		@param [in] HWViewNames		 Names of the HW views in the containing component.
	 *      @param [in] errorList        The list to add the possible error messages to.
	 *      @param [in] parentIdentifier String from parent to help to identify the location of the error.
	 *
	 *      @return True if contents are valid.
	 */
	bool isValid(QStringList const& fileSetNames, QStringList const& HWViewNames, QVector<QString>& errorList,
        QString const& parentIdentifier) const;

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 *		  @param [in] fileSetNames  File set names of the containing component.
	 *		  @param [in] HWViewNames   Names of the HW views in the containing component.
	 * 
	 *      @return True if contents are valid.
	*/
	bool isValid(QStringList const& fileSetNames, QStringList const& HWViewNames) const;

    /*!
     *  Set the hierarchyRef for this view.
	 *
	 *      @param [in] hierarchyRef    The new hierarchyRef.
	 */
	void setHierarchyRef(VLNV const& hierarchyRef);

    /*!
     *  Sets the reference to HW view to which the system is mapped.
     *
     *      @param [in] viewName The name of the HW view.
     */
    void setHWViewRef(QString const& viewName);

	/*!
     *  Get pointer to the hierarchical design for this view.
	 *
	 *      @return The vlnv of a hierarchical design if the view is used to reference a hierarchical design.
	 */
	VLNV getHierarchyRef() const;

    /*!
     *  Returns the reference to HW view to which the system is mapped.
     */
    QString getHWViewRef() const;

	/*!
     *  Get the file set references of the system view.
	 *
	 *      @return QStringList containing the file set names.
	 */
	QStringList getFileSetRefs() const;

	/*!
     *  Set the file set references for the system view.
	 *
	 *      @param [in] fileSetRefs     Contains the file set names.
	 */
	void setFileSetRefs(QStringList const& fileSetRefs);

private:

	//! References a hierarchical design, mandatory only if the view is used to reference a hierarchical design.
	VLNV hierarchyRef_;

    //! The name of the HW view to which the system references.
    QString hwViewRef_;

	//! Contains the references to file sets of the containing component.
	QStringList fileSetRefs_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMVIEW_H
