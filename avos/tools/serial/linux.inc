;+--------------------------------------+
;| Linux.inc                            |
;+--------------------------------------+

	sin_family      equ 0
	sin_port        equ 2
	sin_addr        equ 4
	sin_zero        equ 8
	sin_size        equ 16

    ICANON          equ 2
    
    B0              equ 00000000Q
    CS6             equ 00000020Q
    CS7             equ 00000040Q
    CS8             equ 00000060Q
    CSTOPB          equ 00000100Q
    CREAD           equ 00000200Q
    PARENB          equ 00000400Q
    PARODD          equ 00001000Q
    HUPCL           equ 00002000Q
    CLOCAL          equ 00004000Q
    VTIME           equ 5
    VMIN            equ 6
    
    IGNPAR          equ 0000004Q
    CRTSCTS         equ 020000000000Q
    TCIFLUSH        equ 0
    TCSANOW         equ 0
    
    TIOCMBIS        equ 0x5416
    TIOCMBIC        equ 0x5417
    TIOCM_DTR       equ 0x002
    TIOCM_RTS       equ 0x004

    B2400           equ 0000013Q
    B4800           equ 0000014Q
    B9600           equ 0000015Q
    B19200          equ 0000016Q
    B38400          equ 0000017Q
    B57600          equ 0010001Q
    B115200         equ 0010002Q
    B230400         equ 0010003Q
    B460800         equ 0010004Q
    B500000         equ 0010005Q
    B576000         equ 0010006Q
    B921600         equ 0010007Q
    B1000000        equ 0010010Q
    
    
    extern perror
	extern opendir
	extern closedir
	extern readdir
	extern chdir
    extern tcgetattr
    extern tcflush
    extern tcsetattr
    extern tcdrain
    extern ioctl
    
%macro  sc1     2
	push dword %2
	call %1
	add esp, 4
%endmacro

%macro  sc2     3
	push dword %3
	push dword %2
	call %1
	add esp, 8
%endmacro

%macro  sc3     4
	push dword %4
	push dword %3
	push dword %2
	call %1
	add esp, 12
%endmacro

%macro  sc4     5
	push dword %5
	push dword %4
	push dword %3
	push dword %2
	call %1
	add esp, 16
%endmacro

%macro  sc5     6
	push dword %6
	push dword %5
	push dword %4
	push dword %3
	push dword %2
	call %1
	add esp, 20
%endmacro

%macro  sc6     7
	push dword %7
	push dword %6
	push dword %5
	push dword %4
	push dword %3
	push dword %2
	call %1
	add esp, 24
%endmacro

%macro  sc7     8
	push dword %8
	push dword %7
	push dword %6
	push dword %5
	push dword %4
	push dword %3
	push dword %2
	call %1
	add esp, 28
%endmacro

%macro  sc8     9
	push dword %9
	push dword %8
	push dword %7
	push dword %6
	push dword %5
	push dword %4
	push dword %3
	push dword %2
	call %1
	add esp, 32
%endmacro

%macro  sc9     10
	push dword %10
	push dword %9
	push dword %8
	push dword %7
	push dword %6
	push dword %5
	push dword %4
	push dword %3
	push dword %2
	call %1
	add esp, 36
%endmacro





	extern semop
	extern semget
	extern semctl

	SEM_UNDO        equ     1000h
	GETVAL          equ     12
	SETVAL          equ     16
	SEMPERM         equ     600q
	IPC_EXCL        equ     2000q

	extern fopen
	extern fclose

	extern mprotect

