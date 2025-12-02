local dataset = BI.new("Q1 Aggregation KPIs")

dataset:add("North", 340)
dataset:add("South", 210)
dataset:add("East", 210)
dataset:add("West", 450)
dataset:add("Proj", 100)

dataset:plot()
print("Total:", dataset:sum())
print("Max - Min:", dataset:max() - dataset:min())

BI.text("We are beating projections everywhere!")

BI.render_view({
    title = "Market Share (2025)",
    rows = {
        {name="SegmentA", value=90},
        {name="SegmentB", value=8},
        {name="SegmentC", value=2}
    },
    children = {
        {type="text", value="Analysis Notes: North dominates the market."},
        {type="text", value="Segments B and C are negligible."}
    }
})

BI.text("SegmentA is doing excellent this year.")