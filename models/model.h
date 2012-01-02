/* 
 *
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "port.h"
#include "view.h"
#include "modelparameter.h"

#include <QList>
#include <QMap>
#include <QDomNode>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QStringList>

class VLNV;

/*! \brief Equals to the spirit:model element in the IP-Xact specification
 *
 * The Model describes the views, ports and model-related parameters of a
 * component.
 */
class Model {

public:

	/*! \brief The constructor
	 *
	 * \param modelNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Model(QDomNode &modelNode);

	/*! \brief The default constructor
	 *
	*/
	Model();

	//! \brief Copy constructor
	Model(const Model &other);

	//! \brief Assignment operator
	Model &operator=(const Model &other);

	/*! \brief The destructor
	 *
	 */
	~Model();

	/*! \brief Get the model parameters of this model
	 *
	 * \return QMap containing pointers to the model parameters.
	 * 		Key = name of the model parameter.
	 * 		Value = Pointer to the model parameter.
	 */
	const QMap<QString, QSharedPointer<ModelParameter> >& getModelParameters();

	/*! \brief Get the ports of this model
	 *
	 * \return QMap containing pointers to the ports of this model
	 * 		Key = name of a port
	 * 		Value = pointer to the port
	 */
	const QMap<QString, QSharedPointer<Port> >& getPorts() const;

	/*! \brief Get the views of this model
	 *
	 * \return QList containing pointers to the views of this model
	 */
	const QList<QSharedPointer<View> >& getViews();

	/*! \brief Set the model parameters for this model
	 *
	 * Calling this function will delete the old model parameters.
	 *
	 * \param modelParameters A QMap containing pointers to the parameters.
	 * 		Key = name of the model parameter.
	 * 		Value = pointer to the model parameter.
	 */
	void setModelParameters(const
			QMap<QString, QSharedPointer<ModelParameter> > &modelParameters);

	/*! \brief Set the ports for this model
	 *
	 * Calling this function will delete the old ports of this model
	 *
	 * \param ports A QMap containing pointers to the ports
	 * 		Key = name of a port
	 * 		value = Pointer to the port
	 */
	void setPorts(const QMap<QString, QSharedPointer<Port> > &ports);

	/*! \brief Set the views for this model
	 *
	 * Calling this function will delete the old views of this model
	 *
	 * \param views A QList containing pointers to the views
	 */
	void setViews(const QList<QSharedPointer<View> > &views);

	/*! \brief Write the contents of the class using the writer.
	 *
	 * Uses the specified writer to write the class contents into file as valid
	 * IP-Xact.
	 *
	 * \param writer A reference to a QXmlStreamWriter instance that is used to
	 * write the document into file.
	 *
	 * Exception guarantee: basic
	 * \exception Write_error Occurs if class or one of it's member classes is
	 * not valid IP-Xact at the moment of writing.
	 */
	void write(QXmlStreamWriter& writer);

	/*! \brief Does this model contains a hierarchical view.
	 *
	 * This function checks it's views and if a hierarchical view is found.
	 *
	 * \return True if a hierarchyRef is found in one of the views, otherwise
	 * false.
	 */
	bool hasHierView() const;

	/*! \brief Get the VLNV of a design in model's view.
	 *
	 * This function searches the views of the model for a named view.
	 * If view with a given name is not found then invalid vlnv is returned.
	 * If the model does not have a hierarchical view then invalid vlnv is
	 * returned.
	 * If name is not given then the first found design VLNV is returned.
	 *
	 * \return A vlnv of a design that is used within this
	 * component. Null pointer if design is not found.
	 */
	VLNV getHierRef(const QString viewName = QString()) const;

	/*! \brief Get list of the design VLNVs that are referenced within model.
	 *
	 * \return QList containing pointers to the VLNVs
	 */
	const QList<VLNV> getHierarchyRefs() const;

