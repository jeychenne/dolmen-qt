#ifndef CONSOLE_H
#define CONSOLE_H

#include "extra/qconsole/qconsole.h"
#include "runtime/scripting.h"

class PluginHandler;


class LuaConsole final : public QConsole
{
public:
    LuaConsole(sol::state_view lua, QWidget *parent = nullptr);

    sol::state_view lua_state() const;

    void setHandler(PluginHandler *handler);

protected:
    //execute a validated command (should be reimplemented and called at the end)
    //the return value of the function is the string result
    //res must hold back the return value of the command (0: passed; else: error)
    QString interpretCommand(const QString &command, int *res) override;

private:
    sol::state_view m_lua;
    PluginHandler *plugin_handler; // non-owning reference

    void print(const char *str);

};

#endif // CONSOLE_H
