//-----------------------------------------------------------------------------
// File: GeneratorConfiguration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Container class for generator configuration.
//-----------------------------------------------------------------------------

#ifndef GENERATORCONFIGURATION_H
#define GENERATORCONFIGURATION_H

#include <QString>
#include <QSharedPointer>

class View;
class ComponentInstantiation;

//-----------------------------------------------------------------------------
//! Container class for generator configuration.
//-----------------------------------------------------------------------------
class GeneratorConfiguration 
{
public:

	//! The constructor.
	GeneratorConfiguration();

	//! The destructor.
	~GeneratorConfiguration();
    
    /*!
     *  Sets the active view for the top component in generation.
     *
     *      @param [in] view   The active view.
     */
    void setActiveView(QSharedPointer<View> view);

    /*!
     *  Gets the active view for the top component.
     *
     *      @return The active view.
     */
	QSharedPointer<View> getActiveView() const;
	
    /*!
     *  Sets the component instantiation for the active view.
     *
     *      @param [in] instantiation   The ComponentInstantiation.
     */
	void setInstantiation(QSharedPointer<ComponentInstantiation> instantiation);
	
    /*!
     *  Gets the component instantiation for the active view.
     *
     *      @return The ComponentInstantiation.
     */
	QSharedPointer<ComponentInstantiation> getInstantiation() const;

    /*!
     *  Sets the file set referred by the instantiation.
     *
     *      @param [in] fileSetRef   The fileSetRef to set.
     */
    void setFileSetRef(QString const& fileSetRef);

    /*!
     *  Gets the file set referred by the instantiation.
     *
     *      @return The fileSetRef.
     */
    QString getFileSetRef() const;

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
     *  Sets the path for the output file for the generation.
     *
     *      @param [in] path   The path to set.
     */
    void setOutputPath(QString const& path);

    /*!
     *  Gets the output path for generation.
     *
     *      @return The path to output file.
     */
    QString getOutputPath() const;

private:

	// Disable copying.
	GeneratorConfiguration(GeneratorConfiguration const& rhs);
	GeneratorConfiguration& operator=(GeneratorConfiguration const& rhs);

    //! The path for output file.
    QString outputPath_;

    //! The active view of the top component.
	QSharedPointer<View> activeView_;

	//! The component instantiation of the active view
	QSharedPointer<ComponentInstantiation> instantiation_;

	//! Tthe file set referred by the instantiation.
	QString fileSetRef_;

    //! Flag for indicating if the output file should be saved to top component file sets.
    bool saveToFileset_;
};

#endif // GENERATORCONFIGURATION_H
