f = open('user_sns_mapped_sorted.txt')
fw = open('undirgraph.txt', 'w')
dict = {}
lineno = 0
for line in f:
  vec = line.strip().split()
  followerid = int(vec[0])
  followeeid = int(vec[1])

  fw.write(str(followerid) + ' ' + str(followeeid) + ' 1\n')
  fw.write(str(followeeid) + ' ' + str(followerid) + ' 0\n')

  lineno += 1
  if lineno % 1000000 == 0: print 'processed', lineno, 'edges'
f.close()
fw.close()

