# -*- coding: UTF-8 -*-
import sys, os
sys.dont_write_bytecode = True
import platform


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
	fileList = []
	listFile(fileList)
	lmt = headerLastMod()
	curSys = platform.system()
	#force = raw_input("force all ? (y/n) : ")
	force = 0
	if curSys == 'Windows' :
		import fxc
		force = force in ('y', 'Y', '1')
		for f in fileList :
			inPath = curDir + f
			modTime = None
			if not force :
				modTime = os.stat(inPath).st_mtime
				modTime = max(lmt, modTime)
			fxc.proc(inPath, None, modTime)
	elif curSys == 'Darwin' :
		pass
	os.system('pause')