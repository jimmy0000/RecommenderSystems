indegreevec = []
outdegreevec = []
numvertices = 0
f = open('userid_mapping.txt')
lineno = 0 
for line in f:
  indegreevec.append(0)
  outdegreevec.append(0)
  lineno += 1
f.close()
numvertices = lineno

f = open('user_sns_mapped_sorted.txt')
#f = open('weighteddirgraph_mapped_retweet_sorted.txt')
lineno = 0
for line in f:
  vec = line.strip().split()
  followerid = int(vec[0])
  followeeid = int(vec[1])
  #TODO: Now that user action graphs include the false edge for the sake
  # of BFS, i will need to modify this script
  outdegreevec[followerid] += 1
  indegreevec[followeeid] += 1

  lineno += 1
  if lineno % 1000000 == 0: print 'processed', lineno, 'edges'
f.close()

fw = open('vertex_in_out_degree.txt', 'w')
#fw = open('vertex_in_out_degree_retweet.txt', 'w')
for vertex in range(numvertices):
  fw.write(str(vertex) + ' ' + str(indegreevec[vertex]) + ' ' + str(outdegreevec[vertex]) + '\n')
fw.close()
