-- config.lua
print("BI Engine: Starting Analysis...")

-- 1. Create a new Dataset using our C constructor
local sales = BI.new("Q1 Regional Sales")

-- 2. Add data (The C engine stores this efficiently)
sales:add("North", 340)
sales:add("South", 120)
sales:add("East", 210)
sales:add("West", 450)

-- 3. Lua Logic (We can mix standard Lua with our DSL)
local projection = 50
sales:add("Proj.", projection * 2)

-- 4. Visualize
sales:plot()

print("Analysis Complete.")