#pragma once

#include "Module.h"
#include "interfaces/IRemoteHostExample.h"
#include "interfaces/IRemoteLinker.h"

namespace WPEFramework {
namespace Plugin {

    class RemoteHostExample : public PluginHost::IPlugin {
    public:
        class Config : public Core::JSON::Container {
        private:
            Config(const Config&);
            Config& operator=(const Config&);

        public:
            Config()
                : Core::JSON::Container()
                , SlaveAddress()
                , Name()
            {
                Add(_T("slaveAddress"), &SlaveAddress);
                Add(_T("name"), &Name);
            }

            ~Config()
            {
            }

        public:
            Core::JSON::String SlaveAddress;
            Core::JSON::String Name;
        };

        RemoteHostExample(const RemoteHostExample&) = delete;
        RemoteHostExample& operator=(const RemoteHostExample&) = delete;

        RemoteHostExample()
        {
        }

        virtual ~RemoteHostExample()
        {
        }

        BEGIN_INTERFACE_MAP(RemoteHostExample)
            INTERFACE_AGGREGATE(Exchange::IRemoteHostExample, _implementation)
            INTERFACE_AGGREGATE(Exchange::IRemoteLinker, _implementation)
            INTERFACE_ENTRY(PluginHost::IPlugin)
        END_INTERFACE_MAP

    public:
        //   IPlugin methods
        // -------------------------------------------------------------------------------------------------------
        virtual const string Initialize(PluginHost::IShell* service) override;
        virtual void Deinitialize(PluginHost::IShell* service) override;
        virtual string Information() const override;

    private:
        Exchange::IRemoteHostExample* _implementation;
    };

} // namespace Plugin
} // namespace WPEFramework