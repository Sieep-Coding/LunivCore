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

-- Perform basic analytics
print("Q1 Total:", q1_ds:sum())
print("Q2 Total:", q2_ds:sum())
print("Difference Q2-Q1:", q2_ds:sum() - q1_ds:sum())

-- Advanced analytics: conditional highlights
for i, region in ipairs(regions) do
    local q1_val = q1_values[i]
    local q2_val = q2_values[i]
    if q2_val > q1_val then
        BI.text(region .. " exceeded projections by " .. (q2_val - q1_val))
    else
        BI.text(region .. " lagged projections by " .. (q1_val - q2_val))
    end
end

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
    table.insert(market_view.children, {
        type = "text",
        value = string.format("%s Q1: %d, Q2: %d", region, q1_values[i], q2_values[i])
    })
end

BI.render_view(market_view)

-- Final summary
BI.text("All datasets processed successfully.")
