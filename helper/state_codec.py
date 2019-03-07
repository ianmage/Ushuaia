# -*- coding: UTF-8 -*-
import os, sys, platform


WRITE_R = 0x0000000000000001
WRITE_G = 0x0000000000000002
WRITE_B = 0x0000000000000004
WRITE_A = 0x0000000000000008
WRITE_Z = 0x0000004000000000

WRITE_RGB = WRITE_R | WRITE_G | WRITE_B

DEPTH_TEST_LESS = 0x0000000000000010
DEPTH_TEST_LEQUAL = 0x0000000000000020
DEPTH_TEST_EQUAL = 0x0000000000000030
DEPTH_TEST_GEQUAL = 0x0000000000000040
DEPTH_TEST_GREATER = 0x0000000000000050
DEPTH_TEST_NOTEQUAL = 0x0000000000000060
DEPTH_TEST_NEVER = 0x0000000000000070
DEPTH_TEST_ALWAYS = 0x0000000000000080
DEPTH_TEST_SHIFT = 4
DEPTH_TEST_MASK = 0x00000000000000f0

BLEND_ZERO = 0x0000000000001000
BLEND_ONE = 0x0000000000002000
BLEND_SRC_COLOR = 0x0000000000003000
BLEND_INV_SRC_COLOR = 0x0000000000004000
BLEND_SRC_ALPHA = 0x0000000000005000
BLEND_INV_SRC_ALPHA = 0x0000000000006000
BLEND_DST_ALPHA = 0x0000000000007000
BLEND_INV_DST_ALPHA = 0x0000000000008000
BLEND_DST_COLOR = 0x0000000000009000
BLEND_INV_DST_COLOR = 0x000000000000a000
BLEND_SRC_ALPHA_SAT = 0x000000000000b000
BLEND_FACTOR = 0x000000000000c000
BLEND_INV_FACTOR = 0x000000000000d000
BLEND_SHIFT = 12
BLEND_MASK = 0x000000000ffff000

BLEND_EQUATION_ADD = 0x0000000000000000
BLEND_EQUATION_SUB = 0x0000000010000000
BLEND_EQUATION_REVSUB = 0x0000000020000000
BLEND_EQUATION_MIN = 0x0000000030000000
BLEND_EQUATION_MAX = 0x0000000040000000
BLEND_EQUATION_SHIFT = 28
BLEND_EQUATION_MASK = 0x00000003f0000000

BLEND_INDEPENDENT = 0x0000000400000000
BLEND_ALPHA_TO_COVERAGE = 0x0000000800000000

CULL_CW = 0x0000001000000000
CULL_CCW = 0x0000002000000000
CULL_SHIFT = 36
CULL_MASK = 0x0000003000000000

ALPHA_REF_SHIFT = 40
ALPHA_REF_MASK = 0x0000ff0000000000

PT_TRISTRIP = 0x0001000000000000
PT_LINES = 0x0002000000000000
PT_LINESTRIP = 0x0003000000000000
PT_POINTS = 0x0004000000000000
PT_SHIFT = 48
PT_MASK = 0x0007000000000000

POINT_SIZE_SHIFT = 52
POINT_SIZE_MASK = 0x00f0000000000000

MSAA = 0x0100000000000000
LINEAA = 0x0200000000000000
CONSERVATIVE_RASTER = 0x0400000000000000


STATES = [
	("R", WRITE_R),
	("G", WRITE_G),
	("B", WRITE_B),
	("A", WRITE_A),
	("Z", WRITE_Z),
	# 5
	("LESS", DEPTH_TEST_LESS),
	("LEQUAL", DEPTH_TEST_LEQUAL),
	("EQUAL", DEPTH_TEST_EQUAL),
	("GEQUAL", DEPTH_TEST_GEQUAL),
	("GREATER", DEPTH_TEST_GREATER),
	("NOTEQUAL", DEPTH_TEST_NOTEQUAL),
	("NEVER", DEPTH_TEST_NEVER),
	("ALWAYS", DEPTH_TEST_ALWAYS),
	# 13
	("ZERO", BLEND_ZERO),
	("ONE", BLEND_ONE),
	("SRC_COLOR", BLEND_SRC_COLOR),
	("INV_SRC_COLOR", BLEND_INV_SRC_COLOR),
	("SRC_ALPHA", BLEND_SRC_ALPHA),
	("INV_SRC_ALPHA", BLEND_INV_SRC_ALPHA),
	("DST_ALPHA", BLEND_DST_ALPHA),
	("INV_DST_ALPHA", BLEND_INV_DST_ALPHA),
	("DST_COLOR", BLEND_DST_COLOR),
	("INV_DST_COLOR", BLEND_INV_DST_COLOR),
	("SRC_ALPHA_SAT", BLEND_SRC_ALPHA_SAT),
	("FACTOR", BLEND_FACTOR),
	("INV_FACTOR", BLEND_INV_FACTOR),
	# 26
	("ADD", BLEND_EQUATION_ADD),
	("SUB", BLEND_EQUATION_SUB),
	("REVSUB", BLEND_EQUATION_REVSUB),
	("MIN", BLEND_EQUATION_MIN),
	("MAX", BLEND_EQUATION_MAX),
	# 31
	("INDEPENDENT", BLEND_INDEPENDENT),
	("ALPHA_TO_COVERAGE", BLEND_ALPHA_TO_COVERAGE),
	# 33
	("CW", CULL_CW),
	("CCW", CULL_CCW),
	# 35
	("TRISTRIP", PT_TRISTRIP),
	("LINES", PT_LINES),
	("LINESTRIP", PT_LINESTRIP),
	("POINTS", PT_POINTS),
	# 39
	("MSAA", MSAA),
	("LINEAA", LINEAA),
	("CONSERVATIVE_RASTER", CONSERVATIVE_RASTER)
]


