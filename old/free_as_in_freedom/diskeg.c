
	if (this_request->cmd == WIN_WRITE) {
		hd_out(this_request->hd,this_request->nsector,this_request->
			sector,this_request->head,this_request->cyl,
			this_request->cmd,&write_intr);
		for(i=0 ; i<3000 && !(r=inb_p(HD_STATUS)&DRQ_STAT) ; i++)
			/* nothing */ ;
		if (!r) {
			reset_hd(this_request->hd);
			return;
		}
		port_write(HD_DATA,this_request->bh->b_data+
			512*(this_request->nsector&1),256);
	} else if (this_request->cmd == WIN_READ) {
		hd_out(this_request->hd,this_request->nsector,this_request->
			sector,this_request->head,this_request->cyl,
			this_request->cmd,&read_intr);

void hd_init(void)
{
	int i;

	for (i=0 ; i<NR_REQUEST ; i++) {
		request[i].hd = -1;
		request[i].next = NULL;
	}
	for (i=0 ; i<NR_HD ; i++) {
		hd[i*5].start_sect = 0;
		hd[i*5].nr_sects = hd_info[i].head*
				hd_info[i].sect*hd_info[i].cyl;
	}
	set_trap_gate(0x2E,&hd_interrupt);
	outb_p(inb_p(0x21)&0xfb,0x21);
	outb(inb_p(0xA1)&0xbf,0xA1);
}

static void read_intr(void)
{
	if (win_result()) {
		bad_rw_intr();
		return;
	}
	port_read(HD_DATA,this_request->bh->b_data+
		512*(this_request->nsector&1),256);
	this_request->errors = 0;
	if (--this_request->nsector)
		return;
	this_request->bh->b_uptodate = 1;
	this_request->bh->b_dirt = 0;
	wake_up(&wait_for_request);
	unlock_buffer(this_request->bh);
	this_request->hd = -1;
	this_request=this_request->next;
	do_request();
}

static void write_intr(void)
{
	if (win_result()) {
		bad_rw_intr();
		return;
	}
	if (--this_request->nsector) {
		port_write(HD_DATA,this_request->bh->b_data+512,256);
		return;
	}
	this_request->bh->b_uptodate = 1;
	this_request->bh->b_dirt = 0;
	wake_up(&wait_for_request);
	unlock_buffer(this_request->bh);
	this_request->hd = -1;
	this_request=this_request->next;
	do_request();
}


static void hd_out(unsigned int drive,unsigned int nsect,unsigned int sect,
		unsigned int head,unsigned int cyl,unsigned int cmd,
		void (*intr_addr)(void))
{
	register int port asm("dx");

	if (drive>1 || head>15)
		panic("Trying to write bad sector");
	if (!controller_ready())
		panic("HD controller not ready");
	do_hd = intr_addr;
	outb(_CTL,HD_CMD);
	port=HD_DATA;
	outb_p(_WPCOM,++port);
	outb_p(nsect,++port);
	outb_p(sect,++port);
	outb_p(cyl,++port);
	outb_p(cyl>>8,++port);
	outb_p(0xA0|(drive<<4)|head,++port);
	outb(cmd,++port);
}

static void reset_hd(int nr)
{
	reset_controller();
	hd_out(nr,_SECT,_SECT,_HEAD-1,_CYL,WIN_SPECIFY,&do_request);
}


static void reset_controller(void)
{
	int	i;

	outb(4,HD_CMD);
	for(i = 0; i < 1000; i++) nop();
	outb(0,HD_CMD);
	for(i = 0; i < 10000 && drive_busy(); i++) /* nothing */;
	if (drive_busy())
		printk("HD-controller still busy\n\r");
	if((i = inb(ERR_STAT)) != 1)
		printk("HD-controller reset failed: %02x\n\r",i);
}

static int drive_busy(void)
{
	unsigned int i;

	for (i = 0; i < 100000; i++)
		if (READY_STAT == (inb(HD_STATUS) & (BUSY_STAT | READY_STAT)))
			break;
	i = inb(HD_STATUS);
	i &= BUSY_STAT | READY_STAT | SEEK_STAT;
	if (i == READY_STAT | SEEK_STAT)
		return(0);
	printk("HD controller times out\n\r");
	return(1);
}



static int controller_ready(void)
{
	int retries=1000;

	while (--retries && (inb(HD_STATUS)&0xc0)!=0x40);
	return (retries);
}

#define port_read(port,buf,nr) \
__asm__("cld;rep;insw"::"d" (port),"D" (buf),"c" (nr):"cx","di")

#define port_write(port,buf,nr) \
__asm__("cld;rep;outsw"::"d" (port),"S" (buf),"c" (nr):"cx","si")



#define outb(value,port) \
__asm__ ("outb %%al,%%dx"::"a" (value),"d" (port))


#define inb(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); \
_v; \
})

