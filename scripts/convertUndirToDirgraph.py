f = open('weighteddirgraph_mapped_ataction_sorted.txt')
fw = open('weighteddirgraph_mapped_ataction_sorted_directed.txt', 'w')
for line1 in f:
  line = line1.strip()
  if line.split()[-1] == '0': continue
  fw.write(line + '\n')
fw.close()
f.close()
