#pragma once

#include <cx-engine/core/app/render-pipeline.hpp>
#include <cx-engine/systems/logging/logger.hpp>
#include <cx-engine/defs/types.hpp>

#include <cx-engine/extern/incbin/incbin.h>

namespace cx {

INCTXT_EXTERN(FxaaShaderString);

static const inline StringView gFxaaShaderString
    = StringView(gFxaaShaderStringData, gFxaaShaderStringSize);

class FxaaPass: public RenderPass {
private:
    sf::Shader shader;

public:
    FxaaPass(App& app) : RenderPass(app) {
        bool ok = shader.loadFromMemory(gFxaaShaderString, sf::Shader::Type::Fragment);
        if (!ok) {
            Logger::err("Failed to load FXAA shader");
        } else {
            Logger::debug("FXAA shader loaded");
        }
    }

    void process(sf::RenderTexture& input, sf::RenderTarget& output) override {
        sf::Sprite sprite(input.getTexture());
        shader.setUniform("u_texture", input.getTexture());
        shader.setUniform("u_resolution",
            sf::Vector2f(input.getSize()));

        output.draw(sprite, &shader);
    }
};

}
