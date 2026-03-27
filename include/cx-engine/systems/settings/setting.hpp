#pragma once

#include <cx-engine/defs/types.hpp>

#include <cx-engine/utils/bitflags.hpp>
#include <cx-engine/utils/strconv.hpp>

#include <toml++/toml.hpp>

namespace cx {

enum class SettingFlags {
    None                = 0,
    NeedRecreateWindow  = 1 << 0,
    NeedRefreshWindow   = 1 << 1,
    NeedRestartGame     = 1 << 2,
};

TD_BITFLAGS(SettingFlags);

class Setting {
public:
    StringView key;
    SettingFlags flags;

    StringView displayName;
    StringView description;

    explicit Setting(
        StringView key,
        StringView displayName = "",
        StringView description = "",
        SettingFlags flags = SettingFlags::None
    )
        : key(key)
        , flags(flags)
        , displayName(displayName)
        , description(description)
    { }

    virtual ~Setting() = default;

    virtual void reset() = 0;
    virtual void load(const toml::table& tbl) = 0;
    virtual void save(toml::table& tbl) const = 0;
    virtual bool validate() const = 0;

    virtual String toString() const { return String(); }
    virtual bool setFromString(StringView) { return false; }

    bool hasFlag(SettingFlags flag) const { return bool(flags & flag); }
    void addFlag(SettingFlags flag) { flags |= flag; }
};

}
