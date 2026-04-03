#pragma once

#include <cx-engine/systems/loading/session.hpp>

#include <mutex>
#include <thread>
#include <atomic>
#include <queue>

namespace cx {

class SingleThreadLoadingSession: public LoadingSession {
public:
    SingleThreadLoadingSession(Slice<const LoadingStep> steps);
    ~SingleThreadLoadingSession() override;

    Optional<LoadingEvent> pollEvent() override;
    float getProgress() const override;
    bool isFinished() const override;
    void wait() override;

private:
    void run();
    void processStep(const LoadingStep& step);
    void pushEvent(LoadingEvent event);
    float calculateTotalWeight(const Vec<LoadingStep>& steps);

    Slice<const LoadingStep> steps;
    std::thread workerThread;
    std::atomic<float> globalProgress = 0;
    std::atomic<bool> finished = false;
    
    std::mutex mutex;
    std::queue<LoadingEvent> eventQueue;

    float totalWeight = 0.f;
    float completedWeight = 0.f;
};

} // namespace cx
