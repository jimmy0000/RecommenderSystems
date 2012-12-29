f = open('userid_mapping.txt')
users = {}
lineno = 0
for line in f:
  userid = int(line.strip())
  users[userid] = lineno
  lineno += 1
f.close()


f = open('itemid_mapping.txt')
items = {}
lineno = 0
for line in f:
  itemid = int(line.strip())
  items[itemid] = lineno
  lineno += 1
f.close()

f = open('user_key_word.txt')
fw = open('user_key_word_mapped.txt', 'w')
fwitem = open('item_key_word_mapped.txt', 'w')
lineno = 0
numItemsMappedInUser = 0
for line in f:
  vec = line.strip().split()
  userid = int(vec[0])
  fw.write( str(users[userid]) + ' ' + vec[1] + '\n')
  if userid in items:
    fwitem.write( str(items[userid]) + ' ' + vec[1] + '\n')
    numItemsMappedInUser += 1

  lineno += 1
  if lineno % 1000000 == 0: print 'processed', lineno
fwitem.close()
fw.close()
f.close()
 
print 'numItemsMappedInUser:', numItemsMappedInUser

f = open('item.txt')
fw = open('item_key_word_mapped.txt', 'a+')
lineno = 0
for line in f:
  vec = line.strip().split()
  itemid = int(vec[0])
  fw.write( str(items[itemid]) + ' ' + vec[2] + '\n')
  
  lineno += 1
  if lineno % 1000000 == 0: print 'processed', lineno
fw.close()
f.close()
