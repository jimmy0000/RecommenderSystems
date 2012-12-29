dict = {}
f = open('userid_mapping.txt')
lineno = 0
for line in f:
  dict[int(line.strip())] = lineno
  lineno += 1

f.close()

f = open('user_action.txt')
fw_at = open('weighteddirgraph_mapped_ataction.txt', 'w')
fw_retweet = open('weighteddirgraph_mapped_retweet.txt', 'w')
fw_comment = open('weighteddirgraph_mapped_comment.txt', 'w')
lineno = 0
numLinesSkipped = 0
for line in f:
  vec = line.strip().split()
  followerid = int(vec[0])
  followeeid = int(vec[1])

  if followerid not in dict: 
    numLinesSkipped += 1
    continue

  if followeeid not in dict:
    numLinesSkipped += 1
    continue

  mapped_followerid = dict[followerid]
  mapped_followeeid = dict[followeeid]
  weight_at  = int(vec[2])
  weight_retweet = int(vec[3])
  weight_comment = int(vec[4])

  if weight_at > 0:
    fw_at.write(str(mapped_followerid) + ' ' + str(mapped_followeeid) + ' ' + str(weight_at) + '\n');
    fw_at.write(str(mapped_followeeid) + ' ' + str(mapped_followerid) + ' ' + '0\n');

  if weight_retweet > 0:
    fw_retweet.write(str(mapped_followerid) + ' ' + str(mapped_followeeid) + ' ' + str(weight_retweet) + '\n');
    fw_retweet.write(str(mapped_followeeid) + ' ' + str(mapped_followerid) + ' ' + '0\n');

  if weight_comment > 0:
    fw_comment.write(str(mapped_followerid) + ' ' + str(mapped_followeeid) + ' ' + str(weight_comment) + '\n');
    fw_comment.write(str(mapped_followeeid) + ' ' + str(mapped_followerid) + ' ' + '0\n');
  
  lineno += 1
  if lineno % 1000000 == 0: print 'Processed', lineno, 'edges'
fw_comment.close()
fw_retweet.close()
fw_at.close()
f.close()

print 'Loaded User SNS. Nodes:', len(dict), 'Edges:', lineno
print 'Lines Skipped:', numLinesSkipped
