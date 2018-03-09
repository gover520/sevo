print("Thread in.")

emiter = sevo.emitter()

emiter:on("test", function(msg)
        print("test01: " .. msg)
    end)
emiter:once("test", function(msg)
        print("test once: " .. msg)
    end)
emiter:on("test", function(msg)
        print("test02: " .. msg)
    end)

emiter:emit("test", "Hello")
emiter:emit("test", "World")


local pid = sevo.spawn(
    function()
        print("Thread spawn a worker")
        local msg = sevo.receive()
        print("From " .. msg.from .. ": " .. msg.body)
        sevo.send(msg.from, "Thread will end.")
    end)

sevo.settimeout(
    function()
        sevo.send(pid, { from = sevo.self(), body = "Thread please quit!" })
        local msg = sevo.receive()
        print(msg)
    end, 100)


local intval = sevo.setinterval(
    function()
        if sevo.thread.readable() then
            print("Thread Read: " .. sevo.thread.read())
            sevo.thread.write("World.")

            sevo.clearinterval(sevo.self())
        end
    end, 10)

print("Thread interval: " .. intval)
print("Thread out.")

return function(delta)
    -- This is a tick update callback.
end