;+--------------------------------------+ NCurses
	extern addch
	extern addchnstr
	extern addchstr
	extern addnstr
	extern addstr
	extern attroff
	extern attron
	extern attrset
	extern attr_get
	extern attr_off
	extern attr_on
	extern attr_set
	extern baudrate
	extern beep
	extern bkgd
	extern bkgdset
	extern border
	extern box
	extern can_change_color
	extern cbreak
	extern chgat
	extern clear
	extern clearok
	extern clrtobot
	extern clrtoeol
	extern color_content
	extern color_set
	extern COLOR_PAIR
	extern copywin
	extern curs_set
	extern def_prog_mode
	extern def_shell_mode
	extern delay_output
	extern delch
	extern delscreen
	extern delwin
	extern deleteln
	extern derwin
	extern doupdate
	extern dupwin
	extern echo
	extern echochar
	extern endwin
	extern erasechar
	extern filter
	extern flash
	extern flushinp
	extern getbkgd
	extern getch
	extern getnstr
	extern getstr
	extern getwin
	extern halfdelay
	extern has_colors
	extern has_ic
	extern has_il
	extern hline
	extern idcok
	extern idlok
	extern immedok
	extern inch
	extern inchnstr
	extern inchstr
	extern initscr
	extern init_color
	extern init_pair
	extern innstr
	extern insch
	extern insdelln
	extern insertln
	extern insnstr
	extern insstr
	extern instr
	extern intrflush
	extern isendwin
	extern is_linetouched
	extern is_wintouched
	extern keyname
	extern keypad
	extern killchar
	extern leaveok
	extern longname
	extern meta
	extern move
	extern mvaddch
	extern mvaddchnstr
	extern mvaddchstr
	extern mvaddnstr
	extern mvaddstr
	extern mvchgat
	extern mvcur
	extern mvdelch
	extern mvderwin
	extern mvgetch
	extern mvgetnstr
	extern mvgetstr
	extern mvhline
	extern mvinch
	extern mvinchnstr
	extern mvinchstr
	extern mvinnstr
	extern mvinsch
	extern mvinsnstr
	extern mvinsstr
	extern mvinstr
	extern mvprintw
	extern mvscanw
	extern mvvline
	extern mvwaddch
	extern mvwaddchnstr
	extern mvwaddchstr
	extern mvwaddnstr
	extern mvwaddstr
	extern mvwchgat
	extern mvwdelch
	extern mvwgetch
	extern mvwgetnstr
	extern mvwgetstr
	extern mvwhline
	extern mvwin
	extern mvwinch
	extern mvwinchnstr
	extern mvwinchstr
	extern mvwinnstr
	extern mvwinsch
	extern mvwinsnstr
	extern mvwinsstr
	extern mvwinstr
	extern mvwprintw
	extern mvwscanw
	extern mvwvline
	extern napms
	extern newpad
	extern newterm
	extern newwin
	extern nl
	extern nocbreak
	extern nodelay
	extern noecho
	extern nonl
	extern noqiflush
	extern noraw
	extern notimeout
	extern overlay
	extern overwrite
	extern pair_content
	extern PAIR_NUMBER
	extern pechochar
	extern pnoutrefresh
	extern prefresh
	extern printw
	extern putp
	extern putwin
	extern qiflush
	extern raw
	extern redrawwin
	extern refresh
	extern resetty
	extern reset_prog_mode
	extern reset_shell_mode
	extern ripoffline
	extern savetty
	extern scanw
	extern scr_dump
	extern scr_init
	extern scrl
	extern scroll
	extern scrollok
	extern scr_restore
	extern scr_set
	extern setscrreg
	extern set_term
	extern slk_attroff
	extern slk_attr_off
	extern slk_attron
	extern slk_attr_on
	extern slk_attrset
	extern slk_attr
	extern slk_clear
	extern slk_color
	extern slk_init
	extern slk_label
	extern slk_noutrefresh
	extern slk_refresh
	extern slk_restore
	extern slk_set
	extern slk_touch
	extern standout
	extern standend
	extern start_color
	extern subpad
	extern subwin
	extern syncok
	extern termattrs
	extern term_attrs
	extern termname
	extern tigetflag
	extern tigetnum
	extern tigetstr
	extern timeout
	extern tparm
	extern typeahead
	extern ungetch
	extern untouchwin
	extern use_env
	extern vidattr
	extern vid_attr
	extern vidputs
	extern vline
	extern vwprintw
	extern vw_printw
	extern vwscanw
	extern vw_scanw
	extern waddch
	extern waddchnstr
	extern waddchstr
	extern waddnstr
	extern waddstr
	extern wattron
	extern wattroff
	extern wattrset
	extern wattr_get
	extern wattr_on
	extern wattr_off
	extern wattr_set
	extern wbkgd
	extern wbkgdset
	extern wborder
	extern wchget
	extern wclear
	extern wclrtobot
	extern wclrtoeol
	extern wcolor_set
	extern wcursyncup
	extern wdelch
	extern wdeleteln
	extern wechochar
	extern werase
	extern wgetch
	extern wgetnstr
	extern wgetstr
	extern whline
	extern winch
	extern winchnstr
	extern winchstr
	extern winnstr
	extern winsch
	extern winsdelln
	extern winsertln
	extern winsnstr
	extern winsstr
	extern winstr
	extern wmove
	extern wnoutrefresh
	extern wprintw
	extern wredrawln
	extern wrefresh
	extern wscanw
	extern wscrl
	extern wsetscrreg
	extern wstandout
	extern wstandend
	extern wsyncdown
	extern wsyncup
	extern wtimeout
	extern wtouchln
	extern wvline
	extern mouse_trafo

	A_NORMAL        equ     0
	A_STANDOUT      equ     10000h
	A_UNDERLINE     equ     20000h  
	A_REVERSE       equ     40000h
	A_BLINK         equ     80000h
	A_DIM           equ     100000h
	A_BOLD          equ     200000h

	COLOR_BLACK     equ     0
	COLOR_RED       equ     1
	COLOR_GREEN     equ     2
	COLOR_YELLOW    equ     3
	COLOR_BLUE      equ     4
	COLOR_MAGENTA   equ     5
	COLOR_CYAN      equ     6
	COLOR_WHITE     equ     7

	extern LINES
	extern COLS     
	extern TABSIZE

