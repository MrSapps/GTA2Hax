.586
.model flat, stdcall

.stack 4096

.DATA

EXTERNDEF C flt_77D100:DWORD
EXTERNDEF C flt_77D10C:DWORD
EXTERNDEF C flt_77D114:DWORD
EXTERNDEF C gfAmbient_E10838:DWORD
EXTERNDEF C numLights_2B93E38:DWORD
EXTERNDEF C lights_2B959E0:DWORD
EXTERNDEF C dword_13868:DWORD
EXTERNDEF C gPtr_dword_E13864:DWORD


.CODE

; ---------------------------------------------------------------------------

SLight		struc ;	(sizeof=0x14, mappedto_74)
field_0		dd ?
field_4		dd ?
field_8		dd ?			; offset
field_C		dd ?
field_10	dd ?
SLight		ends

; ---------------------------------------------------------------------------

SLightF8	struc ;	(sizeof=0x2C, mappedto_75) ; XREF: .data:lights_2B959E0/r
field_0		dd ?			; XREF:	gbh_AddLight+1A/w
field_4		dd ?			; XREF:	gbh_AddLight+36/w
field_8		dd ?			; XREF:	gbh_AddLight+EA/w
					; gbh_AddLight+102/r
field_C		dd ?			; XREF:	gbh_AddLight+F4/w
field_10	dd ?			; XREF:	gbh_AddLight+108/w
field_14	dd ?			; XREF:	gbh_AddLight+3F/w
field_18	dd ?			; XREF:	gbh_AddLight+48/w
					; LightVerts_2B52A80+61/o
field_1C	dd ?			; XREF:	gbh_AddLight+51/w
field_20	dd ?			; XREF:	gbh_AddLight+7B/w
field_24	dd ?			; XREF:	gbh_AddLight+9F/w
field_28	dd ?			; XREF:	gbh_AddLight+BE/w
SLightF8	ends


; ---------------------------------------------------------------------------

SVertex		struc ;	(sizeof=0x20, mappedto_67) ; XREF: LightVerts_2B52A80+30/o
					; STile/r
x		dd ?
y		dd ?
z		dd ?
w		dd ?
diff		dd ?			; XREF:	LightVerts_2B52A80+30/o
spec		dd ?
u		dd ?
v		dd ?
SVertex		ends

; =============== S U B	R O U T	I N E =======================================

; Attributes: bp-based frame

; int __stdcall	LightVerts_2B52A80(int vertCount, SVertex *pVerts, int alwaysZero, unsigned __int8 colourRelated)
LightVerts_2B52A80 proc	C		; CODE XREF: DrawQuad_E02CC0+4DFp
					; gbh_DrawTriangle+33Dp

var_34		= qword	ptr -34h
var_2C		= qword	ptr -2Ch
var_24		= qword	ptr -24h
var_1C		= dword	ptr -1Ch
var_18		= dword	ptr -18h
var_14		= dword	ptr -14h
vertCounter	= dword	ptr -10h
var_C		= dword	ptr -0Ch
var_8		= dword	ptr -8
var_4		= dword	ptr -4
vertCount	= dword	ptr  8
pVerts		= dword	ptr  0Ch
colourRelated	= dword	ptr  14h

		push	ebp
		mov	ebp, esp
		sub	esp, 34h
		mov	eax, [ebp+14h]
		xor	ecx, ecx
		and	eax, 0FFh
		mov	[ebp+colourRelated], eax
		mov	eax, [ebp+vertCount]
		fild	[ebp+colourRelated]
		cmp	eax, ecx
		fmul	ds:flt_77D114
		fstp	[ebp+var_4]
		jle	loc_772CA8
		mov	edx, [ebp+pVerts]
		push	edi
		push	esi
		push	ebx
		lea	edi, [edx+SVertex.diff]
		mov	[ebp+vertCounter], eax

loc_772AB6:				; CODE XREF: LightVerts_2B52A80+21Fj
		fld	ds:flt_77D10C
		fld	ds:flt_77D10C
		mov	eax, numLights_2B93E38
        mov eax, [eax]
		fst	[ebp+pVerts]
		fld	ds:flt_77D10C
		cmp	eax, ecx
		fst	[ebp+var_8]
		jle	loc_772CAE
		mov	ecx, dword_13868
        mov ecx, [ecx]
		mov	edx, lights_2B959E0
        add edx, 18h
		mov	[ebp+vertCount], eax

