#include "RemoteInvocation.h"
#include "plugins/IShell.h"
#include "interfaces/IRemoteLinker.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(RemoteInvocation, 1, 0);

    static std::map<uint32_t, Core::IUnknown*> connectionMap;

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

    uint32_t RemoteInvocation::Invocator::LinkByCallsign(const uint16_t port, const uint32_t interfaceId, const uint32_t exchangeId, const string& callsign) 
    {
        uint32_t result = Core::ERROR_NONE;
        printf("## %s\n", _remoteId.c_str());
        string remoteAddress = Core::NodeId(_remoteId.c_str()).HostAddress() + ":" + std::to_string(port);

        auto plugin = reinterpret_cast<Core::IUnknown*>(_service->QueryInterfaceByCallsign(Exchange::IRemoteLinker::ID, callsign));

        if (plugin != nullptr) {
            auto linker = plugin->QueryInterface<Exchange::IRemoteLinker>();

            if (linker != nullptr) {
                result = linker->Connect(remoteAddress, interfaceId, exchangeId);
                linker->Release();

                connectionMap.insert(std::pair<uint32_t, Core::IUnknown*>(exchangeId, plugin));
                plugin->AddRef();
            } else {
                result = Core::ERROR_ILLEGAL_STATE;
            }

            plugin->Release();
        } else {
            result = Core::ERROR_UNAVAILABLE;
        }

        return result;
    }

    uint32_t RemoteInvocation::Invocator::Unlink(const uint32_t exchangeId) 
    {
        uint32_t result = Core::ERROR_NONE;
        auto plugin = connectionMap.find(exchangeId);

        if (plugin != connectionMap.end()) {
            auto linker = plugin->second->QueryInterface<Exchange::IRemoteLinker>();

            if (linker != nullptr) {
                result = linker->Disconnect();

                if (result == Core::ERROR_NONE) {
                    plugin->second->Release();
                    connectionMap.erase(exchangeId);
                }
            } else {
                result = Core::ERROR_ILLEGAL_STATE;
            }
        }
    }

    uint32_t RemoteInvocation::Invocator::Instantiate(const uint16_t port, const Exchange::IRemoteInvocation::ProgramParams& params)
    {
        uint32_t result = Core::ERROR_NONE;
/*
        PluginHost::IShell::ICOMLink* handler(_service->COMLink());
        uint32_t _cid;

        string remoteAddress = Core::NodeId(_remoteId.c_str()).HostAddress() + ":" + std::to_string(port);

        RPC::Object definition(params.callsign, params.locator,
            params.className,
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
            auto remotePlugin = plugin->QueryInterface<Exchange::IRemoteLinker>();
            
            if (remotePlugin != nullptr) {
                
                if(remotePlugin->Connect(remoteAddress, params.interface, params.id) != Core::ERROR_NONE) {
                    TRACE_L1("Error occurred while trying to connect %s to %s", params.className.c_str(), remoteAddress.c_str());
                }
            } else {
                result = Core::ERROR_BAD_REQUEST;
                TRACE_L1("Failed to get IRemoteLinker interface from %s plugin. It is required for remote invocation!", params.className.c_str());
            }

            plugin->Release();
        }
*/
        return result;
    }

    uint32_t RemoteInvocation::Invocator::Terminate(uint32_t connectionId) {
        /*auto foundPid = connectionMap.find(connectionId);

        if (foundPid != connectionMap.end()) {
            Core::Process process(foundPid->second);

            // TODO: Add trying gently first
            process.Kill(true);
            process.WaitProcessCompleted(Core::infinite);
            connectionMap.erase(foundPid);
        }
*/
        return Core::ERROR_NONE;
    }
}
}