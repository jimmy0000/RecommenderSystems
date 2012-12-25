f = open('item_keyword_sorted_mapped.txt')
fw = open('item_keyword_sorted_mapped_deduped.txt', 'w')
prevline = ''
for line1 in f:
  line = line1.strip()
  itemid = int(line.split()[0])
  if prevline != '' and itemid == int(prevline.split()[0]):
    if prevline.find(':') == -1 and line.find(':') != -1:
      tmp = prevline
      prevline = line
      line = tmp

    dict = {}
    keywordidscorelist = prevline.split()[1]
    for keywordidscore in keywordidscorelist.split(';'):
      keywordid = int(keywordidscore.split(':')[0])
      score = float(keywordidscore.split(':')[1])
      dict[keywordid] = score
  
    keywordidlist = line.split()[1]
    for keywordidstr in keywordidlist.split(';'):
      keywordid = int(keywordidstr)
      if keywordid not in dict: dict[keywordid] = 1.0;

    s = ''
    for keywordid, score in sorted(dict.items()):
      s = s + ';' + str(keywordid) + ':' + str(score)

    s = s[1:]
    fw.write(str(itemid) + ' ' + s + '\n')

  prevline = line
fw.close()
f.close()
