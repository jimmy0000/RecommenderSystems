f = open('user_profile.txt')
dict = {}
lineno = 0 
for line in f:
  userid = int(line.strip().split()[0])
  dict[userid] = 0 

  lineno += 1
  if lineno % 1000000 == 0: print 'Processed', lineno, 'nodes'
f.close()

print 'Loaded User Profile. Nodes:', len(dict)

fw = open('userid_mapping.txt', 'w')
lineno = 0
for key in sorted(dict.keys()):
  dict[key] = lineno
  fw.write(str(key) + '\n')
  lineno += 1

fw.close()

print 'Done writing userid mapping file'

fw = open('user_sns_mapped.txt', 'w')
f = open('user_sns.txt')
lineno = 0
for line in f:
  
  vec = line.strip().split()
  followerid = int(vec[0])
  followeeid = int(vec[1])

  fw.write(str(dict[followerid]) + ' ' + str(dict[followeeid]) + '\n')
  lineno += 1
  if lineno % 1000000 == 0: print 'Processed', lineno, 'edges'
f.close()
fw.close()

print 'Done writing the mapped user sns'
