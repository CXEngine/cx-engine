#include <cx-engine/core/app/render-pipeline/ffx.hpp>

#include <cx-engine/core/app.hpp>

namespace cx {

INCTXT(FfxCasShaderString, "assets/cx-engine/shaders/ffxcas.frag");
INCTXT(FfxCasVertexShaderString, "assets/cx-engine/shaders/ffxcas.vert");

FfxPass::FfxPass(App& app) : RenderPass(app) {
    bool ok = shader.loadFromMemory(gFfxCasVertexShaderString, gFfxCasShaderString);
    if (!ok) {
        Logger::err("Failed to load FFX CAS shader");
    } else {
        Logger::debug("FFX CAS shader loaded");
    }
}

void FfxPass::process(sf::RenderTexture& input, sf::RenderTarget& output) {
    sf::Vector2u inSize = input.getSize();
    sf::Vector2u outSize = getOutputSize(inSize);

    float scaleX = static_cast<float>(outSize.x) / inSize.x;
    float scaleY = static_cast<float>(outSize.y) / inSize.y;

    input.setSmooth(true);
    sf::Sprite sprite(input.getTexture());
    sprite.setScale({ scaleX, scaleY });

    shader.setUniform("u_texture", sf::Shader::CurrentTexture);
    shader.setUniform("u_textureSize", sf::Vector2f(input.getSize()));
    shader.setUniform("SHARPENING", 0.3f);
    shader.setUniform("SOURCE_TRC", 4);
    shader.setUniform("TARGET_TRC", 4);
    shader.setUniform("CUSTOM_GAMMA", 2.2f);

    output.draw(sprite, &shader);
}

sf::Vector2u FfxPass::getOutputSize(sf::Vector2u) const {
    return app.getConfig().getTargetResolution();
}

}
