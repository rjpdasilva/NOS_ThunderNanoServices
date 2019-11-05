#pragma once

// !!!! IMPORTANT !!!!
// This plugin should never be started as outofprocess!

#include "Module.h"
#include "interfaces/IRemoteInvocation.h"

namespace WPEFramework {
namespace Plugin {

    class RemoteInvocationPlugin : public PluginHost::IPlugin {
    public:

        class Config : public Core::JSON::Container {
        private:
            Config(const Config&) = delete;
            Config& operator=(const Config&) = delete;

        public:
            Config()
                : Core::JSON::Container()
                , Address("0.0.0.0:9238")
            {
                Add(_T("address"), &Address);
            }

            ~Config()
            {
            }

        public:
            Core::JSON::String Address;
        };

        class RemoteInvocation : public Exchange::IRemoteInvocation {
        private:
            RemoteInvocation(const RemoteInvocation&) = delete;
            RemoteInvocation& operator=(const RemoteInvocation&) = delete;

        public:
            RemoteInvocation(const string& remoteId, PluginHost::IShell* service)
                : _remoteId(remoteId)
                , _service(service)
                , _refCount(1)
            {
            }

            void AddRef() const override {
                _refCount++;

                printf("### Remote invocation refcount increased to %d\n", _refCount);
            };
            uint32_t Release() const override {
                _refCount--;

                printf("### Remote invocation refcount decreased to %d\n", _refCount);

                if (_refCount <= 0) {
                    delete this;
                }

                return 0;
            };

            virtual ~RemoteInvocation()
            {
            }

            BEGIN_INTERFACE_MAP(RemoteInvocation)
            INTERFACE_ENTRY(Exchange::IRemoteInvocation)
            END_INTERFACE_MAP

            //   IRemoteInvocaiton methonds
            // --------------------------------------------------------------------------------------------------------
            uint32_t Instantiate(const uint16_t port, const Exchange::IRemoteInvocation::ProgramParams& params) override;
            uint32_t Terminate(uint32_t connectionId) override;

        private:
            string _remoteId;
            PluginHost::IShell* _service;
            mutable uint32_t _refCount;
        };


        class ExternalAccess : public RPC::Communicator {
        private:
            ExternalAccess() = delete;
            ExternalAccess(const ExternalAccess&) = delete;
            ExternalAccess& operator=(const ExternalAccess&) = delete;

        public:
            ExternalAccess(
                const Core::NodeId& source, 
                PluginHost::IShell* service,
                const Core::ProxyType<RPC::InvokeServer> & engine)
                : RPC::Communicator(source, _T(""), Core::ProxyType<Core::IIPCServer>(engine))
                , _service(service)
            {
                engine->Announcements(Announcement());
                Open(Core::infinite);
            }
            ~ExternalAccess()
            {
                Close(Core::infinite);
            }

        private:
            virtual void* Aquire(uint32_t id, const string& className, const uint32_t interfaceId, const uint32_t versionId)
            {
                void* result = nullptr;

                if (interfaceId == Exchange::IRemoteInvocation::ID) {
                    auto invocator = Core::ProxyType<RemoteInvocation>::Create(Connection(id)->RemoteId(), _service);
                    invocator->AddRef();

                    result = new RemoteInvocation(Connection(id)->RemoteId(), _service);
                }

                return result;
            }

            PluginHost::IShell* _service;
        };

        RemoteInvocationPlugin(const RemoteInvocationPlugin&) = delete;
        RemoteInvocationPlugin& operator=(const RemoteInvocationPlugin&) = delete;

        RemoteInvocationPlugin()
        {
        }

        virtual ~RemoteInvocationPlugin()
        {
        }

        BEGIN_INTERFACE_MAP(RemoteInvocationPlugin)
            INTERFACE_ENTRY(PluginHost::IPlugin)
        END_INTERFACE_MAP

    public:
        //   IPlugin methods
        // -------------------------------------------------------------------------------------------------------
        virtual const string Initialize(PluginHost::IShell* service) override;
        virtual void Deinitialize(PluginHost::IShell* service) override;
        virtual string Information() const override;
    public:
        ExternalAccess* _extService;
    };

} // namespace Plugin
} // namespace WPEFramework