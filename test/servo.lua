
local test_func = {
    function()
        print('-- Show version --')
        print('Sevo Version: ' .. sevo._VERSION)
        print('Sevo Version Num: ' .. sevo._VERSION_NUM)
        print('Sevo Version Full: ' .. sevo._VERSION_FULL)
        print('Sevo Version Major: ' .. sevo._VERSION_MAJOR)
        print('Sevo Version Minor: ' .. sevo._VERSION_MINOR)
        print('Sevo Version Patch: ' .. sevo._VERSION_PATCH)
        print('Sevo OS: ' .. sevo._OS)
    end,
    function()
        print('-- Test re --')
        local re = require("re")
        print(re.find("the number 423 is odd", "[0-9]+"))           --> 12    14
        print(re.match("the number 423 is odd", "({%a+} / .)*"))    --> the    number    is    odd
        print(re.match("the number 423 is odd", "s <- {%d+} / . s"))--> 423
        print(re.gsub("hello World", "[aeiou]", "."))               --> h.ll. W.rld
    end,
    function()
        print('-- Test socket --')
        local socket = require("socket")
        local mime = require("mime")
        print("Socket Ver: " .. socket._VERSION .. ", Mime Ver: " .. mime._VERSION .. ".")
    end,
    function()
        print('-- Test bigint --')
        local a = sevo.int(0)
        print(type(a))
        print(type(0))
        print(type(#a))
        print(a == 0)           -- false, Lua equality must be 2 same objects
        print(a == sevo.int(0))   -- true
        print(a:eq(0))          -- a == 0
        print(a:lt(0))          -- a < 1
        print(a:le(0))          -- a <= 1
        print(not a:le(0))      -- a > 0
        print(not a:lt(0))      -- a >= 0

        print(#a)
        print(sevo.int(2) ^ 100)

        local f = sevo.int('1234567890987654321234567890987654321')
        print(f)
        print(-f)
        print(f:bnot())     -- Lua5.3: ~f

        local e = sevo.int(-9)
        print(e / 2)
        print(e % 2)
        print(e:div(2))     -- Lua5.3: e / 2 or e // 2
        print(e:bnot())     -- Lua5.3: ~e

        local b = sevo.int(123456789)
        print(b:bnot())     -- Lua5.3: ~b
        print(f:band(b))    -- Lua5.3: f & b
        print(f:bor(b))     -- Lua5.3: f | b
        print(f:bxor(b))    -- Lua5.3: f ~ b
        print(2 * b)

        print(f:mul(b))     -- f * b
        print(b:unm())      -- -b
    end,
    function()
        print('-- Test factorial --')
        local n = 100
        local function fact_iter(accum, step)
            if step <= 1 then
                return accum
            end
            return fact_iter(accum * step, step - 1)
        end
        print(fact_iter(sevo.int(1), n))
    end,
    function()
        print('-- Test fibonacci --')
        local n = 100
        local function fib_iter(a, b, step)
            if step <= 0 then
                return a
            end
            return fib_iter(a + b, a, step - 1)
        end
        print(fib_iter(sevo.int(0), sevo.int(1), n))
    end,
    function()
        print('-- Test gcd --')
        local function gcd(a, b)
            if a:eq(0) then
                return b
            end
            return gcd(b % a, a)
        end
        print(gcd(sevo.int(123456789), sevo.int(135792468)))
    end,
    function()
        print('-- Test id --')
        local ids = {}

        sevo.id.init(1234)

        for i=1, 10 do
            table.insert(ids, sevo.id.next())
        end

        for i, v in ipairs(ids) do
            local ts, nodeid, seq = sevo.id.split(v)
            print('ID: ' .. v .. ', Timestamp: ' .. ts .. ', NodeID: ' .. nodeid .. ', Sequence: ' .. seq)
        end
    end,
    function()
        print('-- Test timer --')
        local function timer_iter(t, timeout)
            if t:expired() then
                if timeout > 0 then
                    print('Timer expired: ' .. timeout)
                    t:set(timeout - 1000)
                    return timer_iter(t, timeout - 1000)
                end
                print('timer finish')
            else
                print('timer sleep')
                sevo.time.sleep(200)
                return timer_iter(t, timeout)
            end
        end
        timer_iter(sevo.time.timer(3000), 3000)
    end,
    function()
        print('-- Test ffi --')
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
    end,
}
local test_step = 1

function sevo.load(args)
    print('sevo load')
    print('funcs: ' .. #test_func)
    print('Work Path: ' .. sevo.vfs.getcwd());

    for i, v in ipairs(args) do
        print(i, v)
    end
end

function sevo.update(delta)
    print('----- update ' .. delta .. ', ' .. test_step .. ' -----')
    if test_step > #test_func then
        sevo.event.quit()
    else
        test_func[test_step]()
        test_step = test_step + 1
    end
    print('----- end -----')
end

function sevo.quit()
    print('sevo quit')
end
