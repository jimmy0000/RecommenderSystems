import time

inputFiles = ['data/rec_log_train_sorted_mapped_train.txt',
'data/rec_log_train_sorted_mapped_validation.txt',
'data/rec_log_test_public_result_sorted_mapped.txt',
'data/rec_log_test_private_result_sorted_mapped.txt']

filetypes = ['train', 'validation', 'publictest', 'privatetest']

for index in range(len(inputFiles)):
  print 'processing', inputFiles[index]
  f = open(inputFiles[index], 'rU')
  fw = open('result_' + filetypes[index] + '.txt', 'w')
  lineno = 0
  start_time =  time.clock()
  for line in f:
    result = line.strip().split()[2]
    fw.write(result + '\n')
    lineno += 1
    if lineno % 1000000 == 0: 
      print 'processed', lineno, 'lines in', time.clock() - start_time, 'seconds'

  fw.close()
  f.close()
  
