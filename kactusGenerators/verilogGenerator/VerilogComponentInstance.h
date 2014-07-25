//-----------------------------------------------------------------------------
// File: VerilogComponentInstance.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.7.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef VERILOGCOMPONENTINSTANCE_H
#define VERILOGCOMPONENTINSTANCE_H

#include <kactusGenerators/HDLGenerator/HDLComponentInstance.h>

//-----------------------------------------------------------------------------
// class VerilogComponentInstance.
//-----------------------------------------------------------------------------
class VerilogComponentInstance : public HDLComponentInstance
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent          The parent object.
	 *      @param [in] component       The component the instance represents.
	 *      @param [in] instanceName    The name of the instance.
	 *      @param [in] viewName        The name of the active view of the instance.
	 *      @param [in] description     The description of the instance.	 
	 */
    VerilogComponentInstance(QObject* parent, QSharedPointer<Component> component,
        const QString& instanceName, QString const& viewName,
        const QString& description = QString());

	/*!
	 *  The destructor.
	*/
	virtual ~VerilogComponentInstance();

    virtual void write(QTextStream& stream) const;





    virtual void mapToPort(VhdlConnectionEndPoint const& endpoint, const QString& signalName);

    virtual void mapToSignal(const QString& portName, int portLeft, int portRight, const QString& portType,
        const QString& signalName, int signalLeft, int signalRight, const QString& signalType);

    virtual bool hasConnection(const QString& portName);

    virtual void useDefaultsForOtherPorts();

    virtual QString portType(const QString& portName) const;

    virtual bool isScalarPort(const QString& portName) const;


private:
	// Disable copying.
	VerilogComponentInstance(VerilogComponentInstance const& rhs);

	// Disable assignment.
	VerilogComponentInstance& operator=(VerilogComponentInstance const& rhs);

    void writeParameterAssignments(QTextStream& stream) const;

    void writePortAssignments(QTextStream& stream) const;

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    struct Portmap
    {
        QString name_;
        int left_;
        int right_;
        Portmap(QString name, int left, int right) : name_(name), left_(left), right_(right){}
        bool operator<(Portmap const& other) const {return name_ < other.name_;}
    };

    QMap<Portmap, Portmap> portMaps_;
};
#endif // VERILOGCOMPONENTINSTANCE_H