loc_772AE9:				; CODE XREF: LightVerts_2B52A80+128j
		mov	eax, [edx-18h]
		and	eax, 30000h
		cmp	eax, 10000h
		jnz	loc_772B9E
		fld	dword ptr [edi+70h]
		fsub	dword ptr [edx-4]
		fld	dword ptr [edi+74h]
		fsub	dword ptr [edx]
		fld	dword ptr [edi+78h]
		fsub	dword ptr [edx+4]
		fld	st(2)
		fmulp	st(3), st
		fld	st(1)
		fmul	st, st(2)
		faddp	st(3), st
		fld	st
		fmul	st, st(1)
		faddp	st(3), st
		fstp	st
		fstp	st
		fcom	dword ptr [edx-12]
		fnstsw	ax
		test	ah, 65
		jz	short loc_772B9C
		fstp	dword ptr [ebp+14h] ; pop float	into [ebp+14h]
		mov	esi, [ebp+14h]
		mov	eax, esi
		and	esi, 7FFFFFh
		and	eax, 7F800000h
		sar	eax, 1

        .IF eax & 400000h
            add	eax, 20000000h
        .ELSE
		    add	eax, 1F800000h
		    or	esi, 800000h
        .ENDIF

		mov	ebx, gPtr_dword_E13864
        mov ebx, [ebx]
		and	eax, 7F800000h
		shr	esi, cl
		fld	dword ptr [edx-10h]
		mov	esi, [ebx+esi*4]
		or	esi, eax
		mov	[ebp+14h], esi
		fsub	dword ptr [ebp+14h]
		fmul	dword ptr [edx-8]
		fcom	ds:flt_77D10C
		fnstsw	ax
		test	ah, 41h
		jnz	short loc_772B9C
		fmul	dword ptr [edx-14h]
		fld	dword ptr [edx+8]
		fmul	st, st(1)
		faddp	st(4), st
		fld	dword ptr [edx+0Ch]
		fmul	st, st(1)
		faddp	st(3), st
		fld	dword ptr [edx+10h]
		fmul	st, st(1)
		faddp	st(2), st

loc_772B9C:				; CODE XREF: LightVerts_2B52A80+A9j
					; LightVerts_2B52A80+102j
		fstp	st

loc_772B9E:				; CODE XREF: LightVerts_2B52A80+76j
		mov	eax, [ebp+vertCount]
		add	edx, 2Ch
		dec	eax
		mov	[ebp+vertCount], eax
		jnz	loc_772AE9
		fstp	[ebp+var_8]
		xor	ecx, ecx
		fstp	[ebp+pVerts]

loc_772BB6:				; CODE XREF: LightVerts_2B52A80+232j
		mov	eax, [edi]
		mov	dword ptr [ebp+var_24+4], ecx
		mov	edx, eax
		mov	dword ptr [ebp+var_2C+4], ecx
		shr	edx, 8
		and	edx, 0FFh
		mov	dword ptr [ebp+var_34+4], ecx
		mov	dword ptr [ebp+var_24],	edx
		mov	edx, eax
		fild	[ebp+var_24]
		and	edx, 0FFh
		mov	dword ptr [ebp+var_2C],	edx
		fstp	[ebp+colourRelated]
		fild	[ebp+var_2C]
		shr	eax, 10h
		and	eax, 0FFh
		fstp	[ebp+vertCount]
		mov	dword ptr [ebp+var_34],	eax
		fild	[ebp+var_34]
		fmul	[ebp+var_4]
		fmulp	st(1), st
		fadd	gfAmbient_E10838
		fst	[ebp+var_C]
		fcomp	ds:flt_77D100
		fnstsw	ax
		test	ah, 41h
		jnz	short loc_772C16
		mov	[ebp+var_C], 437F0000h

loc_772C16:				; CODE XREF: LightVerts_2B52A80+18Dj
		fld	[ebp+var_4]
		fmul	[ebp+colourRelated]
		fmul	[ebp+pVerts]
		fadd	gfAmbient_E10838
		fst	[ebp+colourRelated]
		fcomp	ds:flt_77D100
		fnstsw	ax
		test	ah, 41h
		jnz	short loc_772C3C
		mov	[ebp+colourRelated], 437F0000h

loc_772C3C:				; CODE XREF: LightVerts_2B52A80+1B3j
		fld	[ebp+var_4]
		fmul	[ebp+vertCount]
		fmul	[ebp+var_8]
		fadd	gfAmbient_E10838
		fst	[ebp+vertCount]
		fcomp	ds:flt_77D100
		fnstsw	ax
		test	ah, 41h
		jnz	short loc_772C62
		mov	[ebp+vertCount], 437F0000h

loc_772C62:				; CODE XREF: LightVerts_2B52A80+1D9j
		fld	[ebp+var_C]
		fistp	[ebp+var_14]
		fld	[ebp+colourRelated]
		fistp	[ebp+var_18]
		fld	[ebp+vertCount]
		fistp	[ebp+var_1C]
		mov	eax, [ebp+var_14]
		mov	edx, [ebp+var_18]
		mov	esi, [ebp+var_1C]
		add	edi, 20h
		shl	eax, 8
		or	eax, edx
		mov	edx, [edi-20h]
		shl	eax, 8
		and	edx, 0FF000000h
		or	eax, edx
		or	eax, esi
		mov	[edi-20h], eax
		mov	eax, [ebp+vertCounter]
		dec	eax
		mov	[ebp+vertCounter], eax
		jnz	loc_772AB6
		pop	ebx
		pop	esi
		pop	edi

loc_772CA8:				; CODE XREF: LightVerts_2B52A80+24j
		mov	esp, ebp
		pop	ebp
		retn	10h
; ---------------------------------------------------------------------------

loc_772CAE:				; CODE XREF: LightVerts_2B52A80+55j
		fstp	st
		fstp	st
		jmp	loc_772BB6
LightVerts_2B52A80 endp

; ---------------------------------------------------------------------------

PUBLIC LightVerts_2B52A80

END
