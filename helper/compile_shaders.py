# -*- coding: UTF-8 -*-
import sys, os
sys.dont_write_bytecode = True
import shutil


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

	sys.path.append(".")
	import fxc

	fileList = []
	listFile(fileList)
	lastModTime = headerLastMod()
	force = raw_input("force all ? (y/n) : ")

	vary = fxc.parseVaryDef('varying.predef')

	macros = None
	if len(sys.argv) > 1 :
		macros = sys.argv[1]

	force = force in ('y', 'Y', '1')
	for f in fileList :
		modTime = None
		if not force :
			modTime = os.stat(f).st_mtime
			modTime = max(lastModTime, modTime)
		scId = f[3:-3]
		if scId in vary :
			fxc.proc(f, vary[scId], macros, modTime)
	#fxc.press_any_key_exit()