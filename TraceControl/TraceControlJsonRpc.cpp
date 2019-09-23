
#include "Module.h"
#include "TraceControl.h"
#include <interfaces/json/JsonData_TraceControl.h>

namespace WPEFramework {

namespace Plugin {

    using namespace JsonData::TraceControl;

    // Registration
    //

    void TraceControl::RegisterAll()
    {
        Register<StatusParamsData,StatusResultData>(_T("status"), &TraceControl::endpoint_status, this);
        Register<TraceInfo,void>(_T("set"), &TraceControl::endpoint_set, this);
    }

    void TraceControl::UnregisterAll()
    {
        Unregister(_T("set"));
        Unregister(_T("status"));
    }

    JsonData::TraceControl::StateType TraceControl::TranslateState(TraceControl::state state)
    {
        JsonData::TraceControl::StateType newState = JsonData::TraceControl::StateType::DISABLED;

        switch (state)
        {
        case TraceControl::state::ENABLED:
            newState = JsonData::TraceControl::StateType::ENABLED;
            break;
        case TraceControl::state::DISABLED:
            newState = JsonData::TraceControl::StateType::DISABLED;
            break;
        case TraceControl::state::TRISTATED:
            newState = JsonData::TraceControl::StateType::TRISTATED;
            break;
        }

        return newState;
    }

    // API implementation
    //

    // Method: status - Retrieves general information
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t TraceControl::endpoint_status(const StatusParamsData& params, StatusResultData& response)
    {
        uint32_t result = Core::ERROR_NONE;

        response.Console = _config.Console;
        response.Remote.Port = _config.Remote.Port;
        response.Remote.Binding = _config.Remote.Binding;

        Observer::ModuleIterator index(_observer.Modules());

        while (index.Next() == true) {
            string moduleName(Core::ToString(index.Module()));
            Observer::ModuleIterator::CategoryIterator categories(index.Categories());

            if ((params.Module.IsSet() == false) || ((params.Module.IsSet() == true) && (moduleName == params.Module.Value()))) {
                while (categories.Next()) {
                    string categoryName(Core::ToString(categories.Category()));

                    if ((params.Category.IsSet() == false) || ((params.Category.IsSet() == true) && (categoryName == params.Category.Value()))) {
                        Data::Trace trace = Data::Trace(moduleName, categoryName, categories.State());
                        JsonData::TraceControl::TraceInfo traceResponse;
                        traceResponse.Module = trace.Module;
                        traceResponse.Category = trace.Category;
                        traceResponse.State = TranslateState(trace.State);
                        response.Settings.Add(traceResponse);
                    }
                }
            }
        }

        return result;
    }

    // Method: set - Sets traces
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t TraceControl::endpoint_set(const TraceInfo& params)
    {
        uint32_t result = Core::ERROR_NONE;

        _observer.Set((params.State.Value() == JsonData::TraceControl::StateType::ENABLED),
            (params.Module.IsSet() == true ? params.Module.Value() : std::string(EMPTY_STRING)),
            (params.Category.IsSet() == true ? params.Category.Value() : std::string(EMPTY_STRING)));

        return result;
    }
} // namespace Plugin

}

