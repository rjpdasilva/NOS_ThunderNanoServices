#pragma once

#include "Module.h"
#include "interfaces/IMinimalPlugin.h"

namespace WPEFramework {
namespace Plugin {

    class MinimalPlugin : public PluginHost::IPlugin {
    public:
        MinimalPlugin(const MinimalPlugin&) = delete;
        MinimalPlugin& operator=(const MinimalPlugin&) = delete;

        MinimalPlugin()
        {
        }

        virtual ~MinimalPlugin()
        {
        }

        BEGIN_INTERFACE_MAP(MinimalPlugin)
            INTERFACE_ENTRY(PluginHost::IPlugin)
        END_INTERFACE_MAP

    public:
        //   IPlugin methods
        // -------------------------------------------------------------------------------------------------------
        virtual const string Initialize(PluginHost::IShell* service) override;
        virtual void Deinitialize(PluginHost::IShell* service) override;
        virtual string Information() const override;

    private:
        Exchange::IMinimalPlugin* _implementation;
    };

} // namespace Plugin
} // namespace WPEFramework