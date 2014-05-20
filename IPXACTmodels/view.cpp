/* 
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#include "view.h"
#include <library/LibraryManager/vlnv.h>
#include "parameter.h"
#include "generaldeclarations.h"

#include <QDomNode>
#include <QObject>
#include <QString>
#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>
#include "XmlUtils.h"
#include "Kactus2Extension.h"

View::View(QDomNode &viewNode): 
nameGroup_(viewNode),
envIdentifiers_(),
language_(),
languageStrict_(false),
modelName_(),
fileSetRefs_(), 
constraintSetRefs_(),
parameters_(),
hierarchyRef_(), 
defaultFileBuilders_(),
topLevelViewRef_(),
vendorExtensions_()
{
	for (int i = 0; i < viewNode.childNodes().count(); ++i) {
		QDomNode tempNode = viewNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:envIdentifier")) {
			envIdentifiers_.append(tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:language")) {
			language_ = tempNode.childNodes().at(0).nodeValue();

			// get the spirit:strict attribute
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			QString strict = attributeMap.namedItem(QString("spirit:strict")).
					childNodes().at(0).nodeValue();

			languageStrict_ = General::str2Bool(strict, false);
		}

		else if (tempNode.nodeName() == QString("spirit:modelName")) {
			modelName_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:fileSetRef")) {

			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:localName")) {

					// strip the name of embedded whitespace characters
					QString localName = tempNode.childNodes().at(j).
							childNodes().at(0).nodeValue();
					localName = General::removeWhiteSpace(localName);
					fileSetRefs_.append(localName);
				}
			}
		}

		else if (tempNode.nodeName() == QString("spirit:constraintSetRef")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QString constraintSetRef = tempNode.childNodes().
						at(j).childNodes().at(0).nodeValue();
				constraintSetRef = General::removeWhiteSpace(constraintSetRef);
				constraintSetRefs_.append(constraintSetRef);
			}
		}

		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);
				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}

		else if (tempNode.nodeName() == QString("spirit:hierarchyRef")) {
			hierarchyRef_ = General::createVLNV(tempNode, VLNV::DESIGN);
		}

		else if (tempNode.nodeName() == QString("spirit:defaultFileBuilder")) {
			defaultFileBuilders_.append(
				QSharedPointer<FileBuilder>(new FileBuilder(tempNode)));
		}

        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            parseVendorExtensions(tempNode);
        }
    }
}

View::View(const QString name): 
nameGroup_(name), 
envIdentifiers_(),
language_(),
languageStrict_(false),
modelName_(),
fileSetRefs_(), 
constraintSetRefs_(),
parameters_(),
hierarchyRef_(), 
defaultFileBuilders_(),
topLevelViewRef_(),
vendorExtensions_()
{
}

View::View(): 
nameGroup_(),
envIdentifiers_(),
language_(),
languageStrict_(false),
modelName_(),
fileSetRefs_(), 
constraintSetRefs_(),
parameters_(), 
hierarchyRef_(), 
defaultFileBuilders_(),
topLevelViewRef_(),
vendorExtensions_()
{
}

View::View( const View &other ):
nameGroup_(other.nameGroup_),
envIdentifiers_(other.envIdentifiers_),
language_(other.language_),
languageStrict_(other.languageStrict_),
modelName_(other.modelName_),
fileSetRefs_(other.fileSetRefs_),
constraintSetRefs_(other.constraintSetRefs_),
parameters_(),
hierarchyRef_(other.hierarchyRef_),
defaultFileBuilders_(),
topLevelViewRef_(),
vendorExtensions_()
{
	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}

	foreach (QSharedPointer<FileBuilder> builder, other.defaultFileBuilders_) {
		if (builder) {
			QSharedPointer<FileBuilder> copy = QSharedPointer<FileBuilder>(
				new FileBuilder(*builder.data()));
			defaultFileBuilders_.append(copy);
		}
	}

    copyVendorExtensions(other);
}

View & View::operator=( const View &other ) {
	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		envIdentifiers_ = other.envIdentifiers_;
		language_ = other.language_;
		languageStrict_ = other.languageStrict_;
		modelName_ = other.modelName_;
		fileSetRefs_ = other.fileSetRefs_;
		constraintSetRefs_ = other.constraintSetRefs_;
		hierarchyRef_ = other.hierarchyRef_;

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}

		defaultFileBuilders_.clear();
		foreach (QSharedPointer<FileBuilder> builder, other.defaultFileBuilders_) {
			if (builder) {
				QSharedPointer<FileBuilder> copy = QSharedPointer<FileBuilder>(
					new FileBuilder(*builder.data()));
				defaultFileBuilders_.append(copy);
			}
		}

        copyVendorExtensions(other);
	}
	return *this;
}

View::~View() {
	parameters_.clear();
}

void View::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:view");

    nameGroup_.write(writer);

	// write all spirit:envIdentifier elements
	for (int i = 0; i < envIdentifiers_.size(); ++i) {
		writer.writeTextElement("spirit:envIdentifier", envIdentifiers_.at(i));
	}

	// write spirit:hierarchyRef if one exists
	if (hierarchyRef_.isValid()) {
		writer.writeEmptyElement("spirit:hierarchyRef");
		General::writeVLNVAttributes(writer, &hierarchyRef_);
	}

	// if optional spirit:language is defined
	if (!language_.isEmpty()) {
		// start the spirit:language tag
		writer.writeStartElement("spirit:language");

		// write the attribute for the element
		writer.writeAttribute("spirit:strict",
				General::bool2Str(languageStrict_));

		// write the value of the element and close the tag
		writer.writeCharacters(language_);
		writer.writeEndElement(); // spirit:language
	}

	// if optional spirit:modelName is defined
	if (!modelName_.isEmpty()) {
		writer.writeTextElement("spirit:modelName", modelName_);
	}

	// tell each defaultFileBuilder to write itself
	for (int i = 0; i < defaultFileBuilders_.size(); ++i) {
		defaultFileBuilders_.at(i)->write(writer);
	}

	// write all fileSetRefs
	for (int i = 0; i < fileSetRefs_.size(); ++i) {
		writer.writeStartElement("spirit:fileSetRef");
		writer.writeTextElement("spirit:localName", fileSetRefs_.at(i));
		writer.writeEndElement(); // spirit:fileSetRef
	}

	// write all constraintSetRefs
	for (int i = 0; i < constraintSetRefs_.size(); ++i) {
		writer.writeTextElement("spirit:constraintSetRef",
				constraintSetRefs_.at(i));
	}

	// write all the parameters
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// go through each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

    writer.writeStartElement("spirit:vendorExtensions");
    XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
    writer.writeEndElement(); // spirit:vendorExtensions

	writer.writeEndElement(); // spirit:view
}

bool View::isValid( const QStringList& fileSetNames, 
				   QStringList& errorList, 
				   const QString& parentIdentifier ) const {

	bool valid = true;
	const QString thisIdentifier(QObject::tr("view %1").arg(nameGroup_.name_));

	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for view within %1").arg(parentIdentifier));
		valid = false;
	}

	if (envIdentifiers_.isEmpty()) {
		errorList.append(QObject::tr("No environment identifier specified for "
			"view %1 within %2").arg(nameGroup_.name_).arg(parentIdentifier));
		valid = false;
	}

	// if this is flat view then check the flat elements.
	if (!hierarchyRef_.isValid()) {
		
		foreach (QSharedPointer<FileBuilder> fileBuilder, defaultFileBuilders_) {
			if (!fileBuilder->isValid(errorList, thisIdentifier)) {
				valid = false;
			}
		}

		// make sure the referenced file sets are found
		foreach (QString fileSetRef, fileSetRefs_) {
			if (!fileSetNames.contains(fileSetRef)) {
				errorList.append(QObject::tr("View %1 contained reference to file"
					" set %2 which is not found within %3").arg(
					nameGroup_.name_).arg(fileSetRef).arg(parentIdentifier));
				valid = false;
			}
		}

		foreach (QSharedPointer<Parameter> param, parameters_) {
			if (!param->isValid(errorList, thisIdentifier)) {
				valid = false;
			}
		}
	}
	
	return valid;
}

bool View::isValid( const QStringList& fileSetNames ) const {
	if (nameGroup_.name_.isEmpty()) {
		return false;
	}

	if (envIdentifiers_.isEmpty()) {
		return false;
	}

	// if this is flat view then check the flat elements.
	if (!hierarchyRef_.isValid()) {

		foreach (QSharedPointer<FileBuilder> fileBuilder, defaultFileBuilders_) {
			if (!fileBuilder->isValid()) {
				return false;
			}
		}

		// make sure the referenced file sets are found
		foreach (QString fileSetRef, fileSetRefs_) {
			if (!fileSetNames.contains(fileSetRef)) {
				return false;
			}
		}

		foreach (QSharedPointer<Parameter> param, parameters_) {
			if (!param->isValid()) {
				return false;
			}
		}
	}

	return true;
}

void View::setFileSetRefs(const QList<QString> &fileSetRefs) {
	fileSetRefs_.clear();
	fileSetRefs_ = fileSetRefs;

	hierarchyRef_.clear();
    removeTopLevelViewRefExtension();
}

QString View::getLanguage() const {
	return language_;
}

QString View::getName() const {
	return nameGroup_.name_;
}

VLNV View::getHierarchyRef() const {
	return hierarchyRef_;
}

void View::setParameters(const QList<QSharedPointer<Parameter> > &parameters) {
	// delete the old parameters
	parameters_.clear();

	// save the new parameters
	parameters_ = parameters;

	hierarchyRef_.clear();
    removeTopLevelViewRefExtension();
}

void View::setLanguage(const QString &language) {
	language_ = language;

	hierarchyRef_.clear();
    removeTopLevelViewRefExtension();
}

QString View::getModelName() const {
	return modelName_;
}

void View::setLanguageStrict(bool languageStrict) {
	languageStrict_ = languageStrict;
	
	hierarchyRef_.clear();
    removeTopLevelViewRefExtension();
}

QStringList View::getFileSetRefs() const {
	return fileSetRefs_;
}

void View::setHierarchyRef(const VLNV& hierarchyRef) {

	hierarchyRef_ = hierarchyRef;

	// if view is hierarchical then it can't have these elements.
	language_.clear();
	languageStrict_ = false;
	modelName_.clear();
	fileSetRefs_.clear();
	constraintSetRefs_.clear();
	defaultFileBuilders_.clear();
	parameters_.clear();
}

void View::setEnvIdentifiers( const QStringList& envIdentifiers ) {
	envIdentifiers_.clear();
	envIdentifiers_ = envIdentifiers;

}

bool View::getLanguageStrict() const {
	return languageStrict_;
}

const QStringList& View::getEnvIdentifiers() const {
	return envIdentifiers_;
}

QStringList& View::getEnvIdentifiers() {
	return envIdentifiers_;
}

QList<QSharedPointer<Parameter> >& View::getParameters() {
	return parameters_;
}

const QList<QSharedPointer<Parameter> >& View::getParameters() const {
	return parameters_;
}

void View::setName(const QString &name) {
	nameGroup_.name_ = name;
}

void View::setModelName(const QString &modelName) {
	modelName_ = modelName;

	hierarchyRef_.clear();
    removeTopLevelViewRefExtension();
}

void View::addEnvIdentifier(const QString envIdentifier) {
	envIdentifiers_.append(envIdentifier);

}

void View::addFileSetRef(const QString fileSetRef) {

	// the there already exists a reference to that file set
	if (fileSetRefs_.contains(fileSetRef)) {
		// don't add reference to same file set twice
		return;
	}

	// add a reference to a file set
	fileSetRefs_.append(fileSetRef);

	hierarchyRef_.clear();
    removeTopLevelViewRefExtension();
}

QString View::getDisplayName() const {
	return nameGroup_.displayName_;
}

QString View::getDescription() const {
	return nameGroup_.description_;
}

void View::setDisplayName( const QString& displayName ) {
	nameGroup_.displayName_ = displayName;
}

void View::setDescription( const QString& description ) {
	nameGroup_.description_ = description;
}

bool View::isHierarchical() const {

	return hierarchyRef_.isValid();
}

QList<QSharedPointer<FileBuilder> >& View::getDefaultFileBuilders() {
	return defaultFileBuilders_;
}

const QList<QSharedPointer<FileBuilder> >& View::getDefaultFileBuilders() const {
	return defaultFileBuilders_;
}

void View::setDefaultFileBuilders( QList<QSharedPointer<FileBuilder> >& defaultFileBuilders ) {

	defaultFileBuilders_.clear();
	defaultFileBuilders_ = defaultFileBuilders;

	hierarchyRef_.clear();
    removeTopLevelViewRefExtension();
}

QString View::getTopLevelView() const {
    if (topLevelViewRef_.isNull())
    {
        return QString();
    }
    else
    {
	    return topLevelViewRef_->value();
    }
}

void View::setTopLevelView( const QString& viewName ) {
    if (topLevelViewRef_.isNull())
    {
        createTopLevelViewRefExtension(viewName);
    }
	topLevelViewRef_->setValue(viewName);

	// if view is hierarchical then it can't have these elements.
	language_.clear();
	languageStrict_ = false;
	modelName_.clear();
	fileSetRefs_.clear();
	constraintSetRefs_.clear();
	parameters_.clear();
}

General::NameGroup& View::getNameGroup() {
	return nameGroup_;
}

const General::NameGroup& View::getNameGroup() const {
	return nameGroup_;
}

void View::clearHierarchy() {
	hierarchyRef_.clear();
    removeTopLevelViewRefExtension();
}

//-----------------------------------------------------------------------------
// Function: View::parseVendorExtensions()
//-----------------------------------------------------------------------------
void View::parseVendorExtensions(QDomNode const& extensionsNode)
{
    for (int i = 0; i < extensionsNode.childNodes().count(); ++i) 
    {
        QDomNode extensionNode = extensionsNode.childNodes().at(i);
        if (extensionNode.nodeName() == QString("kactus2:topLevelViewRef"))
        {
            createTopLevelViewRefExtension(extensionNode.childNodes().at(0).nodeValue());
        }
        else
        {
            QSharedPointer<VendorExtension> extension = XmlUtils::createVendorExtensionFromNode(extensionNode); 
            vendorExtensions_.append(extension);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: View::createTopLevelViewRefExtension()
//-----------------------------------------------------------------------------
void View::createTopLevelViewRefExtension(QString topLevelViewRef)
{
    if (topLevelViewRef_.isNull())
    {
        topLevelViewRef_ = QSharedPointer<Kactus2Extension>(new Kactus2Extension("kactus2:topLevelViewRef", topLevelViewRef));
        vendorExtensions_.append(topLevelViewRef_);
    }
}

//-----------------------------------------------------------------------------
// Function: View::removeTopLevelViewExtension()
//-----------------------------------------------------------------------------
void View::removeTopLevelViewRefExtension()
{
    vendorExtensions_.removeAll(topLevelViewRef_);
    topLevelViewRef_.clear();
}

//-----------------------------------------------------------------------------
// Function: View::copyVendorExtensions()
//-----------------------------------------------------------------------------
void View::copyVendorExtensions(View const& other)
{
    foreach(QSharedPointer<VendorExtension> extension, other.vendorExtensions_)
    {
        if (extension->type() == "kactus2:topLevelViewRef")
        {
            topLevelViewRef_ = QSharedPointer<Kactus2Extension>(other.topLevelViewRef_->clone());
            vendorExtensions_.append(topLevelViewRef_);
        }
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(extension->clone()));
        }
    }
}
