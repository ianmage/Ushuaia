# -*- coding: UTF-8 -*-
import sys, os
sys.dont_write_bytecode = True
import platform


def listFile(nameList) :
	for f in os.listdir('.') :
		if os.path.isfile(f) and f.endswith('.sc') :
			nameList.append(f)


if __name__ == '__main__' :
	curDir = sys.argv[0].replace('\\', '/')
	curDir = curDir[:curDir.rfind('/')+1]
	fileList = []
	listFile(fileList)
	curSys = platform.system()
	if curSys == 'Windows' :
		import fxc
		for f in fileList :
			fxc.proc(curDir+f, None)
	elif curSys == 'Darwin' :
		pass
	os.system('pause')