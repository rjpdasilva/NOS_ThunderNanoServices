#pragma once

#include "Module.h"
#include "interfaces/IMinimalPluign.h"

namespace WPEFramework {
namespace Plugin {

    class MinimalPluign : public PluginHost::IPlugin {
    public:
        MinimalPluign(const MinimalPluign&) = delete;
        MinimalPluign& operator=(const MinimalPluign&) = delete;

        MinimalPluign()
        {
        }

        virtual ~MinimalPluign()
        {
        }

        BEGIN_INTERFACE_MAP(MinimalPluign)
            INTERFACE_ENTRY(PluginHost::IPlugin)
        END_INTERFACE_MAP

    public:
        //   IPlugin methods
        // -------------------------------------------------------------------------------------------------------
        virtual const string Initialize(PluginHost::IShell* service) override;
        virtual void Deinitialize(PluginHost::IShell* service) override;
        virtual string Information() const override;

    private:
        Exchange::IMinimalPluign* _implementation;
    };

} // namespace Plugin
} // namespace WPEFramework