import numpy as np

globalbias = 0.0
MAX_USERS = 2500000
MAX_ITEMS = 7000
userbias = np.zeros(MAX_USERS)
usercnt = np.zeros(MAX_USERS)
itembias = np.zeros(MAX_ITEMS)
itemcnt = np.zeros(MAX_ITEMS, dtype = int)

f = open('rec_log_train_sorted_mapped.txt')
lineno =  0
for line in f:
  vec = line.split()
  userid = int(vec[0])
  itemid = int(vec[1])
  yesorno = float(vec[2])

  globalbias += yesorno
  userbias[userid] += yesorno
  usercnt[userid] += 1
  itembias[itemid] += yesorno
  itemcnt[itemid] += 1
  lineno += 1
  if lineno % 1000000 == 0: print 'processed', lineno, 'lines'

f.close()

globalbias = globalbias / lineno
print globalbias

for userid in range(MAX_USERS):
  if usercnt[userid] > 0: userbias[userid] = userbias[userid] / usercnt[userid];

for itemid in range(MAX_ITEMS):
  if itemcnt[itemid] > 0: itembias[itemid] = itembias[itemid] / itemcnt[itemid];


fw = open('user_bias.txt', 'w')
fw.write('#userid usercnt userbias globalbias:' + str(globalbias) + '\n')
for userid in range(MAX_USERS):
  fw.write(str(userid) + ' ' + str(usercnt[userid]) + ' ' + str(userbias[userid]) + '\n')
fw.close()

fw = open('item_bias.txt', 'w')
fw.write('#itemid itemcnt itembias globalbias:' + str(globalbias) + '\n')
for itemid in range(MAX_ITEMS):
  fw.write(str(itemid) + ' ' + str(itemcnt[itemid]) + ' ' + str(itembias[itemid]) + '\n')
fw.close()
