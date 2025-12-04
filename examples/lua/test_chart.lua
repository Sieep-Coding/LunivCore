-- Modern Dashboard Demo

print("\n" .. string.rep("=", 80))
print("        MODERN DASHBOARD RENDERING DEMO")
print(string.rep("=", 80))

-- Test 1: Sales Dashboard
print("\n>>> Sales Performance Dashboard\n")
local sales = BI.new("Q3 2024 Sales")
sales:add("North", 2500)
sales:add("South", 1800)
sales:add("East", 3200)
sales:add("West", 2900)

sales:chart("bar")

-- Test 2: Product Distribution
print("\n>>> Product Distribution\n")
BI.chart("pie", {
    {name = "Product A", value = 15000},
    {name = "Product B", value = 23000},
    {name = "Product C", value = 18000},
    {name = "Product D", value = 27000},
})

-- Test 3: Monthly Trend
print("\n>>> Monthly Trend\n")
local trend = BI.new("Revenue Trend")
trend:add("Jan", 450)
trend:add("Feb", 620)
trend:add("Mar", 580)
trend:add("Apr", 790)
trend:add("May", 890)
trend:add("Jun", 950)

trend:chart("line")

-- Test 4: Data Table
print("\n>>> Performance Metrics\n")
BI.chart("table", {
    {name = "Response Time", value = 45},
    {name = "Uptime %", value = 9999},
    {name = "Users", value = 1250},
    {name = "Requests/sec", value = 320},
})

-- Test 5: Histogram
print("\n>>> Distribution Analysis\n")
BI.chart("histogram", {
    {name = "0-100", value = 150},
    {name = "100-200", value = 280},
    {name = "200-300", value = 220},
    {name = "300-400", value = 180},
})

-- Test 6: Scatter Plot
print("\n>>> Correlation Analysis\n")
BI.chart("scatter", {
    {name = "Point 1", value = 200},
    {name = "Point 2", value = 450},
    {name = "Point 3", value = 350},
    {name = "Point 4", value = 600},
    {name = "Point 5", value = 280},
})

-- Test 7: Dashboard View
print("\n>>> Integrated Dashboard View\n")
BI.render_view({
    title = "EXECUTIVE DASHBOARD",
    rows = {
        {name = "Total Revenue", value = 12500},
        {name = "Profit Margin", value = 3750},
        {name = "Customer Count", value = 850},
        {name = "Market Share", value = 2340},
    }
})

-- Summary
print("\n" .. string.rep("=", 80))
print("        All 6 chart types rendered with modern styling")
print(string.rep("=", 80) .. "\n")