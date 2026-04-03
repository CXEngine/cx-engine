#include <cx-engine/systems/loading/sessions/single-thread.hpp>

namespace cx {

SingleThreadLoadingSession::SingleThreadLoadingSession(Slice<const LoadingStep> steps)
    : steps(steps)
{
    totalWeight = calculateTotalWeight(steps);
    workerThread = std::thread([this]() { run(); });
}

SingleThreadLoadingSession::~SingleThreadLoadingSession() {
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

Optional<LoadingEvent> SingleThreadLoadingSession::pollEvent() {
    std::lock_guard<std::mutex> lock(mutex);
    if (eventQueue.empty()) {
        return std::nullopt;
    }
    
    LoadingEvent ev = eventQueue.front();
    eventQueue.pop();
    return ev;
}

float SingleThreadLoadingSession::getProgress() const {
    return globalProgress.load();
}

bool SingleThreadLoadingSession::isFinished() const {
    return finished.load();
}

void SingleThreadLoadingSession::wait() {
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void SingleThreadLoadingSession::run() {
    for (const auto& step: steps) {
        processStep(step);
    }
    
    globalProgress = 1.0f;
    pushEvent({ LoadingEvent::Type::Progress, nullptr, 1.0f });
    finished = true;
}

void SingleThreadLoadingSession::processStep(const LoadingStep& step) {
    pushEvent({ LoadingEvent::Type::StepBegin, &step, globalProgress.load() });

    if (step.task) {
        step.task();
    }

    if (!step.substeps.empty()) {
        for (const auto& substep: step.substeps) {
            processStep(substep);
        }
    }

    if (step.substeps.empty()) {
        completedWeight += step.weight;
        globalProgress = completedWeight / totalWeight;
        pushEvent({ LoadingEvent::Type::Progress, nullptr, globalProgress.load() });
    }

    pushEvent({ LoadingEvent::Type::StepEnd, &step, globalProgress.load() });
}

void SingleThreadLoadingSession::pushEvent(LoadingEvent event) {
    std::lock_guard<std::mutex> lock(mutex);
    eventQueue.push(event);
}

float SingleThreadLoadingSession::calculateTotalWeight(Slice<const LoadingStep> steps) {
    float weight = 0.0f;
    for (const auto& step: steps) {
        if (step.substeps.empty()) {
            weight += step.weight;
        } else {
            weight += calculateTotalWeight(step.substeps);
        }
    }
    return weight;
}

} // namespace cx
