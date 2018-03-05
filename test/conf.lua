
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

    -- you can modify configure in here
    c.cookie = "12345678"
    --c.forcequit = true    -- 不等待调度完毕

    print(package.path)
    print(package.cpath)

    -- set writeable directory
    sevo.vfs.identity("test")
end
