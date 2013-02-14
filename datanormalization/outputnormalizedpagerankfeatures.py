import matplotlib.pyplot as plt
import sys
import numpy as np
import math
import time

pagerankfeatures = np.zeros((1, 1))
BASE_DIRECTORY = './standalonefeatures/'
# ARVIND: This is really ugly preprocessed storage
# I should instead use the python file library.
inputFiles = ['user_mapped_pagerank_ataction_bfsdepth2_rooted.txt',
'user_mapped_pagerank_ataction_bfsdepth2.txt',
'user_mapped_pagerank_ataction_bfsdepth2_undirected_rooted.txt',
'user_mapped_pagerank_ataction_bfsdepth2_undirected.txt',
'user_mapped_pagerank_ataction_bfsdepth3_rooted.txt',
'user_mapped_pagerank_ataction_bfsdepth3.txt',
'user_mapped_pagerank_ataction_bfsdepth3_undirected_rooted.txt',
'user_mapped_pagerank_ataction_bfsdepth3_undirected.txt',
'user_mapped_pagerank_ataction_rooted.txt',
'user_mapped_pagerank_ataction_rooted_undirected.txt',
'user_mapped_pagerank_ataction.txt',
'user_mapped_pagerank_ataction_undirected.txt',
'user_mapped_pagerank_comment_bfsdepth2_rooted.txt',
'user_mapped_pagerank_comment_bfsdepth2.txt',
'user_mapped_pagerank_comment_bfsdepth2_undirected_rooted.txt',
'user_mapped_pagerank_comment_bfsdepth2_undirected.txt',
'user_mapped_pagerank_comment_rooted.txt',
'user_mapped_pagerank_comment_rooted_undirected.txt',
'user_mapped_pagerank_comment.txt',
'user_mapped_pagerank_comment_undirected.txt',
'user_mapped_pagerank_retweet_bfsdepth2_rooted.txt',
'user_mapped_pagerank_retweet_bfsdepth2.txt',
'user_mapped_pagerank_retweet_bfsdepth2_undirected_rooted.txt',
'user_mapped_pagerank_retweet_bfsdepth2_undirected.txt',
'user_mapped_pagerank_retweet_rooted.txt',
'user_mapped_pagerank_retweet_rooted_undirected.txt',
'user_mapped_pagerank_retweet.txt',
'user_mapped_pagerank_retweet_undirected.txt',
'user_mapped_pagerank_rooted.txt',
'user_mapped_pagerank.txt',
'user_mapped_pagerank_undirected_rooted.txt',
'user_mapped_pagerank_undirected.txt']

featuretypes = []

def getNumberOfColumns(filename):
  f = open(filename, 'rU')
  numColumns = 0 
  for line in f:
    numColumns = len(line.strip().split())
    break
  f.close()
  return numColumns

def linecount(filename):
  lineno = 0
  f = open(filename, 'rU')
  for line in f: lineno += 1
  return lineno

def loadPageRankFeatures(filename, featureindex):
  print 'loading PageRank Features', filename, 'featureindex:', featureindex
  global pagerankfeatures
  
  f = open(filename, 'rU')
  lineno = 0
  for line in f:
    if line.startswith('#'): continue
    
    val = float(line.strip().split()[1])
    pagerankfeatures[lineno][featureindex] = val

    lineno += 1

  f.close()

def processFeature(features, featureindex, entityType):
  print 'Processing features:', features.shape, ' featureindex:', featureindex
  global featuretypes

  start_time = time.clock()

  MIN_LOG_VALUE = 1e-11
  DEFAULT_LOG_VALUE = math.log(MIN_LOG_VALUE)

  numEntities = features.shape[0]
  origvalues = np.zeros(numEntities)
  transformedvalues = np.zeros(numEntities, dtype=int)
  logvalues = np.zeros(numEntities)
  transformedlogvalues = np.zeros(numEntities, dtype=int)

  indices = []
  counts = []
  logcounts = []
  for index in xrange(256):
    indices.append(index)
    counts.append(0)
    logcounts.append(0)

  minlogval = minval = float(sys.maxint)
  maxlogval = maxval = -float(sys.maxint)

  for entityid in xrange(numEntities):
    val = features[entityid][featureindex]
    if val < minval: minval = val
    if val > maxval: maxval = val

    origvalues[entityid] = val

  print 'minimum:', minval, 'maximum:', maxval

  for entityid in xrange(numEntities):
    val = origvalues[entityid]
    if math.isnan(val): val = minval
    fraction = (val - minval) / (maxval - minval)
    val = int(fraction * 255)
    counts[val] += 1
    transformedvalues[entityid] = val
    logval = DEFAULT_LOG_VALUE
    if fraction > MIN_LOG_VALUE: logval = math.log(fraction)
    if logval < minlogval: minlogval = logval
    if logval > maxlogval: maxlogval = logval
    logvalues[entityid] = logval

  for entityid in xrange(numEntities):
    fraction = (logvalues[entityid] - minlogval) / (maxlogval - minlogval)
    val = int(fraction * 255)
    logcounts[val] += 1
    transformedlogvalues[entityid] = val

  print 'it took', time.clock() - start_time, 'seconds for processing features shape:', features.shape, 'featureindex:', featureindex
  sys.stdout.flush()

  plt.clf()
  plt.cla()
  plt.plot(indices, counts)
  plt.xlabel('featurevalue')
  plt.ylabel('count')
  plt.title('feature distribution of pagerank_' + featuretypes[featureindex])
  plt.savefig('normalizedfeatures/feature_'+entityType+'_'+featuretypes[featureindex]+'.png', format='png')

  plt.clf()
  plt.cla()
  plt.plot(indices, logcounts)
  plt.xlabel('logfeaturevalue')
  plt.ylabel('count')
  plt.title('feature distribution of log(pagerank_'+ featuretypes[featureindex] +')')
  plt.savefig('normalizedfeatures/feature_log_'+entityType+'_' + featuretypes[featureindex] +'.png', format='png')

  coverage = float(numEntities - counts[0]) * 100.0 / float(numEntities)
  logcoverage = float(numEntities - logcounts[0]) * 100.0 / float(numEntities)
  print 'minimum:', minval, 'maximum:', maxval, '%coverage:', coverage
  print 'log-minimum:', minlogval, 'log-maximum:', maxlogval, '%log-coverage', logcoverage

  flag = raw_input('Enter value in range[0-1] for flag: ')
  flag = int(flag)

  start_time = time.clock()

  if flag == 0:
    for entityid in xrange(numEntities): 
      features[entityid][featureindex] = transformedvalues[entityid]

  else:
    for entityid in xrange(numEntities):
      features[entityid][featureindex] = transformedlogvalues[entityid]

