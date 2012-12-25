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

itemcategories = []
for itemid in range(len(items)): itemcategories.append('')

f = open('item.txt')
lineno = 0
for line in f:
  vec = line.strip().split()
  itemid = items[int(vec[0])]
  category = vec[1]
  itemcategories[itemid] = category

f = open('useritem_follow_mapped_sorted.txt')
fw = open('useritem_categories_follow_mapped_sorted.txt', 'w')
lineno = 0
prevuserid = -1
dict = {}
for line in f:
  vec = line.strip().split()
  userid = int(vec[0])
  itemid = int(vec[1])
  if prevuserid != -1 and userid != prevuserid:
    categorydict = {}
    for itemid in dict.keys():
      category = itemcategories[itemid]
      if category not in categorydict: categorydict[category] = 0
      categorydict[category] += 1

    s = ''
    for category, count in categorydict.items():
      s += ';' + category + ':' + str(count)
    s = s[1:]

    fw.write(str(prevuserid) + ' ' + s + '\n')
    dict = {}

  if itemid not in dict: dict[itemid] = True
  prevuserid = userid
  lineno += 1
  if lineno % 1000000 == 0: print 'Processed', lineno, 'edges'

if prevuserid > 0:
  categorydict = {}
  for itemid in dict.keys():
    category = itemcategories[itemid]
    if category not in categorydict: categorydict[category] = 0
    categorydict[category] += 1

  s = ''
  for category, count in categorydict.items():
    s += ';' + category + ':' + str(count)
    s = s[1:]

  fw.write(str(prevuserid) + ' ' + s + '\n')
fw.close()
f.close()

