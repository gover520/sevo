
function sevo.conf(c)
    print("-- configure info --")
    for k, v in pairs(c) do
        if k == "modules" then
            print(k .. ":")
            for l, m in pairs(v) do
                print(" " .. l .. " = " .. tostring(m))
            end
        else
            print(k .. " = " .. tostring(v))
        end
    end
    print("----- end -----")

    print(package.path)
    print(package.cpath)

    -- you can modify configure in here
    c.nodename = "test" -- 设置节点名称，@前半部分将会是个可写目录
    c.cookie = "12345678"
    --c.forcequit = true    -- 不等待调度完毕
end
