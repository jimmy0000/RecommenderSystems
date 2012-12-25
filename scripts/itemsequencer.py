items = {}
f = open('itemid_mapping.txt')
lineno = 0 
for line in f:
  itemid = int(line.strip().split()[0])
  items[itemid] = lineno 

  lineno += 1
  if lineno % 1000000 == 0: print 'Processed', lineno, 'nodes'
f.close()

print 'Loaded Item Mapping. Nodes:', len(items)

f = open('item.txt')
fw = open('item_mapped.txt', 'w')
lineno = 0
for line in f:
  
  vec = line.strip().split()
  itemid = items[int(vec[0])]
  fw.write(str(itemid) + ' ' + vec[1] + ' ' + vec[2] + '\n')

  lineno += 1
  if lineno % 1000000 == 0: print 'Processed', lineno, 'edges'
fw.close()
f.close()

