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

f = open('userid_mapping.txt')
lineno = 0
users = {}
for line in f:
  userid = int(line.strip().split()[0])
  users[userid] = lineno
  lineno += 1
  
  if lineno % 1000000 == 0: print 'Processed', lineno, 'nodes'
f.close()

print 'Done reading userid mapping file'

f = open('user_sns_sorted.txt')
fw = open('useritem_follow_mapped.txt', 'w')
lineno = 0
for line in f:
  
  vec = line.strip().split()
  userid = users[int(vec[0])]
  followeeid = int(vec[1])
  if followeeid in items:
    itemid = items[followeeid]
    fw.write(str(userid) + ' ' + str(itemid) + '\n')

  lineno += 1
  if lineno % 1000000 == 0: print 'Processed', lineno, 'edges'
fw.close()
f.close()

print 'Done writing the mapped version of items followed by users'

f = open('rec_log_train_sorted_mapped_train.txt')
fw = open('useritem_follow_mapped.txt', 'a+')
lineno = 0
for line in f:
  
  vec = line.strip().split()
  userid = int(vec[0])
  itemid = int(vec[1])
  result = int(vec[2])
  if result == 1:
    fw.write(str(userid) + ' ' + str(itemid) + '\n')

  lineno += 1
  if lineno % 1000000 == 0: print 'Processed', lineno, 'edges'
fw.close()
f.close()
