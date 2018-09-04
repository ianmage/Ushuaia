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
	fLn = slnPreProc(fLn, filters)

	ret = []

	for l in fLn :
		if l.endswith('|Win32\n') :
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
			elif itemName(l) == debugItemEndStr :
				debugItemEndStr = ''

		elif l.find(' Condition="\'$(Configuration)|$(Platform)\'==\'Debug') != -1 :
			debugItemEndStr = itemEndRecord(l)

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


def entry(dirPath) :
	if not os.path.exists(dirPath) or os.path.isfile(dirPath) :
		print 'target not exists !'
		return
	# walkDir
	ld = os.listdir(dirPath)
	for d in ld :
		fPath = '%s/%s' % (dirPath, d)
		if d.endswith('.sln') :
			fileProc(fPath, slnProc)
		elif d.endswith('.vcxproj') :
			fileProc(fPath, projProc)


if __name__ == '__main__' :
	dirPath = sys.argv[1].replace('\\', '/')
	entry(dirPath)
	if platform.system() == 'Windows' :
		os.system("pause")
