# -*- coding: UTF-8 -*-
import sys, os
sys.dont_write_bytecode = True
#import shutil
#import platform


def printErr(msg) :
	print msg
	os.system('pause')
	exit()


def proc(inPath, preDefs) :
	curDir = 'D:/HELPER/3rdParty/bgfx'
	cmd = '%s/.build/win64_vs2017/bin/shadercDebug.exe' % curDir
	cmd += ' -i %s/src -i %s/examples/common' % (curDir, curDir)
	inPath = inPath.replace('\\', '/')
	if inPath.endswith('.sc') :
		lastSlash = inPath.rfind('/')
		inFile = inPath[lastSlash+1:]
		if lastSlash != -1 :
			inDir = inPath[:lastSlash]
			os.chdir(inDir)
		cmd += ' -f %s' % inFile
		outPath = '../res/shaders/dx11/%sbin' % inFile[:-2]
		cmd += ' -o %s' % outPath

		cmd += ' --type'
		if inFile.startswith('vs_') :
			cmd += ' vertex'
		elif inFile.startswith('fs_') :
			cmd += ' fragment'
		else :
			printErr('unknown shader type (v/f)')
		varyName = inFile[3:-3]
		if varyName.endswith('_instance') :
			varyName = varyName[:-9]
		cmd += ' --varyingdef %s.def' % varyName
		cmd += ' --platform'
		#curSys = platform.system()
		#if curSys == 'Windows' :
		cmd += ' windows -O 3'
		if inFile.startswith('vs_') :
			cmd += ' -p vs_5_0'
		elif inFile.startswith('fs_') :
			cmd += ' -p ps_5_0'

		if preDefs :
			cmd += ' --define %s' % preDefs
		#cmd += ' --verbose'
		print cmd
		os.system(cmd)
	else :
		printErr('unknown file ext type.')


if __name__ == '__main__' :
	numArg = len(sys.argv)
	if numArg < 2 :
		printErr('param1 : inPath, [param2 : preDef(A;B;C)]')
	else :
		preDefs = None
		if numArg > 2 :
			preDefs = sys.argv[2]
		proc(sys.argv[1], preDefs)
	os.system('pause')