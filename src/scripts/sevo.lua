------------------------------------------------------------
--  sevo.lua
--
--  copyright (c) 2018 Xiongfei Shi
--
--  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
--  license: Apache-2.0
------------------------------------------------------------

-- Hack require for extensions
_require_ = _G["require"]
_G["require"] = function(name)
    local ok, mod

    -- find global
    ok, mod = pcall(_require_, name)
    if ok then return mod end

    -- find sevo extensions
    ok, mod = pcall(_require_, "_extensions_." .. name)
    if ok then return mod end

    -- find project extensions
    ok, mod = pcall(_require_, "extensions." .. name)
    if ok then return mod end

    return error("module '" .. name .. "' not found:")
end
