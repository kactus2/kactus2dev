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
struct GenerationSettings;

//-----------------------------------------------------------------------------
//! Container class for view selection.
//-----------------------------------------------------------------------------
class ViewSelection
{
public:

	//! The constructor.
    ViewSelection(QString const& targetLanguage,
        bool saveToFileset, QString const& targetGroup,
        GenerationSettings* settings, QSharedPointer<QList<QSharedPointer<View> > > views,
        QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations,
        QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets);

	//! The destructor.
	~ViewSelection();
    
    /*!
     *  Checks if the view configuration is valid.
     *
     *      @param [out] warning   The stated reason for not accepting.
     */
    bool validSelections(QString &warning);
	
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
	QStringList fileSetNames() const;

    /*!
     *  Sets the view for the top component in generation, if it exists.
     *
	 *      @param [in] viewName   The name of the active view.
	 */
    void setView(QString const& viewName);

    /*!
     *  Gets the active view for the top component.
     */
	QSharedPointer<View> getView() const;

    /*!
     *  Gets the name of the active view for the top component.
     */
	QString getViewName() const;
	
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

	//! The component instantiation of the current view.
	QSharedPointer<ComponentInstantiation> instantiation_;

	//! The selected file set and its name.
	QSharedPointer<FileSet> fileSet_;
	QString fileSetRef_;

    //! Flag for indicating if the output file should be saved to top component file sets.
    bool saveToFileset_;
};

#endif // VIEWSELECTION_H
