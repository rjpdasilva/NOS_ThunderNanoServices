#include "RemoteInvocation.h"
#include "plugins/IShell.h"
#include "interfaces/IRemoteHost.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(RemoteInvocation, 1, 0);

    static std::map<uint32_t, uint32_t> connectionPidMap;

    const string RemoteInvocation::Initialize(PluginHost::IShell* service) 
    {
        Config config;
        config.FromString(service->ConfigLine());

        Core::NodeId serverAddress(config.Address.Value().c_str());

        Core::ProxyType<RPC::InvokeServer> server = Core::ProxyType<RPC::InvokeServer>::Create(&Core::WorkerPool::Instance());
        _extService = new ExternalAccess(serverAddress, service, server);

        return (string());
    }

    void RemoteInvocation::Deinitialize(PluginHost::IShell* service) 
    {   
        if (_extService != nullptr) {
            delete _extService;
            _extService = nullptr;
        }
    }

    string RemoteInvocation::Information() const 
    {
        return (string());
    }

    uint32_t RemoteInvocation::Invocator::Instantiate(const uint16_t port, const Exchange::IRemoteInvocation::ProgramParams& params)
    {
        uint32_t result = Core::ERROR_NONE;

        PluginHost::IShell::ICOMLink* handler(_service->COMLink());
        uint32_t _cid;

        string remoteAddress = Core::NodeId(_remoteId.c_str()).HostAddress() + ":" + std::to_string(port);

        RPC::Object definition(params.callsign, params.locator,
            params.className,
            params.id,
            params.interface,
            params.version,
            "",
            "",
            params.threads,
            RPC::Object::HostType::LOCAL, 
            remoteAddress,
            "");

        auto plugin = reinterpret_cast<Core::IUnknown*>(handler->Instantiate(definition, Core::infinite, _cid, params.className, params.callsign));

        if (plugin != nullptr) {
            auto remotePlugin = plugin->QueryInterface<Exchange::IRemoteHost>();
            
            if (remotePlugin != nullptr) {
                if (remotePlugin->SetLocalShell(_service) != Core::ERROR_NONE) {
                    TRACE_L1("Error occurred while trying to set local shell to %s", params.className.c_str());
                } 
                else if(remotePlugin->Connect(remoteAddress, params.interface, params.id) != Core::ERROR_NONE) {
                    TRACE_L1("Error occurred while trying to connect %s to %s", params.className.c_str(), remoteAddress.c_str());
                }
            } else {
                result = Core::ERROR_BAD_REQUEST;
                TRACE_L1("Failed to get IremoteHost interface from %s plugin. It is required for remote invocation!", params.className.c_str());
            }

            plugin->Release();
        }
        
        return result;
    }

    uint32_t RemoteInvocation::Invocator::Terminate(uint32_t connectionId) {
        auto foundPid = connectionPidMap.find(connectionId);

        if (foundPid != connectionPidMap.end()) {
            Core::Process process(foundPid->second);

            // TODO: Add trying gently first
            process.Kill(true);
            process.WaitProcessCompleted(Core::infinite);
            connectionPidMap.erase(foundPid);
        }

        return Core::ERROR_NONE;
    }
}
}