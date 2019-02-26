# -*- coding: UTF-8 -*-
import os, sys, platform
import re

g_namePattern = re.compile('example-\d')


def slnPreProc(fLn, filters) :
	lnBegStr = 'Project("{'
	nameBegPtn = '}") = "'
	nameBegPtnLen = len(nameBegPtn)
	idBegPtn = '.vcxproj", "{'
	idBegPtnLen = len(idBegPtn)

	ret = []
	inSect = False

	for l in fLn :
		if inSect :
			if l == 'EndProject\n' :
				inSect = False
			continue

		if l.startswith(lnBegStr) :
			nameBeg = l.find(nameBegPtn) + nameBegPtnLen
			nameBegStr = l[nameBeg : ]
			if g_namePattern.match(nameBegStr) :
				idBeg = l.find(idBegPtn, nameBeg) + idBegPtnLen
				idStr = l[idBeg : l.find('}', idBeg)]
				filters.add(idStr)
			inSect = True
			continue

		ret.append(l)

	return ret


def slnProc(fLn) :
	filters = set()
	slnPreProc(fLn, filters)

	ret = []

	for l in fLn :
		if l.endswith('|Win32\n') or l.find('|x86')!=-1 :
			continue

		if l.startswith('\t\t{') :
			idStr = l[3 : l.find('}', 3)]
			if idStr in filters :
				continue

		ret.append(l)

	return ret


def itemName(ln) :
	nameBeg = ln.find('<') + 1
	b = ln.find(' ', nameBeg)
	r = ln.find('>', nameBeg)
	if b == -1 :
		nameEnd = r
	elif r == -1 :
		nameEnd = b
	else :
		nameEnd = min(b, r)
	return ln[nameBeg : nameEnd]


def itemEndRecord(ln) :
	endStr = '/' + itemName(ln)
	if ln.rfind('<%s>' % endStr) != -1 :
		endStr = ''
	return endStr


def projProc(fLn) :
	ret = []
	sectEndStr = ''

	debugItemEndStr = ''

	for l in fLn :
		if sectEndStr :
			if itemName(l) == sectEndStr :
				sectEndStr = ''
			continue

		if l.find('|Win32\'"') != -1 :
			sectEndStr = itemEndRecord(l)
			continue

		if debugItemEndStr :
			if l.find('<PreprocessorDefinitions>') != -1 :
				l = l.replace('_HAS_EXCEPTIONS=0;', '')
				l = l.replace('_HAS_ITERATOR_DEBUGGING=0;', '')
				l = l.replace('_SCL_SECURE=0;', '')
				l = l.replace('_SECURE_SCL=0;', '')
				l = l.replace('_ITERATOR_DEBUG_LEVEL=0', '')
			elif itemName(l) == debugItemEndStr :
				debugItemEndStr = ''

		elif l.find(' Condition="\'$(Configuration)|$(Platform)\'==\'Debug') != -1 :
			debugItemEndStr = itemEndRecord(l)

		if l.find('<RuntimeLibrary>MultiThreaded') != -1 :
			rtLibStr = 'DLL</RuntimeLibrary>'
			if l.find(rtLibStr) == -1 :
				l = l.replace('</RuntimeLibrary>', rtLibStr)

		ret.append(l)

	return ret


def mfProc(fLn) :
	ret = []
	nouseSect = False

	for l in fLn :
		if l == '\n' :
			if nouseSect :
				nouseSect = False
				continue

		if l.startswith('PROJECTS := ') :
			l = 'PROJECTS := bgfx bimg bimg_decode bimg_encode bx example-common example-glue fcpp geometryc glsl-optimizer glslang shaderc spirv-opt texturec texturev'

		elif g_namePattern.match(l) :
			nouseSect = True
			continue

		projBeg = l.find(' example-') + 1
		if projBeg > 0 and g_namePattern.match(l[projBeg:]) :
			continue

		ret.append(l)

	return ret


def fileProc(fPath, func) :
	inFile = open(fPath, 'r')
	fLn = inFile.readlines()
	inFile.close()

	outSeq = func(fLn)

	outFile = open(fPath, 'w')
	outFile.writelines(outSeq)
	outFile.close()


def fileEntry(fPath) :
	if fPath.endswith('.sln') :
		fileProc(fPath, slnProc)
	elif fPath.endswith('.vcxproj') :
		fileProc(fPath, projProc)
	elif fPath.endswith('Makefile') :
		fileProc(fPath, mfProc)


def entry(dirPath) :
	if not os.path.exists(dirPath) :
		print 'target not exists !'
		return
	if os.path.isfile(dirPath) :
		fileEntry(dirPath)
	else :	# walkDir
		ld = os.listdir(dirPath)
		for d in ld :
			fPath = '%s/%s' % (dirPath, d)
			fileEntry(fPath)


if __name__ == '__main__' :
	dirPath = sys.argv[1].replace('\\', '/')
	entry(dirPath)
	if platform.system() == 'Windows' :
		os.system("pause")
