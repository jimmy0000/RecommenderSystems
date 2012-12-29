indegreecount = {}
outdegreecount = {}

total_inbound_edges = 0
total_outbound_edges = 0
f = open('vertex_in_out_degree.txt')
for line in f:
  if line.startswith('#'): continue
  vec = line.strip().split()
  indegree = int(vec[1])
  outdegree = int(vec[2])
  
  if indegree not in indegreecount: indegreecount[indegree] = 0
  indegreecount[indegree] += 1
  total_inbound_edges += indegree

  if outdegree not in outdegreecount: outdegreecount[outdegree] = 0
  outdegreecount[outdegree] += 1
  total_outbound_edges += outdegree
f.close()

fw = open('indegree_aggregation.txt', 'w')
for indegree in sorted(indegreecount.keys()):
  fw.write(str(indegree) + ' ' + str(indegreecount[indegree]) + '\n')
fw.close()

fw = open('outdegree_aggregation.txt', 'w')
for outdegree in sorted(outdegreecount.keys()):
  fw.write(str(outdegree) + ' ' + str(outdegreecount[outdegree]) + '\n')
fw.close()

print 'total_inbound_edges', total_inbound_edges, 'total_outbound_edges', total_outbound_edges

