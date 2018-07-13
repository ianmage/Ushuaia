# -*- coding: UTF-8 -*-
import sys, os
sys.dont_write_bytecode = True
import zipfile
import shutil


#isWin = False
#isOSX = False

#def init() :
#	global isWin, isOSX
#	curSys = platform.system()
#	isWin = curSys == 'Windows'
#	isOSX = curSys == 'Darwin'

#init()


resDir = (
	"material/",
	"mesh/",
	"model/",
	"scene/",
	"shaders/",
	"texture/"
)


def gather_desc(fileList) :
	for d in resDir :
		emptyDir = True
		for fn in os.listdir(d) :
			f = d + fn
			if os.path.isfile(f) and f.endswith('.json') :
				fileList.append(f)
				emptyDir = False
		if emptyDir and False :
			fileList.append(d)


def proc(flag) :
	zfPath = '../res.zip'
	zf = zipfile.ZipFile(zfPath, flag, zipfile.ZIP_DEFLATED)

	if flag == 'w' :
		fileList = []
		gather_desc(fileList)
		for f in fileList :
			zf.write(f)
	elif flag == 'r' :
		zf.extractall(".")

	zf.close()


if __name__ == '__main__' :
	curDir = sys.argv[0].replace('\\', '/')
	curDir = curDir[:curDir.rfind('/')+1]
	os.chdir(curDir + '../res')

	argErr = True
	if len(sys.argv) == 2 :
		flag = sys.argv[1].lower()
		if flag in ('r', 'w') :
			proc(flag)
			argErr = False
	if argErr :
		print "ArgErr, need provide r/w mode"
