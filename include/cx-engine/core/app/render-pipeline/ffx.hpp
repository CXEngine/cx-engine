#pragma once
#include <cx-engine/core/app/render-pipeline.hpp>
#include <cx-engine/systems/logging/logger.hpp>

#include <incbin/incbin.h>

namespace cx {

INCTXT_EXTERN(FfxCasShaderString);
INCTXT_EXTERN(FfxCasVertexShaderString);

static inline const StringView gFfxCasShaderString
    = StringView(gFfxCasShaderStringData, gFfxCasShaderStringSize - 1);
static inline const StringView gFfxCasVertexShaderString
    = StringView(gFfxCasVertexShaderStringData, gFfxCasVertexShaderStringSize - 1);

class FfxPass: public RenderPass {
private:
    sf::Shader shader;

public:
    FfxPass(App& app);

    void process(sf::RenderTexture& input, sf::RenderTarget& output) override;

    sf::Vector2u getOutputSize(sf::Vector2u) const override;
};

}
