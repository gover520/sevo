------------------------------------------------------------
--  sevo.lua
--
--  copyright (c) 2018 Xiongfei Shi
--
--  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
--  license: Apache-2.0
------------------------------------------------------------

-- todo: distributed

local M = {}            -- Submodule for concurrent.

M.names = {}            -- Process names and PIDs associative table.
M.processes = {}        -- All the processes in the system.
M.mailboxes = {}        -- Mailboxes associated with processes.
M.timeouts = {}         -- Timeouts for processes that are suspended.
M.links = {}            -- Active links between processes.
M.monitors = {}         -- Active monitors between processes.
M.ondeath = {}          -- Functions to execute on abnormal exit.
M.ondestruction = {}    -- Functions to execute on termination.

-- Root process has PID of 0.
M.processes[0] = coroutine.running()

-- Root process mailbox.
M.mailboxes[0] = {}

local function process_destory()
    local pid = M.self()
    for _, fn in ipairs(M.ondestruction) do
        fn(pid, "normal")
    end
end

local function process_die(pid, reason)
    for _, fn in ipairs(M.ondeath) do
        fn(pid, reason)
    end
end

local function process_resume(co, ...)
    if type(co) ~= "thread" or coroutine.status(co) ~= "suspended" then
        return
    end
    local status, errmsg = coroutine.resume(co, ...)
    if not status then
        local pid = M.whois(co)
        process_die(pid, errmsg)
    end
    return status, errmsg
end

local function scheduler_yield()
    if coroutine.yield() == "EXIT" then M.exit("EXIT") end
end

local function sleep_yield(pid, timeout)
    if pid ~= 0 then return scheduler_yield() end

    local start = sevo.time.millisec()

    while true do
        if #M.mailboxes[pid] > 0 then break end

        local now = sevo.time.millisec()
        local elapsed = now - start

        if M.timeouts[pid] and elapsed >= M.timeouts[pid] then
            M.timeouts[pid] = nil
            return
        end

        sevo.scheduler(elapsed)
        sevo.time.sleep(0)
    end
end

local function process_signal_all(dead, reason)
    if M.links[dead] == nil then return end
    for _, v in pairs(M.links[dead]) do
        M.signal(v, dead, reason)
    end
    M.links[dead] = nil
end

local function process_unlink_all()
    local s = M.self()
    if M.links[s] == nil then return end
    for _, v in pairs(M.links[s]) do
        M.unlink(v)
    end
    M.links[s] = nil
end

local function monitor_notify_all(dead, reason)
    if M.monitors[dead] == nil then return end
    for _, v in pairs(M.monitors[dead]) do
        M.notify(v, dead, reason)
    end
    M.monitors[dead] = nil
end

M.whois = function(co)
    for k, v in pairs(M.processes) do
        if co == v then return k end
    end
end

M.whereis = function(name)
    if type(name) == "number" then return name end
    if not M.names[name] then return end
    return M.names[name]
end

M.self = function()
    local co = coroutine.running()
    if co then return M.whois(co) end
end

M.isalive = function(pid)
    local co = M.processes[pid]
    if co and type(co) == "thread" and coroutine.status(co) ~= "dead" then
        return true
    end
    return false
end

M.register = function(name, pid)
    if M.whereis(name) then return false end
    if not pid then pid = M.self() end
    M.names[name] = pid
    return true
end

M.unregister = function(name)
    if not name then name = M.self() end
    for k, v in pairs(M.names) do
        if name == k or name == v then
            M.names[k] = nil
            return true
        end
    end
    return false
end

M.registered = function()
    local n = {}
    for k, _ in pairs(M.names) do table.insert(n, k) end
    return n
end

M.spawn = function(func, ...)
    local co = coroutine.create(
        function(...)
            coroutine.yield()
            func(...)
            process_destory()
        end)
    table.insert(M.processes, co)
    local pid = #M.processes    -- todo: generate unique process id
    M.mailboxes[pid] = {}
    M.timeouts[pid] = 0
    local status, errmsg = process_resume(co, ...)
    if not status then return nil, errmsg end
    return pid
