#include "BluetoothImplementation.h"

namespace WPEFramework {

namespace Plugin {

    std::map<string, string> BluetoothImplementation::_deviceIdMap;
    Core::JSON::ArrayType<BTDeviceList::BTDeviceInfo> BluetoothImplementation::_jsonDeviceInfoList;

    bool BluetoothImplementation::ConfigureBTAdapter()
    {
        _dbusConnection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL,  NULL);
        if (nullptr ==  _dbusConnection) {
            TRACE(Trace::Error, ("Failed to acquire dbus handle"));
            return false;
        }

        if (!PowerOnBTAdapter()) {
            TRACE(Trace::Error, ("Failed to switch on bluetooth adaptor"));
            return false;
        }

        return true;
    }

    bool BluetoothImplementation::PowerOnBTAdapter()
    {
        GError* dbusError = nullptr;
        GVariant* dbusReply = nullptr;

        dbusReply = g_dbus_connection_call_sync(_dbusConnection, BLUEZ_INTERFACE, BLUEZ_OBJECT_HCI, DBUS_PROP_INTERFACE, "Set", g_variant_new("(ssv)", BLUEZ_INTERFACE_ADAPTER, "Powered", g_variant_new("b", true)), nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &dbusError);
        if (nullptr == dbusReply) {
            TRACE(Trace::Error, ("Failed powerOnBTAdapter dbus call. Error msg :  %s", dbusError->message));
            return false;
        }

        return true;
    }

