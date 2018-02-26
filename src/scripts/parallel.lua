------------------------------------------------------------
--  parallel.lua
--
--  copyright (c) 2018 Xiongfei Shi
--
--  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
--  license: Apache-2.0
------------------------------------------------------------

local sevo = require("sevo")
local env = {}

local function error_handler(errmsg)
    sevo.error(debug.traceback(tostring(errmsg), 3))
end

function sevo.parallel()
    -- Hack require for extensions
    env.require = _G["require"]
    _G["require"] = function(name)
        local ok, mod

        -- find global
        ok, mod = pcall(env.require, name)
        if ok then return mod end

        -- find sevo extensions
        ok, mod = pcall(env.require, "_extensions_." .. name)
        if ok then return mod end

        -- find project extensions
        ok, mod = pcall(env.require, "extensions." .. name)
        if ok then return mod end

        return error("module '" .. name .. "' not found:")
    end

    require("sevo.int")
    require("sevo.logger")
    require("sevo.vfs")
    require("sevo.thread")

    -- Redirect print to sevo.info
    _G["print"] = sevo.info
end

return function()
    sevo.parallel()
    sevo.thread.run()
end
