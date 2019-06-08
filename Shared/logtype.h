#pragma once

/**
 * @brief types of logs.
 */
enum class LogTypes : char
{
    DB = 0,
    CONFIG,
    MODEL,
    CALC,
    NETWORK,
    LOGIN,
    APP,
    IMPORT_EXPORT,
    END
};