    bool BluetoothImplementation::SubscribeDBusSignals()
    {
        _propertiesChanged = g_dbus_connection_signal_subscribe(_dbusConnection, nullptr, DBUS_PROP_INTERFACE, "PropertiesChanged", nullptr, BLUEZ_INTERFACE_ADAPTER, G_DBUS_SIGNAL_FLAGS_NONE, AdapterPropertiesChanged, nullptr, nullptr);
        if (!_propertiesChanged) {
            TRACE(Trace::Error, ("Failed to subscribe PropertiesChanged signal"));
            return false;
        }

        _interfacesAdded = g_dbus_connection_signal_subscribe(_dbusConnection, BLUEZ_INTERFACE, DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesAdded", ROOT_OBJECT, nullptr, G_DBUS_SIGNAL_FLAGS_NONE, InterfacesAdded, nullptr, nullptr);
        if (!_interfacesAdded) {
            TRACE(Trace::Error, ("Failed to subscribe InterfacesAdded signal"));
            return false;
        }

        _interfacesRemoved = g_dbus_connection_signal_subscribe(_dbusConnection, BLUEZ_INTERFACE, DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesRemoved", ROOT_OBJECT, nullptr, G_DBUS_SIGNAL_FLAGS_NONE, InterfacesRemoved, nullptr, nullptr);
        if (!_interfacesRemoved) {
            TRACE(Trace::Error, ("Failed to subscribe InterfacesRemoved signal"));
            return false;
        }

        return true;
    }

    bool BluetoothImplementation::UnsubscribeDBusSignals()
    {
        g_dbus_connection_signal_unsubscribe(_dbusConnection, _propertiesChanged);
        g_dbus_connection_signal_unsubscribe(_dbusConnection, _interfacesAdded);
        g_dbus_connection_signal_unsubscribe(_dbusConnection, _interfacesRemoved);

        return true;
    }

    void BluetoothImplementation::AdapterPropertiesChanged(GDBusConnection* connection, const gchar* senderName, const gchar* objectPath, const gchar* interfaceName, const gchar* signalName, GVariant* parameters, gpointer userData)
    {
        return;
    }

    void BluetoothImplementation::InterfacesAdded(GDBusConnection* connection, const gchar* senderName, const gchar* objectPath, const gchar* interfaceName, const gchar* signalName, GVariant* parameters, gpointer userData)
    {
        string deviceId;
        string deviceName;
        string deviceAddress;
        string devicePropertyType;
        string bluezDeviceName;
        GVariantIter* iterator1;
        GVariantIter* iterator2;
        GVariant* devicePropertyValue;
        BTDeviceList::BTDeviceInfo jsonDeviceInfo;

        g_variant_get(parameters, "(oa{sa{sv}})", &deviceId, &iterator1);
        while (g_variant_iter_loop(iterator1, "{sa{sv}}", &bluezDeviceName, &iterator2)) {
            while (g_variant_iter_loop(iterator2, "{sv}", &devicePropertyType, &devicePropertyValue)) {
                if (strcmp(devicePropertyType.c_str(), "Address") == 0) {
                    g_variant_get(devicePropertyValue, "s", &deviceAddress);
                    jsonDeviceInfo.Address = deviceAddress;
                }

                if (strcmp(devicePropertyType.c_str(), "Name") == 0) {
                    g_variant_get(devicePropertyValue, "s", &deviceName);
                    jsonDeviceInfo.Name = deviceName;
                }
            }
        }
        if (_deviceIdMap.find(deviceAddress.c_str()) == _deviceIdMap.end()) {
             TRACE_L1("Added BT Device. Device ID : [%s]", deviceAddress.c_str());
            _deviceIdMap.insert(std::make_pair(deviceAddress.c_str(), deviceId.c_str()));
            _jsonDeviceInfoList.Add(jsonDeviceInfo);
        }
    }

    void BluetoothImplementation::InterfacesRemoved(GDBusConnection *connection, const gchar *senderName, const gchar *objectPath, const gchar *interfaceName, const gchar *signalName, GVariant* parameters, gpointer userData)
    {
        string deviceId;

        g_variant_get(parameters, "(oas)", &deviceId, nullptr);
        const auto& iterator = _deviceIdMap.find(deviceId.c_str());
        if (iterator != _deviceIdMap.end()) {
            _deviceIdMap.erase(iterator);
            TRACE_L1("Removed BT Device. Device ID : [%s]", deviceId.c_str());
        }
    }

    bool BluetoothImplementation::StartDiscovery()
    {
        GError* dbusError = nullptr;
        GVariant* dbusReply = nullptr;

        dbusReply = g_dbus_connection_call_sync(_dbusConnection, BLUEZ_INTERFACE, BLUEZ_OBJECT_HCI, BLUEZ_INTERFACE_ADAPTER, "StartDiscovery", nullptr, nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &dbusError);
        if (nullptr == dbusReply) {
            TRACE(Trace::Error, ("Failed startDiscovery dbus call. Error msg :  %s", dbusError->message));
            _isScanning = false;
            return false;
        }

        TRACE(Trace::Information, ("Started BT Scan"));
        return true;
    }

    bool BluetoothImplementation::StartScan()
    {
        if (_isScanning == false) {
            TRACE(Trace::Information, ("Start BT Scan"));
            Run();
            _isScanning = true;
            return true;
        } else {
            TRACE(Trace::Error, ("Scanning is already started!"));
            return false;
        }
    }

    bool BluetoothImplementation::StopScan()
    {
        GError* dbusError = nullptr;
        GVariant* dbusReply = nullptr;

        if (_isScanning == true) {
            dbusReply = g_dbus_connection_call_sync(_dbusConnection, BLUEZ_INTERFACE, BLUEZ_OBJECT_HCI, BLUEZ_INTERFACE_ADAPTER, "StopDiscovery", nullptr, nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &dbusError);
            if (nullptr == dbusReply) {
                TRACE(Trace::Error, ("Failed stopDiscovery dbus call. Error msg :  %s", dbusError->message));
                return false;
            }

            TRACE(Trace::Information, ("Stopped BT Scan"));
            if (g_main_is_running(_loop))
                g_main_loop_quit(_loop);

            _isScanning = false;
            return true;
        } else {
            TRACE(Trace::Error, ("Scanning is not started!"));
            return false;
        }
    }

    string BluetoothImplementation::ShowDeviceList()
    {
        std::string deviceInfoList;

        _jsonDeviceInfoList.ToString(deviceInfoList);
        return deviceInfoList;
    }

    bool BluetoothImplementation::Pair(string deviceId)
    {
        const auto& iterator = _deviceIdMap.find(deviceId.c_str());
        if (iterator != _deviceIdMap.end()) {
            TRACE(Trace::Information, ("Pairing BT Device. Device ID : [%s]", deviceId.c_str()));
            if (!PairDevice(iterator->second)) {
                TRACE(Trace::Error, ("Failed to Pair BT Device. Device ID : [%s]", deviceId.c_str()));
                return false;
            }
        }

        TRACE(Trace::Information, ("Paired BT Device. Device ID : [%s]", deviceId.c_str()));
        return true;
    }

    bool BluetoothImplementation::PairDevice(const string deviceId)
    {
        GError* dbusError = nullptr;
        GVariant* dbusReply = nullptr;

        // Turning 'Pairable' On.
        dbusReply = g_dbus_connection_call_sync(_dbusConnection, BLUEZ_INTERFACE, BLUEZ_OBJECT_HCI, DBUS_PROP_INTERFACE, "Set", g_variant_new("(ssv)",BLUEZ_INTERFACE_ADAPTER, "Pairable",g_variant_new("b", true)), nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &dbusError);
        if (nullptr == dbusReply) {
            TRACE(Trace::Error, ("Failed Pairable On dbus call. Error msg :  %s", dbusError->message));
            return false;
        }

        // Pairing wit BT Device.
        dbusReply = g_dbus_connection_call_sync(_dbusConnection, BLUEZ_INTERFACE, deviceId.c_str(), BLUEZ_INTERFACE_DEVICE, "Pair", nullptr, nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &dbusError);
        if (nullptr == dbusReply) {
            std::size_t found = std::string(dbusError->message).find(BLUEZ_OBJECT_EXISTS);
            if (found != std::string::npos) {
                TRACE(Trace::Error, ("BT Device Already Paired. DBus msg : %s", dbusError->message));
                return true;
            }
            TRACE(Trace::Error, ("Failed Pair dbus call. Error msg :  %s", dbusError->message));
            return false;
        }

        return true;
    }

    bool BluetoothImplementation::GetManagedObjects()
    {
        GError* dbusError = nullptr;
        GVariant* dbusReply = nullptr;
        string deviceIdList[50];
        string deviceName;
        string deviceAddress;
        string devicePropertyType;
        string bluezDeviceName;
        GVariantIter* iterator1;
        GVariantIter* iterator2;
        GVariantIter* iterator3;
        GVariant* devicePropertyValue;
        BTDeviceList::BTDeviceInfo pairedDeviceInfo;

        dbusReply = g_dbus_connection_call_sync(_dbusConnection, BLUEZ_INTERFACE, ROOT_OBJECT, DBUS_INTERFACE_OBJECT_MANAGER, "GetManagedObjects", nullptr,G_VARIANT_TYPE("(a{oa{sa{sv}}})"),  G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &dbusError);
        if (nullptr == dbusReply) {
            TRACE(Trace::Error, ("Failed to Get Managed Objects. Error msg :  %s", dbusError->message));
            return false;
        }

        g_variant_get(dbusReply, "(a{oa{sa{sv}}})", &iterator1);

        for (int iterator = 0; g_variant_iter_next(iterator1, "{oa{sa{sv}}}", &deviceIdList[iterator],&iterator2); iterator++) {
            TRACE(Trace::Information, ("Paired Device ID -  : [%s]", deviceIdList[iterator].c_str()));
            while (g_variant_iter_loop(iterator2, "{sa{sv}}", &bluezDeviceName, &iterator3)) {
                while (g_variant_iter_loop(iterator3, "{sv}", &devicePropertyType, &devicePropertyValue)) {
                    if (strcmp(devicePropertyType.c_str(), "Address") == 0) {
                        g_variant_get(devicePropertyValue, "s", &deviceAddress);
                        pairedDeviceInfo.Address = deviceAddress;
                        TRACE(Trace::Information, ("Paired Device Address -  : [%s]", deviceAddress.c_str()));
                    }

                    if (strcmp(devicePropertyType.c_str(), "Name") == 0) {
                        g_variant_get(devicePropertyValue, "s", &deviceName);
                        pairedDeviceInfo.Name = deviceName;
                        TRACE(Trace::Information, ("Paired Device Name -  : [%s]", deviceName.c_str()));
                    }
                }
            }

            if (_deviceIdMap.find(deviceAddress.c_str()) == _deviceIdMap.end()) {
                TRACE_L1("Added BT Device. Device ID : [%s]", deviceAddress.c_str());
                _deviceIdMap.insert(std::make_pair(deviceAddress.c_str(), deviceIdList[iterator].c_str()));
                _jsonDeviceInfoList.Add(pairedDeviceInfo);
            }
        }
        return true;
    }

    bool BluetoothImplementation::Connect(string deviceId)
    {
        GError* dbusError = nullptr;
        GVariant* dbusReply = nullptr;

        const auto& iterator = _deviceIdMap.find(deviceId.c_str());
        if (iterator != _deviceIdMap.end()) {
            TRACE(Trace::Information, ("Connecting BT Device. Device ID : [%s]", deviceId.c_str()));
            dbusReply = g_dbus_connection_call_sync(_dbusConnection, BLUEZ_INTERFACE, iterator->second.c_str(), BLUEZ_INTERFACE_DEVICE, "Connect", nullptr, nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &dbusError);
            if (nullptr == dbusReply) {
                TRACE(Trace::Error, ("Failed Connect dbus call. Error msg :  %s", dbusError->message));
                return false;
            }

            TRACE(Trace::Information, ("Connected BT Device. Device ID : [%s]", deviceId.c_str()));
            return true;
        }

        TRACE(Trace::Error, ("Invalid BT Device ID. Device ID : [%s]", deviceId.c_str()));
        return false;

    }

    bool BluetoothImplementation::Disconnect(string deviceId)
    {
        GError* dbusError = nullptr;
        GVariant* dbusReply = nullptr;

        const auto& iterator = _deviceIdMap.find(deviceId.c_str());
        if (iterator != _deviceIdMap.end()) {
            TRACE(Trace::Information, ("Disconnecting BT Device. Device ID : [%s]", deviceId.c_str()));
            dbusReply = g_dbus_connection_call_sync(_dbusConnection, BLUEZ_INTERFACE, iterator->second.c_str(), BLUEZ_INTERFACE_DEVICE, "Disconnect", nullptr, nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &dbusError);
            if (nullptr == dbusReply) {
                TRACE(Trace::Error, ("Failed Disconnect dbus call. Error msg :  %s", dbusError->message));
                return false;
            }

            TRACE(Trace::Information, ("Disconnected BT Device. Device ID : [%s]", deviceId.c_str()));
            return true;
        }

        TRACE(Trace::Error, ("Invalid BT Device ID. Device ID : [%s]", deviceId.c_str()));
        return false;
    }

    SERVICE_REGISTRATION(BluetoothImplementation, 1, 0);

}
} /* namespace WPEFramework::Plugin */