//-----------------------------------------------------------------------------
// File: ViewSelection.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 13.09.2016
//
// Description:
// Container class for view selection.
//-----------------------------------------------------------------------------

#ifndef VIEWSELECTION_H
#define VIEWSELECTION_H

#include <QString>
#include <QMap>
#include <QSharedPointer>

class View;
class ComponentInstantiation;
class FileSet;

//-----------------------------------------------------------------------------
//! Container class for view selection.
//-----------------------------------------------------------------------------
class ViewSelection
{
public:

	//! The constructor.
	ViewSelection( QString targetLanguage,
		QSharedPointer<QList<QSharedPointer<View> > > views, 
		QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations, 
		QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets);

	//! The destructor.
	~ViewSelection();
	
    /*!
     *  Gets the names of the possible views .
     */
	QStringList viewNames() const;
	
    /*!
     *  Gets the names of the possible component instantiations.
     */
	QStringList instantiationNames() const;
	
    /*!
     *  Gets the names of the file sets.
     */
	QSharedPointer<QStringList> fileSetNames() const;

    /*!
     *  Returns the name of default view, based on the target language.
     *  If there is no suitable view with suitable component instantiation, return empty string.
     *
     *       @return The name of the view.
	 */
	QString getDefaultView();

    /*!
     *  Sets the view for the top component in generation.
     *
     *      @param [in] view   The active view.
	 */
	void setView(QSharedPointer<View> view);

    /*!
     *  Sets the view for the top component in generation, if it exists.
     *
	 *      @param [in] viewName   The name of the active view.
	 *
	 *      @return The component instantiation reference of the view, if the view exists.
	 */
    QString setView(QString viewName);

    /*!
     *  Gets the active view for the top component.
     *
     *      @return The active view.
     */
	QSharedPointer<View> getView() const;
	
    /*!
     *  Sets the component instantiation for the active view.
     *
	 *      @param [in] instantiation   The ComponentInstantiation.

	 *      @return True, if the instantiation exists, else false.
	 */
	bool setInstantiation(QString name);
	
    /*!
     *  Gets the selected component instantiation.
     */
	QSharedPointer<ComponentInstantiation> getInstantiation() const;

    /*!
     *  Gets the name of the selected instantiation, even if it does not exist.
     */
	QString getInstantiationName() const;

    /*!
     *  Sets the file set referred by the instantiation.
     *
     *      @param [in] fileSetRef   The fileSetRef to set.
     */
    bool setFileSet(QString const& fileSetRef);
	
    /*!
     *  Gets the file set referred by the instantiation.
     *
     *      @return The fileSetRef.
     */
	QSharedPointer<FileSet> getFileSet() const;
	
    /*!
     *  Gets the name of the selected file set, even if it does not exist.
     */
    QString getFileSetName() const;

    /*!
     *  Sets whether the output file should be added to top component file sets or not.
     *
     *      @param [in] shouldSave   True for saving, false for not saving.
     */
    void setSaveToFileset(bool shouldSave);

    /*!
     *  Checks if the output file should be added to top component file sets.
     *
     *      @return True, if file should be saved, false otherwise.
     */
    bool getSaveToFileset() const;
	
    /*!
     *  Gets the target language of the generation.
     */
    QString getTargetLanguage() const;
	
    /*!
     *  Gets the language of the selected instantiation, or empty string if none exists.
     */
    QString getCurrentLanguage() const;

private:

	// Disable copying.
	ViewSelection(ViewSelection const& rhs);
	ViewSelection& operator=(ViewSelection const& rhs);

    //! The language for generation.
    QString targetLanguage_;
    //! The possible views, indexed by their names.
    QMap<QString,QSharedPointer<View> > views_;
    //! The possible component instantiations, indexed by their names.
    QMap<QString,QSharedPointer<ComponentInstantiation> > instantiations_;
    //! The possible file sets, indexed by their names.
    QMap<QString,QSharedPointer<FileSet> > fileSets_;

    //! The selected view.
	QSharedPointer<View> view_;

	//! The selected component instantiation and its name.
	QSharedPointer<ComponentInstantiation> instantiation_;
	QString instantiationName_;

	//! The selected file set and its name.
	QSharedPointer<FileSet> fileSet_;
	QString fileSetRef_;

    //! Flag for indicating if the output file should be saved to top component file sets.
    bool saveToFileset_;
};

#endif // VIEWSELECTION_H
