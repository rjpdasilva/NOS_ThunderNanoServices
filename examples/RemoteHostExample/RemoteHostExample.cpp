#include "RemoteHostExample.h"
#include "interfaces/IRemoteInvocation.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(RemoteHostExample, 1, 0);

    const string RemoteHostExample::Initialize(PluginHost::IShell* service) 
    {
        string result;

        Config config;
        config.FromString(service->ConfigLine());

        string name = config.Name.Value();

        // If SlaveAddress will not be set, plugin will be started localy
        // otherwise, plugin started on slave machine will be used
        string remoteTarget = config.SlaveAddress.Value();
        uint32_t connectionId;

        _implementation = service->Root<Exchange::IRemoteHostExample>(connectionId, Core::infinite, "RemoteHostExampleImpl", ~0, remoteTarget);

        if (remoteTarget.empty() == false) {
            string response;
            _implementation->Greet(name, response);

            printf("## RESPONSE: %s\n", response.c_str());
        } else {
            _implementation->SetName(name);
        }

        return result;
    }

    void RemoteHostExample::Deinitialize(PluginHost::IShell* service) 
    {
        if (_implementation != nullptr) {
            _implementation->Release();
            _implementation = nullptr;
        }
    }

    string RemoteHostExample::Information() const 
    {
        return (string());
    }
}
}