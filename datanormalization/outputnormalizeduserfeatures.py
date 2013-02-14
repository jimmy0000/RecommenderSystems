import matplotlib.pyplot as plt
import sys
import numpy as np
import math
import time

users = {}
userfeatures = np.zeros((1, 3), dtype=int)

def loadUserMapping(filename):
  print 'Loading User Mapping'
  global users

  f = open(filename, 'rU')
  lineno = 0
  for line in f:
    users[int(line.strip())] = lineno
    lineno += 1

  f.close()
  return lineno

def loadUserProfile(filename, numUsers):
  print 'Loading User Profile'
  global users
  global userfeatures

  userfeatures = np.zeros( (numUsers, 3) , dtype=int)

  f = open(filename, 'rU')

  lineno = 0
  for line in f:
    vec = line.strip().split()
    userid = users[int(vec[0])]
    age = vec[1]
    if age.find('-') != -1: age = '2012'
    age = int(age)
    if age > 2012: age = 2012
    userfeatures[userid][0] = 2012 - age
    gender = int(vec[2])
    if gender == 3: gender = 0
    userfeatures[userid][1] = gender
    userfeatures[userid][2] = int(vec[3]) 

  f.close()

  minfeaturevalues = userfeatures.min(axis = 0)
  maxfeaturevalues = userfeatures.max(axis = 0)
  print 'minage:', minfeaturevalues[0], 'maxage:', maxfeaturevalues[0]
  print 'mingender:', minfeaturevalues[1], 'maxgender:', maxfeaturevalues[1]
  print 'min#tweets:', minfeaturevalues[2], 'max#tweets:', maxfeaturevalues[2]


def linecount(filename):
  lineno = 0
  f = open(filename, 'rU')
  for line in f: lineno += 1
  return lineno

def processUserNumberOfTweets(numUsers):
  print 'Processing User Number of Tweets'
  global userfeatures

  start_time = time.clock()

  MIN_LOG_VALUE = 1e-11
  DEFAULT_LOG_VALUE = math.log(MIN_LOG_VALUE)

  origvalues = np.zeros(numUsers)
  transformedvalues = np.zeros(numUsers, dtype=int)
  logvalues = np.zeros(numUsers)
  transformedlogvalues = np.zeros(numUsers, dtype=int)

  indices = []
  counts = []
  logcounts = []
  for index in xrange(256):
    indices.append(index)
    counts.append(0)
    logcounts.append(0)

  minlogval = minval = float(sys.maxint)
  maxlogval = maxval = -float(sys.maxint)

  for userid in xrange(numUsers):
    val = userfeatures[userid][2]
    if val < minval: minval = val
    if val > maxval: maxval = val

    origvalues[userid] = val

  print 'minimum:', minval, 'maximum:', maxval

  for userid in xrange(numUsers):
    val = origvalues[userid]
    if math.isnan(val): val = minval
    fraction = (val - minval) / (maxval - minval)
    val = int(fraction * 255)
    counts[val] += 1
    transformedvalues[userid] = val
    logval = DEFAULT_LOG_VALUE
    if fraction > MIN_LOG_VALUE: logval = math.log(fraction)
    if logval < minlogval: minlogval = logval
    if logval > maxlogval: maxlogval = logval
    logvalues[userid] = logval

  for userid in xrange(numUsers):
    fraction = (logvalues[userid] - minlogval) / (maxlogval - minlogval)
    val = int(fraction * 255)
    logcounts[val] += 1
    transformedlogvalues[userid] = val

  print 'it took', time.clock() - start_time, 'seconds for processing user number of tweets'
  sys.stdout.flush()

  plt.clf()
  plt.cla()
  plt.plot(indices, counts)
  plt.xlabel('featurevalue')
  plt.ylabel('count')
  plt.title('feature distribution of user number of tweets')
  plt.savefig('normalizedfeatures/feature_user_numtweets.png', format='png')

  plt.clf()
  plt.cla()
  plt.plot(indices, logcounts)
  plt.xlabel('logfeaturevalue')
  plt.ylabel('count')
  plt.title('feature distribution of log(user number of tweets)')
  plt.savefig('normalizedfeatures/feature_log_user_numtweets.png', format='png')

  coverage = float(numUsers - counts[0]) * 100.0 / float(numUsers)
  logcoverage = float(numUsers - logcounts[0]) * 100.0 / float(numUsers)
  print 'minimum:', minval, 'maximum:', maxval, '%coverage:', coverage
  print 'log-minimum:', minlogval, 'log-maximum:', maxlogval, '%log-coverage', logcoverage

  flag = raw_input('Enter value in range[0-1] for flag: ')
  flag = int(flag)

  start_time = time.clock()

  if flag == 0:
    for userid in xrange(numUsers): 
      userfeatures[userid][2] = transformedvalues[userid]

  else:
    for userid in xrange(numUsers):
      userfeatures[userid][2] = transformedlogvalues[userid]

def outputUserFeatures(inputfile, outputfile, filetype):
  print 'input:', inputfile, 'output:', outputfile
  global userfeatures

  start_time = time.clock()

  f = open(inputfile, 'rU')
  fw = open(outputfile, 'w')

  lineno = 0
  for line in f:
    userid = int(line.strip().split()[0])
    fw.write(str(userfeatures[userid][0]) + ' ' + str(userfeatures[userid][1]) + ' ' + str(userfeatures[userid][2]) + '\n')

    lineno += 1
    if lineno % 1000000 == 0:
      print 'processed', lineno, 'lines in ', time.clock() - start_time , 'seconds'
      sys.stdout.flush()

  fw.close()
  f.close()

  if filetype == 'train':
    agecoverage = 0
    gendercoverage = 0
    numtweetscoverage = 0
    f = open(inputfile, 'rU')
    lineno = 0
    for line in f:
      userid = int(line.strip().split()[0])
      if userfeatures[userid][0] == 0: agecoverage += 1
      if userfeatures[userid][1] == 0: gendercoverage += 1
      if userfeatures[userid][2] == 0: numtweetscoverage += 1
      lineno += 1
    f.close()

    agecoverage = float(lineno - agecoverage) * 100.0 / float(lineno)
    gendercoverage = float(lineno - gendercoverage) * 100.0 / float(lineno)
    numtweetscoverage = float(lineno - numtweetscoverage) * 100.0 / float(lineno)
    print 'agecoverage:', agecoverage, 'gendercoverage:', gendercoverage, 'numtweetscoverage:', numtweetscoverage
    sys.stdout.flush()

def main(user_profile_file, userid_mapping_file, inputfilevec):
  print user_profile_file, userid_mapping_file

  numUsers = loadUserMapping(userid_mapping_file)
  loadUserProfile(user_profile_file, numUsers)
  processUserNumberOfTweets(numUsers)

  filetypes = ['train', 'validation', 'publictest', 'privatetest']
  for index in range(len(inputfilevec)):
    outputfile = 'normalizedfeatures/normalized_user_features_' + filetypes[index] + '.txt'
    outputUserFeatures(inputfilevec[index], outputfile, filetypes[index])

if __name__ == '__main__':
  if len(sys.argv) < 7:
    print 'Usage: python outputnormalizeduserfeatures.py <path_to_user_profile> <path_to_userid_mapping> <path_to_input_train_file> <path_to_input_validation_file> <path_to_input_publictest_file> <path_to_input_privatetest_file>'
  else:
    inputfilevec = []
    for index in range(3, 7): inputfilevec.append(sys.argv[index])

    main(sys.argv[1], sys.argv[2], inputfilevec)

