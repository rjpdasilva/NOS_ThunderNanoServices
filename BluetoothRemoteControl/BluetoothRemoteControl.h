#pragma once

#include "Module.h"

#include "Administrator.h"
#include "WAVRecorder.h"

#include <interfaces/IBluetooth.h>
#include <interfaces/IKeyHandler.h>
#include <interfaces/IVoiceHandler.h>
#include <interfaces/json/JsonData_BluetoothRemoteControl.h>

namespace WPEFramework {

namespace Plugin {

    class BluetoothRemoteControl : public PluginHost::IPlugin
                                 , public PluginHost::IWeb
                                 , public PluginHost::JSONRPC
                                 , public Exchange::IVoiceProducer {
    public:
        enum recorder {
            OFF               = 0x00,
            SINGLE            = 0x10,
            SEQUENCED         = 0x11,
            SINGLE_PERSIST    = 0x20,
            SEQUENCED_PERSIST = 0x21
        };

   private:
        class Config : public Core::JSON::Container {
        public:
            Config(const Config&) = delete;
            Config& operator=(const Config&) = delete;
            Config()
                : Core::JSON::Container()
                , Controller(_T("BluetoothControl"))
                , KeyMap()
                , KeyIngest(true)
                , Recorder(OFF)
            {    
                Add(_T("controller"), &Controller);
                Add(_T("keymap"), &KeyMap);
                Add(_T("keyingest"), &KeyIngest);
                Add(_T("recorder"), &Recorder);
            }
            ~Config()
            {
            }

        public:
            Core::JSON::String Controller;
            Core::JSON::String KeyMap;
            Core::JSON::Boolean KeyIngest;
            Core::JSON::EnumType<recorder> Recorder;
        };

        class GATTRemote : public Bluetooth::GATTSocket {
        private:
            static constexpr uint16_t HID_UUID         = 0x1812;

            class Flow {
            public:
                Flow() = delete;
                Flow(const Flow&) = delete;
                Flow& operator=(const Flow&) = delete;
                Flow(const TCHAR formatter[], ...)
                {
                    va_list ap;
                    va_start(ap, formatter);
                    Trace::Format(_text, formatter, ap);
                    va_end(ap);
                }
                explicit Flow(const string& text)
                    : _text(Core::ToString(text))
                {
                }
                ~Flow()
                {
                }

            public:
                const char* Data() const
                {
                    return (_text.c_str());
                }
                uint16_t Length() const
                {
                    return (static_cast<uint16_t>(_text.length()));
                }

            private:
                std::string _text;
            };

            class Sink : public Exchange::IBluetooth::IDevice::ICallback {
            public:
                Sink() = delete;
                Sink(const Sink&) = delete;
                Sink& operator=(const Sink&) = delete;
                Sink(GATTRemote* parent) : _parent(*parent)
                {
                    ASSERT(parent != nullptr);
                }
                ~Sink() override
                {    
                }

            public:
                void Updated() override
                {
                    _parent.Updated();
                }

                BEGIN_INTERFACE_MAP(Sink)
                    INTERFACE_ENTRY(Exchange::IBluetooth::IDevice::ICallback)
                END_INTERFACE_MAP

            private:
                GATTRemote& _parent;
            };

            class Decoupling : public Core::IDispatch {
            public:
                Decoupling(const Decoupling&) = delete;
                Decoupling& operator=(const Decoupling&) = delete;
                Decoupling(GATTRemote* parent)
                    : _parent(*parent)
                    , _adminLock()
                    , _next(0)
                    , _length(0xFF)
                    , _buffer(reinterpret_cast<uint8_t*>(::malloc(_length)))
                {
                    ASSERT(parent != nullptr);
                }
                ~Decoupling() override
                {
                    ::free(_buffer);
                }

            public:
                void Submit(const uint16_t handle, const uint8_t length, const uint8_t buffer[])
                {
                    ASSERT (length > 0);

                    _adminLock.Lock();

                    if ((_next + length + 3) > _length) {
                        // Oops we need to extend our buffer
                        _length = (((_next + length + 3 + 0xFF) >> 8) << 8);
                        uint8_t* newBuffer = reinterpret_cast<uint8_t*>(::malloc(_length));
                        ::memcpy(newBuffer, _buffer, _next);
                        ::free(_buffer);
                        _buffer = newBuffer;
                    }

                    _buffer[_next++] = length;
                    _buffer[_next++] = (handle >> 8) & 0xFF;
                    _buffer[_next++] = (handle & 0xFF);
                    ::memcpy(&(_buffer[_next]), buffer, length);
                    _next += length;

                    _adminLock.Unlock();
                }
                void Dispatch() override
                {
                    uint16_t offset = 0;

                    _adminLock.Lock();

                    while (offset < _next) {

                        uint8_t length = _buffer[offset++];
                        uint16_t handle = (_buffer[offset] << 8) | (_buffer[offset+1]);
                        offset += 2;
                        _parent.Message(handle, length, &(_buffer[offset]));

                        offset += length;

                        ASSERT (offset <= _next);
                    }

                    ASSERT (offset == _next);

                    _next = 0;

                    _adminLock.Unlock();
                }

            private:
                GATTRemote& _parent;
                Core::CriticalSection _adminLock;
                uint16_t _next;
                uint16_t _length;
                uint8_t* _buffer;
            };

            class AudioProfile : public Exchange::IVoiceProducer::IProfile {
            public:
                AudioProfile(const AudioProfile&) = delete;
                AudioProfile& operator= (const AudioProfile&) = delete;
                AudioProfile(const Exchange::IVoiceProducer::IProfile::codec codec, const uint8_t channels, const uint32_t sampleRate, const uint8_t resolution) 
                    : _codec(codec)
                    , _channels(channels)
                    , _sampleRate(sampleRate)
                    , _resolution(resolution) {
                }
                virtual ~AudioProfile() {
                }

            public:
                Exchange::IVoiceProducer::IProfile::codec Codec() const override {
                    return (_codec);
                }
                uint8_t Channels() const override {
                    return (_channels);
                }
                uint32_t SampleRate() const override {
                    return (_sampleRate);
                }
                uint8_t Resolution() const override {
                    return (_resolution);
                }

                BEGIN_INTERFACE_MAP(AudioProfile)
                    INTERFACE_ENTRY(Exchange::IVoiceProducer::IProfile)
                END_INTERFACE_MAP

            private:
                codec _codec;
                uint8_t _channels;
                uint32_t _sampleRate;
                uint8_t _resolution;
            };

            class Config : public Core::JSON::Container {
            public:
                class Profile : public Core::JSON::Container {
                public:
                    Profile(const Profile&) = delete;
                    Profile& operator=(const Profile&) = delete;
                    Profile() 
                        : Core::JSON::Container()
                        , Codec(Exchange::IVoiceProducer::IProfile::codec::PCM)
                        , SampleRate(16000)
                        , Channels(1)
                        , Resolution(16)
                    {    
                        Add(_T("codec"), &Codec);
                        Add(_T("samplerate"), &SampleRate);
                        Add(_T("channels"), &Channels);
                        Add(_T("resolution"), &Resolution);
                        Add(_T("configuration"), &Configuration);
                    }
                    ~Profile() override 
                    {
                    }

                public:
                    Core::JSON::EnumType<Exchange::IVoiceProducer::IProfile::codec> Codec;
                    Core::JSON::DecUInt32 SampleRate;
                    Core::JSON::DecUInt8 Channels;
                    Core::JSON::DecUInt8 Resolution;
                    Core::JSON::String Configuration;
                };

            public:
                Config(const Config&) = delete;
                Config& operator=(const Config&) = delete;
                Config()
                    : Core::JSON::Container()
                    , AudioProfile()
                    , ServiceUUID()
                    , CommandUUID()
                    , DataUUID()
                {    
                    Add(_T("profile"), &AudioProfile);
                    Add(_T("serviceuuid"), &ServiceUUID);
                    Add(_T("commanduuid"), &CommandUUID);
                    Add(_T("datauuid"), &DataUUID);
                }
                ~Config()
                {
                }

            public:
                Profile AudioProfile;
                Core::JSON::String ServiceUUID;
                Core::JSON::String CommandUUID;
                Core::JSON::String DataUUID;
            };

            class Profile : public Bluetooth::Profile {
            public:
                Profile() = delete;
                Profile(const Profile&) = delete;
                Profile& operator= (const Profile&) = delete;

                Profile(const Config& config) 
                    : Bluetooth::Profile(true) {
                    VoiceService = Bluetooth::UUID(config.ServiceUUID.Value());
                    VoiceCommandChar = Bluetooth::UUID(config.CommandUUID.Value());
                    VoiceDataChar = Bluetooth::UUID(config.DataUUID.Value());
                }
                ~Profile() {
                }

            public:
                uint16_t FindHandle(const Bluetooth::UUID& serviceUuid, const Bluetooth::UUID& charUuid) const
                {
                    const Bluetooth::Profile::Service::Characteristic* characteristic = FindCharacteristic(serviceUuid, charUuid);
                    return (characteristic == nullptr ? 0 : characteristic->Handle());
                }
                uint16_t FindHandle(const Bluetooth::UUID& serviceUuid, const Bluetooth::UUID& charUuid, const Bluetooth::UUID& descUuid) const
                {
                    uint16_t handle = 0;
                    const Bluetooth::Profile::Service::Characteristic* characteristic = FindCharacteristic(serviceUuid, charUuid);
                    if (characteristic != nullptr) {
                        const Bluetooth::Profile::Service::Characteristic::Descriptor* descriptor= (*characteristic)[descUuid];
                        if (descriptor != nullptr) {
                            handle = descriptor->Handle();
                        }
                    }
                    return (handle);
                }

            private:
                const Bluetooth::Profile::Service::Characteristic* FindCharacteristic(const Bluetooth::UUID& serviceUuid, const Bluetooth::UUID& charUuid) const
                {
                    const Bluetooth::Profile::Service::Characteristic* result = nullptr;
                    const Bluetooth::Profile::Service* service = (*this)[serviceUuid];
                    if (service != nullptr) {
                        result = (*service)[charUuid];
                    }
                    return (result);
                }


            public:
                Bluetooth::UUID VoiceService;
                Bluetooth::UUID VoiceCommandChar;
                Bluetooth::UUID VoiceDataChar;
            };

            static Core::NodeId Designator(const uint8_t type, const string& address)
            {
                return (Bluetooth::Address(address.c_str()).NodeId(static_cast<Bluetooth::Address::type>(type), GATTSocket::LE_ATT_CID, 0));
            }

        public:
            class Data : public Core::JSON::Container {
            public:
                Data(const Data&) = delete;
                Data& operator=(const Data&) = delete;
                Data()
                    : Core::JSON::Container()
                    , Name()
                    , Address()
                    , ModelNumber()
                    , SerialNumber()
                    , FirmwareRevision()
                    , ManufacturerName()
                    , SoftwareRevision()
                    , SoftwareRevisionHandle(0)
                    , KeysDataHandle(0)
                    , BatteryLevelHandle(0)
                    , VoiceCommandHandle(0)
                    , VoiceDataHandle(0)
                {
                    Add(_T("name"), &Name);
                    Add(_T("address"), &Address);
                    Add(_T("model"), &ModelNumber);
                    Add(_T("serial"), &SerialNumber);
                    Add(_T("firmware"), &FirmwareRevision);
                    Add(_T("manufacturer"), &ManufacturerName);
                    Add(_T("software"), &SoftwareRevision);
                    Add(_T("version"), &SoftwareRevisionHandle);
                    Add(_T("key"), &KeysDataHandle);
                    Add(_T("battery"), &BatteryLevelHandle);
                    Add(_T("command"), &VoiceCommandHandle);
                    Add(_T("voice"), &VoiceDataHandle);
                }
               ~Data()
                {
                }

            public:
               Core::JSON::String Name;
               Core::JSON::String Address;
               Core::JSON::String ModelNumber;
               Core::JSON::String SerialNumber;
               Core::JSON::String FirmwareRevision;
               Core::JSON::String ManufacturerName;
               Core::JSON::String SoftwareRevision;

               Core::JSON::DecUInt16 SoftwareRevisionHandle;
               Core::JSON::DecUInt16 KeysDataHandle;
               Core::JSON::DecUInt16 BatteryLevelHandle;
               Core::JSON::DecUInt16 VoiceCommandHandle;
               Core::JSON::DecUInt16 VoiceDataHandle;
            };

        public:
            GATTRemote() = delete;
            GATTRemote(const GATTRemote&) = delete;
            GATTRemote& operator=(const GATTRemote&) = delete;
 
            GATTRemote(BluetoothRemoteControl* parent, Exchange::IBluetooth::IDevice* device, const string& configuration)
                : Bluetooth::GATTSocket(Designator(device->Type(), device->LocalId()), Designator(device->Type(), device->RemoteId()), 255)
                , _parent(parent)
                , _adminLock()
                , _profile(nullptr)
                , _command()
                , _device(device)
                , _decoupling(this)
                , _sink(this)
                , _name()
                , _address()
                , _modelNumber()
                , _serialNumber()
                , _manufacturerName()
                , _firmwareRevision()
                , _softwareRevision()
                , _softwareRevisionHandle(0)
                , _keysDataHandle(~0)
                , _batteryLevelHandle(~0)
                , _voiceDataHandle(~0)
                , _voiceCommandHandle(~0)
                , _audioProfile(nullptr)
                , _decoder(nullptr)
            {
                Config config;
                config.FromString(configuration);

                Constructor(config);

                if (_device->IsConnected() == true) {
                    TRACE(Trace::Fatal, (_T("The device is already connected. First disconnect the device")));
                }
                else {
                    _profile = new Profile(config);
                }
            }

            GATTRemote(BluetoothRemoteControl* parent, Exchange::IBluetooth::IDevice* device, const string& configuration, const Data& data)
                : Bluetooth::GATTSocket(Designator(device->Type(), device->LocalId()), Designator(device->Type(), device->RemoteId()), 255)
                , _parent(parent)
                , _adminLock()
                , _profile(nullptr)
                , _command()
                , _device(device)
                , _decoupling(this)
                , _sink(this)
                , _name()
                , _address()
                , _modelNumber(data.ModelNumber.Value())
                , _serialNumber(data.SerialNumber.Value())
                , _manufacturerName(data.ManufacturerName.Value())
                , _firmwareRevision(data.FirmwareRevision.Value())
                , _softwareRevision(data.SoftwareRevision.Value())
                , _softwareRevisionHandle(data.SoftwareRevisionHandle.Value())
                , _keysDataHandle(data.KeysDataHandle.Value())
                , _batteryLevelHandle(data.BatteryLevelHandle.Value())
                , _voiceDataHandle(data.VoiceDataHandle.Value())
                , _voiceCommandHandle(data.VoiceCommandHandle.Value())
                , _audioProfile(nullptr)
                , _decoder(nullptr)
            {
                Config config;
                config.FromString(configuration);

                Constructor(config);
            }

            virtual ~GATTRemote()
            {
                if (GATTSocket::IsOpen() == true) {
                    GATTSocket::Close(Core::infinite);
                }

                if (_device != nullptr) {
                    if (_device->Callback(nullptr) != Core::ERROR_NONE) {
                        TRACE(Trace::Fatal, (_T("Could not remove the callback from the device")));
                    }

                    _device->Release();
                    _device = nullptr;
                }

                if (_decoder != nullptr) {
                    delete _decoder;
                }

                if (_profile != nullptr) {
                    delete _profile;
                }

                _audioProfile->Release();

                PluginHost::WorkerPool::Instance().Revoke(Core::ProxyType<Core::IDispatch>(_decoupling));
                _decoupling.CompositRelease();
            }

        public:
            const string& Name() const 
            {
                return(_name);
            }
            inline const string& Address() const
            {
                return(_address);
            }
            inline const string& ModelNumber() const
            {
                return (_modelNumber);
            }
            inline const string& SerialNumber() const
            {
                return (_serialNumber);
            }
            inline const string& FirmwareRevision() const
            {
                return (_firmwareRevision);
            }
            inline const string& SoftwareRevision() const
            {
                return (_softwareRevision);
            }
            inline const string& ManufacturerName() const
            {
                return (_manufacturerName);
            }
            inline Exchange::IVoiceProducer::IProfile* SelectedProfile() const 
            {
                _audioProfile->AddRef();
                return (_audioProfile);
            }
            void Reconfigure(const string& settings) {
                Config::Profile config; 
                config.FromString(settings);
                
                if (_decoder != nullptr) {
                    delete _decoder;
                }
                _audioProfile->Release();

                _decoder = Decoders::IDecoder::Instance(config.Codec.Value(), config.Configuration.Value());

                if (_decoder == nullptr) {
                    _decoder = nullptr;
                }
                else {
                    _audioProfile = Core::Service<AudioProfile>::Create<AudioProfile>(
                        config.Codec.Value(),
                        config.Channels.Value(),
                        config.SampleRate.Value(),
                        config.Resolution.Value());
                }
            }

        private:
            void Notification(const uint16_t handle, const uint8_t dataFrame[], const uint16_t length) override
            {
                // Decouple the notfications from the communciator thread, they will pop-up and need to be handled
                // by the Message method!
                _decoupling.Submit(handle, static_cast<uint8_t>(length), dataFrame);
                PluginHost::WorkerPool::Instance().Submit(Core::ProxyType<Core::IDispatch>(_decoupling));
            }
            void Message(const uint16_t handle, const uint8_t length, const uint8_t buffer[])
            {
                _adminLock.Lock();

                if ( (handle == _voiceDataHandle) && (_decoder != nullptr) ) {
                    uint8_t decoded[1024];
                    uint16_t sendLength = _decoder->Decode(length, buffer, sizeof(decoded), decoded);
                    if (sendLength > 0) {
                        ASSERT (sendLength <= sizeof(decoded));
                        if (_startFrame == true) {
                            _startFrame = false;
                            _parent->VoiceData(_audioProfile);
                        }
                        _parent->VoiceData(_decoder->Frames(), sendLength, decoded);
                    }
                }
                else if ( (handle == _keysDataHandle) && (length >= 2) ) {
                    uint16_t scancode = ((buffer[1] << 8) | buffer[0]);
                    bool pressed = (scancode != 0);

                    if (pressed == true) {
                        _currentKey = scancode;
                    }

                    _parent->KeyEvent(pressed, _currentKey);
                }
                else if ( (handle == _voiceCommandHandle) && (_decoder != nullptr) ) {

                    // If we start, reset.
                    if (buffer[0] == 0) {
                        // We are done, signal that the button to speak has been released!
                        _parent->VoiceData(nullptr);
                    }
                    else {
                        // Looks like the TPress-to-talk button is pressed...
                        _decoder->Reset();
                        _startFrame = true;
                    }
                }
                else if ( (handle == _batteryLevelHandle) && (length >= 1) ) {
                    _parent->BatteryLevel(buffer[0]);
                }

                _adminLock.Unlock();
            }
            void Constructor(const Config& config) 
            {
                ASSERT(_parent != nullptr);
                ASSERT(_device != nullptr);

                _device->AddRef();
                _decoupling.AddRef();
                _name = _device->Name();
                _address = _device->RemoteId();

                if (_device->Callback(&_sink) != Core::ERROR_NONE) {
                    TRACE(Trace::Fatal, (_T("The device is already in use. Only 1 callback allowed")));
                }

                _decoder = Decoders::IDecoder::Instance(config.AudioProfile.Codec.Value(), config.AudioProfile.Configuration.Value());

                if (_decoder != nullptr) { 
                    _audioProfile = Core::Service<AudioProfile>::Create<AudioProfile>(
                        config.AudioProfile.Codec.Value(),
                        config.AudioProfile.Channels.Value(),
                        config.AudioProfile.SampleRate.Value(),
                        config.AudioProfile.Resolution.Value());
                }

                TRACE(Flow, (_T("The HoG device is ready for operation")));
            }
            bool Initialize() override
            {
                return (Security(BT_SECURITY_LOW));
            }
            void Operational() override
            {
                if (_profile == nullptr) {
                    TRACE(Flow, (_T("The received MTU: %d, no need for discovery, we know it all"), MTU()));

                    // No need to do service discovery if device knows the Handles to use. If so, DeviceDiscovery has 
                    // already been done and the only thing we need to do is get the startingvalues :-)
                    ReadSoftwareRevision();
                }
                else {
                    TRACE(Flow, (_T("The received MTU: %d, we have no clue yet, start discovery"), MTU()));

                    _profile->Discover(CommunicationTimeOut * 20, *this, [&](const uint32_t result) {
                        if (result == Core::ERROR_NONE) {

                            DumpProfile();

                            if ((*_profile)[Bluetooth::UUID(HID_UUID)] == nullptr) {
                                TRACE(Flow, (_T("The given bluetooth device does not support a HID service!!")));
                            }
                            else {
                                TRACE(Flow, (_T("Reading the remaining information")));
                                ReadModelNumber();
                            }
                        }
                        else {
                            TRACE(Flow, (_T("The given bluetooth device could not be read for services!!")));
                        }
                    });
                }
            }
            void ReadModelNumber()
            {
                ASSERT (_profile != nullptr);

                uint16_t modelNumberHandle = _profile->FindHandle(Bluetooth::Profile::Service::DeviceInformation, Bluetooth::Profile::Service::Characteristic::ModelNumberString);
                if (modelNumberHandle != 0) {
                    _command.Read(modelNumberHandle);
                    Execute(CommunicationTimeOut, _command, [&](const GATTSocket::Command& cmd) {
                        if ((cmd.Error() == Core::ERROR_NONE) && (cmd.Result().Error() == 0) && (cmd.Result().Length() >= 1)) {
                            _modelNumber = string(reinterpret_cast<const char*>(cmd.Result().Data()), cmd.Result().Length());
                        } else {
                            TRACE(Flow, (_T("Failed to read model name")));
                        }

                        ReadSerialNumber();
                    });
                } else {
                    TRACE(Flow, (_T("ModelNumberString characteristic not available")));
                    ReadSerialNumber();
                }
            }
            void ReadSerialNumber()
            {
                ASSERT (_profile != nullptr);

                uint16_t serialNumberHandle = _profile->FindHandle(Bluetooth::Profile::Service::DeviceInformation, Bluetooth::Profile::Service::Characteristic::SerialNumberString);
                if (serialNumberHandle != 0) {
                    _command.Read(serialNumberHandle);
                    Execute(CommunicationTimeOut, _command, [&](const GATTSocket::Command& cmd) {
                        if ((cmd.Error() == Core::ERROR_NONE) && (cmd.Result().Error() == 0) && (cmd.Result().Length() >= 1)) {
                            Core::ToHexString(cmd.Result().Data(), cmd.Result().Length(), _serialNumber);
                        } else {
                            TRACE(Flow, (_T("Failed to read serial number")));
                        }

                        ReadFirmwareRevision();
                    });
                } else {
                    TRACE(Flow, (_T("SerialNumberString characteristic not available")));
                    ReadFirmwareRevision();
                }
            }
            void ReadFirmwareRevision()
            {
                ASSERT (_profile != nullptr);

                uint16_t firmwareRevisionHandle = _profile->FindHandle(Bluetooth::Profile::Service::DeviceInformation, Bluetooth::Profile::Service::Characteristic::FirmwareRevisionString);
                if (firmwareRevisionHandle != 0) {
                    _command.Read(firmwareRevisionHandle);
                    Execute(CommunicationTimeOut, _command, [&](const GATTSocket::Command& cmd) {
                        if ((cmd.Error() == Core::ERROR_NONE) && (cmd.Result().Error() == 0) && (cmd.Result().Length() >= 1)) {
                            _firmwareRevision = string(reinterpret_cast<const char*>(cmd.Result().Data()), cmd.Result().Length());
                        } else {
                            TRACE(Flow, (_T("Failed to read firmware revision")));
                        }

                        ReadManufacturerName();
                    });
                } else {
                    TRACE(Flow, (_T("FirmwareRevisionString characteristic not available")));
                    ReadManufacturerName();
                }
            }
            void ReadManufacturerName()
            {
                ASSERT (_profile != nullptr);

                uint16_t manufacturerNameHandle = _profile->FindHandle(Bluetooth::Profile::Service::DeviceInformation, Bluetooth::Profile::Service::Characteristic::ManufacturerNameString);
                if (manufacturerNameHandle != 0) {
                    _command.Read(manufacturerNameHandle);
                    Execute(CommunicationTimeOut, _command, [&](const GATTSocket::Command& cmd) {
                        if ((cmd.Error() == Core::ERROR_NONE) && (cmd.Result().Error() == 0) && (cmd.Result().Length() >= 1)) {
                            _manufacturerName = string(reinterpret_cast<const char*>(cmd.Result().Data()), cmd.Result().Length());
                        } else {
                            TRACE(Flow, (_T("Failed to read manufacturer name")));
                        }
                        EnableEvents();
                    });
                } else {
                    TRACE(Flow, (_T("ManufacturerNameString characteristic not available")));
                    EnableEvents();
                }
            }
            void EnableEvents() {

                uint16_t notificationHandle;
                ASSERT (_profile != nullptr);

                // Set all interesting handles...
                if (_keysDataHandle == static_cast<uint16_t>(~0)) {
                    notificationHandle = _profile->FindHandle(Bluetooth::Profile::Service::HumanInterfaceDevice, Bluetooth::Profile::Service::Characteristic::Report, Bluetooth::Profile::Service::Characteristic::Descriptor::ClientCharacteristicConfiguration);
                    _keysDataHandle = _profile->FindHandle(Bluetooth::Profile::Service::HumanInterfaceDevice, Bluetooth::Profile::Service::Characteristic::Report);
                    EnableEvents(_T("Key handling"), notificationHandle);
                }
                else if (_batteryLevelHandle == static_cast<uint16_t>(~0)) {
                    notificationHandle = _profile->FindHandle(Bluetooth::Profile::Service::BatteryService, Bluetooth::Profile::Service::Characteristic::BatteryLevel, Bluetooth::Profile::Service::Characteristic::Descriptor::ClientCharacteristicConfiguration);
                    _batteryLevelHandle = _profile->FindHandle(Bluetooth::Profile::Service::BatteryService, Bluetooth::Profile::Service::Characteristic::BatteryLevel);
                    EnableEvents(_T("Battery handling"), notificationHandle);
                }
                else if (_voiceCommandHandle == static_cast<uint16_t>(~0)) {
                    notificationHandle = _profile->FindHandle(_profile->VoiceService, _profile->VoiceCommandChar, Bluetooth::Profile::Service::Characteristic::Characteristic::Descriptor::ClientCharacteristicConfiguration);
                    _voiceCommandHandle = _profile->FindHandle(_profile->VoiceService, _profile->VoiceCommandChar);
                    EnableEvents(_T("Voice Command handling"), notificationHandle);
                }
                else if (_voiceDataHandle == static_cast<uint16_t>(~0)) {
                    notificationHandle = _profile->FindHandle(_profile->VoiceService, _profile->VoiceDataChar, Bluetooth::Profile::Service::Characteristic::Descriptor::ClientCharacteristicConfiguration);
                    _voiceDataHandle = _profile->FindHandle(_profile->VoiceService, _profile->VoiceDataChar);
                    EnableEvents(_T("Voice Data handling"), notificationHandle);
                }
                else {
                    _softwareRevisionHandle = _profile->FindHandle(Bluetooth::Profile::Service::DeviceInformation, Bluetooth::Profile::Service::Characteristic::SoftwareRevisionString);

                    // We are done with the profile, all is read, all is set, we hope :-) Time
                    // to register what we did.
                    ReadSoftwareRevision();
                }
            }
            void EnableEvents(const TCHAR* message, const uint16_t handle)
            {
                if (handle == 0) {
                    TRACE(Flow, (_T("There is no notification for X [%d]"), handle, _command.Result().Error()));
                    EnableEvents();
                }
                else {
                    uint16_t val = htobs(1);
                    _command.Write(handle, sizeof(val), reinterpret_cast<const uint8_t*>(&val));
                    Execute(CommunicationTimeOut, _command, [&](const GATTSocket::Command& cmd) {
                        if ((_command.Error() != Core::ERROR_NONE) || (_command.Result().Error() != 0) ) {
                            TRACE(Flow, (_T("Failed to enable event on handle 0x%04X, error: %d"), cmd.Result().Handle(), _command.Result().Error()));
                        }
                        EnableEvents();
                   });
                }
            }
            void ReadSoftwareRevision()
            {
                if (_softwareRevisionHandle != 0) {
                    _command.Read(_softwareRevisionHandle);
                    Execute(CommunicationTimeOut, _command, [&](const GATTSocket::Command& cmd) {
                        if ((cmd.Error() == Core::ERROR_NONE) && (cmd.Result().Error() == 0) && (cmd.Result().Length() >= 1)) {
                            _softwareRevision = string(reinterpret_cast<const char*>(cmd.Result().Data()), cmd.Result().Length());
                        } else {
                            TRACE(Flow, (_T("Failed to read software revision")));
                        }

                        ReadBatteryLevel();
                    });
                } else {
                    TRACE(Flow, (_T("SoftwareRevisionString characteristic not available")));
                    ReadBatteryLevel();
                }
            }
            void ReadBatteryLevel()
            {
                if (_batteryLevelHandle != 0) {
                    _command.Read(_batteryLevelHandle);
                    Execute(CommunicationTimeOut, _command, [&](const GATTSocket::Command& cmd) {
                        if ((cmd.Error() == Core::ERROR_NONE) && (cmd.Result().Error() == 0) && (cmd.Result().Length() >= 1)) {
                            _parent->BatteryLevel(cmd.Result().Data()[0]);
                        } else {
                            TRACE(Flow, (_T("Failed to retrieve battery level")));
                        }
                        Completed();
                   });
                } else {
                    TRACE(Flow, (_T("BatteryLevel characteristic not available")));
                    Completed();
                }
            }
            void Completed() 
            {
                Data info;

                info.ModelNumber = _modelNumber;
                info.SerialNumber = _serialNumber;
                info.FirmwareRevision = _firmwareRevision;
                info.SoftwareRevision = _softwareRevision;
                info.ManufacturerName = _manufacturerName;
                info.SoftwareRevisionHandle = _softwareRevisionHandle;
                info.KeysDataHandle = _keysDataHandle;
                info.BatteryLevelHandle = _batteryLevelHandle;
                info.VoiceCommandHandle = _voiceCommandHandle;
                info.VoiceDataHandle = _voiceDataHandle;
 
                TRACE(Flow, (_T("All required information gathered. Report it to the parent")));
                _parent->Operational(info);
            }
            void Updated()
            {
                _adminLock.Lock();
                if (_device != nullptr) {
                    if ((_device->IsConnected() == true) && (IsOpen() == false)) {
                        TRACE(Trace::Information, (_T("Connecting GATT socket [%s]"), _device->RemoteId().c_str()));
                        uint32_t result = GATTSocket::Open(5000);
                        if (result != Core::ERROR_NONE) {
                            TRACE(Trace::Error, (_T("Failed to open GATT socket [%s]"), _device->RemoteId().c_str()));
                        }
                    } else if (_device->IsValid() == true) {
                        if (IsOpen() == true) {
                            TRACE(Trace::Information, (_T("Disconnecting GATT socket [%s]"), _device->RemoteId().c_str()));
                            uint32_t result = GATTSocket::Close(Core::infinite);
                            if (result != Core::ERROR_NONE) {
                                TRACE(Trace::Error, (_T("Failed to close GATT socket [%s]"), _device->RemoteId().c_str()));
                            }
                        }
                    } else {
                        TRACE(Flow, (_T("Releasing device")));
                        _device->Release();
                        _device = nullptr;
                    }
                }
                _adminLock.Unlock();
            }
            void DumpProfile() const
            {
                Bluetooth::Profile::Iterator serviceIdx = _profile->Services();
                while (serviceIdx.Next() == true) {
                    const Bluetooth::Profile::Service& service = serviceIdx.Current();
                    TRACE(Flow, (_T("[0x%04X] Service: [0x%04X]         %s"), service.Handle(), service.Max(), service.Name().c_str()));

                    Bluetooth::Profile::Service::Iterator characteristicIdx = service.Characteristics();
                    while (characteristicIdx.Next() == true) {
                        const Bluetooth::Profile::Service::Characteristic& characteristic(characteristicIdx.Current());
                        TRACE(Flow, (_T("[0x%04X]    Characteristic [0x%02X]: %s [%d]"), characteristic.Handle(), characteristic.Rights(), characteristic.Name().c_str(), characteristic.Error()));

                        Bluetooth::Profile::Service::Characteristic::Iterator descriptorIdx = characteristic.Descriptors();
                        while (descriptorIdx.Next() == true) {
                            const Bluetooth::Profile::Service::Characteristic::Descriptor& descriptor(descriptorIdx.Current());
                            TRACE(Flow, (_T("[0x%04X]       Descriptor:         %s"), descriptor.Handle(), descriptor.Name().c_str()));
                        }
                    }
                }
            }

        private:
            BluetoothRemoteControl* _parent;
            Core::CriticalSection _adminLock;
            Profile* _profile;
            GATTSocket::Command _command;
            Exchange::IBluetooth::IDevice* _device;
            Core::ProxyObject<Decoupling> _decoupling;
            Core::Sink<Sink> _sink;

            // This is the Name, as depicted by the Bluetooth device (generic IF) 
            string _name;
            string _address;

            // The next section of information is coming from the config that
            // was stored for this remote or it is read from the profile if
            // this is the first reistration
            string _modelNumber;
            string _serialNumber;
            string _manufacturerName;
            string _firmwareRevision;

            // If we are already registered, no need to read the profile as all
            // is coming from the RemoteControl persistent storage file.
            // The only thing that might have changed, and what should be read on 
            // each new creation is the software revision, that might have been 
            // upgraded using Android phone upgrades..
            string _softwareRevision;

            // During the intial cretaion/subscribe of this RemoteControl, the
            // handles that report keys, battery level and Voice (if applicable)
            // are registsered. Remember these to trigger the right hadler if
            // new data comes in.
            uint16_t _softwareRevisionHandle;
            uint16_t _keysDataHandle;
            uint16_t _batteryLevelHandle;
            uint16_t _voiceDataHandle;
            uint16_t _voiceCommandHandle;

            // What profile will this system be working with ???
            AudioProfile* _audioProfile;
            Decoders::IDecoder* _decoder;
            bool _startFrame;
            uint16_t _currentKey;
        };

    public:
        BluetoothRemoteControl(const BluetoothRemoteControl&) = delete;
        BluetoothRemoteControl& operator= (const BluetoothRemoteControl&) = delete;
        BluetoothRemoteControl() 
            : _skipURL()
            , _adminLock()
            , _service()
            , _gattRemote(nullptr)
            , _name(_T("NOT_AVAIALABLE"))
            , _controller()
            , _keyMap()
            , _configLine()
            , _recordFile()
            , _batteryLevel(~0)
            , _voiceHandler(nullptr)
            , _inputHandler(nullptr) 
            , _record(recorder::OFF)
            , _recorder()
        {
            RegisterAll();
        }
        ~BluetoothRemoteControl() override
        {
            UnregisterAll();
        }

    public:
        inline uint8_t BatteryLevel() const {
            return (_batteryLevel);
        }
        string Name() const override {
            return (_name);
        }
        uint32_t Error() const override
        {
            return (_gattRemote != nullptr ? Core::ERROR_NONE : Core::ERROR_UNAVAILABLE);
        }
        string MetaData() const override
        {
            string result;

            _adminLock.Lock();

            if (_gattRemote != nullptr) {
                GATTRemote::Data info;

                info.Name = _gattRemote->Name();
                info.Address = _gattRemote->Address();
                info.ModelNumber = _gattRemote->ModelNumber();
                info.SerialNumber = _gattRemote->SerialNumber();
                info.FirmwareRevision = _gattRemote->FirmwareRevision();
                info.SoftwareRevision = _gattRemote->SoftwareRevision();
                info.ManufacturerName = _gattRemote->ManufacturerName();
                info.BatteryLevelHandle = _batteryLevel;
 
                info.ToString(result);
            }

            _adminLock.Unlock();

            return (result);
        }
        void Configure(const string& settings) override
        {
            _adminLock.Lock();

            if (_gattRemote != nullptr) {
                _gattRemote->Reconfigure(settings);
            }

            _adminLock.Unlock();
        }
        uint32_t Callback(Exchange::IVoiceHandler* callback) override 
        {
            _adminLock.Lock();
            if (_voiceHandler != nullptr) {
                _voiceHandler->Release();
            }
            if (callback != nullptr) {
                callback->AddRef();
            }
            _voiceHandler = callback;
            _adminLock.Unlock();

            return (Core::ERROR_NONE);
        }

        //   IPlugin methods
        // -------------------------------------------------------------------------------------------------------
        virtual const string Initialize(PluginHost::IShell* service) override;
        virtual void Deinitialize(PluginHost::IShell* service) override;
        virtual string Information() const override;

        //   IWeb methods
        // -------------------------------------------------------------------------------------------------------
        virtual void Inbound(Web::Request& request) override;
        virtual Core::ProxyType<Web::Response> Process(const Web::Request& request) override;
 
        BEGIN_INTERFACE_MAP(BluetoothRemoteControl)
            INTERFACE_ENTRY(PluginHost::IPlugin)
            INTERFACE_ENTRY(PluginHost::IWeb)
            INTERFACE_ENTRY(PluginHost::IDispatcher)
            INTERFACE_ENTRY(Exchange::IVoiceProducer)
        END_INTERFACE_MAP

    private:
        // Bluetooth Remote Control implementation
        uint32_t Assign(const string& address);
        uint32_t Revoke();
        void Operational(const GATTRemote::Data& settings);
        void VoiceData(Exchange::IVoiceProducer::IProfile* profile);
        void VoiceData(const uint32_t seq, const uint16_t length, const uint8_t dataBuffer[]);
        void KeyEvent(const bool pressed, const uint16_t keyCode);
        void BatteryLevel(const uint8_t level);
        
        Core::ProxyType<Web::Response> GetMethod(Core::TextSegmentIterator& index);
        Core::ProxyType<Web::Response> PutMethod(Core::TextSegmentIterator& index, const Web::Request& request);
        Core::ProxyType<Web::Response> PostMethod(Core::TextSegmentIterator& index, const Web::Request& request);
        Core::ProxyType<Web::Response> DeleteMethod(Core::TextSegmentIterator& index, const Web::Request& request);

        // JSON-RPC
        void RegisterAll();
        void UnregisterAll();
        uint32_t endpoint_assign(const JsonData::BluetoothRemoteControl::AssignParamsData& params);
        uint32_t endpoint_revoke();
        uint32_t get_name(Core::JSON::String& response) const;
        uint32_t get_address(Core::JSON::String& response) const;
        uint32_t get_info(JsonData::BluetoothRemoteControl::InfoData& response) const;
        uint32_t get_batterylevel(Core::JSON::DecUInt8& response) const;
        uint32_t get_audioprofiles(Core::JSON::ArrayType<Core::JSON::String>& response) const;
        uint32_t get_audioprofile(const string& index, JsonData::BluetoothRemoteControl::AudioprofileData& response) const;
        void event_audiotransmission(const string& profile = "");
        void event_audioframe(const uint32_t& seq, const string& data);
        void event_batterylevelchange(const uint8_t& level);

    private:
        uint8_t _skipURL;
        mutable Core::CriticalSection _adminLock;
        PluginHost::IShell* _service;
        GATTRemote* _gattRemote;
        string _name;
        string _controller;
        string _keyMap;
        string _configLine;
        string _recordFile;
        uint8_t _batteryLevel;
        Exchange::IVoiceHandler* _voiceHandler;
        PluginHost::VirtualInput* _inputHandler;
        recorder _record;
        WAV::Recorder _recorder;

    }; // class BluetoothRemoteControl

} //namespace Plugin

}
