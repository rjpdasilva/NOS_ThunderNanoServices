#include "Module.h"
#include "interfaces/IRemoteHostExample.h"
#include "interfaces/IRemoteLinker.h"

namespace WPEFramework {
namespace Exchange {
    class RemoteLinker : public Exchange::IRemoteLinker {
    public:

        virtual ~RemoteLinker()
        {
            Disconnect();
        }

        // TODO: Change name (sth like Instantiate, Link)
        uint32_t Connect(const string& remoteAddress, const uint32_t interface, const uint32_t exchangeId) override
        {
            uint32_t result = Core::ERROR_GENERAL;

            auto remoteInvokeServer = Core::ProxyType<RPC::WorkerPoolIPCServer>::Create();
            _remoteServer = Core::ProxyType<RPC::CommunicatorClient>::Create(Core::NodeId(remoteAddress.c_str()), Core::ProxyType<Core::IIPCServer>(remoteInvokeServer));

            if (remoteInvokeServer.IsValid()) {
                remoteInvokeServer->Announcements(_remoteServer->Announcement());

                void* requestedInterface = QueryInterface(interface);

                // Now try to create direct connection!
                result = _remoteServer->Open((RPC::CommunicationTimeOut != Core::infinite ? 2 * RPC::CommunicationTimeOut : RPC::CommunicationTimeOut), interface, requestedInterface, exchangeId);

                // QueryInterface added reference for ourselves, now we need to derefenece it
                Release();
            }
            
            return result;
        }

        uint32_t Disconnect() override 
        {
            if (_remoteServer.IsValid()) {
                _remoteServer->Close(2000);
                _remoteServer.Release();
            }

            ASSERT(_remoteServer.IsValid() == false);
        }

    private:
        Core::ProxyType<RPC::CommunicatorClient> _remoteServer;
    };


    class RemoteHostExampleImpl : public RemoteLinker, IRemoteHostExample {
    public:
        RemoteHostExampleImpl() 
            : _name()
        {

        }

        virtual ~RemoteHostExampleImpl() 
        {

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

