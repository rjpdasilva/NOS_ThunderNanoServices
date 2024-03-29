cmake_minimum_required(VERSION 3.3)
find_package(WPEFramework)

# All packages that did not deliver a CMake Find script (and some deprecated scripts that need to be removed)
# are located in the cmake directory. Include it in the search.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

option(PLUGIN_BLUETOOTH "Include Bluetooth plugin" OFF)
option(PLUGIN_BLUETOOTHREMOTECONTROL "Include Bluetooth Remote Control plugin" OFF)
option(PLUGIN_COMMANDER "Include Commander plugin" OFF)
option(PLUGIN_COMPOSITOR "Include Compsitor plugin" OFF)
option(PLUGIN_DEVICEIDENTIFICATION "Include DeviceIdentification plugin" OFF)
option(PLUGIN_DEVICEINFO "Include DeviceInfo plugin" OFF)
option(PLUGIN_DSRESOLUTION "Include DSResolution plugin" OFF)
option(PLUGIN_DHCPSERVER "Include DHCPServer plugin" OFF)
option(PLUGIN_DIALSERVER " Include DIALServer plugin" OFF)
option(PLUGIN_DICTIONARY "Include Dictionary plugin" OFF)
option(PLUGIN_DSGCCCLIENT "Include DSGCClient plugin" OFF)
option(PLUGIN_IOCONNECTOR "Include IOConnector plugin" OFF)
option(PLUGIN_FIRMWARECONTROL "Include FirmwareControl Plugin" OFF)
option(PLUGIN_FRONTPANEL "Include FrontPanel plugin" OFF)
option(PLUGIN_LOCATIONSYNC "Include LocationSync plugin" OFF)
option(PLUGIN_MESSENGER "Include Messenger plugin" OFF)
option(PLUGIN_MONITOR "Include Monitor plugin" OFF)
option(PLUGIN_NETWORKCONTROL "Include NetworkControlplugin" OFF)
option(PLUGIN_OPENCDMI "Include OpenCDMi plugin" OFF)
option(PLUGIN_PACKAGER "Include Packager plugin" OFF)
option(PLUGIN_POWER "Include Power plugin" OFF)
option(PLUGIN_REMOTECONTROL "Include RemoteControl plugin" OFF)
option(PLUGIN_SECURESHELLSERVER "Include SecureShellServer plugin" OFF)
option(PLUGIN_SECURITYAGENT "Include SecurityAgent plugin" OFF)
option(PLUGIN_STREAMER "Include Streamer plugin" OFF)
option(PLUGIN_SNAPSHOT "Include Snapshot plugin" OFF)
option(PLUGIN_SPARK "Include SparkEngine plugin" OFF)
option(PLUGIN_SYSTEMCOMMANDS "Include SystemCommands plugin" OFF)
option(PLUGIN_TIMESYNC "Include TimeSync plugin" OFF)
option(PLUGIN_TRACECONTROL "Include TraceControl plugin" OFF)
option(PLUGIN_WEBKITBROWSER "Include WebKitBrowser plugin" OFF)
option(PLUGIN_WEBPA "Include WebPA plugin" OFF)
option(PLUGIN_WEBPROXY "Include WebProxy plugin" OFF)
option(PLUGIN_WEBSERVER "Include WebServer plugin" OFF)
option(PLUGIN_WEBSHELL "Include WebShell plugin" OFF)
option(PLUGIN_WIFICONTROL "Include WifiControl plugin" OFF)
option(PLUGIN_FILETRANSFER "Include FileTransfer plugin" OFF)
option(EXAMPLES "Include Examples" OFF)

option(WPEFRAMEWORK_CREATE_IPKG_TARGETS "Generate the CPack configuration for package generation" OFF)

# Library installation section
string(TOLOWER ${NAMESPACE} STORAGE_DIRECTORY)

#include(CompileSettings)

# for writing pc and config files
include(CmakeHelperFunctions)

if(BUILD_REFERENCE)
    add_definitions(-DBUILD_REFERENCE=${BUILD_REFERENCE})
endif()

if(PLUGIN_BLUETOOTH)
    add_subdirectory(BluetoothControl)
endif()

if(PLUGIN_BLUETOOTHREMOTECONTROL)
    add_subdirectory(BluetoothRemoteControl)
endif()

if(PLUGIN_COMMANDER)
    add_subdirectory(Commander)
endif()

if(PLUGIN_COMPOSITOR)
    add_subdirectory(Compositor)
endif()

if(PLUGIN_DEVICEIDENTIFICATION)
    add_subdirectory(DeviceIdentification)
endif()

if(PLUGIN_DEVICEINFO)
    add_subdirectory(DeviceInfo)
