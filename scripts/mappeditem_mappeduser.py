f = open('userid_mapping.txt')
users = {}
lineno = 0
for line in f:
  userid = int(line.strip())
  users[userid] = lineno
  lineno += 1
f.close()


f = open('itemid_mapping.txt')
fw = open('mappeditem_mapperuser.txt', 'w')
lineno = 0
for line in f:
  unmappeditemid = int(line.strip())
  mappeduserid = users[unmappeditemid]
  fw.write(str(mappeduserid) + '\n')
  lineno += 1
fw.close()
f.close()


