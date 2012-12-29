#f = open('undirgraph_mapped_sorted.txt')
f = open('weighteddirgraph_mapped_comment_duplicate_comment.txt')
#fw = open('undirgraph_mapped_sorted_deduped.txt', 'w')
fw = open('weighteddirgraph_mapped_comment_sorted.txt', 'w')
dict = {}

lineno = 0
prevfollowerid = -1
prevfolloweeid = -1
prevcount = 0

for line in f:
  vec = line.strip().split()
  followerid = int(vec[0])
  followeeid = int(vec[1])
  count = int(vec[2])

  if prevfollowerid != followerid or prevfolloweeid != followeeid:
    if prevfollowerid != -1 and prevfolloweeid != -1:
      fw.write(str(prevfollowerid) + ' ' + str(prevfolloweeid) + ' ' + str(prevcount) + '\n')
    
    prevcount = count
  else:
    prevcount += count

  prevfollowerid = followerid
  prevfolloweeid = followeeid

  lineno += 1
  if lineno % 1000000 == 0: print 'processed', lineno, 'edges'
f.close()


if prevfollowerid != -1 and prevfolloweeid != -1:
  fw.write(str(prevfollowerid) + ' ' + str(prevfolloweeid) + ' ' + str(prevcount) + '\n')

fw.close()

