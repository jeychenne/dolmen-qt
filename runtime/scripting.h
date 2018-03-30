#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <QString>
#include "string.hpp"
#include "extra/sol.hpp"


// Specialization of QString so that it can be used as a native string in sol.

namespace sol {
template<>
struct is_container<QString> : std::false_type
{ };

}

namespace sol {
namespace stack {


template <>
struct checker<QString> {
    template <typename Handler>
    static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
        int absolute_index = lua_absindex(L, index);
        bool success = stack::check<const char*>(L, absolute_index, handler);
        tracking.use(1);

        return success;
    }
};

template <>
struct getter<QString> {
    QString get(lua_State* L, int index, record  &tracking) {
        tracking.use(1);
        size_t len = 0;
        const char* str = lua_tolstring(L, index, &len);

        return QString::fromUtf8(str, int(len));
    }
};

template <>
struct pusher<QString> {
    static int push(lua_State* L, const QString &str) {
        auto bytes = str.toUtf8();
        lua_pushlstring(L, bytes.data(), bytes.size());

        return 1;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <>
struct checker<std::string_view> {
    template <typename Handler>
    static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
        int absolute_index = lua_absindex(L, index);
        bool success = stack::check<const char*>(L, absolute_index, handler);
        tracking.use(1);

        return success;
    }
};

template <>
struct getter<std::string_view> {
    std::string_view get(lua_State* L, int index, record  &tracking) {
        tracking.use(1);
        size_t len;
        const char* p = lua_tolstring(L, index, &len);
        return std::string_view(p, len);
    }
};

template <>
struct pusher<std::string_view> {
    static int push(lua_State* L, const std::string_view &str) {
        lua_pushlstring(L, str.data(), str.size());
        return 1;
    }
};

} // namespace sol::stack
} // namespace sol


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Specialization of string so that it can be used as a native string in sol.

namespace sol {
template<>
struct is_container<runtime::string> : std::false_type
{ };

}

namespace sol {
namespace stack {


template <>
struct checker<runtime::string> {
    template <typename Handler>
    static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
        int absolute_index = lua_absindex(L, index);
        bool success = stack::check<const char*>(L, absolute_index, handler);
        tracking.use(1);

        return success;
    }
};

template <>
struct getter<runtime::string> {
    runtime::string get(lua_State* L, int index, record  &tracking) {
        tracking.use(1);
        size_t len;
        const char* p = lua_tolstring(L, index, &len);
        return runtime::string(p, len);
    }
};

template <>
struct pusher<runtime::string> {
    static int push(lua_State* L, const runtime::string &str) {
        lua_pushlstring(L, str.data(), str.raw_size());
        return 1;
    }
};

} // namespace sol::stack
} // namespace sol



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace runtime {

int generate_signal();

void initialize(sol::state_view lua);
}



#endif // SCRIPTING_H
