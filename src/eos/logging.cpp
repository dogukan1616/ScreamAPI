#include <scream_api/scream_api.hpp>
#include <scream_api/config.hpp>
#include <game_mode/game_mode.hpp>

#include <koalabox/logger.hpp>

#include <sdk/eos_logging.h>

using namespace scream_api;

DLL_EXPORT(EOS_EResult) EOS_Logging_SetLogLevel(EOS_ELogCategory LogCategory, EOS_ELogLevel LogLevel) {
    LOG_DEBUG("{} -> Category: {}, Level: {}", __func__, (int32_t) LogCategory, (int32_t) LogLevel);

    GET_ORIGINAL_FUNCTION(EOS_Logging_SetLogLevel)

    if (CONFIG.logging && CONFIG.eos_logging) {
        // Override game's preferences
        LogCategory = EOS_ELogCategory::EOS_LC_ALL_CATEGORIES;
        LogLevel = EOS_ELogLevel::EOS_LOG_VeryVerbose;
    }

    return EOS_Logging_SetLogLevel_o(LogCategory, LogLevel);
}

// Note: On x64 executables, this function will be hooked via EAT, hence it may cause issues
DLL_EXPORT(EOS_EResult) EOS_Logging_SetCallback(EOS_LogMessageFunc Callback) {
    LOG_DEBUG("{} -> Callback: {}", __func__, fmt::ptr(Callback))

    GET_ORIGINAL_FUNCTION(EOS_Logging_SetCallback)

    if (CONFIG.logging && CONFIG.eos_logging) {
        static Set<EOS_LogMessageFunc> callbacks;

        callbacks.insert(Callback);

        return EOS_Logging_SetCallback_o(
            [](const EOS_LogMessage* Message) {
                for (const auto& callback: callbacks) {
                    callback(Message);
                }
            }
        );
    } else {
        return EOS_Logging_SetCallback_o(Callback);
    }
}
