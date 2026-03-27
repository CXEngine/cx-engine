#pragma once

#include <cx-engine/defs/types.hpp>

#include <cx-engine/systems/settings/enum-setting.hpp>

#include <SFML/System/Vector2.hpp>

namespace cx {
enum class VideoMode;
enum class Resolution;
enum class MsaaMode;
}

enum class cx::VideoMode {
    Fullscreen,
    Borderless,
    Windowed,
};

TD_DEF_ENUM_STRINGS(cx::VideoMode, "fullscreen", "borderless", "windowed");

enum class cx::Resolution {
    HD,
    FHD,
    QHD,
    UHD,
};

TD_DEF_ENUM_STRINGS(cx::Resolution, "720p", "1080p", "1440p", "4k");

enum class cx::MsaaMode {
    Disabled,
    X2,
    X4,
    X8,
};

TD_DEF_ENUM_STRINGS(cx::MsaaMode, "disabled", "x2", "x4", "x8");

namespace cx {

class AppConfigManager {
public:
    virtual VideoMode      getVideoMode() const = 0;
    virtual sf::Vector2u   getRenderResolution() const = 0;
    virtual sf::Vector2u   getUiResolution() const = 0;
    virtual sf::Vector2u   getTargetResolution() const = 0;
    virtual bool           getFsrEnabled() const = 0;

    virtual Optional<uint> getFpsLimit() const = 0;
    virtual bool           getVsyncEnabled() const = 0;

    virtual bool           getFxaaEnabled() const = 0;
    virtual uint           getMsaaLevel() const = 0;
};

}
