
#include "Module.h"
#include "BluetoothControl.h"
#include <interfaces/json/JsonData_BluetoothControl.h>

/*
    // Copy the code below to BluetoothControl class definition
    // Note: The BluetoothControl class must inherit from PluginHost::JSONRPC

    private:
        void RegisterAll();
        void UnregisterAll();
        uint32_t endpoint_pair(const JsonData::BluetoothControl::PairParamsInfo& params);
        uint32_t endpoint_connect(const JsonData::BluetoothControl::PairParamsInfo& params);
        uint32_t endpoint_scan(const JsonData::BluetoothControl::ScanParamsData& params);
        uint32_t endpoint_stopscan();
        uint32_t endpoint_unpair(const JsonData::BluetoothControl::PairParamsInfo& params);
        uint32_t endpoint_disconnect(const JsonData::BluetoothControl::DisconnectParamsData& params);
        uint32_t get_scanning(Core::JSON::Boolean& response) const;
        uint32_t get_advertising(Core::JSON::Boolean& response) const;
        uint32_t set_advertising(const Core::JSON::Boolean& param);
        uint32_t get_device(const string& index, Core::JSON::ArrayType<JsonData::BluetoothControl::DeviceData>& response) const;
*/

namespace WPEFramework {

namespace Plugin {

    using namespace JsonData::BluetoothControl;

    // Registration
    //

    void BluetoothControl::RegisterAll()
    {
        Register<PairParamsInfo,void>(_T("pair"), &BluetoothControl::endpoint_pair, this);
        Register<PairParamsInfo,void>(_T("connect"), &BluetoothControl::endpoint_connect, this);
        Register<ScanParamsData,void>(_T("scan"), &BluetoothControl::endpoint_scan, this);
        Register<void,void>(_T("stopscan"), &BluetoothControl::endpoint_stopscan, this);
        Register<PairParamsInfo,void>(_T("unpair"), &BluetoothControl::endpoint_unpair, this);
        Register<DisconnectParamsData,void>(_T("disconnect"), &BluetoothControl::endpoint_disconnect, this);
        Property<Core::JSON::Boolean>(_T("scanning"), &BluetoothControl::get_scanning, nullptr, this);
        Property<Core::JSON::Boolean>(_T("advertising"), &BluetoothControl::get_advertising, &BluetoothControl::set_advertising, this);
        Property<Core::JSON::ArrayType<DeviceData>>(_T("device"), &BluetoothControl::get_device, nullptr, this);
    }

    void BluetoothControl::UnregisterAll()
    {
        Unregister(_T("disconnect"));
        Unregister(_T("unpair"));
        Unregister(_T("stopscan"));
        Unregister(_T("scan"));
        Unregister(_T("connect"));
        Unregister(_T("pair"));
        Unregister(_T("device"));
        Unregister(_T("advertising"));
        Unregister(_T("scanning"));
    }

    // API implementation
    //

    // Method: pair - Pair host with bluetooth device
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_UNKNOWN_KEY: Device not found
    //  - ERROR_GENERAL: Failed to pair
    //  - ERROR_ASYNC_ABORTED: Pairing aborted
    uint32_t BluetoothControl::endpoint_pair(const PairParamsInfo& params)
    {
        uint32_t result = Core::ERROR_NONE;
        const string& address = params.Address.Value();

        // TODO...

        return result;
    }

    // Method: connect - Connect with bluetooth device
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_UNKNOWN_KEY: Device not found
    //  - ERROR_GENERAL: Failed to connect
    //  - ERROR_ASYNC_ABORTED: Connecting aborted
    uint32_t BluetoothControl::endpoint_connect(const PairParamsInfo& params)
    {
        uint32_t result = Core::ERROR_NONE;
        const string& address = params.Address.Value();

        // TODO...

        return result;
    }

    // Method: scan - Scan environment for bluetooth devices
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: Scanning failed
    uint32_t BluetoothControl::endpoint_scan(const ScanParamsData& params)
    {
        uint32_t result = Core::ERROR_NONE;
        const bool& lowenergy = params.Lowenergy.Value();
        const bool& limited = params.Limited.Value();
        const bool& passive = params.Passive.Value();
        const uint32_t& duration = params.Duration.Value();

        // TODO...

        return result;
    }

    // Method: stopscan - Connect with bluetooth device
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t BluetoothControl::endpoint_stopscan()
    {
        uint32_t result = Core::ERROR_NONE;

        // TODO...

        return result;
    }

    // Method: unpair - Unpair host from a bluetooth device
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_UNKNOWN_KEY: Device not found
    uint32_t BluetoothControl::endpoint_unpair(const PairParamsInfo& params)
    {
        uint32_t result = Core::ERROR_NONE;
        const string& address = params.Address.Value();

        // TODO...

        return result;
    }

    // Method: disconnect - Disconnects host from bluetooth device
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_UNKNOWN_KEY: Device not found
    uint32_t BluetoothControl::endpoint_disconnect(const DisconnectParamsData& params)
    {
        uint32_t result = Core::ERROR_NONE;
        const string& address = params.Address.Value();
        const uint32_t& reason = params.Reason.Value();

        // TODO...

        return result;
    }

    // Property: scanning - Tells if host is currently scanning for bluetooth devices
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t BluetoothControl::get_scanning(Core::JSON::Boolean& response) const
    {
        // response = ...

        return Core::ERROR_NONE;
    }

    // Property: advertising - Defines if device is advertising to be visible by other bluetooth devices
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t BluetoothControl::get_advertising(Core::JSON::Boolean& response) const
    {
        // response = ...

        return Core::ERROR_NONE;
    }

    // Property: advertising - Defines if device is advertising to be visible by other bluetooth devices
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t BluetoothControl::set_advertising(const Core::JSON::Boolean& param)
    {
        uint32_t result = Core::ERROR_NONE;

        // TODO...

        return result;
    }

    // Property: device - Informations about devices found during scanning
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_UNKNOWN_KEY: Device not found
    uint32_t BluetoothControl::get_device(const string& index, Core::JSON::ArrayType<DeviceData>& response) const
    {
        // response = ...

        return Core::ERROR_NONE;
    }

} // namespace Plugin

}

