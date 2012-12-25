tagdict = {}

f = open('item_tag_mapped_sorted.txt')
lineno = 0
for line in f:
  tags = line.strip().split()[-1].split(';')
  if len(tags) == 0: continue
  if len(tags) == 1 and tags[0] == '0': continue
  for tagstr in tags:
    tagid = int(tagstr)
    if tagid not in tagdict: tagdict[tagid] = 0
    tagdict[tagid] += 1
  
  lineno += 1
f.close()


f = open('item_tag_mapped_sorted.txt')
fw = open('tag_popularity_amongst_items.txt', 'w')
lineno = 0
for line in f:
  tags = line.strip().split()[-1].split(';')
  sumval = 0
  maxval = 0
  if len(tags) >= 1 and tags[0] != '0':
    for tagstr in tags:
      tagid = int(tagstr)
      if tagid in tagdict: 
        count = tagdict[tagid]
        sumval += count
        if count > maxval: maxval = count
      
  fw.write(str(lineno) + ' ' + str(sumval) + ' ' + str(maxval) + '\n')
  lineno += 1
fw.close()
f.close()

