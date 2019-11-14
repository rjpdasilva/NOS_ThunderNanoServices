#include "MinimalPluign.h"
#include "interfaces/IRemoteInvocation.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(MinimalPluign, 1, 0);

    const string MinimalPluign::Initialize(PluginHost::IShell* service) 
    {
        string result;

        uint32_t connectionId;

        auto _implementation = service->Root<Exchange::IMinimalPluign>(connectionId, Core::infinite, "MinimalPluignImpl", ~0);

        return result;
    }

    void MinimalPluign::Deinitialize(PluginHost::IShell* service) 
    {
        if (_implementation != nullptr) {
            _implementation->Release();
            _implementation = nullptr;
        }
    }

    string MinimalPluign::Information() const 
    {
        return (string());
    }
}
}