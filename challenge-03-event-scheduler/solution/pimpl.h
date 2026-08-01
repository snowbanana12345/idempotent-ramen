#pragma once
#include "solution.h"

namespace hftu{
    EventScheduler::EventScheduler() : impl(std::make_unique<Impl>()){

    }

    EventScheduler::~EventScheduler() = default;

    void EventScheduler::schedule(uint64_t event_id, int64_t time_us) {
        this->impl->schedule(event_id, time_us);
    }

    bool EventScheduler::cancel(uint64_t event_id) {
        return this->impl->cancel(event_id);
    }

    uint32_t EventScheduler::advance(int64_t new_time_us, EventCallback cb, void* user_data) {
        return this->impl->advance(new_time_us, cb, user_data);
    }

    uint64_t EventScheduler::size() const {
        return this->impl->size();
    }

    int64_t EventScheduler::next_event_time() const {
        return this->impl->next_event_time();
    }
}