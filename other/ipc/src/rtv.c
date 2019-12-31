#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "rtv.h"

extern s_syn_ctrl syn_ctrl_rtv;
	
static int
GetDepth (int format)
{
	int depth;
	switch (format)
	{
	case VIDEO_PALETTE_JPEG:
	  {
		depth = 8;		
	  }
	  break;
	case VIDEO_PALETTE_RAW:
	  {
		depth = 8;		
	  }
	  break;
	case VIDEO_PALETTE_YUV420P:
	  {
		depth = (8 * 3) >> 1;
	  }
	  break;
	case VIDEO_PALETTE_RGB565:
	  	depth = 16;
	  break;
	case VIDEO_PALETTE_RGB24:
	  	depth = 24;
	  break;
	case VIDEO_PALETTE_RGB32:
	  {
		depth = 32;
	  }
	  break;
	default:
	  	depth = -1;
	  	break;
	}
	return depth;
}

static int
isSpcaChip (const char *BridgeName)
{
	int i = -1;
	int find = -1;
	int size = 0;

	/* Spca506 return more with channel video, cut it */

	/* return Bridge otherwhise -1 */
	for (i = 0; i < MAX_BRIDGE -1; i++)
	{
		size = strlen (Blist[i].name) ;
		logger(TLOG_NOTICE, "is_spca %s \n", Blist[i].name);
	  	if (strncmp (BridgeName, Blist[i].name, size) == 0)
		{
			find = i;
	  		break;
		}
	}

	return find;
}

static int
GetStreamId (const char *BridgeName)
{
	int i = -1;
	int match = -1;
	/* return Stream_id otherwhise -1 */
	if ((match = isSpcaChip (BridgeName)) < 0)
	{
	  logger(TLOG_ERROR, "Not an Spca5xx Camera !!\n");
	  return match;
	}
	switch (match)
	{
	case BRIDGE_SPCA505:
	case BRIDGE_SPCA506:
	  i = YYUV;
	  break;
	case BRIDGE_SPCA501:
	  i = YUYV;
	  break;
	case BRIDGE_SPCA508:
	  i = YUVY;
	  break;
	case BRIDGE_SPCA536:
	case BRIDGE_SPCA504:
	case BRIDGE_SPCA500:
	case BRIDGE_SPCA504B:
	case BRIDGE_SPCA533:
	case BRIDGE_SPCA504C:
	case BRIDGE_ZR364XX:
	case BRIDGE_ZC3XX:
	case BRIDGE_CX11646:
	case BRIDGE_SN9CXXX:
	case BRIDGE_MR97311:   
	  i = JPEG;
	  break;
	case BRIDGE_ETOMS:
	case BRIDGE_SONIX:
	case BRIDGE_SPCA561:
	case BRIDGE_TV8532:
	  i = GBRG;
	  break;
	default:
	  i = UNOW; // -1;
	  logger(TLOG_ERROR, "Unable to find a StreamId !!\n");
	  break;

	}
	return i;
}

static int
GetVideoPict (struct vdIn *vd)
{
	if (ioctl (vd->fd, VIDIOCGPICT, &vd->videopict) < 0)
	{
		logger(TLOG_NOTICE, "Couldnt get videopict params with VIDIOCGPICT");
		return -1;
	}

	logger(TLOG_NOTICE, "VIDIOCGPICT brightnes=%d hue=%d color=%d contrast=%d whiteness=%d"
	  "depth=%d palette=%d\n", vd->videopict.brightness,
	  vd->videopict.hue, vd->videopict.colour, vd->videopict.contrast,
	  vd->videopict.whiteness, vd->videopict.depth,
	  vd->videopict.palette);

	return 0;
}

static void spcaPrintParam (int fd, struct video_param *videoparam)
{
	if(ioctl(fd,SPCAGVIDIOPARAM, videoparam) == -1){
		logger(TLOG_WARNING, "wrong spca5xx device\n");
	} else 
		logger(TLOG_NOTICE, "quality %d autoexpo %d Timeframe %d \n",
			 videoparam->quality,videoparam->autobright,videoparam->time_interval);
}

