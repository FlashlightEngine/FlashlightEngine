// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#include <FlashlightEngine/flpch.hpp>
#include <FlashlightEngine/Export.hpp>

#include <functional>
#include <ostream>
#include <sstream>
#include <type_traits>

namespace Flashlight {
    template <typename Enumeration>
    constexpr auto IntegerFromEnum(const Enumeration value) -> std::underlying_type_t<Enumeration> requires (std::is_enum_v<Enumeration>) {
        return static_cast<std::underlying_type_t<Enumeration>>(value);
    }

    template <typename T>
    constexpr T BitFromNumber(T x) {
        return 1 << x;
    }

    enum class EventType : u8 {
        Undefined = 0,

        WindowClose, WindowResize, WindowMoved, WindowFocus,
        KeyDown, KeyUp, KeyTyped,
        MouseButtonDown, MouseButtonUp, MouseMoved, MouseScroll
    };

    enum class EventCategory : u8 {
        Undefined = 0,

        Application = BitFromNumber(0),
        Input = BitFromNumber(1),
        Keyboard = BitFromNumber(2),
        Mouse = BitFromNumber(3),
        MouseButton = BitFromNumber(4)
    };

    inline EventCategory operator|(const EventCategory a, const EventCategory b);

#define BIND_EVENT_TO_EVENT_HANDLER(x) [this](auto && PH1) { return (x)(PH1); }
    
#define EVENT_TYPE_CLASS(type)                                                  \
static EventType GetStaticType() { return type; }                           \
virtual EventType GetEventType() const override { return GetStaticType(); } \
virtual const char* GetName() const override { return #type; }


#define EVENT_CATEGORY_CLASS(category) \
virtual int GetCategoryFlags() const override { return IntegerFromEnum(category); }

    class FL_API Event {
        friend class EventDispatcher;

        bool m_Handled = false;

    public:
        virtual ~Event() = default;
        [[nodiscard]] virtual EventType GetEventType() const = 0;
        [[nodiscard]] virtual const char* GetName() const = 0;
        [[nodiscard]] virtual i32 GetCategoryFlags() const = 0;

        [[nodiscard]] virtual inline std::string ToString() const;

        inline bool IsInCategory(EventCategory category) const;

        inline bool IsHandled() const;

        inline void Stop();
    };

    class FL_API EventDispatcher {
        Event& m_Event;

    public:
        explicit EventDispatcher(Event& event) : m_Event(event) {
        }

        template <typename T>
        bool Dispatch(std::function<void(T&)> handler);
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& event);

#pragma region Events

#pragma region Application Events

    class FL_API WindowResizeEvent final : public Event {
        u32 m_Width, m_Height;

    public:
        WindowResizeEvent(const u32 width, const u32 height) : m_Width(width), m_Height(height) {
        }

        ~WindowResizeEvent() override = default;

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << "Window Resized: Width=" << m_Width << ", Height=" << m_Height;
            return ss.str();
        }

        [[nodiscard]] inline u32 GetWidth() const {
            return m_Width;
        }

        [[nodiscard]] inline u32 GetHeight() const {
            return m_Height;
        }

        [[nodiscard]] inline std::pair<f32, f32> GetSize() const {
            return std::make_pair(static_cast<f32>(m_Width), static_cast<f32>(m_Height));
        }

        EVENT_TYPE_CLASS(EventType::WindowResize)
        EVENT_CATEGORY_CLASS(EventCategory::Application)
    };

    class FL_API WindowCloseEvent final : public Event {
    public:
        WindowCloseEvent() = default;

        EVENT_TYPE_CLASS(EventType::WindowClose)
        EVENT_CATEGORY_CLASS(EventCategory::Application)
    };

    class FL_API WindowFocusedEvent final : public Event {
        bool m_Focused;

    public:
        explicit WindowFocusedEvent(const bool focused) : m_Focused(focused) {
        }

        [[nodiscard]] inline bool IsFocused() const {
            return m_Focused;
        }

        EVENT_TYPE_CLASS(EventType::WindowFocus)
        EVENT_CATEGORY_CLASS(EventCategory::Application)
    };

    class FL_API WindowMovedEvent final : public Event {
        i32 m_XPos, m_YPos;

    public:
        WindowMovedEvent(const i32 x, const i32 y) : m_XPos(x), m_YPos(y) {
        }

        [[nodiscard]] inline std::string ToString() const override {
            std::stringstream ss;
            ss << "Window moved: X=" << m_XPos << ", Y=" << m_YPos;
            return ss.str();
        }