;+--------------------------------------+ 

	extern sysinfo
	extern ptrace

	extern mmap
	extern munmap
	extern setrlimit
	extern getrlimit
	extern gettimeofday
	extern random
	extern lseek
	extern raise
	extern _exit
	extern accept
	extern alarm
	extern bind
	extern cbreak
	extern close
	extern connect
	extern creat
	extern ctime
	extern curscr
	extern dup
	extern dup2
	extern endwin
	extern errno
	extern execlp
	extern exit
	extern fcntl
	extern fork
	extern fprintf
	extern fsync
	extern ftok
	extern getch
	extern getchar
	extern gethostbyname
	extern gethostname
	extern getpid
	extern getppid
	extern getsockname
	extern getsockopt
	extern has_colors
	extern h_errno
	extern inet_ntoa
	extern initscr
	extern kill
	extern leaveok
	extern listen
	extern memcpy
	extern move
	extern mvaddstr
	extern mvaddnstr
	extern mvprintw
	extern noecho
	extern open
	extern pause
	extern pipe
	extern poll
	extern printf
	extern printw
	extern puts
	extern read
	extern recv
	extern refresh
	extern remove
	extern rename
	extern send
	extern setitimer
	extern setsockopt
	extern shmap
	extern shmat
	extern shmctl
	extern shmdt
	extern shmget
	extern signal
	extern sleep
	extern socket
	extern sprintf
	extern sscanf
	extern strcasecmp
	extern strncasecmp
	extern strcat
	extern strcmp
	extern strncmp
	extern strcpy
	extern strlen
	extern strncpy
	extern strtok
	extern time
	extern usleep
	extern wait
	extern waitpid
	extern wrefresh
	extern write

	extern mysql_field_count
	extern mysql_init
	extern mysql_real_connect
	extern mysql_query
	extern mysql_store_result
	extern mysql_free_result
	extern mysql_close
	extern mysql_errno
	extern mysql_error
	extern mysql_fetch_field
	extern mysql_fetch_row
	extern mysql_ping
	extern mysql_num_rows
	extern mysql_affected_rows
	extern mysql_insert_id
	extern mysql_escape_string
	extern mysql_data_seek

	extern srandom

	ptrace_traceme  equ     0
	ptrace_peektext equ     1
	ptrace_peekdata equ     2
	ptrace_peekusr  equ     3
	ptrace_poketext equ     4
	ptrace_pokedata equ     5
	ptrace_pokeusr  equ     6
	ptrace_cont     equ     7
	ptrace_kill     equ     8
	ptrace_singlestep equ   9
	ptrace_getregs  equ     12
	ptrace_setregs  equ     13
	ptrace_getpregs equ     14
	ptrace_setpregs equ     15
	ptrace_attach   equ     16
	ptrace_detach   equ     17
	ptrace_syscall  equ     24

	PROT_READ       equ     1
	PROT_WRITE      equ     2
	PROT_EXEC       equ     4
	PROT_NONE       equ     8

	MAP_SHARED      equ     1

	IPC_RMID        equ     0
	IPC_CREAT       equ     1000
	

	MSG_WAITALL     equ     100h

	INADDR_ANY      equ     0

	NULL            equ     0
	TRUE            equ     1
	FALSE           equ     0

	SOL_SOCKET      equ     1

	SO_ERROR        equ     4

	MSG_NOSIGNAL    equ     4000h

	WNOHANG         equ     1

	SOCK_STREAM     equ     1
	SOCK_DGRAM      equ     2
	SOCK_RAW        equ     3
	SOCK_RDM        equ     4
	SOCK_SEQPACKET  equ     5
	SOCK_PACKET     equ     10

	IP_OPTIONS      equ     4
	IPPROTO_IP      equ     0
	IPOPT_SSRR      equ     137
	IPOPT_NOP       equ     1

	AF_UNSPEC       equ     0
	AF_UNIX         equ     1
	AF_LOCAL        equ     1
	AF_INET         equ     2
	AF_AX25         equ     3
	AF_IPX          equ     4
	AF_APPLETALK    equ     5
	AF_NETROM       equ     6
	AF_BRIDGE       equ     7
	AF_ATMPVC       equ     8
	AF_X25          equ     9
	AF_INET6        equ     10
	AF_ROSE         equ     11
	AF_DECnet       equ     12
	AF_NETBEUI      equ     13
	AF_SECURITY     equ     14
	pseudo_AF_KEY   equ     15
	AF_NETLINK      equ     16
	AF_ROUTE        equ     AF_NETLINK
	AF_PACKET       equ     17
	AF_ASH          equ     18
	AF_ECONET       equ     19
	AF_ATMSVC       equ     20
	AF_SNA          equ     22
	AF_IRDA         equ     23
	AF_MAX          equ     32

	POLLIN          equ     0001h
	POLLPRI         equ     0002h
	POLLOUT         equ     0004h
	POLLERR         equ     0008h
	POLLHUP         equ     0010h
	POLLNVAL        equ     0020h

	TCP_NODELAY     equ     1
	IPPROTO_TCP     equ     6

	STDIN           equ     0
	STDOUT          equ     1
	STDERR          equ     2

	F_SETFL         equ     4
	O_NONBLOCK      equ     04000

	SEEK_SET        equ     0
	SEEK_CUR        equ     1
	SEEK_END        equ     2

	O_RDONLY        equ     0
	O_WRONLY        equ     1
	O_RDWR          equ     2
	O_APPEND        equ     02000
	O_CREAT         equ     0100
    O_NOCTTY        equ     0400

	SIG_DFL         equ     0
	SIG_IGN         equ     1
	SIG_ERR         equ     -1

	SIGHUP          equ     1
	SIGINT          equ     2
	SIGQUIT         equ     3
	SIGILL          equ     4
	SIGTRAP         equ     5
	SIGABRT         equ     6
	SIGIOT          equ     6
	SIGBUS          equ     7
	SIGFPE          equ     8
	SIGKILL         equ     9
	SIGUSR1         equ     10
	SIGSEGV         equ     11
	SIGUSR2         equ     12
	SIGPIPE         equ     13
	SIGALRM         equ     14
	SIGTERM         equ     15
	SIGSTKFLT       equ     16
	SIGCHLD         equ     17
	SIGCLD          equ     17
	SIGCONT         equ     18
	SIGSTOP         equ     19
	SIGTSTP         equ     20
	SIGTTIN         equ     21
	SIGTTOU         equ     22
	SIGURG          equ     23
	SIGXCPU         equ     24
	SIGXFSZ         equ     25
	SIGVTALRM       equ     26
	SIGPROF         equ     27
	SIGWINCH        equ     28
	SIGIO           equ     29
	SIGPOLL         equ     29
	SIGPWR          equ     30
	SIGUNUSED       equ     31

	ITIMER_REAL     equ     0
	ITIMER_VIRTUAL  equ     1
	ITIMER_PROF     equ     2

	RLIMIT_CPU      equ     0
	RLIMIT_FSIZE    equ     1
	RLIMIT_DATA     equ     2
	RLIMIT_STACK    equ     3
	RLIMIT_CORE     equ     4
	RLIMIT_RSS      equ     5
	RLIMIT_NPROC    equ     6
	RLIMIT_NOFILE   equ     7
	RLIMIT_MEMLOCK  equ     8
	RLIMIT_AS       equ     9

