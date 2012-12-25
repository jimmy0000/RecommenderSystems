import time
import itertools

socialgraph = {}
danglingnodes = {}
newPageRankDict = {}

def LoadSocialNetworkGraph(filename, usermappingfilename):
  global socialgraph
  
  f = open(filename)
  start_clock = time.clock()
  lineno = 0
  for line in f:
    vec = line.strip().split()
    followerid = int(vec[0])
    followeeid = int(vec[1])
    
    if followerid not in socialgraph: socialgraph[followerid] = [0.0, 0, []]
    if followeeid not in socialgraph: socialgraph[followeeid] = [0.0, 0, []]
    
    (pagerank, count, adjlist) = socialgraph[followerid] 
    adjlist.append( followeeid )
    socialgraph[followerid] = (pagerank, count + 1, adjlist)
    
    
    lineno += 1
    if lineno % 1000000 == 0: 
      print 'Time taken to load:', lineno, 'lines is ', time.clock() - start_clock, 'seconds'

  f.close()

  print 'Time taken to load:', lineno, 'lines is ', time.clock() - start_clock, 'seconds'

  numusers = len(socialgraph)
  print numusers

  f = open(usermappingfilename)
  lineno = 0
  for line in f:
    if lineno not in socialgraph: socialgraph[lineno] = [0.0, 0, []]   
 
    lineno += 1
    if lineno % 1000000 == 0: 
      print 'Time taken to load:', lineno, 'lines is ', time.clock() - start_clock, 'seconds'
    
  f.close()
  
  numusers = len(socialgraph)
  print numusers

  for followerid in socialgraph:
    (pagerank, count, adjlist) = socialgraph[followerid] 
    socialgraph[followerid] = (1.0 / float(numusers), count, adjlist)
    if count == 0: danglingnodes[followerid] = True
  #This took roughly 4 mins to load the user_sns.txt graph and 2.1 GB of memory to load this socialgraph[userid] = (pagerank, count, adjlist) mapping
  
  return numusers
  
def ComputePageRank(numusers, s = 0.85, tolerance = 0.0001):
  print 'Beginning module ComputePageRank with parameters:', 'numusers:', numusers, 's:', s, 'tolerance:', tolerance
  iteration = 1
  L1NormDifference = 1
  start_clock = time.clock()
  global newPageRankDict
  global socialgraph
  
  while L1NormDifference > tolerance:
    print 'Iteration:', iteration, 'started at time:', time.clock() - start_clock, 'seconds'
    
    newPageRankDict = {}
    innerproduct = 0.0
    for followerid in danglingnodes: innerproduct += socialgraph[followerid][0]
    print 'Inner Product Computation took:', time.clock() - start_clock, 'seconds'
    
    additionalPageRankTerm = s * innerproduct / float(numusers) + (1.0 - s) / float(numusers)
    for followerid in socialgraph: newPageRankDict[followerid] = additionalPageRankTerm
    for followerid in socialgraph:
      (pagerank, count, friendlist) = socialgraph[followerid]
      for followeeid in friendlist:
        newPageRankDict[followeeid] += s * pagerank / float(count)
    print 'Page Rank Computation took:', time.clock() - start_clock, 'seconds'
    
    L1NormDifference = 0.0
    for (followerid, newpagerank) in newPageRankDict.items():
      (oldpagerank, count, adjlist) = socialgraph[followerid] 
      L1NormDifference += abs(oldpagerank - newpagerank)
      socialgraph[followerid] = (newpagerank, count, adjlist) 
      
    print 'Change in L1Norm:', L1NormDifference, 'Computation took:', time.clock() - start_clock, 'seconds'
    
    iteration += 1
  
  print 'Total Iterations:', iteration, 'took time:', time.clock() - start_clock, 'seconds'
  
  
def OutputPageRankVectorToFile(file):
  f = open(file, 'w')
  for (followerid, pagerank) in newPageRankDict.items():
    text = '%d\t%.9f\n'%(followerid, pagerank)
    f.write(text)
  f.close()
  
def main():
  #numusers = LoadSocialNetworkGraph('user_sns_mapped_sorted.txt', 'userid_mapping.txt')
  numusers = LoadSocialNetworkGraph('undirgraph_mapped_sorted_deduped.txt', 'userid_mapping.txt')
  ComputePageRank(numusers)
  OutputPageRankVectorToFile('user_mapped_pagerank_undirected.txt')
  
if __name__ == '__main__':
  main()
