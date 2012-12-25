import random
import scipy
import time
from sklearn.ensemble import RandomForestClassifier

vec = range(50)

target = []
train = []

start_time = time.clock()

lineno = 0
while lineno < 20000000:
  target.append( random.randrange(2) )
  train.append( random.shuffle(vec) )
  lineno += 1
  if lineno % 1000000 == 0: print lineno

print 'done generating data', time.clock() - start_time, 'seconds'

rf = RandomForestClassifier(n_estimators=500, n_jobs=-1)

print 'training classifier'
start_time = time.clock()
rf.fit(train, target)
print 'done training', time.clock() - start_time, 'seconds'

start_time = time.clock()
rf.predict_proba(train)
print 'done predicting', time.clock() - start_time, 'seconds'