%define CR_UNKNOWN_ERROR        2000
%define CR_SOCKET_CREATE_ERROR  2001
%define CR_CONNECTION_ERROR     2002
%define CR_CONN_HOST_ERROR      2003
%define CR_IPSOCK_ERROR         2004
%define CR_UNKNOWN_HOST         2005
%define CR_SERVER_GONE_ERROR    2006
%define CR_VERSION_ERROR        2007
%define CR_OUT_OF_MEMORY        2008
%define CR_WRONG_HOST_INFO      2009
%define CR_LOCALHOST_CONNECTION 2010
%define CR_TCP_CONNECTION       2011
%define CR_SERVER_HANDSHAKE_ERR 2012
%define CR_SERVER_LOST          2013
%define CR_COMMANDS_OUT_OF_SYNC 2014
%define CR_NAMEDPIPE_CONNECTION 2015
%define CR_NAMEDPIPEWAIT_ERROR  2016
%define CR_NAMEDPIPEOPEN_ERROR  2017
%define CR_NAMEDPIPESETSTATE_ERROR 2018


%define EPERM            1      ; Operation not permitted */
%define ENOENT           2      ; No such file or directory */
%define ESRCH            3      ; No such process */
%define EINTR            4      ; Interrupted system call */
%define EIO              5      ; I/O error */
%define ENXIO            6      ; No such device or address */
%define E2BIG            7      ; Arg list too long */
%define ENOEXEC          8      ; Exec format error */
%define EBADF            9      ; Bad file number */
%define ECHILD          10      ; No child processes */
%define EAGAIN          11      ; Try again */
%define ENOMEM          12      ; Out of memory */
%define EACCES          13      ; Permission denied */
%define EFAULT          14      ; Bad address */
%define ENOTBLK         15      ; Block device required */
%define EBUSY           16      ; Device or resource busy */
%define EEXIST          17      ; File exists */
%define EXDEV           18      ; Cross-device link */
%define ENODEV          19      ; No such device */
%define ENOTDIR         20      ; Not a directory */
%define EISDIR          21      ; Is a directory */
%define EINVAL          22      ; Invalid argument */
%define ENFILE          23      ; File table overflow */
%define EMFILE          24      ; Too many open files */
%define ENOTTY          25      ; Not a typewriter */
%define ETXTBSY         26      ; Text file busy */
%define EFBIG           27      ; File too large */
%define ENOSPC          28      ; No space left on device */
%define ESPIPE          29      ; Illegal seek */
%define EROFS           30      ; Read-only file system */
%define EMLINK          31      ; Too many links */
%define EPIPE           32      ; Broken pipe */
%define EDOM            33      ; Math argument out of domain of func */
%define ERANGE          34      ; Math result not representable */
%define EDEADLK         35      ; Resource deadlock would occur */
%define EDEADLOCK       EDEADLK
%define ENAMETOOLONG    36      ; File name too long */
%define ENOLCK          37      ; No record locks available */
%define ENOSYS          38      ; Function not implemented */
%define ENOTEMPTY       39      ; Directory not empty */
%define ELOOP           40      ; Too many symbolic links encountered */
%define EWOULDBLOCK     EAGAIN  ; Operation would block */
%define ENOMSG          42      ; No message of desired type */
%define EIDRM           43      ; Identifier removed */
%define ECHRNG          44      ; Channel number out of range */
%define EL2NSYNC        45      ; Level 2 not synchronized */
%define EL3HLT          46      ; Level 3 halted */
%define EL3RST          47      ; Level 3 reset */
%define ELNRNG          48      ; Link number out of range */
%define EUNATCH         49      ; Protocol driver not attached */
%define ENOCSI          50      ; No CSI structure available */
%define EL2HLT          51      ; Level 2 halted */
%define EBADE           52      ; Invalid exchange */
%define EBADR           53      ; Invalid request descriptor */
%define EXFULL          54      ; Exchange full */
%define ENOANO          55      ; No anode */
%define EBADRQC         56      ; Invalid request code */
%define EBADSLT         57      ; Invalid slot */
%define EBFONT          59      ; Bad font file format */
%define ENOSTR          60      ; Device not a stream */
%define ENODATA         61      ; No data available */
%define ETIME           62      ; Timer expired */
%define ENOSR           63      ; Out of streams resources */
%define ENONET          64      ; Machine is not on the network */
%define ENOPKG          65      ; Package not installed */
%define EREMOTE         66      ; Object is remote */
%define ENOLINK         67      ; Link has been severed */
%define EADV            68      ; Advertise error */
%define ESRMNT          69      ; Srmount error */
%define ECOMM           70      ; Communication error on send */
%define EPROTO          71      ; Protocol error */
%define EMULTIHOP       72      ; Multihop attempted */
%define EDOTDOT         73      ; RFS specific error */
%define EBADMSG         74      ; Not a data message */
%define EOVERFLOW       75      ; Value too large for defined data type */
%define ENOTUNIQ        76      ; Name not unique on network */
%define EBADFD          77      ; File descriptor in bad state */
%define EREMCHG         78      ; Remote address changed */
%define ELIBACC         79      ; Can not access a needed shared library */
%define ELIBBAD         80      ; Accessing a corrupted shared library */
%define ELIBSCN         81      ; .lib section in a.out corrupted */
%define ELIBMAX         82      ; Attempting to link in too many shared libraries */
%define ELIBEXEC        83      ; Cannot exec a shared library directly */
%define EILSEQ          84      ; Illegal byte sequence */
%define ERESTART        85      ; Interrupted system call should be restarted */
%define ESTRPIPE        86      ; Streams pipe error */
%define EUSERS          87      ; Too many users */
%define ENOTSOCK        88      ; Socket operation on non-socket */
%define EDESTADDRREQ    89      ; Destination address required */
%define EMSGSIZE        90      ; Message too long */
%define EPROTOTYPE      91      ; Protocol wrong type for socket */
%define ENOPROTOOPT     92      ; Protocol not available */
%define EPROTONOSUPPORT 93      ; Protocol not supported */
%define ESOCKTNOSUPPORT 94      ; Socket type not supported */
%define EOPNOTSUPP      95      ; Operation not supported on transport endpoint */
%define EPFNOSUPPORT    96      ; Protocol family not supported */
%define EAFNOSUPPORT    97      ; Address family not supported by protocol */
%define EADDRINUSE      98      ; Address already in use */
%define EADDRNOTAVAIL   99      ; Cannot assign requested address */
%define ENETDOWN        100     ; Network is down */
%define ENETUNREACH     101     ; Network is unreachable */
%define ENETRESET       102     ; Network dropped connection because of reset */
%define ECONNABORTED    103     ; Software caused connection abort */
%define ECONNRESET      104     ; Connection reset by peer */
%define ENOBUFS         105     ; No buffer space available */
%define EISCONN         106     ; Transport endpoint is already connected */
%define ENOTCONN        107     ; Transport endpoint is not connected */
%define ESHUTDOWN       108     ; Cannot send after transport endpoint shutdown */
%define ETOOMANYREFS    109     ; Too many references: cannot splice */
%define ETIMEDOUT       110     ; Connection timed out */
%define ECONNREFUSED    111     ; Connection refused */
%define EHOSTDOWN       112     ; Host is down */
%define EHOSTUNREACH    113     ; No route to host */
%define EALREADY        114     ; Operation already in progress */
%define EINPROGRESS     115     ; Operation now in progress */
%define ESTALE          116     ; Stale NFS file handle */
%define EUCLEAN         117     ; Structure needs cleaning */
%define ENOTNAM         118     ; Not a XENIX named type file */
%define ENAVAIL         119     ; No XENIX semaphores available */
%define EISNAM          120     ; Is a named type file */
%define EREMOTEIO       121     ; Remote I/O error */
%define EDQUOT          122     ; Quota exceeded */
%define ENOMEDIUM       123     ; No medium found */
%define EMEDIUMTYPE     124     ; Wrong 