static
int probePalette ( struct vdIn *vd )
{	
	/* probe palette and set a default one for unknow cams*/
	int pal[] ={VIDEO_PALETTE_JPEG,VIDEO_PALETTE_YUV420P,VIDEO_PALETTE_RGB24,VIDEO_PALETTE_RGB565,VIDEO_PALETTE_RGB32};
	struct video_picture pict;
	int masq = 0x1;
	int i;
	int availpal = 0;
	int defaut = 1;
	/* initialize the internal struct */
	if (ioctl (vd->fd, VIDIOCGPICT, &pict) < 0)
	{
		logger(TLOG_ERROR, "Couldnt get videopict params with VIDIOCGPICT\n");
		return -1;
	}
	/* try each palette we have we skip raw_jpeg */
	for(i = 0; i < 5 ; i++){
		pict.palette = pal[i];
		/* maybe correct the bug on qca driver depth always 24 ? */	
		pict.depth = GetDepth (pal[i]);
		logger(TLOG_NOTICE, "try palette %d depth %d\n",pict.palette,pict.depth);
		if (ioctl (vd->fd, VIDIOCSPICT, &pict) < 0)
		{
			logger(TLOG_WARNING, "Couldnt set palette first try %d \n", pal[i]);
		}
		if (ioctl (vd->fd, VIDIOCGPICT, &pict) < 0)
		{
			logger(TLOG_WARNING, "Couldnt get palette %d \n", pal[i]);
		}
		if (pict.palette != pal[i]){
			logger(TLOG_WARNING, "Damned second try fail \n");	
		}
		else {
			availpal = availpal | masq ;
			logger(TLOG_NOTICE, "Available  palette %d \n", pal[i]);
			if (defaut){
				defaut = 0;
			 	//vd->formatIn = pal[i];
				// vd->bppIn = GetDepth (pal[i]);
			}
		}
		masq = masq << 1;
	}
	vd->palette = availpal;
	//should set default palette here ?
	return 1;	
}

/* return masq byte for the needed size */
static int convertsize( int width, int height)
{
	switch (width){
		case 640:
			if(height == 480)
			return VGA;
		break;
		case 384:
			if(height == 288)
			return PAL;
		break;
		case 352:
			if(height == 288)
			return SIF;
		break;
		case 320:
			if(height == 240)
			return CIF;
		break;
		case 192:
			if(height == 144)
			return QPAL;
		break;
		case 176:
			if(height == 144)
			return QSIF;
		break;
		case 160:
			if(height == 120)
			return QCIF;
		break;
		default:
		break;
	}
	return -1;
}

static int sizeconvert( int *width, int *height, int size)
{
	switch (size){
		case VGA:
			*height = 480;
			*width = 640;		
		break;
		case PAL:
			*height = 288;
			*width = 384;		
		break;
		case SIF:
			*height = 288;
			*width = 352;		
		break;
		case CIF:
			*height = 240;
			*width = 320;		
		break;
		case QPAL:
			*height = 144;
			*width = 192;		
		break;
		case QSIF:
			*height = 144;
			*width = 176;		
		break;
		case QCIF:
			*height = 120;
			*width = 160;		
		break;
		default:
			return -1;
		break;
	}

	return 0;
}

/* test is palette and size are available otherwhise return the next available palette and size 
palette is set by preference order jpeg yuv420p rbg24 rgb565 and rgb32 */
static int check_palettesize(struct vdIn *vd ){
	int needsize = 0;
	int needpalette = 0;
	unsigned char masq = 0x00;
	/* initialize needed size */
	if ((needsize = convertsize(vd->hdrwidth,vd->hdrheight)) < 0){
		logger(TLOG_ERROR, "size seem unavailable fatal errors !!\n");
		return -1;
	}
	/* is there a match with available palette */
	/* check */
	if (!(vd->sizeothers & needsize)){
	if (needsize > 1){
		masq = needsize -1;
	}
	if (( masq & vd->sizeothers) > 1){
		/* check lower masq upper size */
		while (!((needsize = needsize >> 1) & vd->sizeothers) && needsize);
	} 
	else if ((masq & vd->sizeothers) == 0){
		masq = 0xff -(needsize << 1) +1;
		if ((masq & vd->sizeothers) == 0){
			/* no more size available */
			needsize = 0;
		} 
		else {
		/* check upper masq */
			while (!((needsize = needsize << 1) & vd->sizeothers) && needsize);
		}	
	} // maybe == 1
	 
	}
	if(needsize){
		/* set the size now check for a palette */
		if(sizeconvert( &vd->hdrwidth,&vd->hdrheight,needsize) > 0){
			logger(TLOG_ERROR, "size not set fatal errors !!\n");
			return -1;
		}
		if((needpalette = checkpalette(vd) < 0)){
			return -1;
		}
	 } else {
		 logger(TLOG_ERROR, "Damned no match found Fatal errors !!\n");
		 return -1;
	 }
	return needsize;	
}

