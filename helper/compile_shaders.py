# -*- coding: UTF-8 -*-
import sys, os
sys.dont_write_bytecode = True
import shutil


def listFile(dir, nameList) :
	assert(os.path.isdir(dir))
	for f in os.listdir(dir) :
		p = os.path.join(dir,f)
		if os.path.isfile(p) and f.endswith('.sc') :
			nameList.append(p[2:])
		elif os.path.isdir(p) :
			listFile(p, nameList)


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
	listFile('.', fileList)
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
		slashPos = f.rfind('/')
		scId = f[slashPos+4:-3]
		if scId in vary :
			v = f[:slashPos+1] + vary[scId]
			fxc.proc(f, v, macros, modTime)
	#fxc.press_any_key_exit()