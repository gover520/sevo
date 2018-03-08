------------------------------------------------------------
--  parallel.lua
--
--  copyright (c) 2018 Xiongfei Shi
--
--  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
--  license: Apache-2.0
------------------------------------------------------------

local sevo = require("sevo")

function sevo.parallel()
    require("sevo.int")
    require("sevo.logger")
    require("sevo.vfs")
    require("sevo.thread")
    require("sevo.time")
end

return function()
    sevo.parallel()

    local func, tick = sevo.thread.run()
    tick = tick or 10

    local alive = true
    local fps = sevo.time.fps(1000 / tick)

    while alive do
        fps:update()
        alive = sevo.scheduler(fps:delta())
        if alive and func then func(fps:delta()) end
        fps:wait();
    end
end
