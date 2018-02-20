
function sevo.conf(c)
    print('conf version: ' .. c.version)
    print('conf fps: ' .. c.fps)

    -- set writeable directory
    sevo.vfs.identity('test')
end
