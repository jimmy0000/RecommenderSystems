import numpy as np

globalbias = 0.0
MAX_USERS = 2500000
MAX_ITEMS = 7000
userbias = np.zeros(MAX_USERS)
userbiasoffset = np.zeros(MAX_USERS)
usercnt = np.zeros(MAX_USERS, dtype = int)
itembias = np.zeros(MAX_ITEMS)
itembiasoffset = np.zeros(MAX_ITEMS)
itemcnt = np.zeros(MAX_ITEMS, dtype = int)

f = open('user_bias.txt')
for line in f:
  if line.startswith('#'): continue
  vec = line.strip().split()
  userid = int(vec[0])
  userbias[userid] = float(vec[2])
f.close()

f = open('item_bias.txt')
for line in f:
  if line.startswith('#'): continue
  vec = line.strip().split()
  itemid = int(vec[0])
  itembias[itemid] = float(vec[2])
f.close()

f = open('rec_log_train_sorted_mapped.txt')
lineno =  0
for line in f:
  vec = line.split()
  userid = int(vec[0])
  itemid = int(vec[1])
  yesorno = float(vec[2])

  globalbias += yesorno
  userbiasoffset[userid] += yesorno - itembias[itemid]
  usercnt[userid] += 1
  itembiasoffset[itemid] += yesorno - userbias[userid]
  itemcnt[itemid] += 1
  lineno += 1
  if lineno % 1000000 == 0: print 'processed', lineno, 'lines'

f.close()

globalbias = globalbias / lineno
print globalbias

for userid in range(MAX_USERS):
  if usercnt[userid] > 0: userbiasoffset[userid] = userbiasoffset[userid] / usercnt[userid];

for itemid in range(MAX_ITEMS):
  if itemcnt[itemid] > 0: itembiasoffset[itemid] = itembiasoffset[itemid] / itemcnt[itemid];


fw = open('user_bias_offset.txt', 'w')
fw.write('#userid usercnt userbiasoffset globalbias:' + str(globalbias) + '\n')
for userid in range(MAX_USERS):
  fw.write(str(userid) + ' ' + str(usercnt[userid]) + ' ' + str(userbiasoffset[userid]) + '\n')
fw.close()

fw = open('item_bias_offset.txt', 'w')
fw.write('#itemid itemcnt itembiasoffset globalbias:' + str(globalbias) + '\n')
for itemid in range(MAX_ITEMS):
  fw.write(str(itemid) + ' ' + str(itemcnt[itemid]) + ' ' + str(itembiasoffset[itemid]) + '\n')
fw.close()
