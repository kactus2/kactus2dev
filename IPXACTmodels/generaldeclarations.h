/*
*
*  Created on: 27.7.2010
*      Author: Antti Kamppi
*/

#ifndef GENERALDECLARATIONS_H_
#define GENERALDECLARATIONS_H_

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QMap>
#include <QSettings>
#include <QStringList>

/*! Namespace General is used to store the commonly used structs and enums
*
* This module holds the information structures that are used by many modules
* in IP-Xact parsing that are not otherwise connected to each other.
*/
namespace General
{
    //! Describes the mode for an interface.
    enum InterfaceMode {
        MASTER = 0,
        SLAVE,
        INITIATOR,
        TARGET,
        SYSTEM,
        MIRRORED_MASTER,
        MIRRORED_SLAVE,
        MIRRORED_INITIATOR,
        MIRRORED_TARGET,
        MIRRORED_SYSTEM,
        MONITOR,
        INTERFACE_MODE_COUNT
    };

    //! The names to display that match the enum InterfaceMode
    const QMap<InterfaceMode, QString> INTERFACE_MODES
    {
        {MASTER,          QStringLiteral("master")},
        {SLAVE,           QStringLiteral("slave")},
        {SYSTEM,          QStringLiteral("system")},
        {MIRRORED_MASTER, QStringLiteral("mirroredMaster")},
        {MIRRORED_SLAVE,  QStringLiteral("mirroredSlave")},
        {MIRRORED_SYSTEM, QStringLiteral("mirroredSystem")},
        {MONITOR,         QStringLiteral("monitor")}
    };

    const QMap<InterfaceMode, QString> INTERFACE_MODES_2022
    {
        {INITIATOR,          QStringLiteral("initiator")},
        {TARGET,             QStringLiteral("target")},
        {SYSTEM,             QStringLiteral("system")},
        {MIRRORED_INITIATOR, QStringLiteral("mirroredInitiator")},
        {MIRRORED_TARGET,    QStringLiteral("mirroredTarget")},
        {MIRRORED_SYSTEM,    QStringLiteral("mirroredSystem")},
        {MONITOR,            QStringLiteral("monitor")}
    };

    /*!
     *  Check if the selected bus interface mode is compatible with IP-XACT revision 1685-2022.
     *	
     *    @param [in] mode    The selected bus interface mode.
     *
     *    @return True, if the mode is compatible with revision 1685-2022, false otherwise.
     */
    IPXACTMODELS_EXPORT bool modeIsRevision2022(General::InterfaceMode mode);

    /*! Convert QString into boolean value
    *
    *    @param [in] str QString containing the string to be converted
    *    @param [in] defaultValue InterfaceMode value that is returned if str is none of the defined values
    *
    *    @return InterfaceMode matching the str or default value
    */
    IPXACTMODELS_EXPORT InterfaceMode str2Interfacemode(const QString& str, InterfaceMode defaultValue);

    /*! Convert InterfaceMode to QString
    *
    *    @param [in] mode InterfaceMode value that is converted to QString
    *
    *    @return QString that matches the enum value.
    */
    IPXACTMODELS_EXPORT QString interfaceMode2Str(const General::InterfaceMode mode);

    /*!
    *  Returns an interface mode that is compatible with the given interface mode.
    */
    IPXACTMODELS_EXPORT InterfaceMode getCompatibleInterfaceMode(General::InterfaceMode mode);

    /*!
     *  Get the compatible active bus interface modes for the selected bus interface mode.
     *
     *    @param [in] mode    The selected bus interface mode.
     *
     *    @return List of compatible bus interface modes.
     */
    IPXACTMODELS_EXPORT QVector<InterfaceMode> getCompatibleInterfaceModesForActiveInterface(
        General::InterfaceMode mode);

    /*!
     *  Get the compatible hierarchical bus interface modes for the selected bus interface mode.
     *
     *    @param [in] mode    The selected bus interface mode.
     *
     *    @return The connectible bus interface modes.
     */
    IPXACTMODELS_EXPORT QVector<InterfaceMode> getCompatibleInterfaceModeForHierarchicalInterface(
        General::InterfaceMode mode);

