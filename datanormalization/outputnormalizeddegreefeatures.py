import matplotlib.pyplot as plt
import sys
import numpy as np
import math
import time

degreefeatures = np.zeros((1, 1))
BASE_DIRECTORY = './standalonefeatures/'
# ARVIND: This is really ugly preprocessed storage
# I should instead use the python file library.
inputFiles = ['vertex_in_out_degree_ataction_bfsdepth2.txt',
'vertex_in_out_degree_ataction_bfsdepth3.txt',
'vertex_in_out_degree_ataction.txt',
'vertex_in_out_degree_comment_bfsdepth2.txt',
'vertex_in_out_degree_comment.txt',
'vertex_in_out_degree_retweet_bfsdepth2.txt',
'vertex_in_out_degree_retweet.txt',
'vertex_in_out_degree.txt']

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

def loadDegreeFeatures(filename, featureindex):
  print 'loading Degree Features', filename, 'featureindex:', featureindex
  global degreefeatures
  
  f = open(filename, 'rU')
  lineno = 0
  for line in f:
    if line.startswith('#'): continue
    vec = line.strip().split()
    indegree = int(vec[1])
    outdegree = int(vec[2])
    totaldegree = indegree + outdegree
    indegreeratio = 0.0
    if totaldegree != 0: indegreeratio = float(indegree) / float(totaldegree)
    degreefeatures[lineno][featureindex*4 + 0] = indegree
    degreefeatures[lineno][featureindex*4 + 1] = outdegree
    degreefeatures[lineno][featureindex*4 + 2] = totaldegree
    degreefeatures[lineno][featureindex*4 + 3] = indegreeratio
    

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
  plt.title('feature distribution of '+entityType+'_' + featuretypes[featureindex])
  plt.savefig('normalizedfeatures/feature_'+entityType+'_'+featuretypes[featureindex]+'.png', format='png')

  plt.clf()
  plt.cla()
  plt.plot(indices, logcounts)
  plt.xlabel('logfeaturevalue')
  plt.ylabel('count')
  plt.title('feature distribution of log('+entityType+'_'+ featuretypes[featureindex] +')')
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
  global degreefeatures
  numfeatures = degreefeatures.shape[1]

  for index in xrange(numfeatures):
    processFeature(degreefeatures, index, 'vertex')

def outputDegreeFeatures(inputfile, outputfile):
  print 'input:', inputfile, 'output:', outputfile
  global degreefeatures
  global featuretypes
  numfeatures = len(featuretypes)

  start_time = time.clock()

  f = open(inputfile, 'rU')
  fw = open(outputfile, 'w')

  lineno = 0
  for line in f:
    userid = int(line.strip().split()[0])
    res = str(int(degreefeatures[userid][0]))
    for index in xrange(1, numfeatures): res += ' ' + str(int(degreefeatures[userid][index]))
    fw.write(res + '\n')

    lineno += 1
    if lineno % 1000000 == 0:
      print 'processed', lineno, 'lines in ', time.clock() - start_time , 'seconds'
      sys.stdout.flush()

  fw.close()
  f.close()

def outputTrainDegreeFeatures(inputfile, outputfile):
  print 'input:', inputfile, 'output:', outputfile
  global degreefeatures
  global featuretypes
  
  start_time = time.clock()

  numfeatures = len(featuretypes)
  degreefeaturescoverage = np.zeros(numfeatures)

  f = open(inputfile, 'rU')
  fw = open(outputfile, 'w')

  lineno = 0
  for line in f:
    userid = int(line.strip().split()[0])
    val = int(degreefeatures[userid][0])
    res = str(val)
    if val == 0: degreefeaturescoverage[0] += 1

    for index in xrange(1, numfeatures): 
      val = int(degreefeatures[userid][index])
      res += ' ' + str(val)
      if val == 0: degreefeaturescoverage[index] += 1
      
    fw.write(res + '\n')

    lineno += 1
    if lineno % 1000000 == 0:
      print 'processed', lineno, 'lines in ', time.clock() - start_time , 'seconds'
      sys.stdout.flush()

  fw.close()
  f.close()

  for index in xrange(numfeatures):
    degreefeaturescoverage[index] = float(lineno - degreefeaturescoverage[index]) * 100.0 / float(lineno)
    
  for index in range(len(featuretypes)):
    print 'vertex_'+featuretypes[index]+'_coverage:', degreefeaturescoverage[index]
    sys.stdout.flush()

def main(inputfilevec):
  global inputFiles
  global featuretypes
  global degreefeatures

  numUsers = linecount(BASE_DIRECTORY + inputFiles[0])
  # The multiplicative factor of 4 is necessary because each file
  # has 2 columns indegree, outdegree and 2 other derived features
  # totaldegree and indegree ratio
  numFeatures = len(inputFiles) * 4

  degreefeatures = np.zeros( (numUsers, numFeatures) )

  for index in range(len(inputFiles)):
    basetype = inputFiles[index].replace('vertex_in_out_degree_', '').replace('.txt', '')
    basetype = basetype.replace('vertex_in_out_degree', 'vertex')
    featuretypes.append(basetype + '_indegree')
    featuretypes.append(basetype + '_outdegree')
    featuretypes.append(basetype + '_totaldegree')
    featuretypes.append(basetype + '_indegree_ratio')

    loadDegreeFeatures(BASE_DIRECTORY + inputFiles[index], index)

  if len(featuretypes) != numFeatures:
    print 'len(featuretypes):', len(featuretypes), ' != numfeatures:', numFeatures
    sys.exit(1)


  minfeaturevalues = degreefeatures.min(axis = 0)
  maxfeaturevalues = degreefeatures.max(axis = 0)

  for index in range(len(featuretypes)):
    print 'min_vertex_'+featuretypes[index]+':', minfeaturevalues[index], 'max_vertex_'+featuretypes[index]+':', maxfeaturevalues[index]
  
  processAllFeatures()

  filetypes = ['train', 'validation', 'publictest', 'privatetest']
  for index in range(len(inputfilevec)):
    outputfile = 'normalizedfeatures/normalized_vertex_features_' + filetypes[index] + '.txt'
    if index == 0: 
      outputTrainDegreeFeatures(inputfilevec[index], outputfile)
    else:
      outputDegreeFeatures(inputfilevec[index], outputfile)

if __name__ == '__main__':
  if len(sys.argv) < 5:
    print 'Usage: python outputnormalizeddegreefeatures.py <path_to_input_train_file> <path_to_input_validation_file> <path_to_input_publictest_file> <path_to_input_privatetest_file>'
  else:
    inputfilevec = []
    for index in range(1, 5): inputfilevec.append(sys.argv[index])

    main(inputfilevec)

