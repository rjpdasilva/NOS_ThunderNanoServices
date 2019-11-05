#include "MinimalPlugin.h"
#include "interfaces/IRemoteInvocation.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(MinimalPlugin, 1, 0);

    const string MinimalPlugin::Initialize(PluginHost::IShell* service) 
    {
        string result;

        uint32_t connectionId;

        auto _implementation = service->Root<Exchange::IMinimalPlugin>(connectionId, 2000, "MinimalPluginImpl", ~0, "127.0.0.1:5797");

        if (_implementation != nullptr) {
            string result;
            if (_implementation->Greet("Patrick", result) != Core::ERROR_NONE) {
                printf("COnnection failed!\n");
            }
            TRACE_L1("Message to you: %s", result.c_str());
        } else {
            TRACE_L1("Failed to start MinimalPlugin implementaiton");
        }

        _implementation->Release();

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