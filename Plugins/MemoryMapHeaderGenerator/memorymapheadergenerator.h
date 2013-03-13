/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.h
 * 	Project:	Kactus 2
*/

#ifndef MEMORYMAPHEADERGENERATOR_H
#define MEMORYMAPHEADERGENERATOR_H


#include "memorymapheadergenerator_global.h"

#include <PluginSystem/IGeneratorPlugin.h>
#include <models/component.h>
#include <models/librarycomponent.h>
#include <models/design.h>
#include "globalheadersavemodel.h"

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QFileInfo>
#include <QTextStream>
#include <QList>

class IPluginUtility;

/*! \brief The plugin to generate C-headers of component memory maps.
 *
 */
class MEMORYMAPHEADERGENERATOR_EXPORT MemoryMapHeaderGenerator : public QObject, public IGeneratorPlugin {

	Q_OBJECT
	Q_PLUGIN_METADATA(IID "kactus2.plugins.MemoryMapHeaderGenerator" FILE "headerGenerator.json")

	Q_INTERFACES(IPlugin)
	Q_INTERFACES(IGeneratorPlugin)

public:

	//! \brief The name of the file set where generated files are added to.
	static const QString DEFAULT_TARGET_FILESET;

	/*! \brief The constructor
	 *
	 * Method: 		MemoryMapHeaderGenerator
	 * Full name:	MemoryMapHeaderGenerator::MemoryMapHeaderGenerator
	 * Access:		public 
	 *
	*/
	MemoryMapHeaderGenerator();
	
	//! \brief The destructor
	virtual ~MemoryMapHeaderGenerator();

	//! \brief Returns the name of the plugin.
    virtual QString const& getName() const;

	//! \brief Returns the version of the plugin.
    virtual QString const& getVersion() const;

	//! \brief Returns the description of the plugin.
    virtual QString const& getDescription() const;

	//! \brief Returns the icon for the generator.
    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;

    /*! \brief Checks whether the generator supports generation for the given library component.
     *
     * Method: 		checkGeneratorSupport
     * Full name:	MemoryMapHeaderGenerator::checkGeneratorSupport
     * Access:		virtual public 
     *
     * \param libComp The library component for which to check support.
     * \param libDes The design object if the generator is run on a hierarchical component.
     *
     * \return True, if the generator supports the given component. Otherwise false.
    */
    virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp,
		QSharedPointer<LibraryComponent const> libDes = QSharedPointer<LibraryComponent const>()) const;

    /*! \brief Runs the generator.
     *
     * Method: 		runGenerator
     * Full name:	MemoryMapHeaderGenerator::runGenerator
     * Access:		virtual public 
     *
     * \param utility The plugin utility interface.
     * \param libComp The component for which the generator is run.
     * \param libDes The design object if the generator is run on a hierarchical component.
     *
    */
    virtual void runGenerator(IPluginUtility* utility,
                              QSharedPointer<LibraryComponent> libComp,
							  QSharedPointer<LibraryComponent> libDes = QSharedPointer<LibraryComponent const>());

private:

	//! \brief No copying
	MemoryMapHeaderGenerator(const MemoryMapHeaderGenerator& other);

	//! \brief No assignment
	MemoryMapHeaderGenerator& operator=(const MemoryMapHeaderGenerator& other);

	/*! \brief Generate headers for local memory maps of a component.
	 *
	 * Method: 		generateLocalMemMapHeaders
	 * Full name:	MemoryMapHeaderGenerator::generateLocalMemMapHeaders
	 * Access:		private 
	 *
	 * \param comp Pointer to the component which contains the local memory maps.
	 *
	*/
	void generateLocalMemMapHeaders( QSharedPointer<Component> comp );

	/*! \brief Generate global headers for the CPU instances of the design.
	 *
	 * Method: 		generateGlobalHeaders
	 * Full name:	MemoryMapHeaderGenerator::generateGlobalHeaders
	 * Access:		private 
	 *
	 * \param comp Pointer to the top hierarchical component to which the headers are added to.
	 * \param design Pointer to the design which instantiates the CPU instances.
	 *
	*/
	void generateGlobalHeaders(QSharedPointer<Component> comp, QSharedPointer<Design> design);

	/*! \brief Add a generated file to the file sets of a component.
	 *
	 * Method: 		addHeaderFile
	 * Full name:	MemoryMapHeaderGenerator::addHeaderFile
	 * Access:		private 
	 *
	 * \param component Pointer to the component where the file is added to.
	 * \param fileInfo Contains the info on the generated file.
	 * \param filesetName The name of the file set to add the header file to.
	 *
	*/
	void addHeaderFile(QSharedPointer<Component> component, const QFileInfo& fileInfo, const QString& filesetName) const;

	/*! \brief Parse the interface and depending on the type move forward in the connections.
	 *
	 * Method: 		parseInterface
	 * Full name:	MemoryMapHeaderGenerator::parseInterface
	 * Access:		private 
	 *
	 * \param offset The offset used as base for the addresses.
	 * \param stream The text stream where the address defines are written into. 
	 * \param interface Identifies the current interface to parse.
	 *
	*/
	void parseInterface(qint64 offset,
		QTextStream& stream,
		const Design::Interface& interface);

	//! The plugin utility.
	IPluginUtility* utility_;

	//! \brief Pointer to the design being operated when creating global headers.
	QSharedPointer<Design> design_;

	//! \brief The list where all operated interfaces are added to when generating global headers.
	QList<Design::Interface> operatedInterfaces_;
};

#endif // MEMORYMAPHEADERGENERATOR_H
