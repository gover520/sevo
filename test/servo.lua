
function sevo.load(args)
    for i, v in ipairs(args) do
        sevo.info(i, v)
    end

    sevo.info('Sevo Version: ' .. sevo._VERSION)
    sevo.info('Sevo Version Num: ' .. sevo._VERSION_NUM)
    sevo.info('Sevo Version Full: ' .. sevo._VERSION_FULL)
    sevo.info('Sevo Version Major: ' .. sevo._VERSION_MAJOR)
    sevo.info('Sevo Version Minor: ' .. sevo._VERSION_MINOR)
    sevo.info('Sevo Version Patch: ' .. sevo._VERSION_PATCH)
    sevo.info('Sevo OS: ' .. sevo._OS)

    sevo.info('Work Path: ' .. sevo.vfs.getcwd());

    local re = require("re")
    sevo.info(re.find("the number 423 is odd", "[0-9]+"))           --> 12    14
    sevo.info(re.match("the number 423 is odd", "({%a+} / .)*"))    --> the    number    is    odd
    sevo.info(re.match("the number 423 is odd", "s <- {%d+} / . s"))--> 423
    sevo.info(re.gsub("hello World", "[aeiou]", "."))               --> h.ll. W.rld

    local socket = require("socket")
    local mime = require("mime")

    sevo.info("Socket Ver: " .. socket._VERSION .. ", Mime Ver: " .. mime._VERSION .. ".")

    function test_bigint()
        local a = sevo.int(0)
        sevo.info(type(a))
        sevo.info(type(0))
        sevo.info(type(#a))
        sevo.info(a == 0)           -- false, Lua equality must be 2 same objects
        sevo.info(a == sevo.int(0))   -- true
        sevo.info(a:eq(0))          -- a == 0
        sevo.info(a:lt(0))          -- a < 1
        sevo.info(a:le(0))          -- a <= 1
        sevo.info(not a:le(0))      -- a > 0
        sevo.info(not a:lt(0))      -- a >= 0

        sevo.info(#a)
        sevo.info(sevo.int(2) ^ 100)

        local f = sevo.int('1234567890987654321234567890987654321')
        sevo.info(f)
        sevo.info(-f)
        sevo.info(f:bnot())     -- Lua5.3: ~f

        local e = sevo.int(-9)
        sevo.info(e / 2)
        sevo.info(e % 2)
        sevo.info(e:div(2))     -- Lua5.3: e / 2 or e // 2
        sevo.info(e:bnot())     -- Lua5.3: ~e

        local b = sevo.int(123456789)
        sevo.info(b:bnot())     -- Lua5.3: ~b
        sevo.info(f:band(b))    -- Lua5.3: f & b
        sevo.info(f:bor(b))     -- Lua5.3: f | b
        sevo.info(f:bxor(b))    -- Lua5.3: f ~ b
        sevo.info(2 * b)

        sevo.info(f:mul(b))     -- f * b
        sevo.info(b:unm())      -- -b
    end

    test_bigint()

    function factorial(n)
        function fact_iter(accum, step)
            if step <= 1 then
                return accum
            end
            return fact_iter(accum * step, step - 1)
        end
        return fact_iter(sevo.int(1), n)
    end

    sevo.info(factorial(100))

    function fibonacci(n)
        function fib_iter(a, b, step)
            if step <= 0 then
                return a
            end
            return fib_iter(a + b, a, step - 1)
        end
        return fib_iter(sevo.int(0), sevo.int(1), n)
    end

    sevo.info(fibonacci(100))

    function gcd(a, b)
        if a:eq(0) then
            return b
        end
        return gcd(b % a, a)
    end

    sevo.info(gcd(factorial(123), fibonacci(123)))

    function test_id()
        local ids = {}

        sevo.id.init(1234)

        for i=1, 10 do
            table.insert(ids, sevo.id.next())
        end

        for i, v in ipairs(ids) do
            local ts, nodeid, seq = sevo.id.split(v)
            sevo.info('ID: ' .. v .. ', Timestamp: ' .. ts .. ', NodeID: ' .. nodeid .. ', Sequence: ' .. seq)
        end
    end

    test_id()

    function test_timer()
        function timer_iter(t, timeout)
            if t:expired() then
                if timeout > 0 then
                    sevo.info('Timer expired: ' .. timeout)
                    t:set(timeout - 1000)
                    return timer_iter(t, timeout - 1000)
                end
            else
                sevo.time.sleep(100)
                return timer_iter(t, timeout)
            end
        end
        timer_iter(sevo.time.timer(3000), 3000)
    end

    test_timer()

    local ffi = require('ffi')

    ffi.cdef[[
        typedef struct { double x, y; } point_t;
        int printf(const char *fmt, ...);
    ]]

    ffi.C.printf("Hello %s!\n", "world")

    if ffi.os == "Windows" then
        ffi.cdef[[
            int MessageBoxA(void *w, const char *txt, const char *cap, int type);
        ]]
        ffi.C.MessageBoxA(nil, "Hello world!", "Test", 0)
    end

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

    sevo.info(a.x, a.y)     --> 3  4
    sevo.info(#a)           --> 5
    sevo.info(a:area())     --> 25

    local b = a + point(0.5, 8)

    sevo.info(#b)           --> 12.5
end

function sevo.update(delta)
    sevo.info(delta)
    if delta > 40 and delta < 55 then sevo.event.quit('restart') end
    if delta >= 55 then sevo.event.quit() end
end

function sevo.quit()
    sevo.info('sevo quit')
end