static int
SetVideoPict (struct vdIn *vd)
{
	if (ioctl (vd->fd, VIDIOCSPICT, &vd->videopict) < 0)
	{
		logger(TLOG_ERROR, "Couldnt set videopict params with VIDIOCSPICT");
		return -1;
	}	
	logger(TLOG_NOTICE, "VIDIOCSPICT brightnes=%d hue=%d color=%d contrast=%d whiteness=%d"
	  "depth=%d palette=%d\n", vd->videopict.brightness,
	  vd->videopict.hue, vd->videopict.colour, vd->videopict.contrast,
	  vd->videopict.whiteness, vd->videopict.depth,
	  vd->videopict.palette);

  return 0;
}

static int
init_v4l (struct vdIn *vd)
{
	int f;
	int erreur = 0;
	int err;
	if ((vd->fd = open (vd->videodevice, O_RDWR)) == -1)
	{
		logger(TLOG_ERROR, "ERROR opening V4L interface");
		return -1;
	}

	if (ioctl (vd->fd, VIDIOCGCAP, &(vd->videocap)) == -1)
	{
		logger(TLOG_ERROR, "Couldn't get videodevice capability");
		return -1;
	}

	logger(TLOG_NOTICE, "Camera found: %s \n", vd->videocap.name);
	snprintf (vd->cameraname, 32, "%s", vd->videocap.name);

	erreur = GetVideoPict (vd);
	if (ioctl (vd->fd, VIDIOCGCHAN, &vd->videochan) == -1)
	{
		logger(TLOG_NOTICE, "Hmm did not support Video_channel\n");
	  	vd->cameratype = UNOW;
	}
	else
	{
		if (vd->videochan.name){
		  logger(TLOG_NOTICE, "Bridge found: %s \n", vd->videochan.name);
		  snprintf (vd->bridge, 9, "%s", vd->videochan.name);
		  vd->cameratype = GetStreamId (vd->videochan.name);
		  spcaPrintParam (vd->fd,&vd->videoparam);
		}
		else
		{
			logger(TLOG_ERROR, "Bridge not found not a spca5xx Webcam Probing the hardware !!\n");
		  	vd->cameratype = UNOW;
		}
	}
	logger(TLOG_NOTICE, "StreamId: %d  Camera\n", vd->cameratype);
	/* probe all available palette and size */
	if (probePalette(vd ) < 0) {
		logger(TLOG_ERROR, "could't probe video palette Abort !");
		return -1;
	}
	if (probeSize(vd ) < 0) {
	  	logger(TLOG_ERROR, "could't probe video size Abort !");
	  	return -1;
	}

	/* now check if the needed setting match the available
	if not find a new set and populate the change */
	err = check_palettesize(vd);
	logger(TLOG_NOTICE, "Format asked %d check %d\n",vd->formatIn, err);	
	vd->videopict.palette = vd->formatIn;
	vd->videopict.depth = GetDepth (vd->formatIn);
	vd->bppIn = GetDepth (vd->formatIn);

	vd->framesizeIn = (vd->hdrwidth * vd->hdrheight * vd->bppIn) >> 3;

	erreur = SetVideoPict (vd);
	erreur = GetVideoPict (vd);
	if (vd->formatIn != vd->videopict.palette ||
	  vd->bppIn != vd->videopict.depth){
	  	logger(TLOG_ERROR, "could't set video palette Abort !");
		return -1;
	}
	if (erreur < 0){
		logger(TLOG_ERROR, "could't set video palette Abort !");
		return -1;
	}
	if (vd->grabMethod)
	{
	  	logger(TLOG_NOTICE, "grabbing method default MMAP asked \n");
	  	// MMAP VIDEO acquisition
	  	memset (&(vd->videombuf), 0, sizeof (vd->videombuf));
	  	if (ioctl (vd->fd, VIDIOCGMBUF, &(vd->videombuf)) < 0)
		{
	  		logger(TLOG_ERROR, "init VIDIOCGMBUF FAILED\n");
		}
	  	logger(TLOG_NOTICE, "VIDIOCGMBUF size %d  frames %d  offets[0]=%d offsets[1]=%d\n",
	      	vd->videombuf.size, vd->videombuf.frames,
	      	vd->videombuf.offsets[0], vd->videombuf.offsets[1]);
	  		vd->pFramebuffer = (unsigned char *) mmap (0, vd->videombuf.size, PROT_READ | PROT_WRITE,
				MAP_SHARED, vd->fd, 0);
		vd->mmapsize = vd->videombuf.size;
		vd->vmmap.height = vd->hdrheight;
		vd->vmmap.width = vd->hdrwidth;
		vd->vmmap.format = vd->formatIn;
		for (f = 0; f < vd->videombuf.frames; f++)
		{
			vd->vmmap.frame = f;
			if (ioctl (vd->fd, VIDIOCMCAPTURE, &(vd->vmmap)))
			{	
				logger(TLOG_NOTICE, "cmcapture");
			}
		}
		vd->vmmap.frame = 0;
	}
	else
	{
		/* read method */
		/* allocate the read buffer */
		vd->pFramebuffer = (unsigned char *) realloc (vd->pFramebuffer, (size_t) vd->framesizeIn);
		logger(TLOG_NOTICE, "grabbing method READ asked \n");
		if (ioctl (vd->fd, VIDIOCGWIN, &(vd->videowin)) < 0)
			logger(TLOG_ERROR, "VIDIOCGWIN failed \n");
		vd->videowin.height = vd->hdrheight;
		vd->videowin.width = vd->hdrwidth;
		if (ioctl (vd->fd, VIDIOCSWIN, &(vd->videowin)) < 0)
			logger(TLOG_ERROR, "VIDIOCSWIN failed \n");
		logger(TLOG_NOTICE, "VIDIOCSWIN height %d  width %d \n", vd->videowin.height, vd->videowin.width);
	}
	vd->frame_cour = 0;
	return erreur;
}

