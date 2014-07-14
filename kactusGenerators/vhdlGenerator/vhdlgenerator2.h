/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgenerator2.h
 *		Project: Kactus 2
 */

#ifndef VHDLGENERATOR2_H
#define VHDLGENERATOR2_H

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>

#include <library/LibraryManager/vlnv.h>

#include <kactusGenerators/HDLGenerator/HDLGeneratorTemplate.h>
#include <kactusGenerators/HDLGenerator/vhdlportsorter.h>

#include "vhdlgeneric.h"
#include "vhdlport.h"

#include "vhdlsignal.h"
#include "vhdlcomponentdeclaration.h"
#include "vhdlcomponentinstance.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

class LibraryInterface;
class VhdlConnectionEndPoint;

/*! \brief Vhdl Generator generates top-level vhdl for hierarchical component.
 *
 * This class and it's interface provide services to generate vhdl code from
 * a hierarchical view of a component. 
 */
class VhdlGenerator2 : public HDLGeneratorTemplate
{

public:

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of this generator.
	 *
	*/
	VhdlGenerator2(LibraryInterface* handler, QObject* parent);
	
	//! \brief The destructor
	virtual ~VhdlGenerator2();

	/*! \brief Add a new view to the top component's IP-Xact metadata.
	 * 
	 * NOTE: Before calling this function the parse() function must be called and
	 * usually the generateVhdl() also. If generateVhdl() is not called then the
	 * architecture name must be set other way.
	 * 
	 * \param vhdlFileName Absolute file path to the vhdl file to add to the metadata.
	 * This is usually the same file that was set as output file when calling
	 * generateVhdl().
	 *
	 * \return bool True if the view was added successfully.
	*/
	bool addRTLView(QString const& vhdlFileName);

private:
	//! \brief No copying
	VhdlGenerator2(const VhdlGenerator2& other);

	//! \brief No assignment
	VhdlGenerator2& operator=(const VhdlGenerator2& other);

    virtual void writeLibraryIncludes(QTextStream& fileStream);

    virtual HDLModelParameter* createModelParameterObject(QSharedPointer<ModelParameter> &modelParam);

    virtual HDLPort* createPortObject(QSharedPointer<Port> &port);

    virtual VhdlSignal* createSignalObject(QString const& connectionName, QString type, int left, int right, QString const& description);

    virtual QString vectorSignalType();

    virtual QString scalarSignalType();

    virtual bool isScalarType(QString const& type) const;
	
	/*! \brief Write the header of a vhdl file to be created.
	 *
	 * \param outputStream The text stream to write into.
	 * \param fileName The name of the file that is being written.
	 *
	*/
	virtual void writeFileHeader( QTextStream& outputStream, QString const& fileName );
	
	/*! \brief Write the generics of the top entity.
	 *
	 * \param outputStream The text stream to write into.
	 *
	*/
	void writeModelParameters( QTextStream& outputStream );

    virtual void writeModelParameterEndTag(QTextStream& outputStream);

    virtual void writeModelParametersStartTag(QTextStream& outputStream);

    void writeStructuralEndTag(QTextStream& outputStream);

    void writeComponentInstantiationStartTag(QTextStream& outputStream);

    void writeStructuralStartTag(QTextStream& outputStream);

    void writeEntityEndTag(QTextStream& outputStream);

    void writeEntityStartTag(QTextStream& outputStream);

    QStringList getLibrariesForComponent(QSharedPointer<VhdlComponentInstance> compInstance, QString instanceActiveView);

    virtual QStringList getLibrariesForComponent(QSharedPointer<HDLComponentInstance> compInstance, QString instanceActiveView);

    virtual HDLComponentInstance* createComponentInstanceObject(QSharedPointer<Component> component, ComponentInstance const &instance, QString instanceActiveView);

    virtual HDLComponentDeclaration* createComponentDeclarationObject(QSharedPointer<Component> component);

    virtual bool checkPortTypeMatch(QString type, QString connectionType);

    virtual void writePortsEndTag(QTextStream& outputStream);

    virtual void writePortsStartTag(QTextStream& outputStream);

    virtual void writeDescription(QString const& description, QTextStream& stream, QString const& indentation = QString(""));

    virtual QString commentTag() const;

};

#endif // VHDLGENERATOR2_H