def NumToAry79Str(num) :
	aryBegin = 48
	ary = 79
	s = ''
	while num >= ary :
		asc = chr((num % ary) + aryBegin)
		s = '%s%s' %(asc, s)
		num /= ary
	asc = chr(num + aryBegin)
	s = '%s%s' %(asc, s)
	return s


def Ary79StrToNum(s) :
	aryBegin = 48
	ary = 79
	n = 0
	powBase = 1
	i = len(s) - 1
	while i > 0 :
		asc = ord(s[i]) - aryBegin
		n += asc * powBase
		powBase *= ary
		i -= 1
	asc = ord(s[0]) - aryBegin
	n += asc * powBase
	return n


def STATE_BLEND_FUNC_SEPARATE(srcRGB, dstRGB, srcA, dstA) :
	return (srcRGB | (dstRGB << 4)) | ((srcA | (dstA << 4)) << 8)

def STATE_BLEND_FUNC(src, dst) :
	return STATE_BLEND_FUNC_SEPARATE(src, dst, src, dst)
	
def BGFX_STATE_BLEND_EQUATION_SEPARATE(eqtRGB, eqtA) :
	return eqtRGB | (eqtA << 3)
	
def BGFX_STATE_BLEND_EQUATION(eqt) :
	return BGFX_STATE_BLEND_EQUATION_SEPARATE(eqt, eqt)
	
	
def ParseValue(v) :
	ret = []
	
	curSt = 'WRITE :'
	for i in xrange(0, 5) :
		if v & STATES[i][1] :
			curSt += ' ' + STATES[i][0]
	ret.append(curSt)

	curSt = 'DEPTH_TEST : '
	for i in xrange(5, 13) :
		if v & STATES[i][1] :
			curSt += STATES[i][0]
			break
	ret.append(curSt)

	curSt = 'BLEND_SRC_COLOR : '
	for i in xrange(13, 26) :
		if v & STATES[i][1] :
			curSt += STATES[i][0]
			break
	ret.append(curSt)
	curSt = 'BLEND_DST_COLOR : '
	vv = v >> 4
	for i in xrange(13, 26) :
		if vv & STATES[i][1] :
			curSt += STATES[i][0]
			break
	ret.append(curSt)
	curSt = 'BLEND_SRC_ALPHA : '
	vv = v >> 8
	for i in xrange(13, 26) :
		if vv & STATES[i][1] :
			curSt += STATES[i][0]
			break
	ret.append(curSt)
	curSt = 'BLEND_DST_ALPHA : '
	vv = v >> 12
	for i in xrange(13, 26) :
		if vv & STATES[i][1] :
			curSt += STATES[i][0]
			break
	ret.append(curSt)

	curSt = 'BLEND_EQUATION_COLOR : '
	useDefault = True
	for i in xrange(26, 31) :
		if v & STATES[i][1] :
			curSt += STATES[i][0]
			useDefault = False
			break
	if useDefault :
		curSt += 'ADD'
	ret.append(curSt)
	curSt = 'BLEND_EQUATION_ALPHA : '
	useDefault = True
	vv = v >> 3
	for i in xrange(26, 31) :
		if vv & STATES[i][1] :
			curSt += STATES[i][0]
			useDefault = False
			break
	if useDefault :
		curSt += 'ADD'
	ret.append(curSt)
	
	curSt = 'BLEND :'
	for i in xrange(31, 33) :
		if v & STATES[i][1] :
			curSt += ' ' + STATES[i][0]
	ret.append(curSt)
	
	curSt = 'CULL : '
	for i in xrange(33, 35) :
		if v & STATES[i][1] :
			curSt += STATES[i][0]
			break
	ret.append(curSt)
			
	curSt = 'ALPHA_REF : '
	curSt += str((v & ALPHA_REF_MASK) >> ALPHA_REF_SHIFT)
	ret.append(curSt)

	curSt = 'PRIMITIVE : '
	for i in xrange(35, 39) :
		if v & STATES[i][1] :
			curSt += STATES[i][0]
			break
	ret.append(curSt)

	curSt = 'POINT_SIZE : '
	curSt += str((v & POINT_SIZE_MASK) >> POINT_SIZE_SHIFT)
	ret.append(curSt)

	curSt = 'RASTERIZATION : '
	for i in xrange(39, 42) :
		if v & STATES[i][1] :
			curSt += STATES[i][0]
			break
	ret.append(curSt)
	
	return ret


if __name__ == '__main__' :
	c = raw_input("StCode : ")
	v = Ary79StrToNum(c)
	stGroup = ParseValue(v)
	for t in stGroup :
		print t
	if platform.system() == 'Windows' :
		os.system("pause")
