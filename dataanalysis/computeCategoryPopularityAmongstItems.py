categorydict = {}
for i in range(4): categorydict[i] = {}

f = open('item.txt')
lineno = 0
for line in f:
  categories = line.strip().split()[1].split('.')
  if len(categories) == 0: continue
  if len(categories) == 1 and categories[0] == '0': continue
  for i in range(len(categories)):
    category = '.'.join(categories[:i+1])
    if category not in categorydict[i]: categorydict[i][category] = 0
    categorydict[i][category] += 1
  
  lineno += 1
f.close()


f = open('item.txt')
fw = open('category_popularity_amongst_items.txt', 'w')
lineno = 0
for line in f:
  categories = line.strip().split()[1].split('.')
  sumvec = []
  maxvec = []
  for i in range(4):
    sumvec.append(0)
    maxvec.append(0)

  if len(categories) >= 1 and categories[0] != '0':
    for i in range(len(categories)):
      category = '.'.join(categories[:i+1])
      if category in categorydict[i]:
        count = categorydict[i][category]
        sumvec[i] += count
        if count > maxvec[i]: maxvec[i] = count

  sumstr = ''
  maxstr = ''
  for i in range(4):
    sumstr += ' ' + str(sumvec[i])
    maxstr += ' ' + str(maxvec[i])

  fw.write(str(lineno) + sumstr + maxstr + '\n')
  lineno += 1
fw.close()
f.close()

