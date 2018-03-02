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

env.names = {}          -- Process names and PIDs associative table.
env.processes = {}      -- All the processes in the system.
env.ondeath = {}        -- Functions to execute on abnormal exit.
env.ondestruction = {}  -- Functions to execute on termination.
env.mailboxes = {}      -- Mailboxes associated with processes.
env.timeouts = {}       -- Timeouts for processes that are suspended.

local function error_handler(errmsg)
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
    local _, mdir = xpcall(sevo.vfs.mount, error_handler, fullpath, "/")
    if not mdir then
        local _, mzip = xpcall(sevo.vfs.mount, error_handler, fullpath .. ".zip", "/")
        if not mzip then
            sevo.error("Source mounting failed, " .. arg[2])
            return false
        end
    end

    -- concurrent
    sevo.whois = function(co)
        for k, v in pairs(env.processes) do
            if co == v then return k end
        end
    end

    sevo.self = function()
        local co = coroutine.running()
        if co then return sevo.whois(co) end
    end

    sevo.isalive = function(pid)
        local co = env.processes[pid]
        if co and type(co) == "thread" and coroutine.status(co) ~= "dead" then
            return true
        end
        return false
    end

    local function process_destory()
        for _, fn in ipairs(env.ondestruction) do
            fn(sevo.self(), "normal")
        end
    end

    local function process_die(pid, reason)
        for _, fn in ipairs(env.ondeath) do
            fn(pid, reason)
        end
    end

    local function process_resume(co, ...)
        if type(co) ~= "thread" or coroutine.status(co) ~= "suspended" then
            return
        end
        local status, errmsg = coroutine.resume(co, ...)
        if not status then
            local pid = sevo.whois(co)
            process_die(pid, errmsg)
        end
        return status, errmsg
    end

    sevo.spawn = function(func, ...)
        local co = coroutine.create(
            function(...)
                coroutine.yield()
                func(...)
                process_destory()
            end)
        table.insert(env.processes, co)
        local pid = #env.processes
        env.mailboxes[pid] = {}
        env.timeouts[pid] = 0
        local status, errmsg = process_resume(co, ...)
        if not status then return nil, errmsg end
        return pid
    end

    sevo.kill = function(pid, reason)
        if type(env.processes[pid]) == "thread" and
            coroutine.status(env.processes[pid]) == "suspended"
        then
            local status, errmsg = coroutine.resume(env.processes[pid], "exit")
            process_die(pid, reason)
        end
    end

    sevo.whereis = function(name)
        if type(name) == "number" then return name end
        if not env.names[name] then return end
        return env.names[name]
    end

    sevo.register = function(name, pid)
        if sevo.whereis(name) then return false end
        if not pid then pid = sevo.self() end
        env.names[name] = pid
        return true
    end

    sevo.unregister = function(name)
        if not name then name = sevo.self() end
        for k, v in pairs(env.names) do
            if name == k or name == v then
                env.names[k] = nil
                return true
            end
        end
        return false
    end

    sevo.registered = function()
        local n = {}
        for k, _ in pairs(env.names) do table.insert(n, k) end
        return n
    end

    table.insert(env.ondeath, sevo.unregister)
    table.insert(env.ondestruction, sevo.unregister)

    return true
end

function sevo.init()
    local c = {
        version = sevo._VERSION,
        loglevel = "debug",
        cookie = "",
    }

    local result

    if sevo.vfs.info("conf.lua") then
        result = xpcall(require, error_handler, "conf")
        if not result then return false end
    end

    if sevo.conf then
        result = xpcall(sevo.conf, error_handler, c)
        if not result then return false end
    end

    sevo.loglevel(c.loglevel)

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
    }) do
        require("sevo." .. v)
    end

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

    if not sevo.vfs.info("servo.lua") then
        sevo.error("'servo.lua' is not found! What can i do for you?")
        return false
    end

    result = xpcall(require, error_handler, "servo")
    if not result then return false end

    return true
end

function sevo.run()
    if sevo.load then sevo.load(arg) end

    local fps = sevo.time.fps(100)

    return function()
        fps:wait();

        if sevo.event then
            sevo.event.pump()

            for name, a, b, c, d, e, f in sevo.event.poll() do
                if name == "quit" then
                    if sevo.quit then sevo.quit() end
                    return a or 0
                end
                env.handlers[name](a, b, c, d, e, f)
            end
        end

        if sevo.update then sevo.update(fps:delta()) end
    end
end

return function ()
    local func

    local function earlyinit()
        local _, isbooted = xpcall(sevo.boot, error_handler)
        if not isbooted then return 1 end

        local _, isinited = xpcall(sevo.init, error_handler)
        if not isinited then return 1 end

        local result, main = xpcall(sevo.run, error_handler)
        if not result then return 1 end

        func = main
    end

    func = earlyinit

    while func do
        local _, retval = xpcall(func, error_handler)
        if retval then return retval end
        coroutine.yield()
    end

    return 1
end