end

M.exit = function(reason)
    error(reason, 0)
end

M.kill = function(pid, reason)
    if type(M.processes[pid]) == "thread" and
        coroutine.status(M.processes[pid]) == "suspended"
    then
        local status, errmsg = coroutine.resume(M.processes[pid], "EXIT")
        process_die(pid, reason)
    end
end

M.sleep = function(timeout)
    local pid = M.self()
    if timeout then M.timeouts[pid] = timeout end
    sleep_yield(pid, timeout)
    if timeout then M.timeouts[pid] = nil end
end

M.send = function(dest, msg)
    local pid = M.whereis(dest)
    if not pid then return false end
    table.insert(M.mailboxes[pid], msg)
    return true
end

M.receive = function(timeout)
    local pid = M.self()
    if #M.mailboxes[pid] == 0 then M.sleep(timeout) end
    if #M.mailboxes[pid] > 0 then
        return table.remove(M.mailboxes[pid], 1)
    end
end

M.scheduler = function(delta)
    local alive = false

    for k, v in pairs(M.processes) do
        if M.timeouts[k] then
            M.timeouts[k] = M.timeouts[k] - delta
        end

        if #M.mailboxes[k] > 0 or (M.timeouts[k] and M.timeouts[k] <= 0) then
            if M.timeouts[k] then M.timeouts[k] = nil end
            process_resume(v)
        end

        if not alive and coroutine.status(v) ~= "dead" then
            if k ~= 0 then alive = true end
            -- todo: process cleanup
        end
    end

    return alive
end

M.link = function(dest)
    local s = M.self()
    local pid = M.whereis(dest)
    if not pid then return end
    if M.links[s] == nil then M.links[s] = {} end
    if M.links[pid] == nil then M.links[pid] = {} end
    for _, v in pairs(M.links[s]) do
        if pid == v then return end
    end
    table.insert(M.links[s], pid)
    table.insert(M.links[pid], s)
end

M.spawnlink = function(...)
    local pid, errmsg = M.spawn(...)
    if not pid then return nil, errmsg end
    M.link(pid)
    return pid
end

M.unlink = function(dest)
    local s = M.self()
    local pid = M.whereis(dest)
    if not pid then return end
    if M.links[s] == nil or M.links[pid] == nil then
        return
    end
    for k, v in pairs(M.links[s]) do
        if pid == v then M.links[s][k] = nil end
    end
    for k, v in pairs(M.links[pid]) do
        if s == v then M.links[pid][k] = nil end
    end
end

M.signal = function(dest, dead, reason)
    M.kill(dest, reason)
end

M.monitor = function(dest)
    local s = M.self()
    local pid = M.whereis(dest)
    if not pid then return end
    if M.monitors[pid] == nil then M.monitors[pid] = {} end
    for _, v in pairs(M.monitors[pid]) do
        if s == v then return end
    end
    table.insert(M.monitors[pid], s)
end

M.spawnmonitor =  function(...)
    local pid, errmsg = M.spawn(...)
    if not pid then return nil, errmsg end
    M.monitor(pid)
    return pid
end

M.demonitor = function(dest)
    local s = M.self()
    local pid = M.whereis(dest)
    if not pid then return end
    if M.monitors[pid] == nil then return end
    for k, v in pairs(M.monitors[pid]) do
        if s == v then M.monitors[pid][k] = nil end
    end
end

M.notify = function(dest, dead, reason)
    M.send(dest, { signal = 'DOWN', from = dead, reason = reason })
end

table.insert(M.ondeath, M.unregister)
table.insert(M.ondestruction, M.unregister)

table.insert(M.ondeath, process_signal_all)
table.insert(M.ondestruction, process_unlink_all)

table.insert(M.ondeath, monitor_notify_all)
table.insert(M.ondestruction, monitor_notify_all)

for k, v in pairs(env.concurrent) do
    if type(v) == "function" then
        sevo[k] = v
    end
end