	/*! \brief Set the hierarchical for given view.
	 *
	 * If view name is not specified then the hierarchical reference is set for
	 * the first found hierarchical view.
	 *
	 * \param vlnv The reference to set as the hierarhical reference.
	 * \param viewName The name of the view to set the hierarchical reference for.
	 *
	*/
	void setHierRef(const VLNV& vlnv, const QString& viewName = QString());

	/*! \brief Get the specific port of the model.
	 *
	 * This function can be used to find a specific port within a model. If
	 * named port is not found a null pointer is returned.
	 *
	 * \param name Name of the port.
	 *
	 * \return Pointer to the specified port.
	 */
	Port* getPort(const QString& name) const;

	/*! \brief Get the width of the given port.
	 *
	 * \param port Identifies the port in the component.
	 *
	 * \return int The number of bits in the port. -1 if unspecified or port is
	 * not found.
	*/
	int getPortWidth(const QString& port) const;

	/*! \brief Get the specific model paramter.
	 *
	 * This function can be used to find a specific model parameter within a
	 * model. If named model parameter is not found a null pointer is returned.
	 *
	 * \param name Name of the parameter
	 *
	 * \return A pointer to the named model parameter.
	 */
	ModelParameter* getParameter(const QString& name) const;

	/*! \brief Find a specified name within the model.
	 *
	 * \param name Name of the view to find.
	 *
	 * \return Pointer to the specified view. Null pointer if the view can't be
	 * found.
	 */
	View* findView(const QString name) const;

	/*! \brief Add a new view to the model.
	 *
	 * If a view with the same name already exists within this model it is
	 * deleted and replaces by the new view.
	 *
	 * \param newView A pointer to the new view to add to the model.
	 */
	void addView(View* newView);

	/*! \brief Remove the specified name from the model.
	 *
	 * If the named view is not found then nothing is done.
	 *
	 * \param name Name of the view to be removed.
	 */
	void removeView(const QString& name);

    /*! \brief Add a new model parameter to the model.
	 *
	 * \param param Pointer to the model parameter to add
	 *
	 * \return True if model parameter was successfully added. False if a model parameter with the same 
	 * name already existed in the model. If the model parameter exists in the model then 
	 * nothing is added.
	*/
	bool addModelParameter(QSharedPointer<ModelParameter> param);

	/*! \brief Remove a model parameter from the model.
	 *
	 * \param paramName The name of the model parameter to remove.
	 *
	*/
	void removeModelParameter(const QString& paramName);

	/*! \brief Get pointer to the QMap containing the model parameters.
	 *
	 * \return Pointer to the QMap containing the model parameters.
	*/
	QMap<QString, QSharedPointer<ModelParameter> >* getModelParametersPointer();

	/*! \brief Add a new port to the model.
	 *
	 * \param port Pointer to the port to add
	 *
	 * \return True if port was successfully added. False if a port with same 
	 * name already existed in the model. If port exists in the model then 
	 * nothing is added.
	*/
	bool addPort(QSharedPointer<Port> port);

	/*! \brief Remove a port from the model.
	 *
	 * \param portName The name of the port to remove.
	 *
	*/
	void removePort(const QString& portName);

	/*! \brief Rename a port in the model
	 *
	 * \param oldName The old name of the port.
	 * \param newName The new name to be set for the port.
	 *
	 * \return True if renaming was successful. False if port with old name was
	 * not found.
	*/
	bool renamePort(const QString& oldName, const QString& newName);

	/*! \brief Get pointer to the QMap containing the ports
	 *
	 * \return Pointer to the QMap containing the ports.
	*/
	QMap<QString, QSharedPointer<Port> >* getPortsPointer();

	/*! \brief Get names of the ports in this model.
	 *
	 * \return QStringList containing the port names.
	*/
	QStringList getPortNames() const;

	/*! \brief Create a new empty view to model.
	 *
	 * \return Pointer to the created view.
	*/
	View* createView();