int
init_videoIn (struct vdIn *vd, char *device, int width, int height,
      int format, int grabmethod)
{
	int err = -1;
	int i;
	if (vd == NULL || device == NULL)
		return -1;
	if (width == 0 || height == 0)
		return -1;
	if(grabmethod < 0 || grabmethod > 1)
		grabmethod = 1; //read by default;
	// check format 
	vd->videodevice = NULL;
	vd->cameraname = NULL;
	vd->videodevice = NULL;
	vd->videodevice = (char *) realloc (vd->videodevice, 16);
	vd->cameraname = (char *) realloc (vd->cameraname, 32);
	snprintf (vd->videodevice, 12, "%s", device);
	logger(TLOG_NOTICE, "video %s \n",vd->videodevice);
	memset (vd->cameraname, 0, sizeof (vd->cameraname));
	memset(vd->bridge, 0, sizeof(vd->bridge));
	vd->signalquit = 1;
	vd->hdrwidth = width;
	vd->hdrheight = height;
	/* compute the max frame size */
	vd->formatIn = format; 
	vd->bppIn = GetDepth (vd->formatIn);
	vd->grabMethod = grabmethod;		//mmap or read 
	vd->pFramebuffer = NULL;
	/* init and check all setting */
	err = init_v4l (vd);
	/* allocate the 4 frames output buffer */
	for (i = 0; i < OUTFRMNUMB; i++)
	{
		vd->ptframe[i] = NULL;
	  	vd->ptframe[i] = (unsigned char *)realloc(vd->ptframe[i], 
			sizeof(struct frame_t) + (size_t) vd->framesizeIn );
	  	vd->framelock[i] = 0;
	}
	vd->frame_cour = 0;
	pthread_mutex_init (&vd->grabmutex, NULL);
	return err;
}  

/* paraport 2 channels with D/A converter read signal pin 15 or pin 13 */
int openclaimParaport(char *dev)
{
	int fd,i;

	fd = open(dev, O_RDWR );
    if (fd < 0) {
    	logger(TLOG_ERROR, "cannot open device %s\n",dev);
		return -1;
    }
	if(ioctl(fd, PPCLAIM) < 0){
		logger(TLOG_ERROR, "could not claim parport. Did you load the modules parport_pc, ppdev and parport? Check with /sbin/lsmod\n");
		close(fd);
		return -1;
	}
	return(fd);
}

