-- src/lua/tests.lua
-- Test suite for BI Engine with Check Your Lua
-- Run with: make test
-- Or: ./lcore src/lua/tests.lua
-- Or: CYL_QUIET=true ./lcore src/lua/tests.lua

-- Load Check Your Lua from src/lua directory
package.path = "src/lua/?.lua;unittest/?.lua;" .. package.path
local cyl = require('unittest/checkyour')
local describe, it, expect = cyl.describe, cyl.it, cyl.expect

-- Parse command-line arguments for configuration
cyl.parseargs()

-- Check if BI engine is available
if not BI then
    print("[ERROR] BI engine not initialized!")
    print("Make sure your lcore binary registers the BI table in Lua.")
    cyl.exit()
    return
end

describe("BI Engine - Core Functionality", function()
    
    describe("Dataset Creation", function()
        it("creates a new dataset", function()
            local dataset = BI.new("Test Dataset")
            expect.exist(dataset)
        end)
        
        it("stores dataset name correctly", function()
            local dataset = BI.new("Sales Report")
            expect.equal(dataset:name(), "Sales Report")
        end)
        
        it("initializes with empty data", function()
            local dataset = BI.new("Empty Dataset")
            expect.equal(dataset:count(), 0)
        end)
    end)
    
    describe("Data Operations", function()
        local dataset
        
        cyl.before(function()
            dataset = BI.new("Test Data")
        end)
        
        cyl.after(function()
            dataset = nil
        end)
        
        it("adds a single data point", function()
            dataset:add("Region1", 100)
            expect.equal(dataset:count(), 1)
        end)
        
        it("adds multiple data points", function()
            dataset:add("North", 340)
            dataset:add("South", 120)
            dataset:add("East", 210)
            dataset:add("West", 450)
            expect.equal(dataset:count(), 4)
        end)
        
        it("retrieves data by label", function()
            dataset:add("North", 340)
            local val = dataset:get("North")
            expect.equal(val, 340)
        end)
    end)
    
    describe("Aggregation Functions", function()
        local dataset
        
        cyl.before(function()
            dataset = BI.new("Aggregation Test")
            dataset:add("A", 100)
            dataset:add("B", 200)
            dataset:add("C", 300)
            dataset:add("D", 400)
        end)
        
        it("calculates total sum", function()
            local sum = dataset:sum()
            expect.equal(sum, 1000)
        end)
        
        it("calculates average", function()
            local avg = dataset:avg()
            expect.equal(avg, 250)
        end)
        
        it("finds maximum value", function()
            local max_val = dataset:max()
            expect.equal(max_val, 400)
        end)
        
        it("finds minimum value", function()
            local min_val = dataset:min()
            expect.equal(min_val, 100)
        end)
        
        it("counts data points", function()
            local count = dataset:count()
            expect.equal(count, 4)
        end)
    end)
    
    describe("Filtering Operations", function()
        local dataset
        
        cyl.before(function()
            dataset = BI.new("Filter Test")
            dataset:add("North", 340)
            dataset:add("South", 120)
            dataset:add("East", 210)
            dataset:add("West", 450)
        end)
        
        it("filters values above threshold", function()
            local filtered = dataset:filter(function(v) return v > 200 end)
            expect.equal(filtered:count(), 3)
        end)
        
        it("filters values below threshold", function()
            local filtered = dataset:filter(function(v) return v < 250 end)
            expect.equal(filtered:count(), 2)
        end)
    end)
    
    describe("Sorting Operations", function()
        local dataset
        
        cyl.before(function()
            dataset = BI.new("Sort Test")
            dataset:add("C", 300)
            dataset:add("A", 100)
            dataset:add("D", 400)
            dataset:add("B", 200)
        end)
        
        it("sorts ascending by value", function()
            local sorted = dataset:sort('asc')
            local first = sorted:get(1)
            expect.equal(first, 100)
        end)
        
        it("sorts descending by value", function()
            local sorted = dataset:sort('desc')
            local first = sorted:get(1)
            expect.equal(first, 400)
        end)
    end)
    
    describe("Real-world Analysis Scenarios", function()
        it("Q1 Regional Sales analysis", function()
            local sales = BI.new("Q1 Regional Sales")
            sales:add("North", 340)
            sales:add("South", 120)
            sales:add("East", 210)
            sales:add("West", 450)
            
            expect.equal(sales:count(), 4)
            expect.equal(sales:sum(), 1120)
            expect.equal(sales:max(), 450)
            expect.equal(sales:min(), 120)
        end)
        
        it("sales projection with conditionals", function()
            local sales = BI.new("Q1 Regional Sales")
            sales:add("North", 340)
            sales:add("South", 120)
            sales:add("East", 210)
            sales:add("West", 450)
            
            local projection = 50
            if projection >= 50 then
                sales:add("Proj.", projection * 2)
            end
            
            expect.equal(sales:count(), 5)
            expect.equal(sales:get("Proj."), 100)
        end)
        
        it("compares regional performance", function()
            local sales = BI.new("Regional Performance")
            sales:add("North", 340)
            sales:add("South", 120)
            sales:add("East", 210)
            sales:add("West", 450)
            
            local avg = sales:avg()
            expect.equal(avg, 280)
            
            -- West is above average
            expect.truthy(sales:get("West") > avg)
            -- South is below average
            expect.truthy(sales:get("South") < avg)
        end)
        
        it("tracks year-over-year growth", function()
            local previous = BI.new("Previous Year")
            previous:add("Jan", 1000)
            previous:add("Feb", 1100)
            previous:add("Mar", 1200)
            
            local current = BI.new("Current Year")
            current:add("Jan", 1200)
            current:add("Feb", 1320)
            current:add("Mar", 1440)
            
            expect.equal(previous:sum(), 3300)
            expect.equal(current:sum(), 3960)
            expect.truthy(current:sum() > previous:sum())
        end)
    end)
    
    describe("Error Handling", function()
        local dataset
        
        cyl.before(function()
            dataset = BI.new("Error Test")
        end)
        
        it("handles operations on empty dataset", function()
            local empty = BI.new("Empty")
            expect.equal(empty:count(), 0)
        end)
        
        it("handles single data point operations", function()
            dataset:add("Test", 100)
            expect.equal(dataset:count(), 1)
            expect.equal(dataset:sum(), 100)
            expect.equal(dataset:avg(), 100)
        end)
    end)
    
    describe("Visualization Integration", function()
        local dataset
        
        cyl.before(function()
            dataset = BI.new("Viz Test")
            dataset:add("North", 340)
            dataset:add("South", 120)
            dataset:add("East", 210)
            dataset:add("West", 450)
        end)
        
        it("generates plot output without error", function()
            expect.not_fail(function()
                dataset:plot()
            end)
        end)
    end)
    
end)

-- Print final summary and exit
print("\n" .. string.rep("=", 60))
cyl.report()
cyl.exit()