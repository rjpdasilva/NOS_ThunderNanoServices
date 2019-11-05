#include "Module.h"
#include "interfaces/IMinimalPlugin.h"

namespace WPEFramework {
namespace Exchange {

    class MinimalPluginImpl : public IMinimalPlugin {
    public:
        virtual ~MinimalPluginImpl() {
            printf("### Minimal plugin destructor called!\n");
        }

        uint32_t Greet(const string& name, string& greeting);

        BEGIN_INTERFACE_MAP(MinimalPluginImpl)
        INTERFACE_ENTRY(Exchange::IMinimalPlugin)
        END_INTERFACE_MAP
    };

    uint32_t MinimalPluginImpl::Greet(const string& name, string& greeting) {
        greeting = "Hello ";
        greeting += name;

        // SleepS(1000);

        return Core::ERROR_NONE;
    }

    
    SERVICE_REGISTRATION(MinimalPluginImpl, 1, 0);
}
}