	/*! \brief Get list of the view names in this model.
	 *
	 * \return QStringList containing the view names.
	*/
	QStringList getViewNames() const;

	/*! \brief Get the number of views listed in the model.
	 *
	 * \return The number of views listed in the metadata.
	*/
	int viewCount() const;

	/*! \brief Get the direction of the given port.
	 * 
	 * If port is not found then invalid direction is returned.
	 * 
	 * \param portName Name of the port.
	 *
	 * \return General::Direction specifies the direction of the port.
	*/
	General::Direction getPortDirection(const QString& portName) const;

	/*! \brief Get the hierarchical views of the model.
	 *
	 * \return QStringList containing the names of the hierarchical views.
	*/
	QStringList getHierViews() const;

	/*! \brief Get the names of the file sets used by given view.
	 *
	 * \param viewName The name of the view.
	 *
	 * \return QStringList contains the names of the needed file sets.
	*/
	QStringList getFileSetRefs(const QString& viewName) const;

	/*! \brief Checks if the model has model parameters.
	 *
	 * \return bool True if model parameters exist.
	*/
	bool hasModelParameters() const;

	/*! \brief Checks if the model has ports.
	 *
	 * \return bool True if ports exist.
	*/
	bool hasPorts() const;

	/*! \brief Check if the specified port exists on the implementation of the component.
	 *
	 * This function checks the direction of a wired port and returns true for
	 * directions: in, out and inout.
	 * 
	 * If port is not found then false is returned.
	 * 
	 * \param portName The name of the port to check.
	 *
	 * \return bool True if port's direction is in, out or inout.
	*/
	bool isPhysicalPort(const QString& portName) const;

	/*! \brief Checks if the model has views.
	 *
	 * \return bool True if views exist.
	*/
	bool hasViews() const;

	/*! \brief Get the default values of in and inout ports.
	 *
	 * \return QMap<QString, QString> contains the port names and default values.
	 * Key: Name of the port.
	 * Value: The default value for the specified port.
	*/
	QMap<QString, QString> getPortDefaultValues() const;

	/*! \brief Get the type definitions of the wired ports.
	 *
	 *
	 * \return QStringList contains the type definitions set for ports of component.
	*/
	QStringList getPortTypeDefinitions() const;

	/*! \brief Checks if the component has at least one port that's type is defined.
	 * 
	 * This function can be used to check if component needs to have at least one
	 * view because if port type is defined then there has to be view reference.
	 *
	 * \return bool True if at least one port with defined port type exists.
	*/
	bool hasPortTypes() const;

	/*! \brief Get the entity name of the component.
	 *
	 * \param viewName The name of the view that's entity name is requested.
	 *
	 * \return QString contains the name of the entity.
	*/
	QString getEntityName(const QString& viewName) const;

	/*! \brief Get the name of the architecture for the component.
	 *
	 * \param viewName The name of the view that's architecture is wanted.
	 *
	 * \return QString contains the architecture name.
	*/
	QString getArchitectureName(const QString& viewName) const;

	/*! \brief Get list of the physical port names and their bounds.
	 *
	 * \return QList containing the port names and bounds.
	*/
	QList<General::PortBounds> getPortBounds() const;

private:

	/*!
	 * OPTIONAL
	 * Contains the views for this model
	 */
	QList<QSharedPointer<View> > views_;

	/*!
	 * OPTIONAL
	 * Contains the ports for this model.
	 *
	 * Key = name of the port
	 * Value = pointer to the port
	 */
	QMap<QString, QSharedPointer<Port> > ports_;

	/*!
	 * OPTIONAL
	 * Contains the model parameters for this model.
	 *
	 * Key = name of the modelParameter
	 * Value = Pointer to the modelParameter
	 */
	QMap<QString, QSharedPointer<ModelParameter> > modelParameters_;

};

#endif /* MODEL_H_ */
