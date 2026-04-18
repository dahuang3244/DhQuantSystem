#include "dhquant/engine.h"

namespace dhquant {

Engine::Engine(const RuntimeMode mode) noexcept
    : status_{.mode = mode, .running = false} {}

void Engine::start() noexcept { status_.running = true; }

void Engine::stop() noexcept { status_.running = false; }

EngineStatus Engine::status() const noexcept { return status_; }

std::string_view Engine::name() const noexcept { return "dhquant_engine"; }

} // namespace dhquant
