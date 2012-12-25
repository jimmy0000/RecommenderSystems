dict = {}

def PopulateDict(identifier):
  global dict
  f = open('KDD_Track1_solution.csv')
  for line in f:
    if line.startswith('#'): continue
    vec = line.strip().split(",")
    if vec[-1] != identifier : continue
    items = vec[1].split()
    userid = int(vec[0])
    if userid not in dict: dict[userid] = {}
    for stritemid in items:
      itemid = int(stritemid)
      dict[userid][itemid] = True
  f.close()


def main():
  global dict
  PopulateDict('Public')

  f = open('rec_log_test_public_sorted.txt')
  fw = open('rec_log_test_public_result_sorted.txt', 'w')
  lineno = 0
  for line in f:
    vec = line.strip().split()
    userid = int(vec[0])
    itemid = int(vec[1])
    yesorno = -1
    if itemid in dict[userid]: yesorno = 1
    fw.write(vec[0] + ' ' + vec[1] + ' ' + str(yesorno) + ' ' + vec[-1] + '\n')

    lineno += 1
    if lineno % 1000000 == 0: print lineno, 'lines processed'
  fw.close()
  f.close()

  dict = {}
  PopulateDict('Private')
 
  f = open('rec_log_test_private_sorted.txt')
  fw = open('rec_log_test_private_result_sorted.txt', 'w')
  lineno = 0
  for line in f:
    vec = line.strip().split()
    userid = int(vec[0])
    itemid = int(vec[1])
    yesorno = -1
    if itemid in dict[userid]: yesorno = 1
    fw.write(vec[0] + ' ' + vec[1] + ' ' + str(yesorno) + ' ' + vec[-1] + '\n')

    lineno += 1
    if lineno % 1000000 == 0: print lineno, 'lines processed'
  fw.close()
  f.close()

if __name__ == '__main__':
  main()
