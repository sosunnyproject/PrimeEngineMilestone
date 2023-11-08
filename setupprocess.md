Set up PE Workspace

0. install cygwin
1. Folder PEWorkspace: disable read only
2. Tools/setenv.sh : edit with Notepad
```shell
export MAYA_DIR="C:\\Program Files\\Autodesk\\Maya2020"
export VSDIR="C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE"
```
3. in cygwin terminal, 
```bash
$ source setenv.sh
HI there!
-bash: $'\r': command not found
-bash: setenv.sh: line 60: syntax error: unexpected end of file
```
run this: `sed -i 's/\r$//' setenv.sh`
run: `$ source setenv.sh` again.

4. Tools/premake.sh
run: 
```bash
$ ./premake.sh --platformapi=win32d3d9 vs2022
```
If the error below show, run `sed -i 's/\r$//' premake.sh`

```bash 
: No such file or directoryenv.sh
: invalid shell option name: expand_aliases
Current dir: /cygdrive/d/csci522milestone/PEWorkspace/Tools
Changing to Code
./premake.sh: line 8: cd: $'../Code\r': No such file or directory
./premake.sh: line 9: premake: command not found
Current dir: /cygdrive/d/csci522milestone/PEWorkspace/Tools
Changing back
./premake.sh: line 12: cd: $'/cygdrive/d/csci522milestone/PEWorkspace/Tools\r\r': No such file or directory
Current dir: /cygdrive/d/csci522milestone/PEWorkspace/Tools
./premake.sh: line 14: $'\r': command not found
```

5. `run ./rundevenv.sh`
But If the error below show, run `sed -i 's/\r$//' premake.sh`

```bash
No such file or directoryetenv.sh
```
Run again `$ ./rundevenv.sh`

This opens visual studio window. 
Choose the solution inside PEWorkspace/Code and open it.
Run with F5 button, with Debug or Release mode.

6. go to cygwin. Run command. But if the error below show up,,
```bash 
$ ./pyclient.sh
: No such file or directorytenv.sh
: invalid shell option namet: expand_aliases
./pyclient.sh: line 5: $'\r': command not found
./pyclient.sh: line 6: pepython: command not found
./pyclient.sh: line 7: $'\r': command not found
```
```bash
sed -i 's/\r$//' pyclient.sh
```

Then run `$ ./pyclient.sh` again.