#pragma once

inline EventCategory operator|(const EventCategory a, const EventCategory b) {
    return static_cast<EventCategory>(IntegerFromEnum(a) | IntegerFromEnum(b));
}

inline std::string Event::ToString() const {
    return GetName();
}

inline bool Event::IsInCategory(const EventCategory category) const {
    return GetCategoryFlags() & IntegerFromEnum(category);
}

inline bool Event::IsHandled() const {
    return m_Handled;
}

inline void Event::Stop() {
    m_Handled = false;
}

template <typename T>
bool EventDispatcher::Dispatch(std::function<void(T&)> handler) {
    if (m_Event.GetEventType() == T::GetStaticType()) {
        handler(static_cast<T&>(m_Event));
        return true;
    }
    return false;
}

inline std::ostream& operator<<(std::ostream& os, const Event& event) {
    return os << event.ToString();
}