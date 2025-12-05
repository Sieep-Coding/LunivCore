-- Create datasets dynamically and aggregate values
local regions = {"North", "South", "East", "West"}
local q1_values = {340, 210, 210, 450}
local q2_values = {400, 190, 220, 470}

local q1_ds = BI.new("Q1 Aggregation KPIs")
local q2_ds = BI.new("Q2 Aggregation KPIs")

-- Populate datasets using loops
for i, region in ipairs(regions) do
    q1_ds:add(region, q1_values[i])
    q2_ds:add(region, q2_values[i])
end

q1_ds:chart('bar')
q2_ds:chart('bar')

-- Perform basic analytics
BI.text("Q1 Total:" .. q1_ds:sum() .. '\n  ' .. 
"Q2 Total:" .. q2_ds:sum() .. '\n  ' ..  
"Difference Q2-Q1:" .. q2_ds:sum() - q1_ds:sum())

-- Combine datasets into a single view
local market_view = {
    title = "Quarterly Market Share Comparison",
    rows = {},
    children = {}
}

for i, region in ipairs(regions) do
    table.insert(market_view.rows, {
        name = region,
        value = q2_values[i]
    })
end

BI.render_view(market_view)

-- Final summary
BI.text("Contact Luniv Technology for assistance with your dashboard.")
