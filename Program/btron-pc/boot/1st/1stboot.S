! <1stboot.s> --- �ե������ȥ֡���.���ֺǽ�˥����ɤ����.
!
! $Date: 2011/12/27 17:13:35 $
! $Log: 1stboot.S,v $
! Revision 1.1  2011/12/27 17:13:35  liu1
! Initial Version.
!
! Revision 1.2  1999-12-23 17:54:17  kishida0
! loader $B$N0LCV$r(B 1st loader $B$K$&$a$`MM$K%(%s%H%j$rDI2C$7$?(B
!
! Revision 1.1  1999/03/15 02:10:52  monaka
! renamed some asm files included cpp macros. *.s was obsolute.
!
! Revision 1.7  1998/12/09 00:01:27  monaka
! Minor changed. (This is not bug fix)
!
! Revision 1.6  1998/11/20 08:02:16  monaka
! *** empty log message ***
!
! Revision 1.5  1998/05/23 15:29:32  night
! ƣ�ʤ���ޥ����Ѥ��ѹ���
! (�������������ѹ��Ǥϡ�ƣ�ʤ���ޥ���ǤϤ��ޤ�ư���ʤ��ä�)
!
! Revision 1.4  1996/07/06 13:10:58  night
! 2nd boot �˽�����ܹԤ������� 1 �ô֤� delay ���֤�����
!
# Revision 1.3  1996/05/11  14:36:41  night
# 1stboot ��λ���Υ�å���������
#
# Revision 1.2  1996/05/11  14:22:19  night
# 2nd boot �ξ������ä����������ѹ�(1 -> 2)
#
# Revision 1.1  1996/05/05  14:11:59  night
# 1st boot �ץ�����ࡣ
# BIOS �ˤ�� FD �����ɤ߹��ޤ졢2nd boot ���ɤ߹��ࡣ
#
!
! Discription
!	�ܥץ������ϡ�BTRON/386 �Υ����ͥ�����ɥץ������Ǥ��롣
!
!	���Υץ������ϡ��ƣĤ���֡��Ȥ��뤳�Ȥ�����Ȥ��Ƥ��롣
!	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
!
!	���Υץ�����ब�ɣУ̤�������ɤ��줿�����쥸������ξ��֤ϼ���
!	�褦�ˤʤäƤ��롣
!
!	AX,BX,CX,DX			����
!
!	�����ɥ��ɥ쥹	��������	0x07C0
!			���ե��å�	0x0000
!	�����ɥ�����			512 �Х���
!
! ��������
!
!	��ʬ���Ȥ򥳥ԡ� (0x07C0 -> 0x70000)
!	2nd BOOT ����������
!	2nd BOOT ���Τ������ (GDT, IDT, Page table ��ޤ�)
!	2nd BOOT ����Ƭ�˥�����
!

seg_2ndboot = 		0x0800
off_2ndboot = 		0x0000

seg_2ndboot_info =	0x7800
off_2ndboot_info =	0x0000
seg_size_info =		0x7800
off_size_info =		0x0000
seg_loadpoint =		0x0100		! loadpoint = 0x1000 
off_loadpoint =		0x0000
info_sector	=	2		! 512 bytes/sector
info_cylinder	=	0

.text
begtext:

.text

entry		first_boot

		.org	0x0000
first_boot:
		mov	ax,#0x07c0
		mov	ds,ax
		mov	ax,#0x7000
		mov	es,ax
		mov	cx,#512
		sub	si,si
		sub	di,di
		cld
		rep
		movsw
		jmpi	#restart, #0x7000

restart:
		mov	ax, #0x7000
		mov	es, ax
		mov	ss, ax
		mov	ds, ax
		mov	sp, #2048

		mov	bx, #message
		call	print
		call	read2ndboot			! 2nd boot ���ɤ߹���
		call	delay
!		call	kill_motor
		mov	ax, #seg_2ndboot		! 2nd boot �θƤӽФ�
		push	ax
		mov	ax, #off_2ndboot
		push	ax
		reti

delay:
		mov	ax,#0x1000
		mov	bx,#0xffff
delay1:
		dec	bx
		jnz	delay1
		mov	bx,#1000
		dec	ax
		jnz	delay1
		ret

!
! stop motor.
!
kill_motor:
		push	dx
		mov	dx, #0x3f2
		xor	al, al
		outb
		pop	dx
		ret	
read2ndboot:	
		! BIOS �����Ѥ��� FD �ѥ�᡼���ν����
		mov	cx,#0
		sub	si,si
		mov	fs,cx
		mov	bx,#0x78		! fs:dl ���ѥ�᡼���ơ��֥�Υ��ɥ쥹�Ȥʤ�
		push	ds
		seg fs
		lds	si,(bx)			! ds:si ���ơ��֥����������Υ��ԡ���

		mov	cl,#6			! copy 12 bytes
		cld
		push	di

		rep
		movsw

		pop	di
		pop	ds

		movb	4(di),*36		! patch sector count
	
		seg fs
		mov	(bx),di
		seg fs
		mov	2(bx),es
	
		! 2nd BOOT �ξ���������
		mov	bx, #boot2_message
		call	print
		mov	seg_addr, #seg_size_info
		mov	off_addr, #off_size_info
		movb	sector, *info_sector
		movb	cylinder, *info_cylinder
		call	readdisk

		! �ɤ߼�ä������Ȥä�FD����2nd BOOT���Τ��ɤ߼��
		movb	sector, *3
		movb	cylinder, *0
		mov	seg_addr, #seg_loadpoint
		mov	off_addr, #off_loadpoint
		mov	ax, #seg_size_info
		mov	es, ax
		seg es
		mov	ax, off_size_info

load_loop:
!		call	print_dot
		cmp	ax, #0
		jz	loop_done

		cmpb	sector, #19		! �������ֹ椬 19 �ξ��
		jne	l2

		call	print_dot
		mov	sector, #1		! �������ֹ��1���᤹
		incb	cylinder

l2:
		call	readdisk
		add	seg_addr, #0x0020	! 512 bytes/sector
		dec	ax
		incb	sector
		jmp	load_loop

loop_done:
!		mov	bx, #done_message
!		call	print
		ret
				


!
!	�������ؿ���
!
!
! print --- ʸ����򥳥󥽡����ɽ�����롣
!
! ������
!	bx	ɽ������ʸ����Υ��ɥ쥹
!		ʸ����ϡ�<Ĺ��>:1�Х���
!			  <ʸ����>:����255�Х���
!		�ȤʤäƤ��롣
print:
		push	ax
		push	bx
		push	cx
		push	dx

		push	bx
		mov	ah,#0x03		! get cursor pos
		xor	bh,bh
		int	0x10
		pop	bx

		xor	cx, cx
		movb	cl, (bx)
		inc	bx
		mov	bp, bx
		mov	bx, #0x0007
		mov	ax, #0x1301
		int	0x10
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		ret

print_dot:
		pusha
		mov	ax, #0xe2e		! '.'
		int	0x10
		popa
		ret
		
!
!
!

!
!	fatal --- ����Ū�ʼ���! ̵�¥롼�פ�����
!
fatal:
		mov	bx, #fatal_message
		call	print
fatal_loop:
		jmp	fatal_loop

!
!	disk io routines
!
! 	�ǥ�����������ꤷ�����������ɤ߹��ࡣ
!	BIOS �롼���� 0x13 ����Ѥ��롣
!
readdisk:
		push	ax
		push	bx
		push	cx
		push	dx

!		xor	ah, ah			! reset FDC
!		xor	dl, dl			! �ɥ饤���ֹ�
!		int	0x13
retry_reading:
		xor	dx, dx
		movb	al, cylinder
		shr	al, #1
		movb	ch, al			! �������ֹ�Υ��å�

		movb	al, cylinder
		and	al, #0x01
		movb	dh, al			! �إå��ֹ�Υ��å�

		movb	cl, sector		! �������ֹ�Υ��å�

		mov	ax, seg_addr		! �ɤ߹�����Υ���������
		mov	es, ax
		mov	bx, off_addr		! �ɤ߹�����Υ��ե��å���
		movb	al, #0x01
		movb	ah, #0x02
		int	0x13			! BIOS �θƤӽФ�
		jc	fatal

		pop	dx
		pop	cx
		pop	bx
		pop	ax
		ret
bad_rt:
		! reset FDC
		xor	ah, ah
		xor	dl, dl
		int	0x13

		! Print error message
		mov	bx, #retry_message
		call	print

		jmp	retry_reading


seg_addr:	.word	0	! �Хåե��Υ�������
off_addr:	.word	0	! �Хåե��Υ��ե��å�
cylinder:	.byte	0	! �������ֹ�(������ˤϡ��إå��ֹ��ޤޤ��)
sector:		.byte	0	! �������ֹ�
fatal_message:	
		.byte	35
		.ascii	"can't read 2ndboot program from FD."


!
! 	�ǡ����� (���ʸ���󡣡���)
!
message: 	.byte	28
		.ascii	"1st boot for btron/386(PC)"
		.byte   13, 10

message_dot:	.byte	1
		.ascii	"."

retry_message:	.byte	1
		.ascii	"e"

boot2_message:
		.byte   21
		.ascii	"loading 2nd boot..."
		.byte   13, 10

done_message:
		.byte   20
		.ascii	"loaded 2nd boot..."
		.byte	13, 10


seg_addr:	.word	0	! �Хåե��Υ�������
off_addr:	.word	0	! �Хåե��Υ��ե��å�
cylinder:	.byte	0	! �������ֹ�
sector:		.byte	0	! �������ֹ�(������ˤϡ��إå��ֹ��ޤޤ��)
! read_count:	.word	0	! �ɤ߹���֥��å���(512 ñ��)
fatal_message:
		.byte	34
		.ascii	"can't read 2ndboot program from FD."
		.byte	0

.org    498
loader_offset:  .word 0xffff

.org	510
boot_mark:
		.word 0xAA55

.text
endtext:

!.data
!enddata:

!.bss
!endbss:

		end	
	