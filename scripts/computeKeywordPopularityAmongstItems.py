keyworddict = {}

f = open('item_keyword_sorted_mapped_deduped.txt')
lineno = 0
for line in f:
  keywords = line.strip().split()[-1].split(';')
  if len(keywords) == 0: continue
  if len(keywords) == 1 and keywords[0] == '0': continue
  for keywordscore in keywords:
    keywordid = int(keywordscore.split(':')[0])
    if keywordid not in keyworddict: keyworddict[keywordid] = 0
    keyworddict[keywordid] += 1
  
  lineno += 1
f.close()


f = open('item_keyword_sorted_mapped_deduped.txt')
fw = open('keyword_popularity_amongst_items.txt', 'w')
lineno = 0
for line in f:
  keywords = line.strip().split()[-1].split(';')
  sumval = 0
  maxval = 0
  if len(keywords) >= 1 and keywords[0] != '0':
    for keywordscore in keywords:
      keywordid = int(keywordscore.split(':')[0])
      if keywordid in keyworddict: 
        count = keyworddict[keywordid]
        sumval += count
        if count > maxval: maxval = count
      
  fw.write(str(lineno) + ' ' + str(sumval) + ' ' + str(maxval) + '\n')
  lineno += 1
fw.close()
f.close()

