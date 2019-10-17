#include "RemoteInvocation.h"
#include "ocdm/IOCDM.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(RemoteInvocation, 1, 0);

    const string RemoteInvocation::Initialize(PluginHost::IShell* service) 
    {
        // Normal code
        Core::ProxyType<RPC::InvokeServer> server = Core::ProxyType<RPC::InvokeServer>::Create(&Core::WorkerPool::Instance());
        Core::NodeId address("127.0.0.1");
        address.PortNumber(28912);
        _service = new ExternalAccess(address, service, server);

        uint32_t pid;
        _implementation = service->Root<Exchange::IRemoteInvocation>(pid, 2000, "RemoteInvocationImpl");

        auto result = _implementation->Root<OCDM::IAccessorOCDM>("127.0.0.1", 2000, _T("OpenCDMImplementation"));

        return (string());
    }

    void RemoteInvocation::Deinitialize(PluginHost::IShell* service) 
    {
        _implementation->Release();
    }

    string RemoteInvocation::Information() const 
    {
        return (string());
    }
}
}