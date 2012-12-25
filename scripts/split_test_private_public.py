f = open('rec_log_test_sorted.txt')
fwprivate = open('rec_log_test_private_sorted.txt', 'w')
fwpublic = open('rec_log_test_public_sorted.txt', 'w')
split = 1321891200
lineno = 0
for line in f:
  timestamp = int(line.strip().split()[-1])
  if timestamp >= split: fwprivate.write(line)
  else: fwpublic.write(line)

  lineno += 1
  if lineno % 1000000 == 0: print lineno, 'lines processed'
fwpublic.close()
fwprivate.close()
f.close()
