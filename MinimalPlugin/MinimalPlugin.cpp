#include "MinimalPlugin.h"
#include "interfaces/IRemoteInvocation.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(MinimalPlugin, 1, 0);

    const string MinimalPlugin::Initialize(PluginHost::IShell* service) 
    {
        string result;

        uint32_t pid;
        Exchange::IMinimalPlugin* _implementation = (WPEFramework::Exchange::IMinimalPlugin*)service->Root("127.0.0.1:7889", pid, Core::infinite, "MinimalPluginImpl", Exchange::IMinimalPlugin::ID, ~0);

        if (_implementation != nullptr) {
            string result = _implementation->Greet("Patric");
            TRACE_L1("Message to you: %s", result.c_str());
        } else {
            TRACE_L1("Failed to start MinimalPlugin implementaiton");
        }

        return result;
    }

    void MinimalPlugin::Deinitialize(PluginHost::IShell* service) 
    {
        if (_implementation != nullptr) {
            _implementation->Release();
            _implementation = nullptr;
        }
    }

    string MinimalPlugin::Information() const 
    {
        return (string());
    }
}
}