        EVENT_TYPE_CLASS(EventType::WindowMoved)
        EVENT_CATEGORY_CLASS(EventCategory::Application)
    };

#pragma endregion Application Events

#pragma region Keys Events

    class FL_API KeyEvent : public Event {
    public:
        inline i32 GetScancode() const {
            return m_Scancode;
        }

        EVENT_CATEGORY_CLASS(EventCategory::Keyboard | EventCategory::Input)

    protected:
        i32 m_Scancode;

        explicit KeyEvent(const i32 scancode) : m_Scancode(scancode) {
        }
    };

    class FL_API KeyDownEvent final : public KeyEvent {
        i32 m_RepetitionCount;

    public:
        KeyDownEvent(const i32 scancode, const i32 repetitionCount) : KeyEvent(scancode),
                                                                      m_RepetitionCount(repetitionCount) {
        }

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << "Key down: Scancode=" << m_Scancode << " Repeated " << m_RepetitionCount << " times";
            return ss.str();
        }

        EVENT_TYPE_CLASS(EventType::KeyDown)
    };

    class FL_API KeyUpEvent final : public KeyEvent {
    public:
        explicit KeyUpEvent(const i32 scancode) : KeyEvent(scancode) {
        }

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << "Key up: Scancode=" << m_Scancode;
            return ss.str();
        }

        EVENT_TYPE_CLASS(EventType::KeyUp)
    };

    class FL_API KeyTypedEvent final : public KeyEvent {
    public:
        KeyTypedEvent(const i32 scancode) : KeyEvent(scancode) {
        }

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << "Key typed: Scancode=" << m_Scancode;
            return ss.str();
        }

        EVENT_TYPE_CLASS(EventType::KeyTyped)
    };

#pragma endregion Keys Events

#pragma region Mouse Events

    class FL_API MouseMovedEvent final : public Event {
        f32 m_MouseX, m_MouseY;

    public:
        MouseMovedEvent(const f32 x, const f32 y) : m_MouseX(x), m_MouseY(y) {
        }

        [[nodiscard]] inline f32 GetX() const {
            return m_MouseX;
        }

        [[nodiscard]] inline f32 GetY() const {
            return m_MouseY;
        }

        [[nodiscard]] inline std::pair<f32, f32> GetPosition() const {
            return std::make_pair(m_MouseX, m_MouseY);
        }

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << "Mouse moved: X=" << m_MouseX << ", Y=" << m_MouseY;
            return ss.str();
        }

        EVENT_TYPE_CLASS(EventType::MouseMoved)
        EVENT_CATEGORY_CLASS(EventCategory::Mouse | EventCategory::Input)
    };

    class FL_API MouseScrolledEvent final : public Event {
        f32 m_XOffset, m_YOffset;

    public:
        MouseScrolledEvent(const f32 xOffset, const f32 yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {
        }

        [[nodiscard]] inline f32 GetXOffset() const {
            return m_XOffset;
        }

        [[nodiscard]] inline f32 GetYOffset() const {
            return m_YOffset;
        }

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << "Mouse scroll: X offset=" << m_XOffset << ", Y offset=" << m_YOffset;
            return ss.str();
        }

        EVENT_TYPE_CLASS(EventType::MouseScroll)
        EVENT_CATEGORY_CLASS(EventCategory::Mouse | EventCategory::Input)
    };

    class FL_API MouseButtonEvent : public Event {
    public:
        [[nodiscard]] inline i32 GetButton() const {
            return m_Button;
        }

        EVENT_CATEGORY_CLASS(EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton)
    
    protected:
        i32 m_Button;

        explicit MouseButtonEvent(const i32 button) : m_Button(button) {
        
        }
    };

    class FL_API MouseButtonDownEvent final : public MouseButtonEvent {
    public:
        explicit MouseButtonDownEvent(const i32 button) : MouseButtonEvent(button) {
        }

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << "Mouse button down: button=" << m_Button;
            return ss.str();
        }

        EVENT_TYPE_CLASS(EventType::MouseButtonDown)
    };

    class FL_API MouseButtonUpEvent final : public MouseButtonEvent {
    public:
        explicit MouseButtonUpEvent(const i32 button) : MouseButtonEvent(button) {
        }

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << "Mouse button up: button=" << m_Button;
            return ss.str();
        }

        EVENT_TYPE_CLASS(EventType::MouseButtonUp)
    };

#pragma endregion Mouse Events

#pragma endregion Events

#include <FlashlightEngine/Core/Event.inl>
}