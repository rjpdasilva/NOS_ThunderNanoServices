#include "RemoteInvocation.h"
#include "plugins/IShell.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(RemoteInvocationPlugin, 1, 0);

    static std::map<uint32_t, uint32_t> connectionPidMap;

    const string RemoteInvocationPlugin::Initialize(PluginHost::IShell* service) 
    {
        Config config;
        config.FromString(service->ConfigLine());

        Core::NodeId serverAddress(config.Address.Value().c_str());

        Core::ProxyType<RPC::InvokeServer> server = Core::ProxyType<RPC::InvokeServer>::Create(&Core::WorkerPool::Instance());
        _extService = new ExternalAccess(serverAddress, service, server);

        return (string());
    }

    void RemoteInvocationPlugin::Deinitialize(PluginHost::IShell* service) 
    {   
        if (_extService != nullptr) {
            delete _extService;
            _extService = nullptr;
        }
    }

    string RemoteInvocationPlugin::Information() const 
    {
        return (string());
    }

    uint32_t RemoteInvocationPlugin::RemoteInvocation::Instantiate(const uint16_t port, const Exchange::IRemoteInvocation::ProgramParams& params)
    { 
        uint32_t result = Core::ERROR_GENERAL;

        Core::Process::Options options("WPEProcess");
        // Very ugly hack :(
        auto switchCallsing = [](const string& serv, string callsign) {return serv.substr(0, serv.find_last_of('/', serv.length() - 2)) + "/" + callsign + "/";};
        string persistentPath = switchCallsing(_service->PersistentPath(), params.callsign);
        string datapath = switchCallsing(_service->DataPath(), params.callsign);
        string volatilepath = switchCallsing(_service->VolatilePath(), params.callsign);

        string proxypath = _service->ProxyStubPath();
        string systemPath = _service->SystemPath();

        Core::NodeId address(_remoteId.c_str());

        options[_T("-C")] = params.callsign;
        options[_T("-l")] = params.locator;
        options[_T("-c")] = params.className;
        options[_T("-r")] = address.HostAddress() + ":" + std::to_string(port);
        options[_T("-i")] = Core::NumberType<uint32_t>(params.interface).Text();
        options[_T("-e")] = Core::NumberType<uint32_t>(params.logging).Text();
        options[_T("-x")] = Core::NumberType<uint32_t>(params.id).Text();
        options[_T("-V")] = Core::NumberType<uint32_t>(params.version).Text();
        options[_T("-p")] = persistentPath;
        options[_T("-s")] = systemPath;
        options[_T("-d")] = datapath;
        options[_T("-v")] = volatilepath;
        options[_T("-m")] = proxypath;
        options[_T("-t")] = Core::NumberType<uint8_t>(params.threads).Text();

        // TODO: Revisit lifetime of a process
        uint32_t _pid;
        Core::Process fork(false);
        result = fork.Launch(options, &_pid);

        connectionPidMap.insert(std::pair(params.id, _pid));

        return result;
    }

    uint32_t RemoteInvocationPlugin::RemoteInvocation::Terminate(uint32_t connectionId) {
        auto foundPid = connectionPidMap.find(connectionId);

        if (foundPid != connectionPidMap.end()) {
            Core::Process process(foundPid->second);

            // TODO: Add trying gently first
            process.Kill(true);
            connectionPidMap.erase(foundPid);
        }

        return Core::ERROR_NONE;
    }
}
}