int port_setdata2(int fd,unsigned char val2)
{
	int rc;
	struct ppdev_frob_struct frob;
	
	frob.mask = PARPORT_CONTROL_STROBE ;
	/* set the strobe line*/		
	frob.val = PARPORT_CONTROL_STROBE ;// 1;toggle
	ioctl(fd,PPFCONTROL,&frob);
	rc=ioctl(fd, PPWDATA, &val2);
	frob.val = 0;
	ioctl(fd,PPFCONTROL,&frob);
	return(rc);
}

int port_setdata1(int fd,unsigned char bitval)
{
	int rc;
	struct ppdev_frob_struct frob;

	frob.mask = PARPORT_CONTROL_SELECT;
	frob.val= 0;//~PARPORT_CONTROL_SELECT;//0;	
	ioctl(fd,PPFCONTROL,&frob);
	rc = port_setdata2(fd,bitval);
	frob.val= PARPORT_CONTROL_SELECT; //8;		
	ioctl(fd,PPFCONTROL,&frob);	
	return(rc);
}

int rtv_init(void){
	if (videodevice == NULL || *videodevice == 0)
	{
		videodevice = "/dev/video0";
	}
	if(usepartport && partdevice == NULL)
		partdevice = "/dev/parport0";

	memset (&videoIn, 0, sizeof (struct vdIn));
	if (init_videoIn(&videoIn, videodevice, width, height, format,grabmethod) != 0)
	{
		logger(TLOG_ERROR, "damned encore rate !!\n");
		return -1;
	}
	if(usepartport){ 
		fd = openclaimParaport(partdevice);
		if (fd > 0) {
			port_setdata1(fd,128);
			port_setdata2(fd,128);
		} 
		else {
			usepartport = 0;
			fd = 0;
		}
	}
/*
	if((err= pthread_create (&w1, NULL, (void *) grab, NULL)) != 0){
		printf("thread grabbing error %d \n",err);
		close_v4l (&videoIn);
		exit(1);
	}
*/
}


double
ms_time (void)
{
	static struct timeval tod;
	gettimeofday (&tod, NULL);
	return ((double) tod.tv_sec * 1000.0 + (double) tod.tv_usec / 1000.0);
}

int
get_jpegsize (unsigned char *buf, int insize)
{
	int i; 	
	for ( i= 1024 ; i< insize; i++) {
		if ((buf[i] == 0xFF) && (buf[i+1] == 0xD9)) return i+10;
	}
	return -1;
}

int 
convertframe(unsigned char *dst,unsigned char *src, int width,int height, int formatIn, int qualite)
{ 
	int i;
	unsigned char tmp;
	int jpegsize =0;
	switch (formatIn){
		case VIDEO_PALETTE_JPEG:
			jpegsize = get_jpegsize(src, width*height);
			if(jpegsize > 0)
			memcpy(dst,src,jpegsize);	
		break;
	/*	
		case VIDEO_PALETTE_YUV420P:	
			jpegsize = encode_image(src,dst,qualite,YUVto420,width,height);
		break;
		case VIDEO_PALETTE_RGB24:
		 	jpegsize = encode_image(src,dst,qualite,RGBto420,width,height);
		break;
		case VIDEO_PALETTE_RGB565:	
		 	jpegsize = encode_image(src,dst,qualite,RGB565to420,width,height);	
		break;
		case VIDEO_PALETTE_RGB32:	
		 	jpegsize = encode_image(src,dst,qualite,RGB32to420,width,height);	
		break;
	*/	
		default:
		break;
	}
	return jpegsize;
}

