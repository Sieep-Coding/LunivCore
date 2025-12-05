-- Example test file demonstrating Check Your Lua
-- Run with: lua tests.lua
-- Or with options: CYL_QUIET=true lua tests.lua

local cyl = require('checkyour')
local describe, it, expect = cyl.describe, cyl.it, cyl.expect

-- Parse command-line arguments for configuration
cyl.parseargs()

describe("Check Your Lua - Framework Tests", function()
    
    describe("setup and teardown", function()
        local flag = false
        
        cyl.before(function()
            flag = true
        end)
        
        cyl.after(function()
            flag = false
        end)
        
        -- Verify before hook ran
        assert(not flag, "flag should be false outside hooks")
        
        it("checks before and after hooks", function()
            assert(flag, "flag should be true inside test")
        end)
        
        -- Verify after hook will run and reset flag
    end)
    
    describe("expect.equal - deep equality", function()
        it("compares primitive values", function()
            expect.equal(42, 42)
            expect.equal("hello", "hello")
            expect.equal(true, true)
        end)
        
        it("compares tables", function()
            expect.equal({a=1, b=2}, {a=1, b=2})
            expect.equal({1, 2, 3}, {1, 2, 3})
        end)
        
        it("compares nested structures", function()
            local t1 = { person = { name = "Alice", age = 25 } }
            local t2 = { person = { name = "Alice", age = 25 } }
            expect.equal(t1, t2)
        end)
        
        it("detects inequality", function()
            expect.not_equal(1, 2)
            expect.not_equal("hello", "world")
            expect.not_equal({a=1}, {a=2})
        end)
    end)
    
    describe("expect.truthy and expect.falsy", function()
        it("truthy values pass", function()
            expect.truthy(true)
            expect.truthy(1)
            expect.truthy("non-empty")
            expect.truthy({})
            expect.truthy(function() end)
        end)
        
        it("falsy values pass", function()
            expect.falsy(false)
            expect.falsy(nil)
            expect.falsy(0)
        end)
    end)
    
    describe("expect.exist and expect.not_exist", function()
        it("detects existing values", function()
            expect.exist(1)
            expect.exist("")
            expect.exist(false)
            expect.exist({})
        end)
        
        it("detects nil values", function()
            expect.not_exist(nil)
            local undefined
            expect.not_exist(undefined)
        end)
        
        it("checks table fields", function()
            local t = {a = 1, b = 2}
            expect.exist(t.a)
            expect.not_exist(t.c)
        end)
    end)
    
    describe("expect.fail and expect.not_fail", function()
        it("detects function errors", function()
            expect.fail(function()
                error("something went wrong")
            end)
        end)
        
        it("verifies error messages", function()
            expect.fail(function()
                error("invalid input")
            end, "invalid input")
        end)
        
        it("confirms functions don't fail", function()
            expect.not_fail(function()
                return 42
            end)
        end)
        
        it("skipped - would fail without proper error", function()
            expect.not_fail(function() end)
        end, false)
    end)
    
    describe("data types", function()
        it("compares numbers", function()
            expect.equal(1 + 1, 2)
            expect.equal(3 * 3, 9)
            expect.equal(10 / 2, 5)
            expect.equal(10 - 5, 5)
        end)
        
        it("compares strings", function()
            expect.equal("test", "test")
            expect.not_equal("hello", "world")
            expect.truthy("Lua")
        end)
        
        it("checks string length", function()
            expect.equal(#"Hello, World!", 13)
            expect.equal(#"Lua", 3)
            expect.equal(#"", 0)
        end)
        
        it("compares type values", function()
            expect.equal(type(42), "number")
            expect.equal(type("hello"), "string")
            expect.equal(type({}), "table")
            expect.equal(type(true), "boolean")
            expect.equal(type(nil), "nil")
        end)
    end)
    
    describe("tables and arrays", function()
        it("checks table existence", function()
            expect.exist({a=1})
            expect.not_exist(nil)
        end)
        
        it("checks array length", function()
            expect.equal(#{}, 0)
            expect.equal(#{1}, 1)
            expect.equal(#{1, 2, 3}, 3)
        end)
        
        it("accesses array elements", function()
            local array = {10, 20, 30}
            expect.equal(array[1], 10)
            expect.equal(array[2], 20)
            expect.equal(array[3], 30)
        end)
        
        it("checks table fields", function()
            local data = {name = "John", age = 30, active = true}
            expect.equal(data.name, "John")
            expect.equal(data.age, 30)
            expect.truthy(data.active)
        end)
        
        it("handles nested tables", function()
            local nested = {
                person = {
                    name = "Alice",
                    age = 25,
                    contact = {email = "alice@example.com"}
                }
            }
            expect.equal(nested.person.name, "Alice")
            expect.equal(nested.person.age, 25)
            expect.equal(nested.person.contact.email, "alice@example.com")
        end)
    end)
    
    describe("complex assertions", function()
        it("multiple assertions in one test", function()
            local result = {success = true, value = 42}
            expect.equal(result.success, true)
            expect.equal(result.value, 42)
            expect.truthy(result.success)
        end)
        
        it("conditional assertions", function()
            local condition = true
            if condition then
                expect.truthy(condition)
            else
                expect.falsy(condition)
            end
        end)
        
        it("validates data structure", function()
            local user = {
                id = 1,
                name = "Bob",
                email = "bob@example.com",
                roles = {"admin", "user"},
                active = true
            }
            
            expect.exist(user.id)
            expect.equal(user.name, "Bob")
            expect.truthy(user.email)
            expect.equal(#user.roles, 2)
            expect.truthy(user.active)
        end)
    end)
    
    describe("real-world patterns", function()
        it("validates function return values", function()
            local function divide(a, b)
                if b == 0 then error("division by zero") end
                return a / b
            end
            
            expect.equal(divide(10, 2), 5)
            expect.fail(function() divide(10, 0) end, "division by zero")
        end)
        
        it("tests state transformations", function()
            local function increment(state)
                state.count = state.count + 1
                return state
            end
            
            local state = {count = 0}
            increment(state)
            expect.equal(state.count, 1)
            increment(state)
            expect.equal(state.count, 2)
        end)
        
        it("validates list operations", function()
            local function filterNumbers(list)
                local result = {}
                for _, v in ipairs(list) do
                    if type(v) == "number" then
                        table.insert(result, v)
                    end
                end
                return result
            end
            
            local mixed = {1, "two", 3, "four", 5}
            local numbers = filterNumbers(mixed)
            expect.equal(#numbers, 3)
            expect.equal(numbers[1], 1)
            expect.equal(numbers[2], 3)
            expect.equal(numbers[3], 5)
        end)
    end)
    
end)

-- Print final summary and exit
cyl.report()
cyl.exit()