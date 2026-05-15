# DOOM Style Fire
Terminal DOOM style fire using oneAPI libraries

## Compiling - Linux
Install oneAPI BaseKit using your packet manager of choice or from their website https://www.intel.com/content/www/us/en/developer/tools/oneapi/oneapi-toolkit.html
Source oneAPI's environment
`source /opt/intel/oneapi/setvars.sh`
Build
`./build-linux.sh <clean>`

### Caching
**NOTE: Optionally set the CPM_SOURCE_CACHE environment variable to cache cloned repositories outside of the project. If the build/ folder gets deleted, CPM only needs to recompile external libs rather than clone and recompile**

**Bash**

Run this command in the terminal or put into your .bashrc and source it again.

`export CPM_SOURCE_CACHE=~/.cache/cpm/`

