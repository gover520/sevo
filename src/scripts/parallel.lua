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
end

return function()
    sevo.parallel()
    sevo.thread.run()
end
