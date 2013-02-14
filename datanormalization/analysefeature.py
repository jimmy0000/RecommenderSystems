import matplotlib.pyplot as plt
import sys
import numpy as np
import math
import time

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

def process(featurename, filename, filetype, columnindex, flag, minval, maxval, minlogval, maxlogval):
  start_time = time.clock()

  MIN_LOG_VALUE = 1e-11
  DEFAULT_LOG_VALUE = math.log(MIN_LOG_VALUE)

  print filename, columnindex
  filelen = linecount(filename)
  origvalues = np.zeros(filelen)
  transformedvalues = np.zeros(filelen, dtype=int)
  logvalues = np.zeros(filelen)
  transformedlogvalues = np.zeros(filelen, dtype=int)
  
  indices = []
  counts = []
  logcounts = []
  for index in xrange(256): 
    indices.append(index)
    counts.append(0)
    logcounts.append(0)

  f = open(filename, 'rU')
  lineno = 0
  if filetype == 'train':
    minlogval = minval = float(sys.maxint)
    maxlogval = maxval = -float(sys.maxint)

    for line in f:
      val = float(line.strip().split()[columnindex])
      if val < minval: minval = val
      if val > maxval: maxval = val

      origvalues[lineno] = val
      lineno += 1 
  
    print 'minimum:', minval, 'maximum:', maxval
  else:
    for line in f:
      val = float(line.strip().split()[columnindex])
      if val < minval: val = minval
      if val > maxval: val = maxval

      origvalues[lineno] = val
      lineno += 1 
 
  f.close()
  
  for index in xrange(lineno):
    val = origvalues[index]
    if math.isnan(val): val = minval
    fraction = (val - minval) / (maxval - minval)
    val = int(fraction * 255)
    counts[val] += 1
    transformedvalues[index] = val
    logval = DEFAULT_LOG_VALUE
    if fraction > MIN_LOG_VALUE: logval = math.log(fraction)
    if logval < minlogval: minlogval = logval
    if logval > maxlogval: maxlogval = logval
    logvalues[index] = logval

  for index in xrange(lineno):
    fraction = (logvalues[index] - minlogval) / (maxlogval - minlogval)
    val = int(fraction * 255)
    logcounts[val] += 1
    transformedlogvalues[index] = val

  print 'it took', time.clock() - start_time, 'seconds for processing', filename
  sys.stdout.flush()

  if filetype == 'train':
    plt.clf()
    plt.cla()
    plt.plot(indices, counts)
    plt.xlabel('featurevalue')
    plt.ylabel('count')
    plt.title('feature distribution of '+ featurename + ' column:' + str(columnindex))
    plt.savefig('normalizedfeatures/' + featurename + '_column_'+ str(columnindex)+'.png', format='png')

    plt.clf()
    plt.cla()
    plt.plot(indices, logcounts)
    plt.xlabel('logfeaturevalue')
    plt.ylabel('count')
    plt.title('feature distribution of log: '+ featurename + ' column:' + str(columnindex))
    plt.savefig('normalizedfeatures/' + featurename + '_log_column_'+ str(columnindex)+'.png', format='png')

    coverage = float(lineno - counts[0]) * 100.0 / float(lineno)
    logcoverage = float(lineno - logcounts[0]) * 100.0 / float(lineno)
    print 'minimum:', minval, 'maximum:', maxval, '%coverage:', coverage
    print 'log-minimum:', minlogval, 'log-maximum:', maxlogval, '%log-coverage', logcoverage

    flag = raw_input('Enter value in range[0-3] for flag: ')
    flag = int(flag)


  start_time = time.clock()

  if flag % 2 == 1:
    np.savetxt('normalizedfeatures/' + outputFeatureFileName(featurename, filetype, columnindex, ''), transformedvalues, fmt='%d')

  if flag >= 2:
    np.savetxt('normalizedfeatures/' + outputFeatureFileName(featurename, filetype, columnindex, 'log'), transformedlogvalues, fmt='%d')

  print 'it took', time.clock() - start_time, 'seconds for writing output'
  sys.stdout.flush()

  return (flag, minval, maxval, minlogval, maxlogval)

def outputFeatureFileName(featurename, filetype, columnindex, prefix):
  return 'normalizedfeature_' + prefix + featurename + '_' + filetype + '_' + str(columnindex) + '.txt'

def absoluteFeatureName(featurename, filetype):
  return 'feature_' + featurename + '_' + filetype + '.txt'

def main(featurename, begincolumnindex, endcolumnindex):
  print featurename, begincolumnindex, endcolumnindex

  numColumns = getNumberOfColumns('features/' + absoluteFeatureName(featurename, 'train'))

  if endcolumnindex == -1:
    endcolumnindex = numColumns - 1

  print 'NumColumns:', numColumns, 'EndColumnIndex:', endcolumnindex

  for columnindex in range(begincolumnindex, endcolumnindex + 1):
    flag = 0
    minlogval = minval = float(sys.maxint)
    maxlogval = maxval = -float(sys.maxint)
  
    filetypes = ['train', 'validation', 'publictest', 'privatetest']
    for filetype in filetypes:
      (flag, minval, maxval, minlogval, maxlogval) = process(featurename, 'features/' + absoluteFeatureName(featurename, filetype), filetype, columnindex, flag, minval, maxval, minlogval, maxlogval)

if __name__ == '__main__':
  if len(sys.argv) < 4:
    print 'Usage: python analysefeature.py <feature_name> <zero_based_start_column_index> <zero_based_end_column_index>'
  else: 
    main(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
