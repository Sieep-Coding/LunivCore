local dataset = BI.new("Q1 Aggregation KPIs")

dataset:add("North", 340)
dataset:add("South", 210)

print("Total:", dataset:sum())
print("Average:", dataset:avg())
print("Min:", dataset:min())   
print("Max:", dataset:max())
print("Count:", dataset:count())

print("Max - Min:", dataset:max() - dataset:min())

dataset:plot()