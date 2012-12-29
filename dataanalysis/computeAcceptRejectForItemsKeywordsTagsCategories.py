numitems = 6095
itemacceptcount = []
itemrejectcount = []
itemkeywordmap =  []
itemtagmap = []
itemcategorymap = []
for i in range(numitems): 
  itemacceptcount.append(0)
  itemrejectcount.append(0)
  itemkeywordmap.append({})
  itemtagmap.append({})
  itemcategorymap.append(['', '', '', ''])

keywordacceptdict = {}
keywordrejectdict = {}
keyworddict = {}
tagacceptdict = {}
tagrejectdict = {}
tagdict = {}
categoryacceptdict = {}
categoryrejectdict = {}
categorydict = {}
for i in range(4):
  categoryacceptdict[i] = {}
  categoryrejectdict[i] = {}
  categorydict[i] = {}

f = open('item_keyword_sorted_mapped_deduped.txt')
lineno = 0
for line in f:
  keywords = line.strip().split()[-1].split(';')
  if len(keywords) == 0: continue
  if len(keywords) == 1 and keywords[0] == '0': continue
  for keywordscore in keywords:
    keywordid = int(keywordscore.split(':')[0])
    itemkeywordmap[lineno][keywordid] = True
  
  lineno += 1
f.close()

f = open('item_tag_mapped_sorted.txt')
lineno = 0
for line in f:
  tags = line.strip().split()[-1].split(';')
  if len(tags) == 0: continue
  if len(tags) == 1 and tags[0] == '0': continue
  for tagstr in tags:
    tagid = int(tagstr)
    itemtagmap[lineno][tagid] = True 
  lineno += 1
f.close()


f = open('item.txt')
lineno = 0
for line in f:
  categories = line.strip().split()[1].split('.')
  if len(categories) == 0: continue
  if len(categories) == 1 and categories[0] == '0': continue
  for i in range(len(categories)):
    category = '.'.join(categories[:i+1])
    itemcategorymap[lineno][i] = category
  
  lineno += 1
f.close()

f = open('rec_log_train_sorted_mapped_train.txt')
lineno = 0
for line in f:
  vec = line.strip().split()
  itemid = int(vec[1])
  yesorno = int(vec[2])
  if yesorno == 1: itemacceptcount[itemid] += 1
  elif yesorno == -1: itemrejectcount[itemid] += 1

  lineno += 1
  if lineno % 1000000 == 0: print 'processed', lineno, 'lines in training set' 
f.close()

#post process to update counts for keywords, tags and categories corresponding to items
for itemid in range(numitems):
  acceptcount = itemacceptcount[itemid]
  rejectcount = itemrejectcount[itemid]
  
  for keywordid in itemkeywordmap[itemid].keys():
    if keywordid not in keywordacceptdict: keywordacceptdict[keywordid] = 0
    keywordacceptdict[keywordid] += acceptcount

    if keywordid not in keywordrejectdict: keywordrejectdict[keywordid] = 0
    keywordrejectdict[keywordid] += rejectcount

    if keywordid not in keyworddict: keyworddict[keywordid] = 0
    keyworddict[keywordid] += 1

  for tagid in itemtagmap[itemid].keys():
    if tagid not in tagacceptdict: tagacceptdict[tagid] = 0
    tagacceptdict[tagid] += acceptcount

    if tagid not in tagrejectdict: tagrejectdict[tagid] = 0
    tagrejectdict[tagid] += rejectcount

    if tagid not in tagdict: tagdict[tagid] = 0
    tagdict[tagid] += 1

  level = 0
  for category in itemcategorymap[itemid]:
    if category not in categoryacceptdict[level]: 
      categoryacceptdict[level][category] = 0

    categoryacceptdict[level][category] += acceptcount

    if category not in categoryrejectdict[level]:
      categoryrejectdict[level][category] = 0

    categoryrejectdict[level][category] += rejectcount

    if category not in categorydict[level]: 
      categorydict[level][category] = 0 

    categorydict[level][category] += 1
    level += 1

fw = open('acceptreject_items_tags_keywords_categories.txt', 'w')
for itemid in range(numitems):
  acceptsumvec = []
  acceptmaxvec = []
  rejectsumvec = []
  rejectmaxvec = []
  sumvec = []
  maxvec = []

  for i in range(1 + 1 + 4):
    acceptsumvec.append(0)
    acceptmaxvec.append(0)
    rejectsumvec.append(0)
    rejectmaxvec.append(0)
    sumvec.append(0)
    maxvec.append(0)

  index = 0
  for keywordid in itemkeywordmap[itemid].keys():
    if keywordid in keywordacceptdict:
      count = keywordacceptdict[keywordid]
      acceptsumvec[index] += count
      if count > acceptmaxvec[index]: acceptmaxvec[index] = count

    if keywordid in keywordrejectdict:
      count = keywordrejectdict[keywordid]
      rejectsumvec[index] += count
      if count > rejectmaxvec[index]: rejectmaxvec[index] = count

    if keywordid in keyworddict:
      count = keyworddict[keywordid]
      sumvec[index] += count
      if count > maxvec[index]: maxvec[index] = count

  index += 1
  for tagid in itemtagmap[itemid].keys():
    if tagid in tagacceptdict:
      count = tagacceptdict[tagid]
      acceptsumvec[index] += count
      if count > acceptmaxvec[index]: acceptmaxvec[index] = count

    if tagid in tagrejectdict:
      count = tagrejectdict[tagid]
      rejectsumvec[index] += count
      if count > rejectmaxvec[index]: rejectmaxvec[index] = count

    if tagid in tagdict:
      count = tagdict[tagid]
      sumvec[index] += count
      if count > maxvec[index]: maxvec[index] = count

  index += 1
  level = 0
  for category in itemcategorymap[itemid]:
    if category in categoryacceptdict[level]:
      count = categoryacceptdict[level][category]
      acceptsumvec[index + level] += count
      if count > acceptmaxvec[index + level]: 
        acceptmaxvec[index + level] = count

    if category in categoryrejectdict[level]:
      count = categoryrejectdict[level][category]
      rejectsumvec[index + level] += count
      if count > rejectmaxvec[index + level]:
        rejectmaxvec[index + level] = count

    if category in categorydict[level]:
      count = categorydict[level][category]
      sumvec[index + level] += count
      if count > maxvec[index + level]:
        maxvec[index + level] = count

    level += 1

  acceptsumstr = ''
  acceptmaxstr = ''
  rejectsumstr = ''
  rejectmaxstr = ''
  sumstr = ''
  maxstr = ''

  for i in range(1 + 1 + 4):
    acceptsumstr += ' ' + str(acceptsumvec[i])
    acceptmaxstr += ' ' + str(acceptmaxvec[i])
    rejectsumstr += ' ' + str(rejectsumvec[i])
    rejectmaxstr += ' ' + str(rejectmaxvec[i])
    sumstr += ' ' + str(sumvec[i])
    maxstr += ' ' + str(maxvec[i])

  fw.write(str(itemid) + ' ' + str(itemacceptcount[itemid]) + ' ' + str(itemrejectcount[itemid]) + acceptsumstr + acceptmaxstr + rejectsumstr + rejectmaxstr + sumstr + maxstr + '\n')
  
fw.close()
