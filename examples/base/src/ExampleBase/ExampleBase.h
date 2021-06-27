#pragma once

#include "Window.h"

#include <memory>

namespace example
{
    class ExampleBase
    {
    public:
        ExampleBase(const std::string& title) : m_title(title) {}

        void Run();

    protected:
        std::shared_ptr<Window> m_window;
        Input* m_input;

        virtual void Init_() {}
        virtual void Update_(float delta) {}
        virtual void Render_() {}

    private:
        void Init();

    private:
        std::string m_title;
    };

}  // namespace example
