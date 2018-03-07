------------------------------------------------------------
--  echo/servo.lua
--
--  copyright (c) 2018 Xiongfei Shi
--
--  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
--  license: Apache-2.0
------------------------------------------------------------

local socket = require("socket")

local host = "127.0.0.1"
local port = 4433

local listen, client

function sevo.load(args)
    -- 查看luasocket版本
    print("socket version: " .. socket._VERSION)

    -- 创建一个TCP服务
    listen = socket.bind(host, port)
    print("Listen on: " .. tostring(listen:getsockname()))
end

function sevo.update(delta)
    local readable, status

    if client then
        readable, _, status = socket.select({ client }, nil, 0)
        if #readable > 0 then
            -- 有数据到达
            local command, msg = client:receive()

            if command == nil or msg == "close" then
                -- 断开连接
                print("Lost connection: " .. tostring(client:getpeername()))
                client = nil
            else
                -- 打印收到的内容
                print(command)

                if command == "quit" then
                    -- 退出服务
                    sevo.event.quit()
                elseif command == "restart" then
                    -- 重启服务
                    sevo.event.quit("restart")
                end

                -- 回显给客户端
                client:send(command .. "\n")
            end
        end
    else
        readable, _, status = socket.select({ listen }, nil, 0)
        if #readable > 0 then
            -- 新连接到达
            client = listen:accept()
            print("New connection: " .. tostring(client:getpeername()))
        end
    end
end

function sevo.quit()
    listen:close()

    if client then
        client:close()
    end
end
