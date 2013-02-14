import matplotlib.pyplot as plt
import sys
import numpy as np
import math
import time

userfeatures = np.zeros((1, 3))
itemfeatures = np.zeros((1, 3))
featuretypes = ['count', 'bias', 'offset']

def linecount(filename):
  lineno = 0
  f = open(filename, 'rU')
  for line in f: lineno += 1
  return lineno

def printFeatureStatistics(features, entityType):

  minfeaturevalues = features.min(axis = 0)
  maxfeaturevalues = features.max(axis = 0)

  for index in range(len(featuretypes)):
    print 'min'+entityType+featuretypes[index]+':', minfeaturevalues[index], 'max'+entityType+featuretypes[index]+':', maxfeaturevalues[index]

def loadFeatures(filename, features, entityCountIndex, entityBiasOffsetIndex, numEntities, entityType):
  print 'Loading Features', filename, features.shape

  f = open(filename, 'rU')

  lineno = 0
  for line in f:
    if line.startswith('#'): continue
    vec = line.strip().split()
    features[lineno][entityCountIndex] = float(vec[1])
    features[lineno][entityBiasOffsetIndex] = float(vec[2])
    lineno += 1
    if lineno >= numEntities: break

  f.close()

  print 'lineno:', lineno, 'numEntities:', numEntities

  printFeatureStatistics(features, entityType)

def loadUserAndItemBiasFeatures(numUsers, numItems, userBiasFile, userBiasOffsetFile, itemBiasFile, itemBiasOffsetFile):
  global userfeatures
  global itemfeatures

  userfeatures = np.zeros( (numUsers, 3) )
  itemfeatures = np.zeros( (numItems, 3) )

  print 'loading User Bias Features'
  loadFeatures(userBiasFile, userfeatures, 0, 1, numUsers, 'user')

  print 'loading User Bias Offset Features'
  loadFeatures(userBiasOffsetFile, userfeatures, 0, 2, numUsers, 'user')

  print 'loading Item Bias Features'
  loadFeatures(itemBiasFile, itemfeatures, 0, 1, numItems, 'item')

  print 'loading Item Bias Offset Features'
  loadFeatures(itemBiasOffsetFile, itemfeatures, 0, 2, numItems, 'item')

  printFeatureStatistics(userfeatures, 'user')
  printFeatureStatistics(itemfeatures, 'item')

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
  plt.title('feature distribution of bias_' + featuretypes[featureindex])
  plt.savefig('normalizedfeatures/feature_'+entityType+'_'+featuretypes[featureindex]+'.png', format='png')

  plt.clf()
  plt.cla()
  plt.plot(indices, logcounts)
  plt.xlabel('logfeaturevalue')
  plt.ylabel('count')
  plt.title('feature distribution of log(bias_'+ featuretypes[featureindex] +')')
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
  global userfeatures
  global itemfeatures

  for index in xrange(3):
    processFeature(userfeatures, index, 'user')
    processFeature(itemfeatures, index, 'item')

def outputBiasFeatures(inputfile, outputfile, filetype):
  print 'input:', inputfile, 'output:', outputfile
  global userfeatures
  global itemfeatures
  global featuretypes

  start_time = time.clock()

  f = open(inputfile, 'rU')
  fw = open(outputfile, 'w')

  lineno = 0
  for line in f:
    vec = line.strip().split()
    userid = int(vec[0])
    itemid = int(vec[1])
    res = ''
    for index in xrange(3): res += ' ' + str(int(userfeatures[userid][index]))
    for index in xrange(3): res += ' ' + str(int(itemfeatures[itemid][index]))
    fw.write(res[1:] + '\n')

    lineno += 1
    if lineno % 1000000 == 0:
      print 'processed', lineno, 'lines in ', time.clock() - start_time , 'seconds'
      sys.stdout.flush()

  fw.close()
  f.close()

  if filetype == 'train':
    userfeaturescoverage = np.zeros(3)
    itemfeaturescoverage = np.zeros(3)
    
    f = open(inputfile, 'rU')
    lineno = 0
    for line in f:
      vec = line.strip().split()
      userid = int(vec[0])
      itemid = int(vec[1])

      for index in xrange(3):
        if userfeatures[userid][index] == 0: 
          userfeaturescoverage[index] += 1
        
        if itemfeatures[itemid][index] == 0:
          itemfeaturescoverage[index] += 1

      lineno += 1
    f.close()

    for index in xrange(3):
      userfeaturescoverage[index] = float(lineno - userfeaturescoverage[index]) * 100.0 / float(lineno)
      itemfeaturescoverage[index] = float(lineno - itemfeaturescoverage[index]) * 100.0 / float(lineno)
    
    entityType = 'user'
    for index in range(len(featuretypes)):
      print entityType+featuretypes[index]+'coverage:', userfeaturescoverage[index]
      sys.stdout.flush()

    entityType = 'item'
    for index in range(len(featuretypes)):
      print entityType+featuretypes[index]+'coverage:', itemfeaturescoverage[index]
      sys.stdout.flush()

def main(numUsers, numItems, userBiasFile, userBiasOffsetFile, itemBiasFile, itemBiasOffsetFile, inputfilevec):

  loadUserAndItemBiasFeatures(numUsers, numItems, userBiasFile, userBiasOffsetFile, itemBiasFile, itemBiasOffsetFile)

  processAllFeatures()

  filetypes = ['train', 'validation', 'publictest', 'privatetest']
  for index in range(len(inputfilevec)):
    outputfile = 'normalizedfeatures/normalized_bias_features_' + filetypes[index] + '.txt'
    outputBiasFeatures(inputfilevec[index], outputfile, filetypes[index])

if __name__ == '__main__':
  if len(sys.argv) < 11:
    print 'Usage: python outputnormalizeduserfeatures.py <path_to_user_mapping_file> <path_to_item_mapping_file> <path_to_user_bias_file> <path_to_user_bias_offset_file> <path_to_item_bias_file> <path_to_item_bias_offset_file> <path_to_input_train_file> <path_to_input_validation_file> <path_to_input_publictest_file> <path_to_input_privatetest_file>'
  else:
    numUsers = linecount(sys.argv[1])
    numItems = linecount(sys.argv[2])
  
    inputfilevec = []
    for index in range(7, 11): inputfilevec.append(sys.argv[index])

    main(numUsers, numItems, sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6], inputfilevec)

