#pragma once

#include <cx-engine/defs/types.hpp>
#include <cx-engine/utils/buf.hpp>

#include <SFML/Graphics.hpp>

namespace cx {

class App;

class RenderPass {
protected:
    App& app;

public:
    RenderPass(App& app) : app(app) {}
    virtual ~RenderPass() = default;

    virtual void process(sf::RenderTexture& input, sf::RenderTarget& output) = 0;

    virtual sf::Vector2u getOutputSize(sf::Vector2u inputSize) const {
        return inputSize;
    }
};

class RenderPipeline {
private:
    App& app;

    static constexpr usize MaxPassess = 4;
    Buf<RenderPass*, MaxPassess> passes;

    UniquePtr<sf::RenderTexture> pingPtr;
    UniquePtr<sf::RenderTexture> pongPtr;

    void ensureRenderTexture(UniquePtr<sf::RenderTexture>& texturePtr, sf::Vector2u size);

public:
    RenderPipeline(App& app) : app(app) {}
    ~RenderPipeline();

    void clear();

    void addPass(RenderPass* pass);
    void execute(sf::RenderTexture& scene, sf::RenderWindow& window);

};

} // namespace cx
