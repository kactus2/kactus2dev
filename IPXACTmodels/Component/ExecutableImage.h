//-----------------------------------------------------------------------------
// File: ExecutableImage.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.6.2023
//
// Description:
// Implementation for ipxact:executableImage element.
//-----------------------------------------------------------------------------

#ifndef EXECUTABLE_IMAGE_H
#define EXECUTABLE_IMAGE_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/Region.h>
#include <IPXACTmodels/Component/LanguageTools.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:executableImage element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ExecutableImage : public NameGroup, public Extendable
{
public:

	/*!
	 *  The default constructor.
	 *
	 *    @param [in] name            Name of the executable image.
	 */
	ExecutableImage(QString const& name = QString());
	
	//! The destructor.
	virtual ~ExecutableImage();

	//! The copy constructor.
	ExecutableImage(const ExecutableImage& other);

	//! The assignment operator.
	ExecutableImage& operator=(const ExecutableImage& other);

	/*!
	 *  Get the imageId attribute.
	 *
	 *    @return The imageId attribute.
	 */
	QString getImageId() const;

	/*!
	 *  Set the imageId attribute.
	 *
	 *    @param [in] id The id to set.
	 */
	void setImageId(QString const& id);
		
	/*!
	 *  Get the imageType attribute.
	 *
	 *    @return The imageType attribute.
	 */
	QString getImageType() const;

	/*!
	 *  Set the imageType attribute.
	 *
	 *    @param [in] type The imageType to set.
	 */
	void setImageType(QString const& type);

    /*!
	 *  Get the parameters of the executable image.
	 *
	 *    @return Pointer to a list holding the parameters for this executable image.
	 */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

	/*!
	 *  Set the parameter for the executable image.
	 *
	 *    @param [in] parameters The parameters to set.
	 */
	void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters);

	/*!
	 *  Get the language tools defined for the executable image.
	 *
	 *    @return The language tools of the executable image.
	 */
	QSharedPointer<LanguageTools> getLanguageTools() const;

    /*!
     *  Set the language tools for the executable image.
     *
     *    @param [in] languageTools The language tools to set.
     */
    void setLanguageTools(QSharedPointer<LanguageTools> languageTools);

private:

	//! Copy parameters from other executable image.
    void copyParameters(ExecutableImage const& other);

    //! Copy language tools from other executable image.
    void copyLanguageTools(ExecutableImage const& other);

	//! The imageId attribute.
    QString imageId_;
    
	//! The imageType attribute.
	QString imageType_;

	//! The parameters of the executable image.
	QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_ =
		QSharedPointer<QList<QSharedPointer<Parameter> > >(new QList<QSharedPointer<Parameter> >);

	//! The language tools of the exeuctable image.
	QSharedPointer<LanguageTools> languageTools_ = nullptr;
};


#endif // EXECUTABLE_IMAGE_H