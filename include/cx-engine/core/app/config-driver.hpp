#pragma once

#include <cx-engine/defs/types.hpp>

#include <cx-engine/systems/settings/enum-setting.hpp>

#include <SFML/System/Vector2.hpp>

namespace cx {
/// @brief Defines how the application window is displayed on the screen
enum class VideoMode;

/// @brief Defines standard display resolutions
enum class Resolution;

/// @brief Defines available MSAA sample counts
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

/// @brief Abstract interface for providing engine configuration settings.
///
/// ConfigDriver defines the contract for accessing various graphical and system
/// settings. Implementations can fetch these values from files, command line
/// arguments, or provide hardcoded defaults.
class ConfigDriver {
public:
    /// @brief Virtual destructor
    virtual ~ConfigDriver() = default;

    /// @brief Returns the preferred display mode for the application window
    /// @return The desired VideoMode
    virtual VideoMode      getVideoMode() const = 0;

    /// @brief Returns the resolution at which the game world is rendered
    /// @return Rendering resolution in pixels
    virtual sf::Vector2u   getRenderResolution() const = 0;

    /// @brief Returns the resolution used for the user interface layer
    /// @return UI resolution in pixels
    virtual sf::Vector2u   getUiResolution() const = 0;

    /// @brief Returns the actual dimensions of the application window
    /// @return Target window resolution in pixels
    virtual sf::Vector2u   getTargetResolution() const = 0;

    /// @brief Indicates whether FSR upscaling should be applied to the render output
    /// @return True if FSR is enabled
    virtual bool           getFsrEnabled() const = 0;

    /// @brief Returns the frame rate cap
    /// @return Optional FPS limit, or std::nullopt if unlimited
    virtual Optional<uint> getFpsLimit() const = 0;

    /// @brief Indicates whether the engine should synchronize with the refresh rate
    /// @return True if V-Sync is requested
    virtual bool           getVsyncEnabled() const = 0;

    /// @brief Indicates whether FXAA post-processing should be enabled
    /// @return True if FXAA is enabled
    virtual bool           getFxaaEnabled() const = 0;

    /// @brief Returns the number of MSAA samples for hardware anti-aliasing
    /// @return MSAA sample count (e.g., 0, 2, 4, 8)
    virtual uint           getMsaaLevel() const = 0;
};

}
