print("Thread started.")

while not sevo.thread.readable() do
    sevo.time.sleep(10)
end

print("Thread Read: " .. sevo.thread.read())

sevo.thread.write("World.")

local pid = sevo.spawn(
    function()
        print("Thread spawn a worker")
        local msg = sevo.receive()
        print("From " .. msg.from .. ": " .. msg.body)
        sevo.send(msg.from, "Thread will end.")
    end)

print("Thread stop.")

return function(delta)
    sevo.send(pid, { from = sevo.self(), body = "Thread please quit!" })
    local msg = sevo.receive()
    print(msg)
end
