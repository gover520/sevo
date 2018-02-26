------------------------------------------------------------
--  event.lua
--
--  copyright (c) 2018 Xiongfei Shi
--
--  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
--  license: Apache-2.0
------------------------------------------------------------

sevo.event.poll_i = function()
    return table.unpack(sevo.event.poll_t() or {})
end

sevo.event.poll = function()
    return sevo.event.poll_i
end

sevo.event.push = function(...)
    sevo.event.push_t(table.pack(...))
end

sevo.event.quit = function(a)
    sevo.event.push("quit", a or 0)
end
