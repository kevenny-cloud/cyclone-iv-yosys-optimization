"""Fetch portable public build tools into the workspace only."""
from pathlib import Path
import urllib.request,zipfile,hashlib,json,concurrent.futures,shutil
root=Path(__file__).resolve().parent/'windows-build-tools/regenerate-tools'
root.mkdir(exist_ok=True)
items={'cmake':('https://github.com/Kitware/CMake/releases/download/v3.31.6/cmake-3.31.6-windows-x86_64.zip','cmake.zip'),'ninja':('https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-win.zip','ninja.zip'),'winflexbison':('https://github.com/lexxmark/winflexbison/releases/download/v2.5.25/win_flex_bison-2.5.25.zip','winflexbison.zip')}
def fetch(item):
 key,(url,name)=item;path=root/name
 if not path.exists():
  with urllib.request.urlopen(url,timeout=60) as src,path.with_suffix('.partial').open('wb') as dest:shutil.copyfileobj(src,dest)
  path.with_suffix('.partial').rename(path)
 with zipfile.ZipFile(path) as archive:
  for entry in archive.infolist():assert(root/key/entry.filename).resolve().is_relative_to((root/key).resolve())
  archive.extractall(root/key)
 print(key,path.stat().st_size,flush=True)
 return key,{'source':url,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()}
with concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:result=dict(pool.map(fetch,items.items()))
(root/'sources.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
