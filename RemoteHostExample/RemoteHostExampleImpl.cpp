#include "Module.h"
#include "interfaces/IRemoteHostExample.h"
#include "interfaces/IRemoteHost.h"

namespace WPEFramework {
namespace Exchange {

    class RemoteHostExampleImpl : public RemoteHost, IRemoteHostExample {
    public:
        virtual ~RemoteHostExampleImpl() {
            printf("### Minimal plugin destructor called!\n");
        }

        uint32_t Greet(const string& name, string& greeting);

        BEGIN_INTERFACE_MAP(RemoteHostExampleImpl)
            INTERFACE_ENTRY(Exchange::IRemoteHostExample)
            INTERFACE_ENTRY(Exchange::IRemoteHost)
        END_INTERFACE_MAP
    };

    uint32_t RemoteHostExampleImpl::Greet(const string& name, string& greeting) {
        string myName = "";
        auto s = GetLocalShell()->SubSystems();
        greeting = "Hello " + myName;
        //greeting += myName;
        greeting += "ei";

        return Core::ERROR_NONE;
    }

    SERVICE_REGISTRATION(RemoteHostExampleImpl, 1, 0);
}
}

