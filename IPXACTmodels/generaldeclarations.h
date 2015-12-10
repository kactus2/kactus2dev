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
        SYSTEM,
        MIRROREDMASTER,
        MIRROREDSLAVE,
        MIRROREDSYSTEM,
        MONITOR,
        INTERFACE_MODE_COUNT
    };

    //! The names to display that match the enum InterfaceMode
    const QString INTERFACE_MODE_NAMES[] = {
        "master",
        "slave",
        "system",
        "mirroredMaster",
        "mirroredSlave",
        "mirroredSystem",
        "monitor",
        "undefined"
    };

    /*! Convert QString into boolean value
    *
    *      @param [in] str QString containing the string to be converted
    *      @param [in] defaultValue InterfaceMode value that is returned if str is none of the defined values
    *
    *      @return InterfaceMode matching the str or default value
    */
    IPXACTMODELS_EXPORT InterfaceMode str2Interfacemode(const QString& str, InterfaceMode defaultValue);

    /*! Convert InterfaceMode to QString
    *
    *      @param [in] mode InterfaceMode value that is converted to QString
    *
    *      @return QString that matches the enum value.
    */
    IPXACTMODELS_EXPORT QString interfaceMode2Str(const General::InterfaceMode mode);

    /*!
    *  Returns an interface mode that is compatible with the given interface mode.
    */
    InterfaceMode getCompatibleInterfaceMode(General::InterfaceMode mode);

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
    *      @param [in] str QString containing the string to be converted
    *      @param [in] defaultValue The value that will be returned if no match is found
    *      @return General::DriverType that matches the str or default value.
    */
    IPXACTMODELS_EXPORT General::DriverType str2DriverType(QString str, General::DriverType defaultValue);

    /*! Convert the value of the DriverType into QString.
    *
    *      @param [in] type The DriverType to be converted.
    *
    *      @return QString matching the driver type.
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
    *      @param [in] str QString containing the string to be converted
    *      @param [in] defaultValue The value that will be returned if no match is found
    *      @return General::Usage that matches the str or default value.
    */
    IPXACTMODELS_EXPORT General::Usage str2Usage(QString str, General::Usage defaultValue);

    /*! Convert the Usage value to QString
    *
    *      @param [in] usage Usage value to be converted to QString.
    *
    *      @return QString that matches the enum value.
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
        "oneToClear",
        "oneToSet",
        "oneToToggle",
        "zeroToClear",
        "zeroToSet",
        "zeroToToggle",
        "clear",
        "set",
        "modify",
        ""
    };

    /*! Convert the value of the QString into enum ModifiedWrite.
    *
    * If the string does not match any of the enum values then General::MODIFIED_WRITE_COUNT
    * is returned.
    * 
    *      @param [in] str contains the string to be converted.
    *
    *      @return The enum value that matches the string.
    */
    IPXACTMODELS_EXPORT General::ModifiedWrite str2ModifiedWrite(const QString& str);

    /*! Convert the enum value to a matching string.
    *
    *      @param [in] modWrite The enum value to be converted to a string.
    *
    *      @return QString that matches the enum value.
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
        "clear",
        "set",
        "modify",
        ""
    };

    /*! Convert the enum value to a matching string.
    *
    *      @param [in] readAction The enum value to be converted to a string.
    *
    *      @return QString that matches the enum value.
    */
    IPXACTMODELS_EXPORT QString readAction2Str(const General::ReadAction readAction);

    /*! Convert the string into matching enum ReadAction value.
    * 
    * If the string does not match any of the defined enum values then
    * General::READ_ACTION_COUNT is returned.
    * 
    *      @param [in] str Contains the string to be converted.
    *
    *      @return The enum value that matches the string.
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
        "unConstrained",
        "restore",
        "writeAsRead",
        "readOnly",
        ""
    };

    /*! Convert the enum value to a matching string.
    *
    *      @param [in] testConstraint The enum value to be converted to a string.
    *
    *      @return QString that matches the enum value.
    */
    IPXACTMODELS_EXPORT QString testConstraint2Str(const General::TestConstraint testConstraint);

    /*! Convert the string into matching enum TestConstraint value.
    * 
    * If the string does not match any of the defined enum values then
    * General::TESTCONSTRAINT_COUNT is returned.
    *
    *      @param [in] str Contains the string to be converted.
    *
    *      @return The enum value that matches the string.
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
    struct PortBounds {

        //! The name of the port.
        QString portName_;

        //! The left bound of the port.
        int left_;

        //! The right bound of the port.
        int right_;

        //! Default constructor
        IPXACTMODELS_EXPORT PortBounds();

        /*! The constructor
        * 
        * Constructs a port with given name and both bounds set to 0.
        * 
        *      @param [in] portName The name of the port.
        *
        */
        IPXACTMODELS_EXPORT PortBounds(const QString& portName);

        /*! The constructor
        *
        *      @param [in] portName The name of the port.
        *      @param [in] left The left bound of the port.
        *      @param [in] right The right bound of the port.
        *
        */
        IPXACTMODELS_EXPORT PortBounds(const QString& portName, const int left, const int right);

        //! Copy constructor
        IPXACTMODELS_EXPORT PortBounds(const PortBounds& other);

        //! Assignment operator
        IPXACTMODELS_EXPORT PortBounds& operator=(const PortBounds& other);

        //! Operator <
        IPXACTMODELS_EXPORT bool operator<(const PortBounds& other) const;

        //! Operator ==
        IPXACTMODELS_EXPORT bool operator==(const PortBounds& other) const;

        //! Operator !=
        IPXACTMODELS_EXPORT bool operator!=(const PortBounds& other) const;
    };

    /*! Convert the info into string.
    *
    *      @param [in] portName The name of the port.
    *      @param [in] leftBound The left bound of the port.
    *      @param [in] rightBound The right bound of the port.
    *
    *      @return QString String that matches the info ( <portName>[<left>..<right>] )
    */
    IPXACTMODELS_EXPORT QString port2String(const QString& portName, int leftBound, int rightBound);

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
        PortAlignment(const PortAlignment& other);

        //! Assignment operator
        PortAlignment& operator=(const PortAlignment& other);
    };

    /*! Calculate the alignment for the ports from the port maps.
    *
    *      @param [in] portMap1 Pointer to the port map of port 1.
    *      @param [in] phys1LeftBound The left bound for the physical port 1.
    *      @param [in] phys1RightBound The right bound for the physical port1.
    *      @param [in] portMap2 Pointer to the port map of port 2.
    *      @param [in] phys2LeftBound The left bound for the physical port 2.
    *      @param [in] phys2RightBound The right bound for the physical port 2.
    *
    *      @return General::PortAlignment Contains the alignments for the ports.
    */
    /*IPXACTMODELS_EXPORT PortAlignment calculatePortAlignment(const PortMap* portMap1, 
    int phys1LeftBound,
    int phys1RightBound,
    const PortMap* portMap2,
    int phys2LeftBound,
    int phys2RightBound);*/



    /*! Convert a boolean value into QString
    *
    *      @param [in] value The variable to convert into QString
    *      @return if value = true, returns "true"
    * 			if value = false, returns "false"
    */
    IPXACTMODELS_EXPORT QString bool2Str(bool value);

    /*! Convert QString into boolean value
    *
    *      @param [in] str QString containing the string to be converted
    *      @param [in] defaultValue boolean value that is returned if str is neither
    * "true" or "false"
    *
    *      @return if str = "true" returns true
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
        *      @param [in] filePath the path of the file
        *      @param [in] libraryName Name of the library
        */
        LibraryFilePair(const QString filePath, const QString libraryName);

        /*! The == operator
        *
        *      @param [in] other The other LibaryFilePair to check
        *
        *      @return bool True if they have the same file path and library name.
        */
        bool operator==(const LibraryFilePair& other) const;

        /*! The copy constructor
        *
        *      @param [in] other Reference to the LibraryFilePair to copy
        *
        */
        LibraryFilePair(const LibraryFilePair& other);

        /*! The assignment operator
        *
        *      @param [in] other Reference to the LibraryFilePair to assign
        *
        *      @return Reference to this LibraryFilePair.
        */
        LibraryFilePair& operator=(const LibraryFilePair& other);
    };

    /*! Get a relative file path from one location to another.
    *
    * This function can be used to create a QString that contains the relative
    * file path from a directory to a single file. If either of the parameters
    * doesn't exist in the file system a null string is returned.
    *
    *      @param [in] from Path to the directory that is the base where the path is
    * generated to.
    *      @param [in] to The path to a file which is the target of the generated relative
    * path.
    *
    *      @return QString containing the relative path.
    */
    IPXACTMODELS_EXPORT QString getRelativePath(const QString from, const QString to);

    /*! Get a relative file path from one location to another.
    *
    * This function can be used to create a QString that contains the relative
    * file path from a directory to a single file. This function can be used to get
    * relative path to a file that does not exist. The source file must exist.
    *
    *      @param [in] from Path to the directory that is the base where the path is
    * generated to.
    *      @param [in] to The path to a file which is the target of the generated relative
    * path.
    *
    *      @return QString containing the relative path.
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
    *      @param [in] originalPath The absolute path that is used as base for relative path
    *      @param [in] relativePath The relative path that is used to find the file or
    * directory.
    *
    *      @return The absolute path of the target directory/file
    */
    IPXACTMODELS_EXPORT QString getAbsolutePath(const QString& originalPath, const QString& relativePath);

    /*! Convert a string to unsigned int format.
    * 
    * The multiples in the string are converted as following:
    * k/K = 2^10
    * M   = 2^20
    * G   = 2^30
    * T   = 2^40
    * P   = 2^50
    * 
    *      @param [in] str The string to convert.
    *
    *      @return The result of the conversion.
    */
    quint64 str2Uint(const QString& str);
}

#endif /* GENERALDECLARATIONS_H_ */
