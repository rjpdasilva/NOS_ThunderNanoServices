#pragma once

// !!!! IMPORTANT !!!!
// This plugin should never be started as outofprocess!

#include "Module.h"
#include "interfaces/IRemoteInvocation.h"

namespace WPEFramework {
namespace Plugin {

    class RemoteInvocation : public PluginHost::IPlugin, public Exchange::IRemoteInvocation {
    public:
        class ExternalAccess : public RPC::Communicator {
        private:
            ExternalAccess() = delete;
            ExternalAccess(const ExternalAccess&) = delete;
            ExternalAccess& operator=(const ExternalAccess&) = delete;

        public:
            ExternalAccess(
                const Core::NodeId& source, 
                PluginHost::IShell* service,
                const Core::ProxyType<RPC::InvokeServer> & engine,
                RemoteInvocation* parent)
                : RPC::Communicator(source, _T(""), Core::ProxyType<Core::IIPCServer>(engine))
                , _service(service)
                , _parent(parent)
            {
                engine->Announcements(Announcement());
                Open(Core::infinite);
            }
            ~ExternalAccess()
            {
                Close(Core::infinite);
            }

        private:
            virtual void* Aquire(const string& className, const uint32_t interfaceId, const uint32_t versionId)
            {
                void* result = nullptr;
                uint32_t pid;
                //result = _service->Root(pid, Core::infinite, className, interfaceId, versionId);
                _parent->AddRef();

                return (_parent->QueryInterface(Exchange::IRemoteInvocation::ID));
            }

            PluginHost::IShell* _service;
            RemoteInvocation* _parent;
        };

        RemoteInvocation(const RemoteInvocation&) = delete;
        RemoteInvocation& operator=(const RemoteInvocation&) = delete;

        RemoteInvocation()
        {
        }

        virtual ~RemoteInvocation()
        {
        }

        BEGIN_INTERFACE_MAP(RemoteInvocation)
            INTERFACE_ENTRY(PluginHost::IPlugin)
            INTERFACE_ENTRY(Exchange::IRemoteInvocation)
        END_INTERFACE_MAP

    public:
        //   IPlugin methods
        // -------------------------------------------------------------------------------------------------------
        virtual const string Initialize(PluginHost::IShell* service) override;
        virtual void Deinitialize(PluginHost::IShell* service) override;
        virtual string Information() const override;

        //   IRemoteInvocaiton methonds
        // --------------------------------------------------------------------------------------------------------
        void Start(const string callingDevice, const Exchange::IRemoteInvocation::ProgramParams& params) override;

    public:
        PluginHost::IShell* _service;
        ExternalAccess* _extService;
        Exchange::IRemoteInvocation* _implementation;
    };

} // namespace Plugin
} // namespace WPEFramework