#include <QMessageBox>
#include "LuaConsole.h"
#include "runtime/PluginHandler.h"
#include "extra/lua/lua.h"

#define PROMPT ">> "


LuaConsole::LuaConsole(sol::state_view lua, QWidget *parent) :
    QConsole(parent, LUA_COPYRIGHT), m_lua(lua)
{
    runtime::initialize(m_lua);
    plugin_handler = nullptr;

    // Redirect stdout to console.
    m_lua["__dolmen_print"] = [&](const char *s) {
        this->print(s);
    };

    // Clear console.
    m_lua["clear"] = [&]() { this->reset(); };

    // Override builtin print function.
    m_lua.script(R"_(
               print = function(...)
                   local arg = {...}

                   for i,v in ipairs(arg) do
                       if type(v) == "boolean" then
                           arg[i] = tostring(v)
                       end
                   end

                   __dolmen_print(table.concat(arg))
               end)_");

    setPrompt(PROMPT);
}

QString LuaConsole::interpretCommand(const QString &command, int *res)
{
    try
    {

        std::string code = command.toUtf8().data();
        sol::protected_function_result result = m_lua.safe_script(code);
//        std::string escaped("return tostring(");
//        escaped.append(code);
//        escaped.push_back(')');
//        sol::protected_function_result result;

//        try {
//            result = lua.safe_script(escaped);
//        }
//        catch (...) {
//            result = lua.safe_script(code);
//        }

        if (result.valid())
        {

            for (int i = result.return_count(); i > 0; i--)
            {
                // Convert each value to string, if posible.
                const char *value = lua_tostring(m_lua.lua_state(), -i);

                if (value && strlen(value) > 0) {
                    this->print(value);
                }
            }

            *res = 0;
            return QString();
        }
        else
        {
            *res = 1;
        }
    }
    catch (std::exception &e)
    {
        *res = -1;
        QString error(e.what());

        if (error.startsWith("sol: ")) {
            error.replace(0, 5, QString());
        }

        return error;
    }

    return QString();
}

sol::state_view LuaConsole::lua_state() const
{
    return m_lua;
}

void LuaConsole::setHandler(PluginHandler *handler)
{
    plugin_handler = handler;
}

void LuaConsole::print(const char *str)
{
    //BUG: if the cursor is not at the end of line, it prints before the last character.
    textCursor().movePosition(QTextCursor::EndOfLine);
    textCursor().insertText("\n");
    textCursor().insertText(QString(str));
}
