print('Sonic Version: ' .. sonic._version)
print('Sonic Full Version: ' .. sonic._version_full)
print('Sonic Major Version: ' .. sonic._version_major)
print('Sonic Minor Version: ' .. sonic._version_minor)
print('Sonic Patch Version: ' .. sonic._version_patch)
print('Sonic OS: ' .. sonic._os)

local re = require("re")
print(re.find("the number 423 is odd", "[0-9]+"))           --> 12    14
print(re.match("the number 423 is odd", "({%a+} / .)*"))    --> the    number    is    odd
print(re.match("the number 423 is odd", "s <- {%d+} / . s"))--> 423
print(re.gsub("hello World", "[aeiou]", "."))               --> h.ll. W.rld

function test_bigint()
    local a = sonic.int(0)
    print(type(a))
    print(type(0))
    print(type(#a))
    print(a == 0)           -- false, Lua equality must be 2 same objects
    print(a == sonic.int(0))   -- true
    print(a:eq(0))          -- a == 0
    print(a:lt(0))          -- a < 1
    print(a:le(0))          -- a <= 1
    print(not a:le(0))      -- a > 0
    print(not a:lt(0))      -- a >= 0

    print(#a)
    print(sonic.int(2) ^ 100)

    local f = sonic.int('1234567890987654321234567890987654321')
    print(f)
    print(-f)
    print(f:bnot())     -- Lua5.3: ~f

    local e = sonic.int(-9)
    print(e / 2)
    print(e % 2)
    print(e:div(2))     -- Lua5.3: e / 2 or e // 2
    print(e:bnot())     -- Lua5.3: ~e

    local b = sonic.int(123456789)
    print(b:bnot())     -- Lua5.3: ~b
    print(f:band(b))    -- Lua5.3: f & b
    print(f:bor(b))     -- Lua5.3: f | b
    print(f:bxor(b))    -- Lua5.3: f ~ b
    print(2 * b)

    print(f:mul(b))     -- f * b
    print(b:unm())      -- -b
end

test_bigint()

function factorial(n)
    function fact_iter(accum, step)
        if step <= 1 then
            return accum
        end
        return fact_iter(accum * step, step - 1)
    end
    return fact_iter(sonic.int(1), n)
end

print(factorial(100))

function fibonacci(n)
    function fib_iter(a, b, step)
        if step <= 0 then
            return a
        end
        return fib_iter(a + b, a, step - 1)
    end
    return fib_iter(sonic.int(0), sonic.int(1), n)
end

print(fibonacci(100))

function gcd(a, b)
    if a:eq(0) then
        return b
    end
    return gcd(b % a, a)
end

print(gcd(factorial(123), fibonacci(123)))

function test_id()
    local ids = {}

    sonic.id.init(1234)

    for i=1, 10 do
        table.insert(ids, sonic.id.next())
    end

    for i, v in ipairs(ids) do
        local ts, nodeid, seq = sonic.id.split(v)
        print('ID: ' .. v .. ', Timestamp: ' .. ts .. ', NodeID: ' .. nodeid .. ', Sequence: ' .. seq)
    end
end

test_id()

function test_timer()
    function timer_iter(t, timeout)
        if t:expired() then
            if timeout > 0 then
                print('Timer expired: ' .. timeout)
                t:set(timeout - 1000)
                return timer_iter(t, timeout - 1000)
            end
        else
            return timer_iter(t, timeout)
        end
    end
    timer_iter(sonic.timer(3000), 3000)
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

print(a.x, a.y)     --> 3  4
print(#a)           --> 5
print(a:area())     --> 25

local b = a + point(0.5, 8)

print(#b)           --> 12.5

return '123'
