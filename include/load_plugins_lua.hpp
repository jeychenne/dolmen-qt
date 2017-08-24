#ifndef LOAD_PLUGINS_LUA_INCLUDE
#define LOAD_PLUGINS_LUA_INCLUDE

const char *load_plugins_script = R"_(
local function load_plugin(plugin)
    local init_file = filesystem.join(plugin, "initialize.lua")

    if filesystem.is_file(init_file) then
        dofile(init_file);
    end
end

local function load_plugins()
	local plugin_dir = dolmen.plugin_directory()
    local dirs = filesystem.list_directory(plugin_dir, false)

    for i, v in ipairs(dirs) do
		local path = filesystem.join(plugin_dir, v)
        load_plugin(path)
    end
end

load_plugins()
)_";

#endif /* LOAD_PLUGINS_LUA_INCLUDE */