int videoGrab(void){
	static	int frame = 0;
	struct vdIn *vd = videoIn;
	int len;
	int status;
	int count = 0;
	int size;
	int erreur = 0;
	int jpegsize = 0;
	int qualite = 1024;
	struct frame_t *headerframe;
	double timecourant =0;
	double temps = 0;

	timecourant = ms_time();
	if (vd->grabMethod)
	{
		vd->vmmap.height = vd->hdrheight;
		vd->vmmap.width = vd->hdrwidth;
		vd->vmmap.format = vd->formatIn;
		if (ioctl (vd->fd, VIDIOCSYNC,&vd->vmmap.frame) < 0)
		{
			logger(TLOG_ERROR, "cvsync err\n");
			erreur = -1;
		}

		/* Is there someone using the frame */  
		while((vd->framelock[vd->frame_cour] != 0) && vd->signalquit)
			usleep(1000);
		pthread_mutex_lock (&vd->grabmutex);
		temps = ms_time();
		jpegsize= convertframe(vd->ptframe[vd->frame_cour]+ sizeof(struct frame_t),
		vd->pFramebuffer + vd->videombuf.offsets[vd->vmmap.frame],
		vd->hdrwidth,vd->hdrheight,vd->formatIn,qualite);

		headerframe=(struct frame_t*)vd->ptframe[vd->frame_cour];
		snprintf(headerframe->header,5,"%s","SPCA"); 
		headerframe->seqtimes = ms_time();
		headerframe->deltatimes=(int)(headerframe->seqtimes-timecourant); 
		headerframe->w = vd->hdrwidth;
		headerframe->h = vd->hdrheight;
		headerframe->size = (( jpegsize < 0)?0:jpegsize);
		headerframe->format = vd->formatIn;
		headerframe->nbframe = frame++; 

		// printf("compress frame %d times %f\n",frame, headerframe->seqtimes-temps);
		pthread_mutex_unlock (&vd->grabmutex); 
		/************************************/

		if ((ioctl (vd->fd, VIDIOCMCAPTURE, &(vd->vmmap))) < 0)
		{
			logger(TLOG_ERROR, "cmcapture");
			logger(TLOG_ERROR, ">>cmcapture err %d\n", status);
			erreur = -1;
		}
		vd->vmmap.frame = (vd->vmmap.frame + 1) % vd->videombuf.frames;
		vd->frame_cour = (vd->frame_cour +1) % OUTFRMNUMB;
		//printf("frame nb %d\n",vd->vmmap.frame);
	}
	else
	{
		/* read method */
		size = vd->framesizeIn;
		len = read (vd->fd, vd->pFramebuffer, size);
		if (len <= 0 )
		{
			logger(TLOG_ERROR, "v4l read error\n");
			logger(TLOG_ERROR, "len %d asked %d \n", len, size);
			return 0;
		}
		/* Is there someone using the frame */
		while((vd->framelock[vd->frame_cour] != 0)&& vd->signalquit)
			usleep(1000);
		pthread_mutex_lock (&vd->grabmutex);
		temps = ms_time();
		jpegsize= convertframe(vd->ptframe[vd->frame_cour]+ sizeof(struct frame_t),
		vd->pFramebuffer ,
		vd->hdrwidth,vd->hdrheight,vd->formatIn,qualite); 
		headerframe=(struct frame_t*)vd->ptframe[vd->frame_cour];
		snprintf(headerframe->header,5,"%s","SPCA"); 
		headerframe->seqtimes = ms_time();
		headerframe->deltatimes=(int)(headerframe->seqtimes-timecourant); 
		headerframe->w = vd->hdrwidth;
		headerframe->h = vd->hdrheight;
		headerframe->size = (( jpegsize < 0)?0:jpegsize);; 
		headerframe->format = vd->formatIn; 
		headerframe->nbframe = frame++; 
		//  printf("compress frame %d times %f\n",frame, headerframe->seqtimes-temps);
		vd->frame_cour = (vd->frame_cour +1) % OUTFRMNUMB;  
		pthread_mutex_unlock (&vd->grabmutex); 
		/************************************/
	}
	return erreur;
}


int rtv_send(char *buf, int buf_len)
{
	s_cmd_req_head cmd_req_head;
	cmd_package_head_construct(&cmd_req_head, CMD_ID_RTV_SEND_REQ, buf_len);

	for(int i = 0; i < MAX_RTV_USER; i++){
		if(rtv_sock[i][1] == 1){
			send(int sockid, (char *) cmd_req_head, sizeof(s_cmd_req_head), 0);
			send(socket_id, buf, buf_len, 0);
		}
	}	

	return 0;
}

// 实时视频主线程:获取视频数据，然后直接发送出去
int thread_rtv_main()
{
    while(!mapThreadID2Handler[THREAD_RTV].thread_quit_flag)
    {
        // 如果没有rtv请求，则阻塞等待
        thread_syn_wait(&syn_ctrl_rtv.mutex, &syn_ctrl_rtv.cond, &syn_ctrl_rtv.flag);

        // 获取视频数据
		if(videoGrab() != 0){
			logger(TLOG_ERROR, "Grab failed.\n");
		}
        // 发送
        if(rtv_send((unsigned char *)headerframe, sizeof(struct frame_t) != 0){
			logger(TLOG_ERROR, "Send failed.\n");
        }			
    }

}

// 设置视频参数:帧率，亮度等
int vm_para_set()
{

}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
