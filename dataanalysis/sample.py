import math

vec = []
trainpercentage = 0.85

f = open('rec_log_train_sorted_mapped.txt')
fwtrain = open('rec_log_train_sorted_mapped_train.txt', 'w')
fwvalid = open('rec_log_train_sorted_mapped_validation.txt', 'w')

prevuserid = 0
numitems = 0

for line in f:
  userid = int(line.split()[0])
  if userid != prevuserid:
    numTrainingElements = int(math.ceil(trainpercentage * numitems))
    for vecindex in range(numTrainingElements): fwtrain.write(vec[vecindex])
    for vecindex in range(numTrainingElements, numitems): fwvalid.write(vec[vecindex])
    numitems = 0
    vec = []

  numitems += 1
  vec.append(line)
  prevuserid = userid

if numitems > 0:
  numTrainingElements = int(math.ceil(trainpercentage * numitems))
  for vecindex in range(numTrainingElements): fwtrain.write(vec[vecindex])
  for vecindex in range(numTrainingElements, numitems): fwvalid.write(vec[vecindex])
  
fwtrain.close()
fwvalid.close()
f.close()
