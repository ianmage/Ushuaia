# -*- coding: UTF-8 -*-
import sys, os
import zipfile
sys.dont_write_bytecode = True
#import platform


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


def proc() :
	zfPath = '../res.zip'
	zf = zipfile.ZipFile(zfPath, 'w', zipfile.ZIP_DEFLATED)

	fileList = []
	gather_desc(fileList)
	for f in fileList :
		zf.write(f)

	zf.close()


if __name__ == '__main__' :
	curDir = sys.argv[0].replace('\\', '/')
	curDir = curDir[:curDir.rfind('/')+1]
	os.chdir(curDir + '../res')

	proc()