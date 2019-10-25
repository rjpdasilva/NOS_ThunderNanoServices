#include "RemoteInvocation.h"
#include "plugins/IShell.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(RemoteInvocation, 1, 0);

    const string RemoteInvocation::Initialize(PluginHost::IShell* service) 
    {
        // Normal code
        Core::ProxyType<RPC::InvokeServer> server = Core::ProxyType<RPC::InvokeServer>::Create(&Core::WorkerPool::Instance());
        Core::NodeId address("0.0.0.0:5797");
        _extService = new ExternalAccess(address, service, server, this);

        _service = service;

        return (string());
    }

    void RemoteInvocation::Deinitialize(PluginHost::IShell* service) 
    {
        _service = nullptr;
        
        if (_extService != nullptr) {
            delete _extService;
            _extService = nullptr;
        }
    }

    string RemoteInvocation::Information() const 
    {
        return (string());
    }

    void RemoteInvocation::Start(const string callingDevice, const Exchange::IRemoteInvocation::ProgramParams& params) {
        Core::Process::Options options("WPEProcess");

        options[_T("-C")] = params.callsign;
        options[_T("-l")] = params.locator;
        options[_T("-c")] = params.className;
        options[_T("-r")] = callingDevice;
        options[_T("-i")] = Core::NumberType<uint32_t>(params.interface).Text();
        options[_T("-e")] = Core::NumberType<uint32_t>(params.logging).Text();
        options[_T("-x")] = Core::NumberType<uint32_t>(params.id).Text();
        options[_T("-V")] = Core::NumberType<uint32_t>(params.version).Text();
        options[_T("-p")] = _service->PersistentPath();
        options[_T("-s")] = "/home/mipo57/Pulpit/metrological/scripts/local/staging/usr/lib/wpeframework/plugins";
        options[_T("-d")] = _service->DataPath();
        //options[_T("-a")] = _service->;
        options[_T("-v")] = _service->VolatilePath();
        options[_T("-m")] = _service->DataPath();
        options[_T("-t")] = Core::NumberType<uint8_t>(params.threads).Text();

        uint32_t _pid;
        Core::Process fork(false);
        fork.Launch(options, &_pid);
    }
}
}