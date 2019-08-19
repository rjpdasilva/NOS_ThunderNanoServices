
// C++ classes for BluetoothControl API JSON-RPC API.
// Generated automatically from 'BluetoothControl.json'.

// Note: This code is inherently not thread safe. If required, proper synchronisation must be added.

#pragma once

#include <core/JSON.h>

namespace WPEFramework {

namespace JsonData {

    namespace BluetoothControl {

        // Common classes
        //

        class PairParamsInfo : public Core::JSON::Container {
        public:
            PairParamsInfo()
                : Core::JSON::Container()
            {
                Add(_T("address"), &Address);
            }

            PairParamsInfo(const PairParamsInfo&) = delete;
            PairParamsInfo& operator=(const PairParamsInfo&) = delete;

        public:
            Core::JSON::String Address; // Address of the deveice to pair with
        }; // class PairParamsInfo

        // Method params/result classes
        //

        class DeviceData : public Core::JSON::Container {
        public:
            DeviceData()
                : Core::JSON::Container()
            {
                Init();
            }

            DeviceData(const DeviceData& other)
                : Core::JSON::Container()
                , Address(other.Address)
                , Name(other.Name)
                , Lowenergy(other.Lowenergy)
                , Connected(other.Connected)
                , Paired(other.Paired)
                , Reason(other.Reason)
            {
                Init();
            }

            DeviceData& operator=(const DeviceData& rhs)
            {
                Address = rhs.Address;
                Name = rhs.Name;
                Lowenergy = rhs.Lowenergy;
                Connected = rhs.Connected;
                Paired = rhs.Paired;
                Reason = rhs.Reason;
                return (*this);
            }

        private:
            void Init()
            {
                Add(_T("address"), &Address);
                Add(_T("name"), &Name);
                Add(_T("lowenergy"), &Lowenergy);
                Add(_T("connected"), &Connected);
                Add(_T("paired"), &Paired);
                Add(_T("reason"), &Reason);
            }

        public:
            Core::JSON::String Address; // Device address
            Core::JSON::String Name; // Device name
            Core::JSON::Boolean Lowenergy; // Describes if device is running in low energy mode (BLE)
            Core::JSON::Boolean Connected; // Describes if device is connected to host
            Core::JSON::Boolean Paired; // Describes if device is paired with host
            Core::JSON::DecUInt32 Reason; // Code for reason of last disconnection
        }; // class DeviceData

        class DisconnectParamsData : public Core::JSON::Container {
        public:
            DisconnectParamsData()
                : Core::JSON::Container()
            {
                Add(_T("address"), &Address);
                Add(_T("reason"), &Reason);
            }

            DisconnectParamsData(const DisconnectParamsData&) = delete;
            DisconnectParamsData& operator=(const DisconnectParamsData&) = delete;

        public:
            Core::JSON::String Address; // Address of the deveice to pair with
            Core::JSON::DecUInt32 Reason; // Code for reason od disconnection
        }; // class DisconnectParamsData

        class ScanParamsData : public Core::JSON::Container {
        public:
            ScanParamsData()
                : Core::JSON::Container()
            {
                Add(_T("lowenergy"), &Lowenergy);
                Add(_T("limited"), &Limited);
                Add(_T("passive"), &Passive);
                Add(_T("duration"), &Duration);
            }

            ScanParamsData(const ScanParamsData&) = delete;
            ScanParamsData& operator=(const ScanParamsData&) = delete;

        public:
            Core::JSON::Boolean Lowenergy; // Perform a scan in low-energy mode
            Core::JSON::Boolean Limited;
            Core::JSON::Boolean Passive; // Perform a passive scan if set to true. In passive mode host will only listen to advertisments. It won't send scan requests
            Core::JSON::DecUInt32 Duration; // Scanning duration in seconds
        }; // class ScanParamsData

    } // namespace BluetoothControl

} // namespace JsonData

}

