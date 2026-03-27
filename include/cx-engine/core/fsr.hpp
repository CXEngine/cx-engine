#pragma once

#include <cx-engine/systems/settings/enum-setting.hpp>

namespace cx {

enum class FsrPreset {
    Disabled,
    UltraQuality,
    Quality,
    Balanced,
    Performance,
};

static constexpr float FsrPresetUltraQuality = 0.77f;
static constexpr float FsrPresetQuality = 0.67f;
static constexpr float FsrPresetBalanced = 0.59f;
static constexpr float FsrPresetPerformance = 0.5f;

static constexpr float getFsrPresetScaling(FsrPreset mode) {
    switch (mode) {
    case FsrPreset::Disabled:
        return 1.f;
    case FsrPreset::UltraQuality:
        return FsrPresetUltraQuality;
    case FsrPreset::Quality:
        return FsrPresetQuality;
    case FsrPreset::Balanced:
        return FsrPresetBalanced;
    case FsrPreset::Performance:
        return FsrPresetPerformance;
    }

    return 1.f;
}

}

TD_DEF_ENUM_STRINGS(cx::FsrPreset,
    "disabled",
    "ultra-quality",
    "quality",
    "balanced",
    "performance"
);
