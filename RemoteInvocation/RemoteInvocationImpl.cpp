#include "Module.h"
#include "interfaces/IRemoteInvocation.h"

namespace WPEFramework {
namespace Exchange {

    class RemoteInvocationImpl : public IRemoteInvocation {
    public:
        virtual ~RemoteInvocationImpl() {}

        WPEFramework::Core::IUnknown* Root(const string& callingMachine, const uint32_t waitTime, const string className, const uint32_t interface, const uint32_t version) override;

        BEGIN_INTERFACE_MAP(RemoteInvocationImpl)
        INTERFACE_ENTRY(Exchange::IRemoteInvocation)
        END_INTERFACE_MAP
    };

    WPEFramework::Core::IUnknown* RemoteInvocationImpl::Root(const string& callingMachine, const uint32_t waitTime, const string className, const uint32_t interface, const uint32_t version) {
        // Client code

        Core::NodeId address(callingMachine.c_str());

        PluginHost::IShell::ICOMLink* handler(COMLink());
        void* result = nullptr;

        // This method can only be used in the main process. Only this process, can instantiate a new process
        ASSERT(handler != nullptr);

        if (handler != nullptr) {
            string locator(rootObject.Locator.Value());
            if (locator.empty() == true) {
                locator = Locator();
            }

            RPC::Object definition(Callsign(), locator,
                className,
                interface,
                version,
                rootObject.User.Value(),
                rootObject.Group.Value(),
                rootObject.Threads.Value(),
                RPC::Object::HostType::DISTRIBUTED, 
                rootObject.Configuration.Value(),
                address);

            uint32_t pid;
            result = handler->Instantiate(definition, waitTime, pid, ClassName(), Callsign());

            return (result);
        }
    }

    SERVICE_REGISTRATION(RemoteInvocationImpl, 1, 0);
}
}

