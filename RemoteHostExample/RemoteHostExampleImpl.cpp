#include "Module.h"
#include "interfaces/IRemoteHostExample.h"
#include "RemoteHostExample.h"

namespace WPEFramework {
namespace Exchange {

    class RemoteHostExampleImpl : public RPC::RemoteLinker, IRemoteHostExample {
    public: 
        RemoteHostExampleImpl() 
            : _name()
        {

        }

        virtual ~RemoteHostExampleImpl() 
        {

        }

        uint32_t Initialize(PluginHost::IShell* service) override;
        uint32_t Greet(const string& name, string& greeting) override;

        BEGIN_INTERFACE_MAP(RemoteHostExampleImpl)
            INTERFACE_ENTRY(Exchange::IRemoteHostExample)
            INTERFACE_ENTRY(RPC::IRemoteLinker)
        END_INTERFACE_MAP

    private:
        string _name;
    };

    uint32_t RemoteHostExampleImpl::Initialize(PluginHost::IShell* service) {
        Plugin::RemoteHostExample::Config config;
        config.FromString(service->ConfigLine());
        
        _name = config.Name.Value();

        return Core::ERROR_NONE;
    }


    uint32_t RemoteHostExampleImpl::Greet(const string& message, string& response) {

        printf("#######################################\n");
        printf("%s\n", message.c_str());
        printf("#######################################\n");

        response += "Hello from the other side, my name is " + _name;

        return Core::ERROR_NONE;
    }

    SERVICE_REGISTRATION(RemoteHostExampleImpl, 1, 0);
}
}

