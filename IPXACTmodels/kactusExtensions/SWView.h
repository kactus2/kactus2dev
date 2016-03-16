//-----------------------------------------------------------------------------
// File: SWView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// SW view class for making VLNV references to SW designs.
//-----------------------------------------------------------------------------

#ifndef SWVIEW_H
#define SWVIEW_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/FileBuilder.h>

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/kactusExtensions/BSPBuildCommand.h>
#include <IPXACTmodels/kactusExtensions/SWFileBuilder.h>

#include <QString>
#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QList>

//-----------------------------------------------------------------------------
//! SW view class for making VLNV references to SW designs.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SWView : public NameGroup, public VendorExtension
{
public:
    /*!
     *  Constructor.
	 *
	 *      @param [in] viewNode A reference to a QDomNode to parse the information from.
	 */
	SWView(QDomNode &viewNode);

	/*!
     *  The constructor
     *
	 *      @param [in] name The name of the view.
	 */
	SWView(const QString& name);

	/*!
     *  Default constructor.
	 */
	SWView();

	//! Copy constructor.
	SWView(const SWView &other);

	//! Assignment operator.
	SWView &operator=(const SWView &other);

	/*!
     *  Destructor.
	 */
	~SWView();

    /*!
     *  Clone the sw view.
     *
     *      @return the cloned swView.
     */
    virtual SWView* clone() const;

    /*!
     *  Get the type of this extension.
     *
     *      @return The type of this extension.
     */
    virtual QString type() const;

	/*! 
     *  Writes the contents of the class using the writer.
	 *
	 *  Uses the specified writer to write the class contents into file as valid
	 *  IP-Xact.
	 *
	 *			@param [in] withinHWComp Specifies if the SW view is within HW or SW component.
	 *      @param [in] writer A reference to a QXmlStreamWriter instance that is used to
	 *                         write the document into file.
	 */
    virtual void write(QXmlStreamWriter& writer) const;

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 *		@param [in] fileSetNames	  Contains the names of the component's file sets.
	 *		@param [in] cpuNames			  Contains the names of the component's CPU elements.
	 *		@param [in] errorList        The list to add the possible error messages to.
	 *      @param [in] parentIdentifier String from parent to help to identify the location of the error.
	 *
	 *      @return True if contents are valid.
	 */
	bool findErrorsIn(QStringList const& fileSetNames, QStringList const& cpuNames, QVector<QString>& errorList,
        QString const& parentIdentifier) const;

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 *      @param [in] fileSetNames    Contains the names of the component's file sets.
	 *      @param [in] cpuNames        Contains the names of the component's CPU elements.
	 * 
	 *      @return True if contents are valid.
	 */
	bool isValid(QStringList const& fileSetNames, QStringList const& cpuNames) const;

	/*!
     *  Get pointer to the hierarchical design for this view.
	 *
	 *      @return A pointer to the vlnv of a hierarchical design if the view is used to reference a hierarchical design.
	 */
	VLNV getHierarchyRef() const;

	/*!
     *  Set the hierarchyRef for this view.
	 *
	 *      @param [in] hierarchyRef    A pointer to the new hierarchyRef
	 */
	void setHierarchyRef(const VLNV& hierarchyRef);

	/*!
     *  Get the file set references of the SW view.
	 *
	 *      @return QStringList containing the file set names.
	 */
	QStringList getFileSetRefs() const;

	/*!
     *  Get the file set references of the SW view.
	 *
	 *      @return QStringList containing the file set names.
	 */
	QStringList getFileSetRefs();

	/*!
     *  Set the file set references for the SW view.
	 *
	 *      @param [in] fileSetRefs     Contains the file set names to set.
	 */
	void setFileSetRefs(QStringList const& fileSetRefs);

	/*!
     *  Add a file set reference to the SW view.
	 *
	 *      @param [in] fileSetName     The name of the file set to refer to.
	 */
	void addFileSetRef(const QString& fileSetName);

	//! Get the SW build commands of the SW view.
    QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > getSWBuildCommands() const;

    /*!
     *  Set the sw build commands.
     *
     *      @param [in] newSWBuildCommands  Pointer to a list of new sw build commands.
     */
    void setSWBuildCommands(QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > newSWBuildCommands);

	//! Get the build command for the board support package.
	QSharedPointer<BSPBuildCommand> getBSPBuildCommand();

    void setBSPBuildCommand(QSharedPointer<BSPBuildCommand> newCommand);

    /*!
     *  Set the status of the view in HW component.
     *
     *      @param [in] newWithinHWStatus   True if the view is in hw component, false otherwise.
     */
    void setViewInHWComponentStatus(bool newWithinHWStatus);

private:

    /*!
     *  Copy the SW build commands.
     *
     *      @param [in] other   SWView being copied.
     */
    void copySwBuildCommands(const SWView& other);

    /*!
     *  Copy the BSP build command.
     *
     *      @param [in] other   SWView being copied.
     */
    void copyBSPBuildCommand(const SWView& other);

	//! References a hierarchical design, mandatory only if the view is used to reference a hierarchical design.
	VLNV hierarchyRef_;

	//! The file set references used by this SW view.
	QStringList filesetRefs_;

	//! Contains the SW build commands for SW view.
    QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > swBuildCommands_;

	//! Contains the command options to build the BSP package for HW component.
	QSharedPointer<BSPBuildCommand> bspCommand_;

};

//-----------------------------------------------------------------------------

#endif // SWVIEW_H
