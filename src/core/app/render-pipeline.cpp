#include <cx-engine/core/app/render-pipeline.hpp>

#include <cx-engine/systems/logging/logger.hpp>

namespace cx {

RenderPipeline::~RenderPipeline() {
    for (auto& pass : passes) {
        delete pass;
    }
}

void RenderPipeline::clear() {
    passes.clear();
    pingPtr.reset();
    pongPtr.reset();
}

void RenderPipeline::addPass(RenderPass* pass) {
    if (passes.isFull()) {
        Logger::err("RenderPipeline: Cannot add more passes, pipeline is full.");
        return;
    }
    passes.push(pass);
}

void RenderPipeline::ensureRenderTexture(UniquePtr<sf::RenderTexture>& texturePtr, sf::Vector2u size) {
    if (!texturePtr || texturePtr->getSize() != size) {
        try {
            texturePtr = std::make_unique<sf::RenderTexture>(size);
            texturePtr->setSmooth(true);
        } catch (const sf::Exception& err) {
            Logger::err("Failed to create render texture", {
                { "cause", err.what() },
            });
            texturePtr.reset();
        }
    }
}

void RenderPipeline::execute(sf::RenderTexture& scene, sf::RenderWindow& window) {
    if (passes.isEmpty()) {
        sf::Sprite sprite(scene.getTexture());
        window.draw(sprite);
        return;
    }

    sf::RenderTexture* currentInput = &scene;
    sf::Vector2u currentInputSize = scene.getSize();

    ensureRenderTexture(pingPtr, currentInputSize);
    ensureRenderTexture(pongPtr, currentInputSize);

    for (usize i = 0; i < passes.getElementCount(); ++i) {
        RenderPass* pass = passes[i];

        UniquePtr<sf::RenderTexture>& currentOutputPtr = (i % 2 == 0) ? pongPtr : pingPtr;

        sf::Vector2u desiredOutputSize = pass->getOutputSize(currentInputSize);

        ensureRenderTexture(currentOutputPtr, desiredOutputSize);

        sf::RenderTexture* currentOutput = currentOutputPtr.get();

        currentOutput->clear(sf::Color::Transparent);

        if (i == passes.getElementCount() - 1) {
            pass->process(*currentInput, window);
        } else {
            pass->process(*currentInput, *currentOutput);
            currentOutput->display();
        }

        currentInput = currentOutput;
        currentInputSize = desiredOutputSize;
    }
}

}
