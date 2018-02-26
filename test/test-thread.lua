require("sevo.time")

print("Thread started.")

while not sevo.thread.readable() do
    sevo.time.sleep(10)
end

print("Thread Read: " .. sevo.thread.read())

sevo.thread.write("World.")

print("Thread stop.")
