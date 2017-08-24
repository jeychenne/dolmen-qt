#ifndef SIGNAL_LUA_INCLUDE
#define SIGNAL_LUA_INCLUDE

const char *signal_script = R"_(
local sigslots = {}

signal = {}


function signal.connect(id, slot)
    local slots = sigslots[id]

    if slots then
        -- Check whether the signal is already there
        for i, s in ipairs(slots) do
            if slot == s then
                return
            end
        end

        table.insert(slots, slot)
    else
        sigslots[id] = { slot }
    end
end

function signal.disconnect(id, slot)
    local slots = sigslots[id]

    if slots then
        for i, s in ipairs(slots) do
            if slot == s then
                sigslots:remove(i)
                return
            end
        end
    end
end

function signal.emit(id, ...)
    local arg = {...}
    local slots = sigslots[id]

    if slots ~= nil then
		local result = {}

        for _, slot in ipairs(slots) do
            local r = slot(table.unpack(arg))
			table.insert(result, r)
        end

		return result
    end
end
)_";

#endif /* SIGNAL_LUA_INCLUDE */