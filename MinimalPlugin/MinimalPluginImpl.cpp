#include "Module.h"
#include "interfaces/IMinimalPlugin.h"
#include "interfaces/IRemoteHost.h"

namespace WPEFramework {
namespace Exchange {

    class MinimalPluginImpl : public RemoteHost, IMinimalPlugin {
    public:
        virtual ~MinimalPluginImpl() {
            printf("### Minimal plugin destructor called!\n");
        }

        uint32_t Greet(const string& name, string& greeting);

        BEGIN_INTERFACE_MAP(MinimalPluginImpl)
            INTERFACE_ENTRY(Exchange::IMinimalPlugin)
            INTERFACE_ENTRY(Exchange::IRemoteHost)
        END_INTERFACE_MAP
    };

    uint32_t MinimalPluginImpl::Greet(const string& name, string& greeting) {
        string myName = "Stephan";
        
        greeting = "Hello " + name + ", I'm " + myName;
        
        return Core::ERROR_NONE;
    }

    SERVICE_REGISTRATION(MinimalPluginImpl, 1, 0);
}
}