def processAllFeatures():
  global pagerankfeatures
  numfeatures = pagerankfeatures.shape[1]

  for index in xrange(numfeatures):
    processFeature(pagerankfeatures, index, 'pagerank')

def outputPageRankFeatures(inputfile, outputfile):
  print 'input:', inputfile, 'output:', outputfile
  global pagerankfeatures
  global featuretypes
  numfeatures = len(featuretypes)

  start_time = time.clock()

  f = open(inputfile, 'rU')
  fw = open(outputfile, 'w')

  lineno = 0
  for line in f:
    userid = int(line.strip().split()[0])
    res = str(int(pagerankfeatures[userid][0]))
    for index in xrange(1, numfeatures): res += ' ' + str(int(pagerankfeatures[userid][index]))
    fw.write(res + '\n')

    lineno += 1
    if lineno % 1000000 == 0:
      print 'processed', lineno, 'lines in ', time.clock() - start_time , 'seconds'
      sys.stdout.flush()

  fw.close()
  f.close()

def outputTrainPageRankFeatures(inputfile, outputfile):
  print 'input:', inputfile, 'output:', outputfile
  global pagerankfeatures
  global featuretypes
  
  start_time = time.clock()

  numfeatures = len(featuretypes)
  pagerankfeaturescoverage = np.zeros(numfeatures)

  f = open(inputfile, 'rU')
  fw = open(outputfile, 'w')

  lineno = 0
  for line in f:
    userid = int(line.strip().split()[0])
    val = int(pagerankfeatures[userid][0])
    res = str(val)
    if val == 0: pagerankfeaturescoverage[0] += 1

    for index in xrange(1, numfeatures): 
      val = int(pagerankfeatures[userid][index])
      res += ' ' + str(val)
      if val == 0: pagerankfeaturescoverage[index] += 1
      
    fw.write(res + '\n')

    lineno += 1
    if lineno % 1000000 == 0:
      print 'processed', lineno, 'lines in ', time.clock() - start_time , 'seconds'
      sys.stdout.flush()

  fw.close()
  f.close()

  for index in xrange(numfeatures):
    pagerankfeaturescoverage[index] = float(lineno - pagerankfeaturescoverage[index]) * 100.0 / float(lineno)
    
  for index in range(len(featuretypes)):
    print 'pagerank_'+featuretypes[index]+'_coverage:', pagerankfeaturescoverage[index]
    sys.stdout.flush()

def main(inputfilevec):
  global inputFiles
  global featuretypes
  global pagerankfeatures

  numUsers = linecount(BASE_DIRECTORY + inputFiles[0])
  numFeatures = len(inputFiles)

  pagerankfeatures = np.zeros( (numUsers, numFeatures) )

  for index in range(len(inputFiles)):
    featuretypes.append( inputFiles[index].replace('user_mapped_pagerank_', '').replace('.txt', '') )
    loadPageRankFeatures(BASE_DIRECTORY + inputFiles[index], index)

  minfeaturevalues = pagerankfeatures.min(axis = 0)
  maxfeaturevalues = pagerankfeatures.max(axis = 0)

  for index in range(len(featuretypes)):
    print 'min_pagerank_'+featuretypes[index]+':', minfeaturevalues[index], 'max_pagerank_'+featuretypes[index]+':', maxfeaturevalues[index]
  
  processAllFeatures()

  filetypes = ['train', 'validation', 'publictest', 'privatetest']
  for index in range(len(inputfilevec)):
    outputfile = 'normalizedfeatures/normalized_pagerank_features_' + filetypes[index] + '.txt'
    if index == 0: 
      outputTrainPageRankFeatures(inputfilevec[index], outputfile)
    else:
      outputPageRankFeatures(inputfilevec[index], outputfile)

if __name__ == '__main__':
  if len(sys.argv) < 5:
    print 'Usage: python outputnormalizedpagerankfeatures.py <path_to_input_train_file> <path_to_input_validation_file> <path_to_input_publictest_file> <path_to_input_privatetest_file>'
  else:
    inputfilevec = []
    for index in range(1, 5): inputfilevec.append(sys.argv[index])

    main(inputfilevec)

