import time

adjlist = []
degree = []
pagerank = []
danglingnodes = []
newpagerank = []

def LoadSocialNetworkGraph(filename, usermappingfilename):
  global socialgraph
  
  f = open(usermappingfilename)
  start_clock = time.clock()
  lineno = 0
  for line in f:
    adjlist.append([])
    degree.append(0)
    pagerank.append(0.0)   
 
    lineno += 1
    if lineno % 1000000 == 0: 
      print 'Time taken to load:', lineno, 'lines is ', time.clock() - start_clock, 'seconds'
    
  f.close()

  numusers = len(adjlist)
  print numusers

  f = open(filename)
  lineno = 0
  for line in f:
    vec = line.strip().split()
    followerid = int(vec[0])
    followeeid = int(vec[1])
    
    adjlist[followerid].append( followeeid )
    degree[followerid] += 1
    
    lineno += 1
    if lineno % 1000000 == 0: 
      print 'Time taken to load:', lineno, 'lines is ', time.clock() - start_clock, 'seconds'

  f.close()

  print 'Time taken to load:', lineno, 'lines is ', time.clock() - start_clock, 'seconds'

  default_pagerank_value = 1.0 / float(numusers)
  for followerid in range(numusers):
    pagerank[followerid] = default_pagerank_value
    if degree[followerid] == 0: danglingnodes.append(followerid)
  #This took roughly 4 mins to load the user_sns.txt graph and 2.1 GB of memory to load this socialgraph[userid] = (pagerank, count, adjlist) mapping
  
  return numusers
  
def ComputePageRank(numusers, s = 0.85, tolerance = 0.0001, MAXIMUM_ITERATIONS = 150):
  print 'Beginning module ComputePageRank with parameters:', 'numusers:', numusers, 's:', s, 'tolerance:', tolerance
  iteration = 1
  L1NormDifference = 1
  start_clock = time.clock()
  global adjlist
  global degree
  global pagerank
  global newpagerank
  
  while L1NormDifference > tolerance:
    print 'Iteration:', iteration, 'started at time:', time.clock() - start_clock, 'seconds'
    
    newpagerank = []
    innerproduct = 0.0
    for followerid in danglingnodes: innerproduct += pagerank[followerid]
    print 'Inner Product Computation took:', time.clock() - start_clock, 'seconds'
    
    additionalPageRankTerm = s * innerproduct / float(numusers)
    for followerid in range(numusers): newpagerank.append( additionalPageRankTerm )
    for followerid in range(numusers):
      curpagerank = pagerank[followerid]
      newpagerank[followerid] += (1.0 - s) * curpagerank
      count = degree[followerid]
      for followeeid in adjlist[followerid]:
        newpagerank[followeeid] += s * curpagerank / float(count)
    print 'Page Rank Computation took:', time.clock() - start_clock, 'seconds'
  
    #sumOfPageRank = 0.0
    #for followerid in socialgraph: 
    #  sumOfPageRank += newPageRankDict[followerid]
    #print sumOfPageRank
    
    #for followerid in socialgraph:
    #  newPageRankDict[followerid] /= sumOfPageRank
  
    L1NormDifference = 0.0
    for followerid in range(numusers):
      updatedpagerank = newpagerank[followerid]
      oldpagerank = pagerank[followerid] 
      L1NormDifference += abs(oldpagerank - updatedpagerank)
      pagerank[followerid] = updatedpagerank 
      
    print 'Change in L1Norm:', L1NormDifference, 'Computation took:', time.clock() - start_clock, 'seconds'
    
    iteration += 1
    if iteration > MAXIMUM_ITERATIONS: break
  
  print 'Total Iterations:', iteration, 'took time:', time.clock() - start_clock, 'seconds'
  
  
def OutputPageRankVectorToFile(numusers, file):
  global pagerank
  f = open(file, 'w')
  for followerid in range(numusers):
    text = '%d\t%.9f\n'%(followerid, pagerank[followerid])
    f.write(text)
  f.close()
  
def main():
  numusers = LoadSocialNetworkGraph('weighteddirgraph_mapped_retweet_sorted_bfsdepth2.txt', 'userid_mapping.txt')
  ComputePageRank(numusers)
  OutputPageRankVectorToFile(numusers, 'user_mapped_pagerank_retweet_bfsdepth2_undirected_rooted.txt')
  
if __name__ == '__main__':
  main()