    /*!
    * Specifies the required driver type
    */
    enum DriverType {
        ANY, 		// any logic signal or value
        CLOCK, 		// repeating type waveform
        SINGLESHOT,	// non-repeating type waveform
        NO_DRIVER
    };

    /*! Convert the value of the QString into enum DriverType
    *
    * if The str does not match any of the enum values the default value
    * specifies as parameter will be returned
    *
    *    @param [in] str QString containing the string to be converted
    *    @param [in] defaultValue The value that will be returned if no match is found
    *    @return General::DriverType that matches the str or default value.
    */
    IPXACTMODELS_EXPORT General::DriverType str2DriverType(QString str, General::DriverType defaultValue);

    /*! Convert the value of the DriverType into QString.
    *
    *    @param [in] type The DriverType to be converted.
    *
    *    @return QString matching the driver type.
    */
    IPXACTMODELS_EXPORT QString driverType2Str(General::DriverType type);

    /*! Equals to ipxact:usage element within ipxact:memoryBlockData
    *
    * Usage specifies the type of usage for the address block to which it
    * belongs
    */
    enum Usage {
        MEMORY = 0,
        REGISTER,
        RESERVED,
        USAGE_COUNT
    };

    /*! Convert the value of the QString into enum Usage
    *
    * if The str does not match any of the enum values the default value
    * specifies as parameter will be returned
    *
    *    @param [in] str QString containing the string to be converted
    *    @param [in] defaultValue The value that will be returned if no match is found
    *    @return General::Usage that matches the str or default value.
    */
    IPXACTMODELS_EXPORT General::Usage str2Usage(QString str, General::Usage defaultValue);

    /*! Convert the Usage value to QString
    *
    *    @param [in] usage Usage value to be converted to QString.
    *
    *    @return QString that matches the enum value.
    */
    IPXACTMODELS_EXPORT QString usage2Str(const General::Usage usage);

    /*! Equals to ipxact:modifiedWriteValue within ipxact:fieldData.
    *
    * ModifiedWrite describes the manipulation of data written to a field.
    */
    enum ModifiedWrite {
        ONE_TO_CLEAR = 0,
        ONE_TO_SET,
        ONE_TO_TOGGLE,
        ZERO_TO_CLEAR,
        ZERO_TO_SET,
        ZERO_TO_TOGGLE,
        CLEAR,
        SET,
        MODIFY,
        MODIFIED_WRITE_COUNT
    };

    //! Contains the strings that match the enum ModifiedWrite values.
    const QString MODIFIED_WRITE_STRINGS[] = {
        QLatin1String("oneToClear"),
        QLatin1String("oneToSet"),
        QLatin1String("oneToToggle"),
        QLatin1String("zeroToClear"),
        QLatin1String("zeroToSet"),
        QLatin1String("zeroToToggle"),
        QLatin1String("clear"),
        QLatin1String("set"),
        QLatin1String("modify"),
        QLatin1String("")
    };

    /*! Convert the value of the QString into enum ModifiedWrite.
    *
    * If the string does not match any of the enum values then General::MODIFIED_WRITE_COUNT
    * is returned.
    *
    *    @param [in] str contains the string to be converted.
    *
    *    @return The enum value that matches the string.
    */
    IPXACTMODELS_EXPORT General::ModifiedWrite str2ModifiedWrite(const QString& str);

    /*! Convert the enum value to a matching string.
    *
    *    @param [in] modWrite The enum value to be converted to a string.
    *
    *    @return QString that matches the enum value.
    */
    IPXACTMODELS_EXPORT QString modifiedWrite2Str(const General::ModifiedWrite modWrite);

