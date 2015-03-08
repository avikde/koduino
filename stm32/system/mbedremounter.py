import re, subprocess
 
p = re.compile(r'(/dev/\S+) on (\S*(MBED|NUCLEO).*) \(msdos.*read-only.*\)')
for r in p.finditer(subprocess.check_output('mount')):
  (d, m) = r.group(1,2)
  subprocess.check_call(['umount', d])
  subprocess.check_call(['mkdir', m])
  subprocess.check_call(['mount', '-w', '-o', 'sync', '-t', 'msdos', d, m])