#define outb_p(value,port) \
__asm__ ("outb %%al,%%dx\n" \
		"\tjmp 1f\n" \
		"1:\tjmp 1f\n" \
		"1:"::"a" (value),"d" (port))

#define inb_p(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al\n" \
	"\tjmp 1f\n" \
	"1:\tjmp 1f\n" \
	"1:":"=a" (_v):"d" (port)); \
_v; \
})



_hd_interrupt:
	pushl %eax
	pushl %ecx
	pushl %edx
	push %ds
	push %es
	push %fs
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	movl $0x17,%eax
	mov %ax,%fs
	movb $0x20,%al
	outb %al,$0x20		# EOI to interrupt controller #1
	jmp 1f			# give port chance to breathe
1:	jmp 1f
1:	outb %al,$0xA0		# same to controller #2
	movl _do_hd,%eax
	testl %eax,%eax
	jne 1f
	movl $_unexpected_hd_interrupt,%eax
1:	call *%eax		# "interesting" way of handling intr.
	pop %fs
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %eax
	iret


/* currently supports only 1 hd, put type here */
#define HARD_DISK_TYPE 17

/*
 * Ok, hard-disk-type is currently hardcoded. Not beatiful,
 * but easier. We don't use BIOS for anything else, why should
 * we get HD-type from it? Get these values from Reference Guide.
 */

#if HARD_DISK_TYPE == 17
#define _CYL	977
#define _HEAD	5
#define __WPCOM	300
#define _LZONE	977
#define _SECT	17
#define _CTL	0
#elif HARD_DISK_TYPE == 18
#define _CYL	977
#define _HEAD	7
#define __WPCOM	(-1)
#define _LZONE	977
#define _SECT	17
#define _CTL	0
#else
#error Define HARD_DISK_TYPE and parameters, add your own entries as well
#endif

/* Controller wants just wp-com/4 */
#if __WPCOM >= 0
#define _WPCOM ((__WPCOM)>>2)
#else
#define _WPCOM __WPCOM
#endif

/* Hd controller regs. Ref: IBM AT Bios-listing */
#define HD_DATA		0x1f0	/* _CTL when writing */
#define HD_ERROR	0x1f1	/* see err-bits */
#define HD_NSECTOR	0x1f2	/* nr of sectors to read/write */
#define HD_SECTOR	0x1f3	/* starting sector */
#define HD_LCYL		0x1f4	/* starting cylinder */
#define HD_HCYL		0x1f5	/* high byte of starting cyl */
#define HD_CURRENT	0x1f6	/* 101dhhhh , d=drive, hhhh=head */
#define HD_STATUS	0x1f7	/* see status-bits */
#define HD_PRECOMP HD_ERROR	/* same io address, read=error, write=precomp */
#define HD_COMMAND HD_STATUS	/* same io address, read=status, write=cmd */

#define HD_CMD		0x3f6

/* Bits of HD_STATUS */
#define ERR_STAT	0x01
#define INDEX_STAT	0x02
#define ECC_STAT	0x04	/* Corrected error */
#define DRQ_STAT	0x08
#define SEEK_STAT	0x10
#define WRERR_STAT	0x20
#define READY_STAT	0x40
#define BUSY_STAT	0x80

/* Values for HD_COMMAND */
#define WIN_RESTORE		0x10
#define WIN_READ		0x20
#define WIN_WRITE		0x30
#define WIN_VERIFY		0x40
#define WIN_FORMAT		0x50
#define WIN_INIT		0x60
#define WIN_SEEK 		0x70
#define WIN_DIAGNOSE		0x90
#define WIN_SPECIFY		0x91

/* Bits for HD_ERROR */
#define MARK_ERR	0x01	/* Bad address mark ? */
#define TRK0_ERR	0x02	/* couldn't find track 0 */
#define ABRT_ERR	0x04	/* ? */
#define ID_ERR		0x10	/* ? */
#define ECC_ERR		0x40	/* ? */
#define	BBD_ERR		0x80	/* ? */


