struct vdIn {
	int fd;
	char *videodevice ;
	struct video_mmap vmmap;
	struct video_capability videocap;
	int mmapsize;
	struct video_mbuf videombuf;
	struct video_picture videopict;
	struct video_window videowin;
	struct video_channel videochan;
	struct video_param videoparam;	
	int cameratype ;
	char *cameraname;
	char bridge[9];
	int sizenative; // available size in jpeg
	int sizeothers;	// others palette 
	int palette; // available palette
	int norme ; // set spca506 usb video grabber
	int channel ; // set spca506 usb video grabber
	int grabMethod ;
	unsigned char *pFramebuffer;
	unsigned char *ptframe[4];
	int framelock[4];
	pthread_mutex_t grabmutex;
	int framesizeIn ;
	volatile int frame_cour;
	int bppIn;
	int  hdrwidth;
	int  hdrheight;
	int  formatIn;
	int signalquit;	
};

char *videodevice = NULL;
char *partdevice = NULL;
int usepartport = 0;
int err;
int grabmethod = 1;
int format = VIDEO_PALETTE_YUV420P;
int width = 352;
int height = 288;
char *separateur;
char *sizestring = NULL;
char *mode = NULL;
int i;
int serv_sock,new_sock;
pthread_t w1;
pthread_t server_th;
int sin_size;
unsigned short serverport = 7070;	

enum {
	JPEG = 0,
	YUVY,
	YYUV,
	YUYV,
	GREY,
	GBRG,
	SN9C,
	GBGR,
	UNOW,
};

/* V4L1 extension API */
#define VIDEO_PALETTE_JPEG  21
/* in case default setting */
#define WIDTH 352
#define HEIGHT 288
#define BPPIN 8
#define OUTFRMNUMB 4
/* ITU-R-BT.601 PAL/NTSC */
#define MASQ 1
#define VGA MASQ
#define PAL (MASQ << 1)
#define SIF (MASQ << 2)
#define CIF (MASQ << 3)
#define QPAL (MASQ << 4)
#define QSIF (MASQ << 5)
#define QCIF (MASQ << 6)

/* specific for the spca5xx webcam */
enum {
	BRIDGE_SPCA505 = 0,
        BRIDGE_SPCA506,
	BRIDGE_SPCA501,
	BRIDGE_SPCA508,
	BRIDGE_SPCA504,
	BRIDGE_SPCA500,
	BRIDGE_SPCA504B,
	BRIDGE_SPCA533,
	BRIDGE_SPCA504C,
	BRIDGE_SPCA561,
	BRIDGE_SPCA536,
	BRIDGE_SONIX,
	BRIDGE_ZR364XX,
	BRIDGE_ZC3XX,
	BRIDGE_CX11646,
	BRIDGE_TV8532,
	BRIDGE_ETOMS,
	BRIDGE_SN9CXXX,
	BRIDGE_MR97311,
	BRIDGE_UNKNOW,
	MAX_BRIDGE,
};

static struct bridge_list Blist[]={
	{BRIDGE_SPCA505,"SPCA505"},
	{BRIDGE_SPCA506,"SPCA506"},
	{BRIDGE_SPCA501,"SPCA501"},
	{BRIDGE_SPCA508,"SPCA508"},
	{BRIDGE_SPCA504,"SPCA504"},
	{BRIDGE_SPCA500,"SPCA500"},
	{BRIDGE_SPCA504B,"SPCA504B"},
	{BRIDGE_SPCA533,"SPCA533"},
	{BRIDGE_SPCA504C,"SPCA504C"},
	{BRIDGE_SPCA561,"SPCA561"},
	{BRIDGE_SPCA536,"SPCA536"},
	{BRIDGE_SONIX,"SN9C102"},
	{BRIDGE_ZR364XX,"ZR364XX"},
	{BRIDGE_ZC3XX,"ZC301-2"},
	{BRIDGE_CX11646,"CX11646"},
	{BRIDGE_TV8532,"TV8532"},
	{BRIDGE_ETOMS,"ET61XX51"},
	{BRIDGE_SN9CXXX,"SN9CXXX"},
	{BRIDGE_MR97311,"MR97311"},
	{BRIDGE_UNKNOW,"UNKNOW"},
	{-1,NULL}
};

#define MAX_RTV_USER 4
int rtv_sock[MAX_RTV_USER][2];
