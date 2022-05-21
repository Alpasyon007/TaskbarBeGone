﻿#pragma once

#include "Class.g.h"

namespace winrt::TaskbarBeGone_Background_Task::implementation
{
    struct Class : ClassT<Class>
    {
        Class() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::TaskbarBeGone_Background_Task::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