    /*! Equals the ipxact:readAction within ipxact:fieldData.
    *
    * ReadAction describes an action that happens to a field after a read operation.
    */
    enum ReadAction {
        READ_CLEAR = 0,
        READ_SET,
        READ_MODIFY,
        READ_ACTION_COUNT
    };

    //! Contains the strings that match the enum ReadAction values.
    const QString READ_ACTION_STRINGS[] = {
        QLatin1String("clear"),
        QLatin1String("set"),
        QLatin1String("modify"),
        QLatin1String("")
    };

    /*! Convert the enum value to a matching string.
    *
    *    @param [in] readAction The enum value to be converted to a string.
    *
    *    @return QString that matches the enum value.
    */
    IPXACTMODELS_EXPORT QString readAction2Str(const General::ReadAction readAction);

    /*! Convert the string into matching enum ReadAction value.
    *
    * If the string does not match any of the defined enum values then
    * General::READ_ACTION_COUNT is returned.
    *
    *    @param [in] str Contains the string to be converted.
    *
    *    @return The enum value that matches the string.
    */
    IPXACTMODELS_EXPORT General::ReadAction str2ReadAction(const QString& str);

    /*! Equals the ipxact:testConstraint attribute within ipxact:fieldData.
    *
    * TestConstraint defines the constraint for the field during automated testing.
    */
    enum TestConstraint {
        TEST_UNCONSTRAINED = 0,
        TEST_RESTORE,
        TEST_WRITE_AS_READ,
        TEST_READ_ONLY,
        TESTCONSTRAINT_COUNT
    };

    //! Contains the strings that match the enum TestConstraint values.
    const QString TEST_CONSTRAINT_STRINGS[TESTCONSTRAINT_COUNT+1] = {
        QLatin1String("unconstrained"),
        QLatin1String("restore"),
        QLatin1String("writeAsRead"),
        QLatin1String("readOnly"),
        QLatin1String("")
    };

    /*! Convert the enum value to a matching string.
    *
    *    @param [in] testConstraint The enum value to be converted to a string.
    *
    *    @return QString that matches the enum value.
    */
    IPXACTMODELS_EXPORT QString testConstraint2Str(const General::TestConstraint testConstraint);

    /*! Convert the string into matching enum TestConstraint value.
    *
    * If the string does not match any of the defined enum values then
    * General::TESTCONSTRAINT_COUNT is returned.
    *
    *    @param [in] str Contains the string to be converted.
    *
    *    @return The enum value that matches the string.
    */
    IPXACTMODELS_EXPORT General::TestConstraint str2TestConstraint(const QString& str);

    /*! Used in ipxact:clockDriver element in IP-Xact specification.
    *
    * Used to describe elements:
    * ipxact:clockPeriod
    * ipxact:clockPulseOffset
    * ipxact:clockPulseDuration
    */

    //! Specifies a port name and it's bounds.
    struct IPXACTMODELS_EXPORT PortBounds {

        //! The name of the port.
        QString portName_;

        //! The left bound of the port.
        int left_;

        //! The right bound of the port.
        int right_;

        /*! The constructor
        *
        *    @param [in] portName The name of the port.
        *    @param [in] left     The left bound of the port.
        *    @param [in] right    The right bound of the port.
        *
        */
        PortBounds(const QString& portName, const int left = 0, const int right = 0);

        //! Copy constructor
        PortBounds(const PortBounds& other) = default;

        //! Assignment operator
        PortBounds& operator=(const PortBounds& other) = default;

        //! Operator <
        bool operator<(const PortBounds& other) const;

        //! Operator ==
        bool operator==(const PortBounds& other) const;

        //! Operator !=
        bool operator!=(const PortBounds& other) const;
    };

    //! Port alignment is used to contain the physical bounds of two ports.
    struct IPXACTMODELS_EXPORT PortAlignment {

        //! The calculated left bound for port 1.
        QString port1Left_;

        //! The calculated right bound for port 1.
        QString  port1Right_;

        //! The calculated left bound for port 2.
        QString  port2Left_;

        //! The calculated right bound for port 2.
        QString  port2Right_;

