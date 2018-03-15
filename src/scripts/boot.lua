------------------------------------------------------------
--  boot.lua
--
--  copyright (c) 2018 Xiongfei Shi
--
--  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
--  license: Apache-2.0
------------------------------------------------------------

local sevo = require("sevo")
local M = {}

M.queue = {}    -- The events queue

function M.error_handler(errmsg)
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
    local _, mdir = xpcall(sevo.vfs.mount, M.error_handler, fullpath, "/")
    if not mdir then
        local _, mzip = xpcall(sevo.vfs.mount, M.error_handler, fullpath .. ".zip", "/")
        if not mzip then
            sevo.error("Source mounting failed, " .. arg[2])
            return false
        end
    end

    return true
end

function sevo.init()
    M.conf = {
        nodename = nil,
        version = sevo._VERSION,
        loglevel = "debug",
        cookie = "",
        tick = 10,  -- Tick time 10ms
        forcequit = false,
    }

    local result

    -- configure
    if sevo.vfs.info("conf.lua") then
        result = xpcall(require, M.error_handler, "conf")
        if not result then return false end
    end

    if sevo.conf then
        result = xpcall(sevo.conf, M.error_handler, M.conf)
        if not result then return false end
    end

    sevo.loglevel(M.conf.loglevel)

    -- nodename
    if not M.conf.nodename then
        local np = arg[2]:gsub("\\", "/")
        local entry = string.match(np, "^.+/(.+)$") or np
        M.conf.nodename = string.match(entry, "(.+)%.%w+$") or entry
    end

    if not string.find(M.conf.nodename, '@') then
        local socket = require("socket")
        local hostname = socket.dns.gethostname()
        local _, resolver = socket.dns.toip(hostname)
        local fqdn
        for _, v in pairs(resolver.ip) do
            fqdn, _ = socket.dns.tohostname(v)
            if string.find(fqdn, '%w+%.%w+') then break end
            fqdn = nil
        end

        M.conf.nodename = M.conf.nodename .. "@" .. (fqdn or hostname)
    end

    local node, host = string.match(M.conf.nodename, "^(%a[%w_]*)@(.+)$")

    sevo.node(M.conf.nodename)
    sevo.vfs.identity(node)

    -- event
    sevo.event = {}

    sevo.event.pump = function()
    end

    sevo.event.poll = function()
        return function()
            local data = table.remove(M.queue, 1)
            return table.unpack(data or {})
        end
    end

    sevo.event.push = function(...)
        table.insert(M.queue, table.pack(...))
    end

    sevo.event.quit = function(a)
        sevo.event.push("quit", a or 0)
    end

    M.handlers = setmetatable({
        quit = function()
            return
        end,
    }, {
        __index = function(self, name)
            error("Unknown event: " .. name)
        end,
    })

    for i, v in ipairs({
        "id",
        "time",
        "hash",
        "rand",
        "net",
        "secure",
        "thread",
        "dbm",
        "env",
        "math",
    }) do
        require("sevo." .. v)
    end

    -- servo
    if not sevo.vfs.info("servo.lua") then
        sevo.error("'servo.lua' is not found! What can i do for you?")
        return false
    end

    result = xpcall(require, M.error_handler, "servo")
    if not result then return false end

    -- FPS
    M.conf.fps = 1000 / M.conf.tick

    return true
end

function sevo.run()
    if sevo.load then sevo.load(arg) end

    local fps = sevo.time.fps(M.conf.fps)

    return function()
        fps:update()

        local alive = sevo.scheduler(fps:delta())

        if sevo.event then
            sevo.event.pump()

            for name, a, b, c, d, e, f in sevo.event.poll() do
                if name == "quit" then
                    if sevo.quit then sevo.quit() end

                    if alive and not M.conf.forcequit then
                        sevo.warn("Some process still alived, waiting for scheduling.")

                        while alive do
                            fps:update()
                            alive = sevo.scheduler(fps:delta())
                            fps:wait();
                        end
                    end

                    return a or 0
                end
                M.handlers[name](a, b, c, d, e, f)
            end
        end

        if sevo.update then sevo.update(fps:delta()) end

        fps:wait();
    end
end

return function ()
    local func

    local function earlyinit()
        local _, isbooted = xpcall(sevo.boot, M.error_handler)
        if not isbooted then return 1 end

        local _, isinited = xpcall(sevo.init, M.error_handler)
        if not isinited then return 1 end

        local result, main = xpcall(sevo.run, M.error_handler)
        if not result then return 1 end

        func = main
    end

    func = earlyinit

    while func do
        local _, retval = xpcall(func, M.error_handler)
        if retval then return retval end
        coroutine.yield()
    end

    return 1
end
