#include "Module.h"
#include "interfaces/IMinimalPlugin.h"

namespace WPEFramework {
namespace Exchange {

    class MinimalPluginImpl : public IMinimalPlugin {
    public:
        virtual ~MinimalPluginImpl() {}

        string Greet(const string& name) override;

        BEGIN_INTERFACE_MAP(MinimalPluginImpl)
        INTERFACE_ENTRY(Exchange::IMinimalPlugin)
        END_INTERFACE_MAP
    };

    string MinimalPluginImpl::Greet(const string& name) {
        string str = "Hello ";
        str += name;

        return str;
    }
    
    SERVICE_REGISTRATION(MinimalPluginImpl, 1, 0);
}
}

