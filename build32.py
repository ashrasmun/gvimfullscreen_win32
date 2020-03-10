import subprocess
import os

def nmake(vcvars: str, nmake_cmd: str) -> None:
    cmd = 'cls&"' + vcvars + '"&"' + nmake_cmd + '"'
    print(cmd)
    subprocess.run(cmd, shell=True)
    return

def main() -> None:
    include = os.getenv('INCLUDE')

    prof2017_vcvars = r'c:\Program Files (x86)\Microsoft Visual Studio' \
            r'\2017\Professional\VC\Auxiliary\Build\vcvars32.bat'
    prof2017_nmake = r'c:\Program Files (x86)\Microsoft Visual Studio' \
            r'\2017\Professional\VC\Tools\MSVC\14.16.27023' \
            r'\bin\Hostx86\x86\nmake.exe'

    comm2019_vcvars = r'c:\Program Files (x86)\Microsoft Visual Studio' \
            r'\2019\Community\VC\Auxiliary\Build\vcvars32.bat'
    comm2019_nmake = r'c:\Program Files (x86)\Microsoft Visual Studio' \
            r'\2019\Community\VC\Tools\MSVC\14.21.27702' \
            r'\bin\Hostx86\x86\nmake.exe'

    if not include:
        if os.path.isfile(prof2017_vcvars):
            nmake(prof2017_vcvars, prof2017_nmake)
        elif os.path.isfile(comm2019_vcvars):
            nmake(comm2019_vcvars, comm2019_nmake)
        else:
            print('Cannot locate vcvars32.bat')
            exit(1)

    return

if __name__ == '__main__':
    main()
