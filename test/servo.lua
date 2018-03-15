
local test_func = {
    function()
        print("-- Show version --")
        print("Sevo Version: " .. sevo._VERSION)
        print("Sevo Version Num: " .. sevo._VERSION_NUM)
        print("Sevo Version Full: " .. sevo._VERSION_FULL)
        print("Sevo Version Major: " .. sevo._VERSION_MAJOR)
        print("Sevo Version Minor: " .. sevo._VERSION_MINOR)
        print("Sevo Version Patch: " .. sevo._VERSION_PATCH)
        print("Sevo OS: " .. sevo._OS)
        print("CPU Num: " .. sevo._CPUNUM)
    end,
    function()
        print("-- Test re --")
        local re = require("re")
        print(re.find("the number 423 is odd", "[0-9]+"))           --> 12    14
        print(re.match("the number 423 is odd", "({%a+} / .)*"))    --> the    number    is    odd
        print(re.match("the number 423 is odd", "s <- {%d+} / . s"))--> 423
        print(re.gsub("hello World", "[aeiou]", "."))               --> h.ll. W.rld
    end,
    function()
        print("-- Test bigint --")
        local a = sevo.int(0)
        print("type(a):", type(a))
        print("type(0):", type(0))
        print("type(#a):", type(#a))
        print("0 == a:", 0 == a)
        print("a == 0:", a == 0)    -- false, Lua equality must be 2 same objects
        print("a == sevo.int(0): ", a == sevo.int(0))   -- true
        print("a:eq(0):", a:eq(0))  -- a == 0
        print("#a == 0:", #a == 0)  -- true
        print("a < 0:", a < 0)
        print("a <= 0:", a <= 0)
        print("a > 0:", a > 0)
        print("a >= 0:", a >= 0)
        print("0 < a:", 0 < a)
        print("0 <= a:", 0 <= a)
        print("0 > a:", 0 > a)
        print("0 >= a:", 0 >= a)

        print("a:", a)
        print("#a:", #a)
        print("2 ^ 100:", sevo.int(2) ^ 100)

        local f = sevo.int("1234567890987654321234567890987654321")
        print("f:", f)
        print("-f:", -f)
        print("~f", ~f)

        local e = sevo.int(-9)
        print("e:", e)
        print("e / 2:", e / 2)
        print("e %% 2:", e % 2)
        print("e // 2:", e // 2)
        print("~e:", ~e)

        local b = sevo.int(123456789)
        print("b:", b)
        print("~b:", ~b)
        print("f & b:", f & b)
        print("f | b:", f | b)
        print("f ~ b:", f ~ b)
        print("2 * b:", 2 * b)

        print("f * b:", f * b)
        print("-b:", -b)
    end,
    function()
        print("-- Test factorial --")

        local function fact_iter(accum, step)
            if step <= 1 then
                return accum
            end
            return fact_iter(accum * step, step - 1)
        end

        print("factorial(100):", fact_iter(sevo.int(1), 100))
    end,
    function()
        print("-- Test fibonacci --")

        local function fib_iter(a, b, step)
            if step <= 0 then
                return a
            end
            return fib_iter(a + b, a, step - 1)
        end

        print("fibonacci(100):", fib_iter(sevo.int(0), sevo.int(1), 100))
    end,
    function()
        print("-- Test gcd --")

        local function gcd(a, b)
            if a:eq(0) then
                return b
            end
            return gcd(b % a, a)
        end

        print("gcd:", gcd(sevo.int(123456789), sevo.int(135792468)))
    end,
    function()
        print("-- Test id --")
        local ids = {}

        sevo.id.init(1234)

        for i=1, 10 do
            table.insert(ids, sevo.id.next())
        end

        for i, v in ipairs(ids) do
            local ts, nodeid, seq = sevo.id.split(v)
            print("ID: " .. v .. ", Timestamp: " .. ts .. ", NodeID: " .. nodeid .. ", Sequence: " .. seq)
        end
    end,
    function()
        print("-- Test timer --")
        local function timer_iter(t, timeout)
            if t:expired() then
                if timeout > 0 then
                    print("Timer expired: " .. timeout)
                    t:set(timeout - 1000)
                    return timer_iter(t, timeout - 1000)
                end
                print("timer finish")
            else
                print("timer sleep")
                sevo.time.sleep(200)
                return timer_iter(t, timeout)
            end
        end
        timer_iter(sevo.time.timer(2000), 2000)
    end,
    function()
        print("-- Test ffi --")
        local ffi = require("ffi")

        ffi.cdef[[
            typedef struct { double x, y; } point_t;
            int printf(const char *fmt, ...);
        ]]

        ffi.C.printf("Hello %s!\n", "world")

        local point
        local mt = {
            __add = function(a, b) return point(a.x+b.x, a.y+b.y) end,
            __len = function(a) return math.sqrt(a.x*a.x + a.y*a.y) end,
            __index = {
                area = function(a) return a.x*a.x + a.y*a.y end,
            },
        }
        point = ffi.metatype("point_t", mt)

        local a = point(3, 4)

        print("a.x, a.y:", a.x, a.y)     --> 3  4
        print("#a:", #a)           --> 5
        print("a:area():", a:area())     --> 25

        local b = a + point(0.5, 8)

        print("#b:", #b)           --> 12.5
    end,
    function()
        print(sevo.hash.h32("123456"))
        print(sevo.hash.h64("123456"))
        print(sevo.hash.m32("123456"))
        print(sevo.hash.m64("123456"))
        print(sevo.hash.crc32("123456"))
        print(sevo.hash.crc64("123456"))

        local a1 = sevo.hash.adler32("HelloWorld.")
        local a2 = sevo.hash.adler32("Hello")
        local a3 = sevo.hash.adler32("World.")
        local a4 = sevo.hash.adler32_combine(a2, a3, 6)
        local a5 = sevo.hash.adler32("elloWorld.")
        local a6 = sevo.hash.adler32("HelloWorld")
        local a7 = sevo.hash.adler32_rolling(a6, string.byte('.'), string.byte('H'), 10)

        print("a1: " .. a1)
        print("a2: " .. a2)
        print("a3: " .. a3)
        print("a4: " .. a4 .. " should be equal a1: " .. a1)
        print("a5: " .. a5)
        print("a6: " .. a6)
        print("a7: " .. a7 .. " should be equal a5: " .. a5)

        local m1 = sevo.hash.md5()
        m1:update("123456")
        print(m1:digest())

        local m2 = sevo.hash.md5()
        m2:update("123456")
        print(m2:hexdigest())

        local s1 = sevo.hash.sha1()
        s1:update("123456")
        print(s1:digest())

        local s2 = sevo.hash.sha1()
        s2:update("123456")
        print(s2:hexdigest())
    end,
    function()
        print("RandMax: " .. sevo.rand.randmax)
        for i = 0, 10 do print("Rand: " .. sevo.rand.randint()) end
        for i = 0, 10 do print("Rand10: " .. sevo.rand.randint(10)) end
        for i = 0, 10 do print("Rand10-100: " .. sevo.rand.randint(10, 100)) end
        for i = 0, 10 do print("Random: " .. sevo.rand.random()) end
        for i = 0, 10 do print("Prime: " .. sevo.rand.prime()) end
        for i = 0, 10 do print("Prime10: " .. sevo.rand.prime(10)) end
        for i = 0, 10 do print("Prime10-100: " .. sevo.rand.prime(10, 100)) end
        for i = 0, 10 do print("Normal: " .. sevo.rand.normal(5., 1.)) end
        for i = 0, 10 do print("Exp: " .. sevo.rand.exp(2.)) end
        for i = 0, 10 do print("Poisson: " .. sevo.rand.poisson(4000.)) end
        for i = 0, 10 do print("Binomial: " .. sevo.rand.binomial(4000, 0.3)) end
    end,
    function()
        sevo.vfs.mkdir("dir/info")
        sevo.vfs.write("dir/info/lasttime.txt", tostring(os.date()))
        print(sevo.vfs.read("dir/info/lasttime.txt"))

        local fs = sevo.vfs.files("dir/info")

        print("Files: " .. #fs)
        for i, v in ipairs(fs) do
            print(i, v)
        end

        sevo.vfs.remove("dir/info/lasttime.txt")
        sevo.vfs.remove("dir/info")
        sevo.vfs.remove("dir")
    end,
    function()
        local iv = "12345678"
        xtea = sevo.secure.xtea(sevo.hash.md5("123456"):digest())
        local e1, i = xtea:encode(iv, "HelloWorld.")
        local e2 = xtea:encode(i, "1234567890")
        local d1, j = xtea:decode(iv, e1)
        local d2 = xtea:decode(j, e2)

        print(e1, i)
        print(e2)
        print(d1, j)
        print(d2)
    end,
    function()
        local mode = "udp"
        local n = sevo.net.new()

        local s = n:server(mode, "0.0.0.0", 12345)
        local c = n:connect(mode, "127.0.0.1", 12345)

        local a, p = s:addr("local")
        print("Listen on " .. a .. ":" .. p)

        local function addrstr(a, p)
            return a .. ":" .. p
        end

        local function sessmode(e)
            return "Session(" .. e.who:mode() .. ") "
        end

        local function process_event(n, r)
            local e = n:receive()

            if not e then
                return r
            end

            local a, p = e.who:addr("remote")

            if e.cmd == "incoming" then
                print(sessmode(e) .. addrstr(a, p) .. " incoming")
            elseif e.cmd == "conn-timeout" then
                print(sessmode(e) .. addrstr(a, p) .. " connect timeout")
            elseif e.cmd == "halo" then
                print(sessmode(e) .. addrstr(a, p) .. " halo, " .. e.protover)
                e.who:auth("123456")
            elseif e.cmd == "auth" then
                print(sessmode(e) .. addrstr(a, p) .. " (" .. e.who:addr("hwaddr") .. ") auth, " .. e.passwd)
                if sevo.rand.randint(100) > 10 then
                    e.who:accept("Hello Guys!")
                else
                    e.who:reject("Ugly!");
                end
            elseif e.cmd == "accepted" then
                print(sessmode(e) .. addrstr(a, p) .. " accepted, " .. e.welcome)
                e.who:ping(sevo.time.millisec())
                e.who:send("Really Good!")
            elseif e.cmd == "rejected" then
                print(sessmode(e) .. addrstr(a, p) .. " rejected, " .. e.reason)
            elseif e.cmd == "ping" then
                print(sessmode(e) .. addrstr(a, p) .. " ping, " .. e.time)
                e.who:pong(e.time)
            elseif e.cmd == "pong" then
                print(sessmode(e) .. addrstr(a, p) .. " pong, " .. e.time)
            elseif e.cmd == "outgoing" then
                print(sessmode(e) .. addrstr(a, p) .. " outgoing")
                r = r + 1
            elseif e.cmd == "data" then
                print(sessmode(e) .. addrstr(a, p) .. " data, " .. e.data)
                e.who:send("That's all right!")
                e.who:close()
            end

            return process_event(n, r)
        end

        local keep_running = 2
        local f = sevo.time.fps(100)

        while keep_running > 0 do
            f:wait()
            keep_running = keep_running - process_event(n, 0)
            n:update()
        end
    end,
    function()
        local t1 = sevo.thread.new("test-thread")
        local t2 = sevo.thread.new("th1", [[
                print("Th1 started.")
                print(coroutine.running())
                print("Th1 stop.")
            ]])
        print("Status1: " .. t1:status())
        print("Status2: " .. t2:status())

        t1:start()
        print("Status1: " .. t1:status())

        t2:start()
        print("Status2: " .. t2:status())

        t1:write("Hello")

        while not t1:readable() do
            sevo.time.sleep(10)
        end
        print("Read: " .. t1:read())

        t1:join()
        print("Status1: " .. t1:status())

        t2:join()
        print("Status2: " .. t2:status())
    end,
    function()
        local db = sevo.dbm.open("testdb")
        db:put("hello", "world")
        db:put("niubi", "tiger")
        db:put("aaa", "bbbb")
        db:put("ccc", "ddd")
        db:put("eee", "fff")
        db:put("aaa", "ggg")
        db:del("ccc")
        db:del("eee")

        db:sync()

        print(db:get("hello"))
        print(db:get("niubi"))
        print(db:get("aaa"))

        local ks = db:keys()

        for k, v in ipairs(ks) do
            print(k, v)
        end

        db:close()
    end,
    function()
        print(sevo.node())

        local pid = sevo.spawn(
            function()
                local msg = sevo.receive()
                print("worker from: " .. msg.from)
                print("worker body: " .. msg.body)
                sevo.send(msg.from, { from=sevo.self(), body="Byebye" })
            end)
        sevo.send(pid, { from=sevo.self(), body="HelloWorld" })
        local msg = sevo.receive()
        print("main from: " .. msg.from)
        print("main body: " .. msg.body)

        local function monitor(pid)
            sevo.monitor(pid)
            local msg = sevo.receive()
            print("monitor received signal: " .. msg.signal .. ", from: " .. msg.from .. ", reason: " .. msg.reason)
        end

        local function ping(n)
            sevo.link("pong")

            for i = 1, n do
                sevo.send("pong", { from = sevo.self(), body = "ping" })
                local msg = sevo.receive()
                if msg.body == "pong" then
                    print("ping received pong.")
                end
            end

            print("ping finished")
            sevo.exit("finished")   -- 没有exit调用，pong进程不会结束
        end

        local function pong()
            sevo.register("pong", sevo.self())

            while true do
                local msg = sevo.receive()
                if msg.body == "ping" then
                    print("pong received ping")
                    sevo.send(msg.from, { body = "pong" })
                end
            end
            print("pong finished")
        end

        pid = sevo.spawn(pong)
        sevo.spawn(monitor, pid)
        pid = sevo.spawn(ping, 3)
        sevo.spawn(monitor, pid)
    end,
    function()
        print(sevo.env.add("Hello", "World"))
        print(sevo.env.add("Hello", "Kitty"))
        print(sevo.env.add("test", "123"))
        print(sevo.env.set("test", "234"))
        print(sevo.env.set("abc", "bcd"))
        print(sevo.env.set("cde", "efg"))
        print(sevo.env.set("bcd", "wer"))
        print(sevo.env.set("qwe", "fsdf"))
        print(sevo.env.get("Hello"))
        print(sevo.env.get("test"))
        print(sevo.env.get("abc"))
        print(sevo.env.get("cde"))
        print(sevo.env.del("bcd"))
        print(sevo.env.del("qwe"))

        local ks = sevo.env.keys()

        for k, v in ipairs(ks) do
            print(k, v)
        end
    end,
    function ()
        local socket = require("socket")
        local mime = require("mime")
        print("socket version: " .. socket._VERSION)
        print("mime version: " .. mime._VERSION)
    end,
    function ()
        local a = sevo.math.vec2()
        local b = sevo.math.vec2(3)
        local c = sevo.math.vec2(5, 7)

        local e = sevo.math.vec3()
        local f = sevo.math.vec3(3)
        local g = sevo.math.vec3(5, 7)
        local h = sevo.math.vec3(8, 6, 4)

        local m = sevo.math.mat33()
        local n = sevo.math.mat33(11, 22, 33)
        local k = sevo.math.mat33(1, 2, 3,
                                  4, 5, 6,
                                  7, 8, 9)

        print("a", a)
        print("#a", #a)
        print("a:lensq", a:lensq())
        print("b", b)
        print("#b", #b)
        print("b:lensq", b:lensq())
        print("c", c)
        print("#c", #c)
        print("c:lensq", c:lensq())

        print("e", e)
        print("#e", #e)
        print("e:lensq", e:lensq())
        print("f", f)
        print("#f", #f)
        print("f:lensq", f:lensq())
        print("g", g)
        print("#g", #g)
        print("g:lensq", g:lensq())
        print("h", h)
        print("#h", #h)
        print("h:lensq", h:lensq())

        print("-b", -b)
        print("b + c", b + c)
        print("b - c", b - c)
        print("c * 2", c * 2)
        print("c / 2", c / 2)
        print("b:dot(c)", b:dot(c))
        print("b:cross(c)", b:cross(c))
        print("c:dim", c:dim())
        print("c:normalize()", c:normalize())
        print("c", c)
        print("#c", #c)
        print("c:lensq", c:lensq())
        print("c:dot(c:rotate(rad(90)))", c:dot(c:rotate(sevo.math.radian(90))))

        print("-g", -g)
        print("g + h", g + h)
        print("g - h", g - h)
        print("h * 2", h * 2)
        print("h / 2", h / 2)
        print("g:dot(h)", g:dot(h))
        print("g:cross(h)", g:cross(h))
        print("h:dim", h:dim())
        print("h:normalize()", h:normalize())
        print("h", h)
        print("#h", #h)
        print("h:lensq", h:lensq())
        print("h:dot(h:rotatx(rad(90)))", h:dot(h:rotatex(sevo.math.radian(90))))
        print("h:dot(h:rotaty(rad(90)))", h:dot(h:rotatey(sevo.math.radian(90))))
        print("h:dot(h:rotatz(rad(90)))", h:dot(h:rotatez(sevo.math.radian(90))))

        print("c:xy", c:xy())
        print("h:xyz", h:xyz())

        print("m", m)
        print("n", n)
        print("k", k);
    end,
}
local test_step = 1

function sevo.load(args)
    print("sevo load")
    print("funcs: " .. #test_func)
    print("Work Path: " .. sevo.vfs.getcwd());

    for i, v in ipairs(args) do
        print(i, v)
    end
end

function sevo.update(delta)
    local t1, t2
    print("----- update " .. delta .. ", " .. test_step .. " -----")

    t1 = sevo.time.millisec()

    if test_step > #test_func then
        --if 0 == delta % 2 then
            sevo.event.quit()
        --else
        --    sevo.event.quit("restart")
        --end
    else
        test_func[test_step]()
        test_step = test_step + 1
    end

    t2 = sevo.time.millisec()

    print("----- end " .. t2 - t1 .. "ms -----")
end

function sevo.quit()
    print("sevo quit")
end
