BI.text('Exploratory Analysis With LunivCore')

BI.text('Process Data')
print('→ loading data...')

local data = BI.new('Journal #1 Data (Sales $)')
data:add("Midwest", 1124)
data:add("Northeast", 2545)
data:add("West", 728)
data:add("South", 1954)
data:add("Texas", 1720)
data:add("California", 2140)

print('✓ finished loading data...\n')

BI.text('First, we need to get some basic statistics.')

print('Computing statistics...\n')
print(string.format('  Total Sales:    $%d', data:sum()))
print(string.format('  Average Sales:  $%.2f', data:avg()))
print(string.format('  Min Sales:      $%d', data:min()))
print(string.format('  Max Sales:      $%d', data:max()))
print(string.format('  Regions:        %d\n', data:count()))

BI.text('Now let us visualize the data distribution')

print('Rendering bar chart...')
data:chart('bar')

print('Rendering second chart...')
data:chart('pie')

print('Rendering table...')
data:chart('table')

BI.text('Key Findings')

print('\nAnalysis Summary:')
print(string.format('  • Top Region: California ($%d)', data:max()))
print(string.format('  • Lowest Region: West ($%d)', data:min()))
print(string.format('  • Standard Coverage: %d regions analyzed', data:count()))
print(string.format('  • Revenue Range: $%d', data:max() - data:min()))
print(string.format('  • Average per Region: $%.0f\n', data:avg()))

BI.text('Conclusion')

print('The regional sales data shows:')
print('  - Northeast leads with $2,545 in sales')
print('  - California performs well at $2,140')
print('  - West region has lowest sales at $728')
print('  - Significant variance across regions suggests targeted marketing opportunities')
print()