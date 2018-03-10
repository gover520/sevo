------------------------------------------------------------
--  bigint/servo.lua
--
--  copyright (c) 2018 Xiongfei Shi
--
--  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
--  license: Apache-2.0
------------------------------------------------------------

local bigint_func = {
    function()
        print("-- Test bigint --")

        local a = sevo.int(0)

        print("type(a):", type(a))      -- userdata
        print("type(0):", type(0))      -- number
        print("type(#a):", type(#a))    -- number

        -- Lua equality must be 2 same objects
        print("0 == a:", 0 == a)    -- false
        print("a == 0:", a == 0)    -- false
        print("a == sevo.int(0): ", a == sevo.int(0))   -- true
        print("a:eq(0):", a:eq(0))  -- true
        print("#a == 0", #a == 0)   -- true

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
        print("~f:", ~f)

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
        print("f ~ b", f ~ b)
        print("2 * b:", 2 * b)

        print("f * b", f * b)
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

        print(fact_iter(sevo.int(1), 100))
    end,
    function()
        print("-- Test fibonacci --")

        local function fib_iter(a, b, step)
            if step <= 0 then
                return a
            end
            return fib_iter(a + b, a, step - 1)
        end

        print(fib_iter(sevo.int(0), sevo.int(1), 100))
    end,
    function()
        print("-- Test gcd --")

        local function gcd(a, b)
            if a:eq(0) then
                return b
            end
            return gcd(b % a, a)
        end

        print(gcd(sevo.int(123456789), sevo.int(135792468)))
    end,
}

local test_step = 1

function sevo.load(args)
end

function sevo.update(delta)
    if test_step > #bigint_func then
        sevo.event.quit()
    else
        bigint_func[test_step]()
        test_step = test_step + 1
    end
end

function sevo.quit()
end
