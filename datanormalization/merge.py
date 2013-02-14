import sys
import time

BASE_DIRECTORY = './validation/'
inputFiles = ['result_validation.txt',
'normalized_bias_features_validation.txt',
'normalized_user_features_validation.txt',
'normalized_item_features_validation.txt',
'normalizedfeature_itemitemcosinesimilarity_validation_2.txt',
'normalizedfeature_itemitempearsonsimilarity_validation_2.txt',
'normalizedfeature_rsvdrating_validation_2.txt',
'normalizedfeature_rsvdrating_validation_3.txt',
'normalizedfeature_rsvdrating_validation_4.txt',
'normalizedfeature_rsvdkeywordsimilarity_validation_2.txt',
'normalizedfeature_rsvdkeywordsimilarity_validation_3.txt',
'normalizedfeature_rsvdtagsimilarity_validation_2.txt',
'normalizedfeature_rsvdtagsimilarity_validation_3.txt',
'normalizedfeature_categorysimilarity_validation_2.txt',
'normalizedfeature_categorysimilarity_validation_3.txt',
'normalizedfeature_categorysimilarity_validation_4.txt',
'normalized_pagerank_features_validation.txt',
'normalized_vertex_features_validation.txt',
'normalizedfeature_logfriendgraph_validation_2.txt',
'normalizedfeature_logfriendgraph_validation_3.txt',
'normalizedfeature_logfriendgraph_validation_4.txt',
'normalizedfeature_logfriendgraph_validation_5.txt',
'normalizedfeature_logfriendgraph_validation_6.txt',
'normalizedfeature_logatactiongraph_validation_2.txt',
'normalizedfeature_logatactiongraph_validation_3.txt',
'normalizedfeature_logatactiongraph_validation_4.txt',
'normalizedfeature_logatactiongraph_validation_5.txt',
'normalizedfeature_logatactiongraph_validation_6.txt',
'normalizedfeature_logatactiongraph_validation_7.txt',
'normalizedfeature_logatactiongraph_validation_8.txt',
'normalizedfeature_logatactiongraph_validation_9.txt',
'normalizedfeature_atactiongraph_validation_10.txt',
'normalizedfeature_logatactiongraph_validation_11.txt',
'normalizedfeature_logatactiongraph_bfsdepth2_validation_2.txt',
'normalizedfeature_logatactiongraph_bfsdepth2_validation_3.txt',
'normalizedfeature_logatactiongraph_bfsdepth2_validation_4.txt',
'normalizedfeature_atactiongraph_bfsdepth2_validation_5.txt',
'normalizedfeature_logatactiongraph_bfsdepth2_validation_6.txt',
'normalizedfeature_logatactiongraph_bfsdepth2_validation_7.txt',
'normalizedfeature_logatactiongraph_bfsdepth2_validation_8.txt',
'normalizedfeature_logatactiongraph_bfsdepth2_validation_9.txt',
'normalizedfeature_atactiongraph_bfsdepth2_validation_10.txt',
'normalizedfeature_logatactiongraph_bfsdepth2_validation_11.txt',
'normalizedfeature_logatactiongraph_bfsdepth3_validation_2.txt',
'normalizedfeature_logatactiongraph_bfsdepth3_validation_3.txt',
'normalizedfeature_logatactiongraph_bfsdepth3_validation_4.txt',
'normalizedfeature_atactiongraph_bfsdepth3_validation_5.txt',
'normalizedfeature_logatactiongraph_bfsdepth3_validation_6.txt',
'normalizedfeature_logatactiongraph_bfsdepth3_validation_7.txt',
'normalizedfeature_logatactiongraph_bfsdepth3_validation_8.txt',
'normalizedfeature_atactiongraph_bfsdepth3_validation_9.txt',
'normalizedfeature_atactiongraph_bfsdepth3_validation_10.txt',
'normalizedfeature_logatactiongraph_bfsdepth3_validation_11.txt',
'normalizedfeature_logcommentgraph_validation_2.txt',
'normalizedfeature_logcommentgraph_validation_3.txt',
'normalizedfeature_logcommentgraph_validation_4.txt',
'normalizedfeature_logcommentgraph_validation_5.txt',
'normalizedfeature_logcommentgraph_validation_6.txt',
'normalizedfeature_logcommentgraph_validation_7.txt',
'normalizedfeature_logcommentgraph_validation_8.txt',
'normalizedfeature_logcommentgraph_validation_9.txt',
'normalizedfeature_logcommentgraph_validation_10.txt',
'normalizedfeature_logcommentgraph_validation_11.txt',
'normalizedfeature_logcommentgraph_bfsdepth2_validation_2.txt',
'normalizedfeature_logcommentgraph_bfsdepth2_validation_3.txt',
'normalizedfeature_logcommentgraph_bfsdepth2_validation_4.txt',
'normalizedfeature_commentgraph_bfsdepth2_validation_5.txt',
'normalizedfeature_logcommentgraph_bfsdepth2_validation_6.txt',
'normalizedfeature_logcommentgraph_bfsdepth2_validation_7.txt',
'normalizedfeature_logcommentgraph_bfsdepth2_validation_8.txt',
'normalizedfeature_logcommentgraph_bfsdepth2_validation_9.txt',
'normalizedfeature_commentgraph_bfsdepth2_validation_10.txt',
'normalizedfeature_logcommentgraph_bfsdepth2_validation_11.txt',
'normalizedfeature_logretweetgraph_validation_2.txt',
'normalizedfeature_logretweetgraph_validation_3.txt',
'normalizedfeature_logretweetgraph_validation_4.txt',
'normalizedfeature_logretweetgraph_validation_5.txt',
'normalizedfeature_logretweetgraph_validation_6.txt',
'normalizedfeature_logretweetgraph_validation_7.txt',
'normalizedfeature_logretweetgraph_validation_8.txt',
'normalizedfeature_logretweetgraph_validation_9.txt',
'normalizedfeature_logretweetgraph_validation_10.txt',
'normalizedfeature_logretweetgraph_validation_11.txt',
'normalizedfeature_logretweetgraph_bfsdepth2_validation_2.txt',
'normalizedfeature_logretweetgraph_bfsdepth2_validation_3.txt',
'normalizedfeature_logretweetgraph_bfsdepth2_validation_4.txt',
'normalizedfeature_retweetgraph_bfsdepth2_validation_5.txt',
'normalizedfeature_logretweetgraph_bfsdepth2_validation_6.txt',
'normalizedfeature_logretweetgraph_bfsdepth2_validation_7.txt',
'normalizedfeature_logretweetgraph_bfsdepth2_validation_8.txt',
'normalizedfeature_logretweetgraph_bfsdepth2_validation_9.txt',
'normalizedfeature_retweetgraph_bfsdepth2_validation_10.txt',
'normalizedfeature_logretweetgraph_bfsdepth2_validation_11.txt']

outputFile = 'extraction_validation.txt'

linecount = 0
f = open(BASE_DIRECTORY + inputFiles[0], 'rU')
for line in f:
  linecount += 1
f.close()

fileHandles = []
for index in range(len(inputFiles)):
  handle = open(BASE_DIRECTORY + inputFiles[index], 'rU')
  fileHandles.append( handle )

numFileHandles = len(fileHandles)

if numFileHandles != len(inputFiles):
  print 'ERROR: numFileHandles:', numFileHandles, ' != len(inputFiles)', len(inputFiles)
  sys.exit(1)

fw = open(outputFile, 'w')
start_clock = time.clock()

for lineno in xrange(linecount):
  for index in xrange(numFileHandles):
    handle = fileHandles[index]
    fw.write( handle.readline().strip() )
    if index < numFileHandles - 1: fw.write(' ')
    else: fw.write('\n')

  if lineno % 1000000 == 0:
    print 'it took', time.clock() - start_clock, 'seconds for processing', lineno, 'lines'

fw.close()
