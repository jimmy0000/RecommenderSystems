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

f = open('rec_log_train_sorted.txt')
fw = open('rec_log_train_sorted_mapped.txt', 'w')
prevuserid = -1
previtemid = -1
lineno = 0
for line in f:
  vec = line.strip().split()
  userid = int(vec[0])
  itemid = int(vec[1])
  if userid != prevuserid or itemid != previtemid:
    fw.write( str(users[userid]) + ' ' + str(items[itemid]) + ' ' + vec[2] + ' ' + vec[3] + '\n')

  prevuserid = userid
  previtemid = itemid
  lineno += 1
  if lineno % 1000000 == 0: print 'processed', lineno
fw.close()
f.close()
