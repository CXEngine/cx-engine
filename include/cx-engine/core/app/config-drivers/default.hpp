#pragma once

#include <cx-engine/core/app/config-driver.hpp>

namespace cx {

/// @brief Default configuration driver for prototyping and debugging.
///
/// This driver provides hardcoded default values for various engine settings.
/// It is intended to be used when no external configuration is available
/// or during early development stages to skip configuration loading.
class DefaultConfigDriver: public cx::ConfigDriver {
public:
    /// @brief Default constructor
    DefaultConfigDriver() = default;

    /// @brief Returns VideoMode::Windowed
    /// @return The hardcoded windowing mode
    VideoMode getVideoMode() const override { return VideoMode::Windowed; }

    /// @brief Returns the target resolution as rendering resolution
    /// @return The current target resolution
    sf::Vector2u getRenderResolution() const override { return getTargetResolution(); }

    /// @brief Returns the target resolution as UI resolution
    /// @return The current target resolution
    sf::Vector2u getUiResolution() const override { return getTargetResolution(); }

    /// @brief Returns hardcoded 1920x1080 resolution
    /// @return The 1920x1080 vector
    sf::Vector2u getTargetResolution() const override { return { 1920, 1080 }; }

    /// @brief Returns false
    /// @return Constant false value
    bool getFsrEnabled() const override { return false; }

    /// @brief Returns true
    /// @return Constant true value
    bool getVsyncEnabled() const override { return true; }    

    /// @brief Returns std::nullopt
    /// @return Empty optional
    Optional<uint> getFpsLimit() const override { return std::nullopt; }

    /// @brief Returns false
    /// @return Constant false value
    bool getFxaaEnabled() const override { return false; }

    /// @brief Returns 4
    /// @return Constant MSAA sample count
    uint getMsaaLevel() const override { return 4; }
};

} // namespace cx
