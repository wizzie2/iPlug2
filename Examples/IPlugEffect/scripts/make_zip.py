import zipfile, os, fileinput, string, sys, shutil

scriptpath = os.path.dirname(os.path.realpath(__file__))
projectpath = os.path.abspath(os.path.join(scriptpath, os.pardir))

IPLUG2_ROOT = "../../.."

sys.path.insert(0, os.path.join(scriptpath, IPLUG2_ROOT + '\Scripts'))

from parse_config import parse_config

MajorStr = ""
MinorStr = "" 
BugfixStr = ""
BUNDLE_NAME = ""

def  main():
  if len(sys.argv) != 2:
    print("Usage: make_zip.py demo(0 or 1)")
    sys.exit(1)
  else:
    demo=int(sys.argv[1])

  config = parse_config(projectpath)
  
  dir = projectpath + "\\build-win\\zip"
  
  if os.path.exists(dir):
    shutil.rmtree(dir)
  
  os.makedirs(dir)
  
  installer = "\\build-win\\installer\\IPlugEffect Installer.exe"
   
  if demo:
    installer = "\\build-win\\installer\\IPlugEffect Demo Installer.exe"
   
  files = [
    projectpath + installer,
    projectpath + "\installer\changelog.txt",
    projectpath + "\installer\known-issues.txt",
    projectpath + "\manual\IPlugEffect manual.pdf" 
  ]

  zipname = "IPlugEffect-v" + config['FULL_VER_STR']
  
  if demo:
    zipname = zipname + "-win-demo.zip"
  else:
    zipname = zipname + "-win.zip"

  zf = zipfile.ZipFile(projectpath + "\\build-win\\zip\\" + zipname, mode="w")

  for f in files:
    print("adding " + f)
    zf.write(f, os.path.basename(f));

  zf.close()
  print("wrote " + zipname)

if __name__ == '__main__':
  main()
