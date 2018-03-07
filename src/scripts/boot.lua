------------------------------------------------------------
--  boot.lua
--
--  copyright (c) 2018 Xiongfei Shi
--
--  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
--  license: Apache-2.0
------------------------------------------------------------

local sevo = require("sevo")
local env = {}

function env.error_handler(errmsg)
    sevo.error(debug.traceback(tostring(errmsg), 3))
end

function sevo.boot()
    require("sevo.int")
    require("sevo.logger")
    require("sevo.vfs")

    local function get_fullpath(p)
        local np = p:gsub("\\", "/")

        if np:find("/") == 1 or np:find("%a:") == 1 then
            return np
        end

        local cwd = sevo.vfs.getcwd()
        cwd = cwd:gsub("\\", "/")

        if cwd:sub(-1) ~= "/" then
            cwd = cwd .. "/"
        end

        return cwd .. np
    end

    if #arg < 2 then
        sevo.error("Parameter error, no working directory!")
        return false
    end

    -- source directory
    local fullpath = get_fullpath(arg[2])
    local _, mdir = xpcall(sevo.vfs.mount, env.error_handler, fullpath, "/")
    if not mdir then
        local _, mzip = xpcall(sevo.vfs.mount, env.error_handler, fullpath .. ".zip", "/")
        if not mzip then
            sevo.error("Source mounting failed, " .. arg[2])
            return false
        end
    end

    return true
end

function sevo.init()
    env.conf = {
        version = sevo._VERSION,
        loglevel = "debug",
        cookie = "",
        tick = 10,  -- Tick time 10ms
        forcequit = false,
    }

    local result

    -- configure
    if sevo.vfs.info("conf.lua") then
        result = xpcall(require, env.error_handler, "conf")
        if not result then return false end
    end

    if sevo.conf then
        result = xpcall(sevo.conf, env.error_handler, env.conf)
        if not result then return false end
    end

    sevo.loglevel(env.conf.loglevel)

    for i, v in ipairs({
        "id",
        "time",
        "event",
        "hash",
        "rand",
        "net",
        "secure",
        "thread",
        "dbm",
        "env",
        "socket"
    }) do
        require("sevo." .. v)
    end

    -- event
    if sevo.event then
        local function createhandlers()
            env.handlers = setmetatable({
                quit = function()
                    return
                end,
            }, {
                __index = function(self, name)
                    error("Unknown event: " .. name)
                end,
            })
        end

        sevo.event.init()
        createhandlers()
    end

    -- servo
    if not sevo.vfs.info("servo.lua") then
        sevo.error("'servo.lua' is not found! What can i do for you?")
        return false
    end

    result = xpcall(require, env.error_handler, "servo")
    if not result then return false end

    -- FPS
    env.conf.fps = 1000 / env.conf.tick

    return true
end

function sevo.run()
    if sevo.load then sevo.load(arg) end

    local fps = sevo.time.fps(env.conf.fps)

    return function()
        fps:update()

        local alive = sevo.scheduler(fps:delta())

        if sevo.event then
            sevo.event.pump()

            for name, a, b, c, d, e, f in sevo.event.poll() do
                if name == "quit" then
                    if sevo.quit then sevo.quit() end

                    if alive and not env.conf.forcequit then
                        sevo.warn("Some process still alived, waiting for scheduling.")

                        while alive do
                            fps:update()
                            alive = sevo.scheduler(fps:delta())
                            fps:wait();
                        end
                    end

                    return a or 0
                end
                env.handlers[name](a, b, c, d, e, f)
            end
        end

        if sevo.update then sevo.update(fps:delta()) end

        fps:wait();
    end
end

return function ()
    local func

    local function earlyinit()
        local _, isbooted = xpcall(sevo.boot, env.error_handler)
        if not isbooted then return 1 end

        local _, isinited = xpcall(sevo.init, env.error_handler)
        if not isinited then return 1 end

        local result, main = xpcall(sevo.run, env.error_handler)
        if not result then return 1 end

        func = main
    end

    func = earlyinit

    while func do
        local _, retval = xpcall(func, env.error_handler)
        if retval then return retval end
        coroutine.yield()
    end

    return 1
end
