#include "Module.h"
#include "interfaces/IRemoteHostExample.h"
#include "interfaces/IRemoteLinker.h"

namespace WPEFramework {
namespace Exchange {

    class RemoteHostExampleImpl : public RemoteLinker, IRemoteHostExample {
    public:
        RemoteHostExampleImpl() 
            : _name()
        {

        }

        virtual ~RemoteHostExampleImpl() 
        {
            printf("### Remote Host Example Destructor called!\n");
        }

        uint32_t SetName(const string& name) override;
        uint32_t Greet(const string& name, string& greeting) override;

        BEGIN_INTERFACE_MAP(RemoteHostExampleImpl)
            INTERFACE_ENTRY(Exchange::IRemoteHostExample)
            INTERFACE_ENTRY(Exchange::IRemoteLinker)
        END_INTERFACE_MAP

    private:
        string _name;
    };

    uint32_t RemoteHostExampleImpl::SetName(const string& name) {
        _name = name;

        return Core::ERROR_NONE;
    }

    uint32_t RemoteHostExampleImpl::Greet(const string& name, string& response) {
        response = "Hello " + name + ", my name is " + _name;

        return Core::ERROR_NONE;
    }

    SERVICE_REGISTRATION(RemoteHostExampleImpl, 1, 0);
}
}

