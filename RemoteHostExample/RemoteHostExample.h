#pragma once

#include "Module.h"
#include "interfaces/IRemoteHostExample.h"

namespace WPEFramework {
namespace Plugin {

    class RemoteHostExample : public PluginHost::IPlugin {
    public:
        RemoteHostExample(const RemoteHostExample&) = delete;
        RemoteHostExample& operator=(const RemoteHostExample&) = delete;

        RemoteHostExample()
        {
        }

        virtual ~RemoteHostExample()
        {
        }

        BEGIN_INTERFACE_MAP(RemoteHostExample)
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