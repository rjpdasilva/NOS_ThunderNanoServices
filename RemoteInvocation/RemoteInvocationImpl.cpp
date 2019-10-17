#include "module.h"
#include "interfaces/IRemoteInvocation.h"


namespace WPEFramework {
namespace Exchange {

    struct RemoteInvocationImpl : public IRemoteInvocation {

        enum { ID = ID_REMOTEINVOCATION };

        virtual ~RemoteInvocation() {}

        Core::IUnknown* Root(const string& targetMachine, const uint32_t waitTime, const string className, const uint32_t interface, const uint32_t version) override;
    };
}
}

Core::IUnknown* RemoteInvocationImpl::Root(const string& targetMachine, const uint32_t waitTime, const string className, const uint32_t interface, const uint32_t version) {
    // Client code
    Core::NodeID address(targetMachine.c_str());
    address.Port(28912);

    auto client = Core::ProxyType<RPC::CommunicatorClient>::Create(address);
    Core::IUnknown* result = client->Open<OCDM::IAccessorOCDM>(_T("OpenCDMImplementation"));

    client->Release();

    return result;
}