        //! Tells if the calculated alignment is valid or not.
        bool invalidAlignment_;

        /*! The default constructor
        *
        * Constructs the struct with all bounds valued at -1 and invalid as true.
        */
        PortAlignment();

        //! Copy constructor
        PortAlignment(const PortAlignment& other) = default;

        //! Assignment operator
        PortAlignment& operator=(const PortAlignment& other) = default;
    };

    /*! Convert a boolean value into QString
    *
    *    @param [in] value The variable to convert into QString
    *    @return if value = true, returns "true"
    * 			if value = false, returns "false"
    */
    IPXACTMODELS_EXPORT QString bool2Str(bool value);

    /*! Convert QString into boolean value
    *
    *    @param [in] str QString containing the string to be converted
    *    @param [in] defaultValue boolean value that is returned if str is neither
    * "true" or "false"
    *
    *    @return if str = "true" returns true
    * 			if str = "false" returns false
    * 			if str is something else returns defaultValue
    */
    bool str2Bool(const QString str, bool defaultValue);

    /*! Contains the file path for a file and the library for the file.
    *
    * This is used to get a file name from fileSet and the logical name for it.
    */
    struct LibraryFilePair {

        //! Contains the file path and file name.
        QString filePath_;

        //! Contains the library name for the file.
        QString libraryName_;

        /*! The constructor for the struct
        *
        *    @param [in] filePath the path of the file
        *    @param [in] libraryName Name of the library
        */
        LibraryFilePair(const QString filePath, const QString libraryName);

        //! The destructor.
        ~LibraryFilePair() = default;
        
        /*! The == operator
        *
        *    @param [in] other The other LibaryFilePair to check
        *
        *    @return bool True if they have the same file path and library name.
        */
        bool operator==(const LibraryFilePair& other) const;

        /*! The copy constructor
        *
        *    @param [in] other Reference to the LibraryFilePair to copy
        *
        */
        LibraryFilePair(const LibraryFilePair& other) = default;

        /*! The assignment operator
        *
        *    @param [in] other Reference to the LibraryFilePair to assign
        *
        *    @return Reference to this LibraryFilePair.
        */
        LibraryFilePair& operator=(const LibraryFilePair& other) = default;
    };

    /*! Get a relative file path from one location to another.
    *
    * This function can be used to create a QString that contains the relative
    * file path from a directory to a single file. If either of the parameters
    * doesn't exist in the file system a null string is returned.
    *
    *    @param [in] from Path to the directory that is the base where the path is
    * generated to.
    *    @param [in] to The path to a file which is the target of the generated relative
    * path.
    *
    *    @return QString containing the relative path.
    */
    IPXACTMODELS_EXPORT QString getRelativePath(QString from, QString to);

    /*! Get a relative file path from one location to another.
    *
    * This function can be used to create a QString that contains the relative
    * file path from a directory to a single file. This function can be used to get
    * relative path to a file that does not exist. The source file must exist.
    *
    *    @param [in] from Path to the directory that is the base where the path is
    * generated to.
    *    @param [in] to The path to a file which is the target of the generated relative
    * path.
    *
    *    @return QString containing the relative path.
    */
    IPXACTMODELS_EXPORT QString getRelativeSavePath(const QString& from, const QString& to);

    /*! Get an absolute path of a file.
    *
    * This function takes original path and uses it as base. Then the relative
    * path is used from the original path to find the correct location of a file.
    * The absolute path to the file is returned. If the relative path contains a
    * file but it is not found null string is returned. Original path may contain
    * a file path or only a directory path but only directory path is used.
    *
    *    @param [in] originalPath The absolute path that is used as base for relative path
    *    @param [in] relativePath The relative path that is used to find the file or
    * directory.
    *
    *    @return The absolute path of the target directory/file
    */
    IPXACTMODELS_EXPORT QString getAbsolutePath(const QString& originalPath, const QString& relativePath);

}

#endif /* GENERALDECLARATIONS_H_ */
