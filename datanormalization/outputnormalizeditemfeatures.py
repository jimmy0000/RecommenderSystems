import matplotlib.pyplot as plt
import sys
import numpy as np
import math
import time

itemfeatures = np.zeros((1, 3))
featuretypes = [
'item_accept_count', 'item_reject_count', 
'sum_keyword_accept_count', 'sum_tag_accept_count', 'sum_category_0_accept_count', 'sum_category_0_1_accept_count', 'sum_category_0_1_2_accept_count', 'sum_category_0_1_2_3_accept_count', 
'max_keyword_accept_count', 'max_tag_accept_count', 'max_category_0_accept_count', 'max_category_0_1_accept_count', 'max_category_0_1_2_accept_count', 'max_category_0_1_2_3_accept_count', 
'sum_keyword_reject_count', 'sum_tag_reject_count', 'sum_category_0_reject_count', 'sum_category_0_1_reject_count', 'sum_category_0_1_2_reject_count', 'sum_category_0_1_2_3_reject_count', 
'max_keyword_reject_count', 'max_tag_reject_count', 'max_category_0_reject_count', 'max_category_0_1_reject_count', 'max_category_0_1_2_reject_count', 'max_category_0_1_2_3_reject_count',
'sum_keyword_popularity', 'sum_tag_popularity', 'sum_category_0_popularity', 'sum_category_0_1_popularity', 'sum_category_0_1_2_popularity', 'sum_category_0_1_2_3_popularity', 
'max_keyword_popularity', 'max_tag_popularity', 'max_category_0_popularity', 'max_category_0_1_popularity', 'max_category_0_1_2_popularity', 'max_category_0_1_2_3_popularity',
'item_accept_ratio', 'sum_keyword_accept_ratio', 'sum_tag_accept_ratio', 'sum_category_0_accept_ratio', 'sum_category_0_1_accept_ratio', 'sum_category_0_1_2_accept_ratio', 'sum_category_0_1_2_3_accept_ratio']

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


def loadItemFeatures(itemAcceptRejectFile):
  global itemfeatures
  global featuretypes

  numItems = linecount(itemAcceptRejectFile)
  numColumns = getNumberOfColumns(itemAcceptRejectFile)
  numfeatures = numColumns - 1
  numfeatures += 1 + 1 + 1 + 4 # accept/ (accept + reject) ratio for item,
                               # keyword, tag, 4 levels of category
  print 'Total Number Of Features', numfeatures

  if numfeatures != len(featuretypes):
    print 'ERROR: numfeatures', numfeatures, 'does not equal len(featuretypes)', len(featuretypes)
    sys.exit(1)

  itemfeatures = np.zeros( (numItems, numfeatures) )

  accept_indices = [0, 2, 3, 4, 5, 6, 7]
  reject_indices = [1, 14, 15, 16, 17, 18, 19]
  if len(accept_indices) != len(reject_indices):
    print 'len(accept_indices) != len(reject_indices)'
    sys.exit(1)

  numAcceptRejectRatioFeatures = len(accept_indices)

  f = open(itemAcceptRejectFile, 'rU')
  lineno = 0
  for line in f:
    if line.startswith('#'): continue
    vec = line.strip().split()
    for index in xrange(1,numColumns):
      itemfeatures[lineno][index-1] = float(vec[index])

    for index in xrange(numAcceptRejectRatioFeatures):
      accept_count = itemfeatures[lineno][ accept_indices[index] ]
      reject_count = itemfeatures[lineno][ reject_indices[index] ]
      if accept_count + reject_count == 0: accept_ratio = 0.0
      else: accept_ratio = accept_count / (accept_count + reject_count)
      itemfeatures[lineno][numColumns - 1 + index] = accept_ratio

    lineno += 1

  f.close()

  minfeaturevalues = itemfeatures.min(axis = 0)
  maxfeaturevalues = itemfeatures.max(axis = 0)

  for index in range(len(featuretypes)):
    print 'min_item_'+featuretypes[index]+':', minfeaturevalues[index], 'max_item_'+featuretypes[index]+':', maxfeaturevalues[index]


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
  plt.title('feature distribution of item_' + featuretypes[featureindex])
  plt.savefig('normalizedfeatures/feature_'+entityType+'_'+featuretypes[featureindex]+'.png', format='png')

  plt.clf()
  plt.cla()
  plt.plot(indices, logcounts)
  plt.xlabel('logfeaturevalue')
  plt.ylabel('count')
  plt.title('feature distribution of log(item_'+ featuretypes[featureindex] +')')
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
  global itemfeatures
  numfeatures = itemfeatures.shape[1]

  for index in xrange(numfeatures):
    processFeature(itemfeatures, index, 'item')

def outputItemFeatures(inputfile, outputfile, filetype):
  print 'input:', inputfile, 'output:', outputfile
  global itemfeatures
  global featuretypes
  numfeatures = len(featuretypes)

  start_time = time.clock()

  f = open(inputfile, 'rU')
  fw = open(outputfile, 'w')

  lineno = 0
  for line in f:
    itemid = int(line.strip().split()[1])
    res = str(int(itemfeatures[itemid][0]))
    for index in xrange(1, numfeatures): res += ' ' + str(int(itemfeatures[itemid][index]))
    fw.write(res + '\n')

    lineno += 1
    if lineno % 1000000 == 0:
      print 'processed', lineno, 'lines in ', time.clock() - start_time , 'seconds'
      sys.stdout.flush()

  fw.close()
  f.close()

  if filetype == 'train':
    itemfeaturescoverage = np.zeros(numfeatures)
    
    f = open(inputfile, 'rU')
    lineno = 0
    for line in f:
      itemid = int(line.strip().split()[1])

      for index in xrange(numfeatures):
        if itemfeatures[itemid][index] == 0:
          itemfeaturescoverage[index] += 1

      lineno += 1
    f.close()

    for index in xrange(numfeatures):
      itemfeaturescoverage[index] = float(lineno - itemfeaturescoverage[index]) * 100.0 / float(lineno)
    
    for index in range(len(featuretypes)):
      print 'item_'+featuretypes[index]+'_coverage:', itemfeaturescoverage[index]
      sys.stdout.flush()

def main(itemAcceptRejectFile, inputfilevec):
  #NOTE: itemAcceptRejectFile already contains (sum, max) (keyword, tag, category) popularity features as well
  loadItemFeatures(itemAcceptRejectFile)

  processAllFeatures()

  filetypes = ['train', 'validation', 'publictest', 'privatetest']
  for index in range(len(inputfilevec)):
    outputfile = 'normalizedfeatures/normalized_item_features_' + filetypes[index] + '.txt'
    outputItemFeatures(inputfilevec[index], outputfile, filetypes[index])

if __name__ == '__main__':
  if len(sys.argv) < 6:
    print 'Usage: python outputnormalizeditemfeatures.py <item_accept_reject_file> <path_to_input_train_file> <path_to_input_validation_file> <path_to_input_publictest_file> <path_to_input_privatetest_file>'
  else:
    inputfilevec = []
    for index in range(2, 6): inputfilevec.append(sys.argv[index])

    main(sys.argv[1], inputfilevec)