endif()

if(PLUGIN_DSRESOLUTION)
    add_subdirectory(DSResolution)
endif()

if(PLUGIN_DHCPSERVER)
    add_subdirectory(DHCPServer)
endif()

if(PLUGIN_DIALSERVER)
    add_subdirectory(DIALServer)
endif()

if(PLUGIN_DICTIONARY)
    add_subdirectory(Dictionary)
endif()

if(PLUGIN_DSGCCCLIENT)
    add_subdirectory(DsgccClient)
endif()

if(PLUGIN_DSRESOLUTION)
    add_subdirectory(DSResolution)
endif()

if(PLUGIN_FIRMWARECONTROL)
    add_subdirectory(FirmwareControl)
endif()

if(PLUGIN_FRONTPANEL)
    add_subdirectory(FrontPanel)
endif()

if(PLUGIN_IOCONNECTOR)
    add_subdirectory(IOConnector)
endif()

if(PLUGIN_SICONTROL)
    add_subdirectory (SIControl)
endif(PLUGIN_SICONTROL)

if(PLUGIN_LOCATIONSYNC)
    add_subdirectory(LocationSync)
endif()

if(PLUGIN_MESSENGER)
    add_subdirectory(Messenger)
endif()

if(PLUGIN_MONITOR)
    add_subdirectory(Monitor)
endif()

if(PLUGIN_NETWORKCONTROL)
    add_subdirectory(NetworkControl)
endif()

if(PLUGIN_OPENCDMI)
    add_subdirectory(OpenCDMi)
endif()

if(PLUGIN_POWER)
    add_subdirectory(Power)
endif()

if(PLUGIN_PACKAGER)
    add_subdirectory(Packager)
endif()

if(PLUGIN_REMOTECONTROL)
    add_subdirectory(RemoteControl)
endif()

if(PLUGIN_RTSPCLIENT)
    add_subdirectory(RtspClient)
endif()

if(PLUGIN_SECURESHELLSERVER)
    add_subdirectory(SecureShellServer)
endif()

if (PLUGIN_SECURITYAGENT)
    add_subdirectory(SecurityAgent)
endif()

if(PLUGIN_SNAPSHOT)
    add_subdirectory(Snapshot)
endif()

if(PLUGIN_SPARK)
    add_subdirectory(Spark)
endif()

if(PLUGIN_STREAMER)
    add_subdirectory(Streamer)
endif()

if(PLUGIN_SYSTEMCOMMANDS)
    add_subdirectory(SystemCommands)
endif()

if(PLUGIN_SYSTEMDCONNECTOR)
    add_subdirectory(SystemdConnector)
endif()

if(PLUGIN_TIMESYNC)
    add_subdirectory(TimeSync)
endif()

if(PLUGIN_TRACECONTROL)
    add_subdirectory(TraceControl)
endif()

if(PLUGIN_WEBKITBROWSER)
    add_subdirectory(WebKitBrowser)
endif()

if(PLUGIN_WEBPA)
    add_subdirectory(WebPA)
endif()

if(PLUGIN_WEBPROXY)
    add_subdirectory(WebProxy)
endif()

if(PLUGIN_WEBSERVER)
    add_subdirectory(WebServer)
endif()

if(PLUGIN_WEBSHELL)
    add_subdirectory(WebShell)
endif()

if(PLUGIN_WIFICONTROL)
    add_subdirectory(WifiControl)
endif()

if(PLUGIN_FILETRANSFER)
    add_subdirectory(FileTransfer)
endif()

if(EXAMPLES)
    add_subdirectory(examples)
endif()

if(WPEFRAMEWORK_CREATE_IPKG_TARGETS)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_COMPONENTS_GROUPING IGNORE)

    set(CPACK_DEBIAN_PACKAGE_NAME "${WPEFRAMEWORK_PLUGINS_OPKG_NAME}")
    set(CPACK_DEBIAN_PACKAGE_VERSION "${WPEFRAMEWORK_PLUGINS_OPKG_VERSION}")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${WPEFRAMEWORK_PLUGINS_OPKG_ARCHITECTURE}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${WPEFRAMEWORK_PLUGINS_OPKG_MAINTAINER}")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${WPEFRAMEWORK_PLUGINS_OPKG_DESCRIPTION}")
    set(CPACK_PACKAGE_FILE_NAME "${WPEFRAMEWORK_PLUGINS_OPKG_FILE_NAME}")

    # list of components from which packages will be generated
    set(CPACK_COMPONENTS_ALL
        ${NAMESPACE}WebKitBrowser
        WPEInjectedBundle
    )

    include(CPack)
endif()


