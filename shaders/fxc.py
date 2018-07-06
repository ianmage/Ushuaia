# -*- coding: UTF-8 -*-
import sys, os
sys.dont_write_bytecode = True
import platform


isWin = False
isOSX = False

def init() :
	global isWin, isOSX
	curSys = platform.system()
	isWin = curSys == 'Windows'
	isOSX = curSys == 'Darwin'

init()

if isOSX :
	import termios


def press_any_key_exit(msg="Press Any Key Exit...\n") :
	if isOSX :
		# 获取标准输入的描述符
		fd = sys.stdin.fileno()

		# 获取标准输入(终端)的设置
		old_ttyinfo = termios.tcgetattr(fd)

		# 配置终端
		new_ttyinfo = old_ttyinfo[:]

		# 使用非规范模式(索引3是c_lflag 也就是本地模式)
		new_ttyinfo[3] &= ~termios.ICANON
		# 关闭回显(输入不会被显示)
		new_ttyinfo[3] &= ~termios.ECHO

		# 输出信息
		sys.stdout.write(msg)
		sys.stdout.flush()
		# 使设置生效
		termios.tcsetattr(fd, termios.TCSANOW, new_ttyinfo)
		# 从终端读取
		os.read(fd, 7)

		# 还原终端设置
		termios.tcsetattr(fd, termios.TCSANOW, old_ttyinfo)
	elif isWin :
		os.system('pause')


def printErr(msg) :
	print msg
	press_any_key_exit()
	exit()


def proc(inPath, preDefs, srcModTime=None) :
	if isWin :
		curDir = 'D:/HELPER/3rdParty/bgfx'
		cmd = '%s/.build/win64_vs2017/bin/shadercDebug.exe' % curDir
	elif isOSX :
		curDir = '~/Programs/3rdParty/bgfx'
		cmd = '~/Programs/bgfx_tools/shaderc'
	cmd += ' -i %s/src -i %s/examples/common' % (curDir, curDir)
	inPath = inPath.replace('\\', '/')
	if inPath.endswith('.sc') :
		lastSlash = inPath.rfind('/')
		inFile = inPath[lastSlash+1:]
		if lastSlash != -1 :
			inDir = inPath[:lastSlash]
			os.chdir(inDir)
		cmd += ' -f %s' % inFile
		if isWin :
			outPath = '../res/shaders/dx11/'
		elif isOSX :
			outPath = '../res/shaders/metal/'
		outPath += '%sbin' % inFile[:-2]
		cmd += ' -o %s' % outPath

		if srcModTime is not None :
			if os.path.isfile(outPath) :
				dstModTime = os.stat(outPath).st_mtime
				if srcModTime < dstModTime :
					return

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

		if isWin :
			cmd += ' windows -O 3'
			if inFile.startswith('vs_') :
				cmd += ' -p vs_5_0'
			elif inFile.startswith('fs_') :
				cmd += ' -p ps_5_0'
		elif isOSX :
			cmd += ' osx -p metal'

		if preDefs :
			cmd += ' --define %s' % preDefs
		#cmd += ' --verbose'
		#print cmd
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
		proc(sys.argv[1], preDefs, None)
	press_any_key_exit()