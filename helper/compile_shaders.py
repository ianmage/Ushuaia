# -*- coding: UTF-8 -*-
import sys, os
sys.dont_write_bytecode = True
sys.path.append("../shaders")
import fxc


def listFile(nameList) :
	for f in os.listdir('.') :
		if os.path.isfile(f) and f.endswith('.sc') :
			nameList.append(f)


def headerLastMod() :
	ret = 0
	for f in os.listdir('.') :
		if os.path.isfile(f) and (f.endswith('.sh') or f.endswith('.def')) :
			ret = max(os.stat(f).st_mtime, ret)
	return ret


if __name__ == '__main__' :
	curDir = sys.argv[0].replace('\\', '/')
	curDir = curDir[:curDir.rfind('/')+1]
	os.chdir(curDir + '../shaders')
	fileList = []
	listFile(fileList)
	lastModTime = headerLastMod()
	force = raw_input("force all ? (y/n) : ")

	force = force in ('y', 'Y', '1')
	for f in fileList :
		inPath = curDir + f
		modTime = None
		if not force :
			modTime = os.stat(inPath).st_mtime
			modTime = max(lastModTime, modTime)
		fxc.proc(inPath, None, modTime)
	fxc.press_any_key_exit()