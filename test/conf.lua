
function sevo.conf(c)
    print("conf version: " .. c.version)
    print("conf fps: " .. c.fps)

    c.fps = 5

    -- set writeable directory
    sevo.vfs.identity("test")
end
