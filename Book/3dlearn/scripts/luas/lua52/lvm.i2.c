#pragma once

typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);


typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);
typedef int (*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);


typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);

typedef double lua_Number;

typedef ptrdiff_t lua_Integer;

typedef unsigned int lua_Unsigned;


__declspec(dllexport) lua_State *(lua_newstate) (lua_Alloc f, void *ud);
__declspec(dllexport) void       (lua_close) (lua_State *L);
__declspec(dllexport) lua_State *(lua_newthread) (lua_State *L);
__declspec(dllexport) lua_CFunction (lua_atpanic) (lua_State *L, lua_CFunction panicf);

__declspec(dllexport) const lua_Number *(lua_version) (lua_State *L);


__declspec(dllexport) int   (lua_absindex) (lua_State *L, int idx);
__declspec(dllexport) int   (lua_gettop) (lua_State *L);
__declspec(dllexport) void  (lua_settop) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_pushvalue) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_remove) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_insert) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_replace) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_copy) (lua_State *L, int fromidx, int toidx);
__declspec(dllexport) int   (lua_checkstack) (lua_State *L, int sz);
__declspec(dllexport) void  (lua_xmove) (lua_State *from, lua_State *to, int n);
__declspec(dllexport) int             (lua_isnumber) (lua_State *L, int idx);
__declspec(dllexport) int             (lua_isstring) (lua_State *L, int idx);
__declspec(dllexport) int             (lua_iscfunction) (lua_State *L, int idx);
__declspec(dllexport) int             (lua_isuserdata) (lua_State *L, int idx);
__declspec(dllexport) int             (lua_type) (lua_State *L, int idx);
__declspec(dllexport) const char     *(lua_typename) (lua_State *L, int tp);
__declspec(dllexport) lua_Number      (lua_tonumberx) (lua_State *L, int idx, int *isnum);
__declspec(dllexport) lua_Integer     (lua_tointegerx) (lua_State *L, int idx, int *isnum);
__declspec(dllexport) lua_Unsigned    (lua_tounsignedx) (lua_State *L, int idx, int *isnum);
__declspec(dllexport) int             (lua_toboolean) (lua_State *L, int idx);
__declspec(dllexport) const char     *(lua_tolstring) (lua_State *L, int idx, size_t *len);
__declspec(dllexport) size_t          (lua_rawlen) (lua_State *L, int idx);
__declspec(dllexport) lua_CFunction   (lua_tocfunction) (lua_State *L, int idx);
__declspec(dllexport) void	       *(lua_touserdata) (lua_State *L, int idx);
__declspec(dllexport) lua_State      *(lua_tothread) (lua_State *L, int idx);
__declspec(dllexport) const void     *(lua_topointer) (lua_State *L, int idx);


__declspec(dllexport) void  (lua_arith) (lua_State *L, int op);


__declspec(dllexport) int   (lua_rawequal) (lua_State *L, int idx1, int idx2);
__declspec(dllexport) int   (lua_compare) (lua_State *L, int idx1, int idx2, int op);


__declspec(dllexport) void        (lua_pushnil) (lua_State *L);
__declspec(dllexport) void        (lua_pushnumber) (lua_State *L, lua_Number n);
__declspec(dllexport) void        (lua_pushinteger) (lua_State *L, lua_Integer n);
__declspec(dllexport) void        (lua_pushunsigned) (lua_State *L, lua_Unsigned n);
__declspec(dllexport) const char *(lua_pushlstring) (lua_State *L, const char *s, size_t l);
__declspec(dllexport) const char *(lua_pushstring) (lua_State *L, const char *s);
__declspec(dllexport) const char *(lua_pushvfstring) (lua_State *L, const char *fmt,
													  va_list argp);
__declspec(dllexport) const char *(lua_pushfstring) (lua_State *L, const char *fmt, ...);
__declspec(dllexport) void  (lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
__declspec(dllexport) void  (lua_pushboolean) (lua_State *L, int b);
__declspec(dllexport) void  (lua_pushlightuserdata) (lua_State *L, void *p);
__declspec(dllexport) int   (lua_pushthread) (lua_State *L);


__declspec(dllexport) void  (lua_getglobal) (lua_State *L, const char *var);
__declspec(dllexport) void  (lua_gettable) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_getfield) (lua_State *L, int idx, const char *k);
__declspec(dllexport) void  (lua_rawget) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_rawgeti) (lua_State *L, int idx, int n);
__declspec(dllexport) void  (lua_rawgetp) (lua_State *L, int idx, const void *p);
__declspec(dllexport) void  (lua_createtable) (lua_State *L, int narr, int nrec);
__declspec(dllexport) void *(lua_newuserdata) (lua_State *L, size_t sz);
__declspec(dllexport) int   (lua_getmetatable) (lua_State *L, int objindex);
__declspec(dllexport) void  (lua_getuservalue) (lua_State *L, int idx);


__declspec(dllexport) void  (lua_setglobal) (lua_State *L, const char *var);
__declspec(dllexport) void  (lua_settable) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_setfield) (lua_State *L, int idx, const char *k);
__declspec(dllexport) void  (lua_rawset) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_rawseti) (lua_State *L, int idx, int n);
__declspec(dllexport) void  (lua_rawsetp) (lua_State *L, int idx, const void *p);
__declspec(dllexport) int   (lua_setmetatable) (lua_State *L, int objindex);
__declspec(dllexport) void  (lua_setuservalue) (lua_State *L, int idx);


__declspec(dllexport) void  (lua_callk) (lua_State *L, int nargs, int nresults, int ctx,
										 lua_CFunction k);

__declspec(dllexport) int   (lua_getctx) (lua_State *L, int *ctx);
__declspec(dllexport) int   (lua_pcallk) (lua_State *L, int nargs, int nresults, int errfunc,
										  int ctx, lua_CFunction k);

__declspec(dllexport) int   (lua_load) (lua_State *L, lua_Reader reader, void *dt,
										const char *chunkname,
										const char *mode);
__declspec(dllexport) int (lua_dump) (lua_State *L, lua_Writer writer, void *data);


__declspec(dllexport) int  (lua_yieldk) (lua_State *L, int nresults, int ctx,
										 lua_CFunction k);
__declspec(dllexport) int  (lua_resume) (lua_State *L, lua_State *from, int narg);
__declspec(dllexport) int  (lua_status) (lua_State *L);

__declspec(dllexport) int (lua_gc) (lua_State *L, int what, int data);
__declspec(dllexport) int   (lua_error) (lua_State *L);
__declspec(dllexport) int   (lua_next) (lua_State *L, int idx);
__declspec(dllexport) void  (lua_concat) (lua_State *L, int n);
__declspec(dllexport) void  (lua_len)    (lua_State *L, int idx);
__declspec(dllexport) lua_Alloc (lua_getallocf) (lua_State *L, void **ud);
__declspec(dllexport) void      (lua_setallocf) (lua_State *L, lua_Alloc f, void *ud);


typedef struct lua_Debug lua_Debug;  

typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);

__declspec(dllexport) int (lua_getstack) (lua_State *L, int level, lua_Debug *ar);
__declspec(dllexport) int (lua_getinfo) (lua_State *L, const char *what, lua_Debug *ar);
__declspec(dllexport) const char *(lua_getlocal) (lua_State *L, const lua_Debug *ar, int n);
__declspec(dllexport) const char *(lua_setlocal) (lua_State *L, const lua_Debug *ar, int n);
__declspec(dllexport) const char *(lua_getupvalue) (lua_State *L, int funcindex, int n);
__declspec(dllexport) const char *(lua_setupvalue) (lua_State *L, int funcindex, int n);
__declspec(dllexport) void *(lua_upvalueid) (lua_State *L, int fidx, int n);
__declspec(dllexport) void  (lua_upvaluejoin) (lua_State *L, int fidx1, int n1,
											   int fidx2, int n2);
__declspec(dllexport) int (lua_sethook) (lua_State *L, lua_Hook func, int mask, int count);
__declspec(dllexport) lua_Hook (lua_gethook) (lua_State *L);
__declspec(dllexport) int (lua_gethookmask) (lua_State *L);
__declspec(dllexport) int (lua_gethookcount) (lua_State *L);

struct lua_Debug {
	int event;
	const char *name;	
	const char *namewhat;	
	const char *what;	
	const char *source;	
	int currentline;	
	int linedefined;	
	int lastlinedefined;	
	unsigned char nups;	
	unsigned char nparams;
	char isvararg;        
	char istailcall;	
	char short_src[60]; 

	struct CallInfo *i_ci;  
};
typedef unsigned int lu_int32;
typedef size_t lu_mem;
typedef ptrdiff_t l_mem;


typedef unsigned char lu_byte;

typedef union { double u; void *s; long l; } L_Umaxalign;

typedef double l_uacNumber;


typedef lu_int32 Instruction;

typedef union GCObject GCObject;

typedef struct GCheader {
	GCObject *next; lu_byte tt; lu_byte marked;
} GCheader;
typedef union Value Value;

typedef struct lua_TValue TValue;

union Value {
	GCObject *gc;    
	void *p;         
	int b;           
	lua_CFunction f; 

};

struct lua_TValue {
	union { struct { Value v__; int tt__; } i; double d__; } u;
};

typedef TValue *StkId;  
typedef union TString {
	L_Umaxalign dummy;  
	struct {
		GCObject *next; lu_byte tt; lu_byte marked;
		lu_byte reserved;
		unsigned int hash;
		size_t len;  
	} tsv;
} TString;

typedef union Udata {
	L_Umaxalign dummy;  
	struct {
		GCObject *next; lu_byte tt; lu_byte marked;
		struct Table *metatable;
		struct Table *env;
		size_t len;  
	} uv;
} Udata;
typedef struct Upvaldesc {
	TString *name;  
	lu_byte instack;  
	lu_byte idx;  
} Upvaldesc;
typedef struct LocVar {
	TString *varname;
	int startpc;  
	int endpc;    
} LocVar;


typedef struct Proto {
	GCObject *next; lu_byte tt; lu_byte marked;
	TValue *k;  
	Instruction *code;
	struct Proto **p;  
	int *lineinfo;  
	LocVar *locvars;  
	Upvaldesc *upvalues;  
	union Closure *cache;  
	TString  *source;  
	int sizeupvalues;  
	int sizek;  
	int sizecode;
	int sizelineinfo;
	int sizep;  
	int sizelocvars;
	int linedefined;
	int lastlinedefined;
	GCObject *gclist;
	lu_byte numparams;  
	lu_byte is_vararg;
	lu_byte maxstacksize;  
} Proto;
typedef struct UpVal {
	GCObject *next; lu_byte tt; lu_byte marked;
	TValue *v;  
	union {
		TValue value;  
		struct {  
			struct UpVal *prev;
			struct UpVal *next;
		} l;
	} u;
} UpVal;

typedef struct CClosure {
	GCObject *next; lu_byte tt; lu_byte marked; lu_byte isC; lu_byte nupvalues; GCObject *gclist;
	lua_CFunction f;
	TValue upvalue[1];  
} CClosure;

typedef struct LClosure {
	GCObject *next; lu_byte tt; lu_byte marked; lu_byte isC; lu_byte nupvalues; GCObject *gclist;
	struct Proto *p;
	UpVal *upvals[1];  
} LClosure;

typedef union Closure {
	CClosure c;
	LClosure l;
} Closure;
typedef union TKey {
	struct {
		union { struct { Value v__; int tt__; } i; double d__; } u;
		struct Node *next;  
	} nk;
	TValue tvk;
} TKey;

typedef struct Node {
	TValue i_val;
	TKey i_key;
} Node;

typedef struct Table {
	GCObject *next; lu_byte tt; lu_byte marked;
	lu_byte flags;  
	lu_byte lsizenode;  
	struct Table *metatable;
	TValue *array;  
	Node *node;
	Node *lastfree;  
	GCObject *gclist;
	int sizearray;  
} Table;
extern const TValue luaO_nilobject_;

extern int luaO_int2fb (unsigned int x);
extern int luaO_fb2int (int x);
extern int luaO_ceillog2 (unsigned int x);
extern lua_Number luaO_arith (int op, lua_Number v1, lua_Number v2);
extern int luaO_str2d (const char *s, size_t len, lua_Number *result);
extern int luaO_hexavalue (int c);
extern const char *luaO_pushvfstring (lua_State *L, const char *fmt,
									  va_list argp);
extern const char *luaO_pushfstring (lua_State *L, const char *fmt, ...);
extern void luaO_chunkid (char *out, const char *source, size_t len);
typedef enum {
	TM_INDEX,
	TM_NEWINDEX,
	TM_GC,
	TM_MODE,
	TM_LEN,
	TM_EQ,  
	TM_ADD,
	TM_SUB,
	TM_MUL,
	TM_DIV,
	TM_MOD,
	TM_POW,
	TM_UNM,
	TM_LT,
	TM_LE,
	TM_CONCAT,
	TM_CALL,
	TM_N		
} TMS;
extern const char *const luaT_typenames_[((9+1)+2)];

extern const TValue *luaT_gettm (Table *events, TMS event, TString *ename);
extern const TValue *luaT_gettmbyobj (lua_State *L, const TValue *o,
									  TMS event);
extern void luaT_init (lua_State *L);

extern void __declspec(noreturn) luaM_toobig (lua_State *L);

extern void *luaM_realloc_ (lua_State *L, void *block, size_t oldsize,
							size_t size);
extern void *luaM_growaux_ (lua_State *L, void *block, int *size,
							size_t size_elem, int limit,
							const char *what);

typedef struct Zio ZIO;


typedef struct Mbuffer {
	char *buffer;
	size_t n;
	size_t buffsize;
} Mbuffer;
extern char *luaZ_openspace (lua_State *L, Mbuffer *buff, size_t n);
extern void luaZ_init (lua_State *L, ZIO *z, lua_Reader reader,
					   void *data);
extern size_t luaZ_read (ZIO* z, void* b, size_t n);	


struct Zio {
	size_t n;			
	const char *p;		
	lua_Reader reader;		
	void* data;			
	lua_State *L;			
};

extern int luaZ_fill (ZIO *z);
struct lua_longjmp;  
typedef struct stringtable {
	GCObject **hash;
	lu_int32 nuse;  
	int size;
} stringtable;


typedef struct CallInfo {
	StkId func;  
	StkId	top;  
	struct CallInfo *previous, *next;  
	short nresults;  
	lu_byte callstatus;
	union {
		struct {  
			StkId base;  
			const Instruction *savedpc;
		} l;
		struct {  
			int ctx;  
			lua_CFunction k;  
			ptrdiff_t old_errfunc;
			ptrdiff_t extra;
			lu_byte old_allowhook;
			lu_byte status;
		} c;
	} u;
} CallInfo;
typedef struct global_State {
	lua_Alloc frealloc;  
	void *ud;         
	lu_mem totalbytes;  
	l_mem GCdebt;  
	lu_mem lastmajormem;  
	stringtable strt;  
	TValue l_registry;
	lu_byte currentwhite;
	lu_byte gcstate;  
	lu_byte gckind;  
	lu_byte gcrunning;  
	int sweepstrgc;  
	GCObject *allgc;  
	GCObject *finobj;  
	GCObject **sweepgc;  
	GCObject *gray;  
	GCObject *grayagain;  
	GCObject *weak;  
	GCObject *ephemeron;  
	GCObject *allweak;  
	GCObject *tobefnz;  
	UpVal uvhead;  
	Mbuffer buff;  
	int gcpause;  
	int gcmajorinc;  
	int gcstepmul;  
	lua_CFunction panic;  
	struct lua_State *mainthread;
	const lua_Number *version;  
	TString *memerrmsg;  
	TString *tmname[TM_N];  
	struct Table *mt[9];  
} global_State;


struct lua_State {
	GCObject *next; lu_byte tt; lu_byte marked;
	lu_byte status;
	StkId top;  
	global_State *l_G;
	CallInfo *ci;  
	const Instruction *oldpc;  
	StkId stack_last;  
	StkId stack;  
	int stacksize;
	unsigned short nny;  
	unsigned short nCcalls;  
	lu_byte hookmask;
	lu_byte allowhook;
	int basehookcount;
	int hookcount;
	lua_Hook hook;
	GCObject *openupval;  
	GCObject *gclist;
	struct lua_longjmp *errorJmp;  
	ptrdiff_t errfunc;  
	CallInfo base_ci;  
};

union GCObject {
	GCheader gch;  
	union TString ts;
	union Udata u;
	union Closure cl;
	struct Table h;
	struct Proto p;
	struct UpVal uv;
	struct lua_State th;  
};

extern void luaE_setdebt (global_State *g, l_mem debt);
extern void luaE_freethread (lua_State *L, lua_State *L1);
extern CallInfo *luaE_extendCI (lua_State *L);
extern void luaE_freeCI (lua_State *L);
extern void __declspec(noreturn) luaG_typeerror (lua_State *L, const TValue *o,
												 const char *opname);
extern void __declspec(noreturn) luaG_concaterror (lua_State *L, StkId p1, StkId p2);
extern void __declspec(noreturn) luaG_aritherror (lua_State *L, const TValue *p1,
												  const TValue *p2);
extern void __declspec(noreturn) luaG_ordererror (lua_State *L, const TValue *p1,
												  const TValue *p2);
extern void __declspec(noreturn) luaG_runerror (lua_State *L, const char *fmt, ...);
extern void __declspec(noreturn) luaG_errormsg (lua_State *L);


typedef void (*Pfunc) (lua_State *L, void *ud);
extern int luaD_protectedparser (lua_State *L, ZIO *z, const char *name,
								 const char *mode);
extern void luaD_hook (lua_State *L, int event, int line);
extern int luaD_precall (lua_State *L, StkId func, int nresults);
extern void luaD_call (lua_State *L, StkId func, int nResults,
					   int allowyield);
extern int luaD_pcall (lua_State *L, Pfunc func, void *u,
					   ptrdiff_t oldtop, ptrdiff_t ef);
extern int luaD_poscall (lua_State *L, StkId firstResult);
extern void luaD_reallocstack (lua_State *L, int newsize);
extern void luaD_growstack (lua_State *L, int n);
extern void luaD_shrinkstack (lua_State *L);
extern void __declspec(noreturn) luaD_throw (lua_State *L, int errcode);
extern int luaD_rawrunprotected (lua_State *L, Pfunc f, void *ud);

extern Proto *luaF_newproto (lua_State *L);
extern Closure *luaF_newCclosure (lua_State *L, int nelems);
extern Closure *luaF_newLclosure (lua_State *L, Proto *p);
extern UpVal *luaF_newupval (lua_State *L);
extern UpVal *luaF_findupval (lua_State *L, StkId level);
extern void luaF_close (lua_State *L, StkId level);
extern void luaF_freeproto (lua_State *L, Proto *f);
extern void luaF_freeclosure (lua_State *L, Closure *c);
extern void luaF_freeupval (lua_State *L, UpVal *uv);
extern const char *luaF_getlocalname (const Proto *func, int local_number,
									  int pc);

extern void luaC_freeallobjects (lua_State *L);
extern void luaC_step (lua_State *L);
extern void luaC_forcestep (lua_State *L);
extern void luaC_runtilstate (lua_State *L, int statesmask);
extern void luaC_fullgc (lua_State *L, int isemergency);
extern GCObject *luaC_newobj (lua_State *L, int tt, size_t sz,
							  GCObject **list, int offset);
extern void luaC_barrier_ (lua_State *L, GCObject *o, GCObject *v);
extern void luaC_barrierback_ (lua_State *L, GCObject *o);
extern void luaC_barrierproto_ (lua_State *L, Proto *p, Closure *c);
extern void luaC_checkfinalizer (lua_State *L, GCObject *o, Table *mt);
extern void luaC_checkupvalcolor (global_State *g, UpVal *uv);
extern void luaC_changemode (lua_State *L, int mode);
enum OpMode {iABC, iABx, iAsBx, iAx};  

typedef enum {

	OP_MOVE,
	OP_LOADK,
	OP_LOADKX,
	OP_LOADBOOL,
	OP_LOADNIL,
	OP_GETUPVAL,
	OP_GETTABUP,
	OP_GETTABLE,
	OP_SETTABUP,
	OP_SETUPVAL,
	OP_SETTABLE,
	OP_NEWTABLE,
	OP_SELF,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_POW,
	OP_UNM,
	OP_NOT,
	OP_LEN,
	OP_CONCAT,
	OP_JMP,
	OP_EQ,
	OP_LT,
	OP_LE,
	OP_TEST,
	OP_TESTSET,
	OP_CALL,
	OP_TAILCALL,
	OP_RETURN,
	OP_FORLOOP,
	OP_FORPREP,
	OP_TFORCALL,
	OP_TFORLOOP,
	OP_SETLIST,
	OP_CLOSURE,
	OP_VARARG,
	OP_EXTRAARG
} OpCode;

enum OpArgMask {
	OpArgN,  
	OpArgU,  
	OpArgR,  
	OpArgK   
};
extern const lu_byte luaP_opmodes[(((int)(OP_EXTRAARG)) + 1)];

extern const char *const luaP_opnames[(((int)(OP_EXTRAARG)) + 1)+1];  
extern void luaS_resize (lua_State *L, int newsize);
extern Udata *luaS_newudata (lua_State *L, size_t s, Table *e);
extern TString *luaS_newlstr (lua_State *L, const char *str, size_t l);
extern TString *luaS_new (lua_State *L, const char *str);

extern const TValue *luaH_getint (Table *t, int key);
extern void luaH_setint (lua_State *L, Table *t, int key, TValue *value);
extern const TValue *luaH_getstr (Table *t, TString *key);
extern const TValue *luaH_get (Table *t, const TValue *key);
extern TValue *luaH_newkey (lua_State *L, Table *t, const TValue *key);
extern TValue *luaH_set (lua_State *L, Table *t, const TValue *key);
extern Table *luaH_new (lua_State *L);
extern void luaH_resize (lua_State *L, Table *t, int nasize, int nhsize);
extern void luaH_resizearray (lua_State *L, Table *t, int nasize);
extern void luaH_free (lua_State *L, Table *t);
extern int luaH_next (lua_State *L, Table *t, StkId key);
extern int luaH_getn (Table *t);

extern int luaV_equalobj_ (lua_State *L, const TValue *t1, const TValue *t2);

extern int luaV_lessthan (lua_State *L, const TValue *l, const TValue *r);
extern int luaV_lessequal (lua_State *L, const TValue *l, const TValue *r);
extern const TValue *luaV_tonumber (const TValue *obj, TValue *n);
extern int luaV_tostring (lua_State *L, StkId obj);
extern void luaV_gettable (lua_State *L, const TValue *t, TValue *key,
						   StkId val);
extern void luaV_settable (lua_State *L, const TValue *t, TValue *key,
						   StkId val);
extern void luaV_finishOp (lua_State *L);
extern void luaV_execute (lua_State *L);
extern void luaV_concat (lua_State *L, int total);
extern void luaV_arith (lua_State *L, StkId ra, const TValue *rb,
						const TValue *rc, TMS op);
extern void luaV_objlen (lua_State *L, StkId ra, const TValue *rb);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

const TValue *luaV_tonumber (const TValue *obj, TValue *n) {
	lua_Number num;
	if (((((obj)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) return obj;
	if (((((obj))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) && luaO_str2d(((const char *)(((&((obj)->u.i.v__).gc->ts)) + 1)), (&(&((obj)->u.i.v__).gc->ts)->tsv)->len, &num)) {
		{ TValue *io_=(n); ((io_)->u.d__)=(num); ((void)0); };
		return n;
	}
	else
		return ((void *)0);
}

int luaV_tostring (lua_State *L, StkId obj) {
	if (!((((obj)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
		return 0;
	else {
		char s[32];
		lua_Number n = (((obj)->u.d__));
		int l = sprintf((s), "%.14g", (n));
		{ TValue *io=(obj); ((io)->u.i.v__).gc=((GCObject *)((luaS_newlstr(L, s, l)))); (((io)->u.i.tt__) = (0x7FF7A500 | (((4) | (1 << 6))))); ((void)0); };
		return 1;
	}
}

static void traceexec (lua_State *L) {
	CallInfo *ci = L->ci;
	lu_byte mask = L->hookmask;
	if ((mask & (1 << 3)) && L->hookcount == 0) {
		(L->hookcount = L->basehookcount);
		luaD_hook(L, 3, -1);
	}
	if (mask & (1 << 2)) {
		Proto *p = ((&(((ci)->func)->u.i.v__).gc->cl.l))->p;
		int npc = (((int)((ci->u.l.savedpc) - (p)->code)) - 1);
		int newline = (((p)->lineinfo) ? (p)->lineinfo[npc] : 0);
		if (npc == 0 ||  
			ci->u.l.savedpc <= L->oldpc ||  
			newline != (((p)->lineinfo) ? (p)->lineinfo[(((int)((L->oldpc) - (p)->code)) - 1)] : 0))  
			luaD_hook(L, 2, newline);
	}
	L->oldpc = ci->u.l.savedpc;
	if (L->status == 1) {  
		ci->u.l.savedpc--;  
		luaD_throw(L, 1);
	}
}

static void callTM (lua_State *L, const TValue *f, const TValue *p1,
					const TValue *p2, TValue *p3, int hasres) {
						ptrdiff_t result = ((char *)(p3) - (char *)L->stack);
						{ const TValue *o2_=(f); TValue *o1_=(L->top++); o1_->u = o2_->u; ((void)0); };  
						{ const TValue *o2_=(p1); TValue *o1_=(L->top++); o1_->u = o2_->u; ((void)0); };  
						{ const TValue *o2_=(p2); TValue *o1_=(L->top++); o1_->u = o2_->u; ((void)0); };  
						if (!hasres)  
						{ const TValue *o2_=(p3); TValue *o1_=(L->top++); o1_->u = o2_->u; ((void)0); };  
						if (L->stack_last - L->top <= (0)) luaD_growstack(L, 0); else ((void)0);;

						luaD_call(L, L->top - (4 - hasres), hasres, ((L->ci)->callstatus & (1<<0)));
						if (hasres) {  
							p3 = ((TValue *)((char *)L->stack + (result)));
							{ const TValue *o2_=(--L->top); TValue *o1_=(p3); o1_->u = o2_->u; ((void)0); };
						}
}

void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val) {
	int loop;
	for (loop = 0; loop < 100; loop++) {
		const TValue *tm;
		if (((((t))->u.i.tt__) == (0x7FF7A500 | (((5) | (1 << 6)))))) {  
			Table *h = (&((t)->u.i.v__).gc->h);
			const TValue *res = luaH_get(h, key); 
			if (!((((res))->u.i.tt__) == (0x7FF7A500 | (0))) ||  
				(tm = ((h->metatable) == ((void *)0) ? ((void *)0) : ((h->metatable)->flags & (1u<<(TM_INDEX))) ? ((void *)0) : luaT_gettm(h->metatable, TM_INDEX, ((L->l_G))->tmname[TM_INDEX]))) == ((void *)0)) { 
					{ const TValue *o2_=(res); TValue *o1_=(val); o1_->u = o2_->u; ((void)0); };
					return;
			}

		}
		else if (((((tm = luaT_gettmbyobj(L, t, TM_INDEX)))->u.i.tt__) == (0x7FF7A500 | (0))))
			luaG_typeerror(L, t, "index");
		if ((((((((tm)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((tm)->u.i.tt__) & 0xff) & 0x0F) == 6)) {
			callTM(L, tm, t, key, val, 1);
			return;
		}
		t = tm;  
	}
	luaG_runerror(L, "loop in gettable");
}

void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val) {
	int loop;
	for (loop = 0; loop < 100; loop++) {
		const TValue *tm;
		if (((((t))->u.i.tt__) == (0x7FF7A500 | (((5) | (1 << 6)))))) {  
			Table *h = (&((t)->u.i.v__).gc->h);
			TValue *oldval = ((TValue *)(luaH_get(h, key)));

			if (!((((oldval))->u.i.tt__) == (0x7FF7A500 | (0))) ||

				((tm = ((h->metatable) == ((void *)0) ? ((void *)0) : ((h->metatable)->flags & (1u<<(TM_NEWINDEX))) ? ((void *)0) : luaT_gettm(h->metatable, TM_NEWINDEX, ((L->l_G))->tmname[TM_NEWINDEX]))) == ((void *)0) &&

				(oldval != (&luaO_nilobject_) ||

				(oldval = luaH_newkey(L, h, key), 1)))) {

					{ const TValue *o2_=(val); TValue *o1_=(oldval); o1_->u = o2_->u; ((void)0); };  
					((h)->flags = 0);
					{ if ((((((((val)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((val)->u.i.tt__) & 0xff) & (1 << 6)) && ((((((val)->u.i.v__).gc))->gch.marked) & (((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)(((((GCObject *)((h)))))))))->gch.marked) & ((1<<(2))))) luaC_barrierback_(L,(((GCObject *)((h))))); };
					return;
			}

		}
		else  
			if (((((tm = luaT_gettmbyobj(L, t, TM_NEWINDEX)))->u.i.tt__) == (0x7FF7A500 | (0))))
				luaG_typeerror(L, t, "index");

		if ((((((((tm)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((tm)->u.i.tt__) & 0xff) & 0x0F) == 6)) {
			callTM(L, tm, t, key, val, 0);
			return;
		}
		t = tm;  
	}
	luaG_runerror(L, "loop in settable");
}

static int call_binTM (lua_State *L, const TValue *p1, const TValue *p2,
					   StkId res, TMS event) {
						   const TValue *tm = luaT_gettmbyobj(L, p1, event);  
						   if (((((tm))->u.i.tt__) == (0x7FF7A500 | (0))))
							   tm = luaT_gettmbyobj(L, p2, event);  
						   if (((((tm))->u.i.tt__) == (0x7FF7A500 | (0)))) return 0;
						   callTM(L, tm, p1, p2, res, 1);
						   return 1;
}

static const TValue *get_equalTM (lua_State *L, Table *mt1, Table *mt2,
								  TMS event) {
									  const TValue *tm1 = ((mt1) == ((void *)0) ? ((void *)0) : ((mt1)->flags & (1u<<(event))) ? ((void *)0) : luaT_gettm(mt1, event, ((L->l_G))->tmname[event]));
									  const TValue *tm2;
									  if (tm1 == ((void *)0)) return ((void *)0);  
									  if (mt1 == mt2) return tm1;  
									  tm2 = ((mt2) == ((void *)0) ? ((void *)0) : ((mt2)->flags & (1u<<(event))) ? ((void *)0) : luaT_gettm(mt2, event, ((L->l_G))->tmname[event]));
									  if (tm2 == ((void *)0)) return ((void *)0);  
									  if (((((((tm1)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? ((((tm2)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) : (((tm1)->u.i.tt__) == ((tm2)->u.i.tt__))) && luaV_equalobj_(((void *)0),tm1,tm2)))  
										  return tm1;
									  return ((void *)0);
}

static int call_orderTM (lua_State *L, const TValue *p1, const TValue *p2,
						 TMS event) {
							 if (!call_binTM(L, p1, p2, L->top, event))
								 return -1;  
							 else
								 return !(((((L->top))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((L->top))->u.i.tt__) == (0x7FF7A500 | (1))) && (((L->top)->u.i.v__).b) == 0));
}

static int l_strcmp (const TString *ls, const TString *rs) {
	const char *l = ((const char *)((ls) + 1));
	size_t ll = ls->tsv.len;
	const char *r = ((const char *)((rs) + 1));
	size_t lr = rs->tsv.len;
	for (;;) {
		int temp = strcoll(l, r);
		if (temp != 0) return temp;
		else {  
			size_t len = strlen(l);  
			if (len == lr)  
				return (len == ll) ? 0 : 1;
			else if (len == ll)  
				return -1;  

			len++;
			l += len; ll -= len; r += len; lr -= len;
		}
	}
}

int luaV_lessthan (lua_State *L, const TValue *l, const TValue *r) {
	int res;
	if (((((l)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((r)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
		return (((((l)->u.d__)))<((((r)->u.d__))));
	else if (((((l))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) && ((((r))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))))
		return l_strcmp((&((l)->u.i.v__).gc->ts), (&((r)->u.i.v__).gc->ts)) < 0;
	else if ((res = call_orderTM(L, l, r, TM_LT)) < 0)
		luaG_ordererror(L, l, r);
	return res;
}

int luaV_lessequal (lua_State *L, const TValue *l, const TValue *r) {
	int res;
	if (((((l)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((r)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
		return (((((l)->u.d__)))<=((((r)->u.d__))));
	else if (((((l))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) && ((((r))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))))
		return l_strcmp((&((l)->u.i.v__).gc->ts), (&((r)->u.i.v__).gc->ts)) <= 0;
	else if ((res = call_orderTM(L, l, r, TM_LE)) >= 0)  
		return res;
	else if ((res = call_orderTM(L, r, l, TM_LT)) < 0)  
		luaG_ordererror(L, l, r);
	return !res;
}


int luaV_equalobj_ (lua_State *L, const TValue *t1, const TValue *t2) {
	const TValue *tm;
	((void)0);
	switch (((((((t1)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((t1)->u.i.tt__) & 0xff) & 0x3F)) {
	case 0: return 1;
	case 3: return (((((t1)->u.d__)))==((((t2)->u.d__))));
	case 1: return (((t1)->u.i.v__).b) == (((t2)->u.i.v__).b);  
	case 2: return (((t1)->u.i.v__).p) == (((t2)->u.i.v__).p);
	case (6 | (1 << 4)): return (((t1)->u.i.v__).f) == (((t2)->u.i.v__).f);
	case 4: return (((&((t1)->u.i.v__).gc->ts)) == ((&((t2)->u.i.v__).gc->ts)));
	case 7: {
		if ((&(&((t1)->u.i.v__).gc->u)->uv) == (&(&((t2)->u.i.v__).gc->u)->uv)) return 1;
		else if (L == ((void *)0)) return 0;
		tm = get_equalTM(L, (&(&((t1)->u.i.v__).gc->u)->uv)->metatable, (&(&((t2)->u.i.v__).gc->u)->uv)->metatable, TM_EQ);
		break;  
			}
	case 5: {
		if ((&((t1)->u.i.v__).gc->h) == (&((t2)->u.i.v__).gc->h)) return 1;
		else if (L == ((void *)0)) return 0;
		tm = get_equalTM(L, (&((t1)->u.i.v__).gc->h)->metatable, (&((t2)->u.i.v__).gc->h)->metatable, TM_EQ);
		break;  
			}
	default:
		((void)0);
		return (((t1)->u.i.v__).gc) == (((t2)->u.i.v__).gc);
	}
	if (tm == ((void *)0)) return 0;  
	callTM(L, tm, t1, t2, L->top, 1);  
	return !(((((L->top))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((L->top))->u.i.tt__) == (0x7FF7A500 | (1))) && (((L->top)->u.i.v__).b) == 0));
}

void luaV_concat (lua_State *L, int total) {
	((void)0);
	do {
		StkId top = L->top;
		int n = 2;  
		if (!(((((top-2))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) || ((((top-2)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) || !(((((top-1))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) || (luaV_tostring(L, top-1)))) {
			if (!call_binTM(L, top-2, top-1, top-2, TM_CONCAT))
				luaG_concaterror(L, top-2, top-1);
		}
		else if ((&(&((top-1)->u.i.v__).gc->ts)->tsv)->len == 0)  
			(void)(((((top - 2))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) || (luaV_tostring(L, top - 2)));  
		else if (((((top-2))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) && (&(&((top-2)->u.i.v__).gc->ts)->tsv)->len == 0) {
			{ TValue *io=(top-2); ((io)->u.i.v__).gc=((GCObject *)(((&((top-1)->u.i.v__).gc->ts)))); (((io)->u.i.tt__) = (0x7FF7A500 | (((4) | (1 << 6))))); ((void)0); };  
		}
		else {

			size_t tl = (&(&((top-1)->u.i.v__).gc->ts)->tsv)->len;
			char *buffer;
			int i;

			for (i = 1; i < total && (((((top-i-1))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) || (luaV_tostring(L, top-i-1))); i++) {
				size_t l = (&(&((top-i-1)->u.i.v__).gc->ts)->tsv)->len;
				if (l >= (((size_t)(~(size_t)0)-2)/sizeof(char)) - tl)
					luaG_runerror(L, "string length overflow");
				tl += l;
			}
			buffer = luaZ_openspace(L, &(L->l_G)->buff, tl);
			tl = 0;
			n = i;
			do {  
				size_t l = (&(&((top-i)->u.i.v__).gc->ts)->tsv)->len;
				memcpy(buffer+tl, ((const char *)(((&((top-i)->u.i.v__).gc->ts)) + 1)), l * sizeof(char));
				tl += l;
			} while (--i > 0);
			{ TValue *io=(top-n); ((io)->u.i.v__).gc=((GCObject *)((luaS_newlstr(L, buffer, tl)))); (((io)->u.i.tt__) = (0x7FF7A500 | (((4) | (1 << 6))))); ((void)0); };
		}
		total -= n-1;  
		L->top -= n-1;  
	} while (total > 1);  
}

void luaV_objlen (lua_State *L, StkId ra, const TValue *rb) {
	const TValue *tm;
	switch (((((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((rb)->u.i.tt__) & 0xff) & 0x0F)) {
	case 5: {
		Table *h = (&((rb)->u.i.v__).gc->h);
		tm = ((h->metatable) == ((void *)0) ? ((void *)0) : ((h->metatable)->flags & (1u<<(TM_LEN))) ? ((void *)0) : luaT_gettm(h->metatable, TM_LEN, ((L->l_G))->tmname[TM_LEN]));
		if (tm) break;  
		{ TValue *io_=(ra); ((io_)->u.d__)=(((lua_Number)((luaH_getn(h))))); ((void)0); };  
		return;
			}
	case 4: {
				{ TValue *io_=(ra); ((io_)->u.d__)=(((lua_Number)(((&(&((rb)->u.i.v__).gc->ts)->tsv)->len)))); ((void)0); };
				return;
			}
	default: {  
		tm = luaT_gettmbyobj(L, rb, TM_LEN);
		if (((((tm))->u.i.tt__) == (0x7FF7A500 | (0))))  
			luaG_typeerror(L, rb, "get length of");
		break;
			 }
	}
	callTM(L, tm, rb, rb, ra, 1);
}

void luaV_arith (lua_State *L, StkId ra, const TValue *rb,
				 const TValue *rc, TMS op) {
					 TValue tempb, tempc;
					 const TValue *b, *c;
					 if ((b = luaV_tonumber(rb, &tempb)) != ((void *)0) &&
						 (c = luaV_tonumber(rc, &tempc)) != ((void *)0)) {
							 lua_Number res = luaO_arith(op - TM_ADD + 0, (((b)->u.d__)), (((c)->u.d__)));
							 { TValue *io_=(ra); ((io_)->u.d__)=(res); ((void)0); };
					 }
					 else if (!call_binTM(L, rb, rc, ra, op))
						 luaG_aritherror(L, rb, rc);
}
static Closure *getcached (Proto *p, UpVal **encup, StkId base) {
	Closure *c = p->cache;
	if (c != ((void *)0)) {  
		int nup = p->sizeupvalues;
		Upvaldesc *uv = p->upvalues;
		int i;
		for (i = 0; i < nup; i++) {  
			TValue *v = uv[i].instack ? base + uv[i].idx : encup[uv[i].idx]->v;
			if (c->l.upvals[i]->v != v)
				return ((void *)0);  
		}
	}
	return c;  
}

static void pushclosure (lua_State *L, Proto *p, UpVal **encup, StkId base,
						 StkId ra) {
							 int nup = p->sizeupvalues;
							 Upvaldesc *uv = p->upvalues;
							 int i;
							 Closure *ncl = luaF_newLclosure(L, p);
							 { TValue *io=(ra); ((io)->u.i.v__).gc=((GCObject *)((ncl))); (((io)->u.i.tt__) = (0x7FF7A500 | ((((6 | (0 << 4))) | (1 << 6))))); ((void)0); };  
							 for (i = 0; i < nup; i++) {  
								 if (uv[i].instack)  
									 ncl->l.upvals[i] = luaF_findupval(L, base + uv[i].idx);
								 else  
									 ncl->l.upvals[i] = encup[uv[i].idx];
							 }
							 { if (((((((GCObject *)((p)))))->gch.marked) & ((1<<(2))))) luaC_barrierproto_(L,p,ncl); };
							 p->cache = ncl;  
}


void luaV_finishOp (lua_State *L) {
	CallInfo *ci = L->ci;
	StkId base = ci->u.l.base;
	Instruction inst = *(ci->u.l.savedpc - 1);  
	OpCode op = (((OpCode)(((inst)>>0) & ((~((~(Instruction)0)<<(6)))<<(0)))));
	switch (op) {  
	case OP_ADD: case OP_SUB: case OP_MUL: case OP_DIV:
	case OP_MOD: case OP_POW: case OP_UNM: case OP_LEN:
	case OP_GETTABUP: case OP_GETTABLE: case OP_SELF: {
						  { const TValue *o2_=(--L->top); TValue *o1_=(base + (((int)(((inst)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0)))))); o1_->u = o2_->u; ((void)0); };
						  break;
					  }
	case OP_LE: case OP_LT: case OP_EQ: {
		int res = !(((((L->top - 1))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((L->top - 1))->u.i.tt__) == (0x7FF7A500 | (1))) && (((L->top - 1)->u.i.v__).b) == 0));
		L->top--;

		((void)0);
		if (op == OP_LE &&  
			((((luaT_gettmbyobj(L, base + (((int)(((inst)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))), TM_LE)))->u.i.tt__) == (0x7FF7A500 | (0))))
			res = !res;  
		((void)0);
		if (res != (((int)(((inst)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0))))))  
			ci->u.l.savedpc++;  
		break;
				}
	case OP_CONCAT: {
		StkId top = L->top - 1;  
		int b = (((int)(((inst)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))));      
		int total = ((int)((top - 1 - (base + b))));  
		{ const TValue *o2_=(top); TValue *o1_=(top - 2); o1_->u = o2_->u; ((void)0); };  
		if (total > 1) {  
			L->top = top - 1;  
			luaV_concat(L, total);  
		}

		{ const TValue *o2_=(L->top - 1); TValue *o1_=(ci->u.l.base + (((int)(((inst)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0)))))); o1_->u = o2_->u; ((void)0); };
		L->top = ci->top;  
		break;
					}
	case OP_TFORCALL: {
		((void)0);
		L->top = ci->top;  
		break;
					  }
	case OP_CALL: {
		if ((((int)(((inst)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))) - 1 >= 0)  
			L->top = ci->top;  
		break;
				  }
	case OP_TAILCALL: case OP_SETTABUP:  case OP_SETTABLE:
		break;
	default: ((void)0);
	}
}

void luaV_execute (lua_State *L) 
{
	CallInfo *ci = L->ci;
	LClosure *cl;
	TValue *k;
	StkId base;

newframe:  
	((void)0);
	cl = (&((ci->func)->u.i.v__).gc->cl.l);
	k = cl->p->k;
	base = ci->u.l.base;

	for (;;)
	{
		Instruction i = *(ci->u.l.savedpc++);
		StkId ra;
		if ((L->hookmask & ((1 << 2) | (1 << 3))) &&
			(--L->hookcount == 0 || L->hookmask & (1 << 2)))
		{
			traceexec(L);
			base = ci->u.l.base; ;
		}

		ra = base+(int)
			(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0)));
		switch((OpCode)(((i)>>0) 
			& ((~((~(Instruction)0)<<(6)))<<(0)))) 
		{
		case OP_MOVE: 
			{
				const TValue *o2_=base+(int)
					(i>>(((0 + 6) + 8) + 9) & (~(~(Instruction)0)<<9)<<0); 
				TValue *o1_=(ra); o1_->u = o2_->u;
				;
			} break;
		case OP_LOADK: {TValue *rb = k + (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0))))); { const TValue *o2_=(rb); TValue *o1_=(ra); o1_->u = o2_->u; ((void)0); };} break;
		case OP_LOADKX: {TValue *rb; ((void)0); rb = k + (((int)(((*ci->u.l.savedpc++)>>(0 + 6)) & ((~((~(Instruction)0)<<((9 + 9 + 8))))<<(0))))); { const TValue *o2_=(rb); TValue *o1_=(ra); o1_->u = o2_->u; ((void)0); };} break;
		case OP_LOADBOOL: {{ TValue *io=(ra); ((io)->u.i.v__).b=((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); (((io)->u.i.tt__) = (0x7FF7A500 | (1))); }; if ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) ci->u.l.savedpc++;} break;
		case OP_LOADNIL: {int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))); do { (((ra++)->u.i.tt__) = (0x7FF7A500 | (0))); } while (b--);} break;
		case OP_GETUPVAL: {int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))); { const TValue *o2_=(cl->upvals[b]->v); TValue *o1_=(ra); o1_->u = o2_->u; ((void)0); };} break;
		case OP_GETTABUP: {int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))); { {luaV_gettable(L, cl->upvals[b]->v, ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))), ra);}; base = ci->u.l.base; };} break;
		case OP_GETTABLE: {{ {luaV_gettable(L, (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))), ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))), ra);}; base = ci->u.l.base; };} break;
		case OP_SETTABUP: {int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0))))); { {luaV_settable(L, cl->upvals[a]->v, ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))), ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))));}; base = ci->u.l.base; };} break;
		case OP_SETUPVAL: {UpVal *uv = cl->upvals[(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))]; { const TValue *o2_=(ra); TValue *o1_=(uv->v); o1_->u = o2_->u; ((void)0); }; { if ((((((((ra)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((ra)->u.i.tt__) & 0xff) & (1 << 6)) && ((((((ra)->u.i.v__).gc))->gch.marked) & (((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)((uv)))))->gch.marked) & ((1<<(2))))) luaC_barrier_(L,(((GCObject *)((uv)))),(((ra)->u.i.v__).gc)); };} break;
		case OP_SETTABLE: {{ {luaV_settable(L, ra, ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))), ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))));}; base = ci->u.l.base; };} break;
		case OP_NEWTABLE: {int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))); int c = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))); Table *t = luaH_new(L); { TValue *io=(ra); ((io)->u.i.v__).gc=((GCObject *)((t))); (((io)->u.i.tt__) = (0x7FF7A500 | (((5) | (1 << 6))))); ((void)0); }; if (b != 0 || c != 0) luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c)); { {{if ((L->l_G)->GCdebt > 0) {L->top = ra + 1; luaC_step(L); L->top = ci->top;;}; ((void)0);}; {((void) 0); ((void) 0);};;}; base = ci->u.l.base; }} break;
		case OP_SELF: {StkId rb = (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); { const TValue *o2_=(rb); TValue *o1_=(ra+1); o1_->u = o2_->u; ((void)0); }; { {luaV_gettable(L, rb, ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))), ra);}; base = ci->u.l.base; };} break;
		case OP_ADD: {{ TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); if (((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) { lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__)); { TValue *io_=(ra); ((io_)->u.d__)=(((nb)+(nc))); ((void)0); }; } else { { {luaV_arith(L, ra, rb, rc, TM_ADD);}; base = ci->u.l.base; }; } };} break;
		case OP_SUB: {{ TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); if (((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) { lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__)); { TValue *io_=(ra); ((io_)->u.d__)=(((nb)-(nc))); ((void)0); }; } else { { {luaV_arith(L, ra, rb, rc, TM_SUB);}; base = ci->u.l.base; }; } };} break;
		case OP_MUL: {{ TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); if (((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) { lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__)); { TValue *io_=(ra); ((io_)->u.d__)=(((nb)*(nc))); ((void)0); }; } else { { {luaV_arith(L, ra, rb, rc, TM_MUL);}; base = ci->u.l.base; }; } };} break;
		case OP_DIV: {{ TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); if (((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) { lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__)); { TValue *io_=(ra); ((io_)->u.d__)=(((nb)/(nc))); ((void)0); }; } else { { {luaV_arith(L, ra, rb, rc, TM_DIV);}; base = ci->u.l.base; }; } };} break;
		case OP_MOD: {{ TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); if (((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) { lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__)); { TValue *io_=(ra); ((io_)->u.d__)=(((nb) - floor((nb)/(nc))*(nc))); ((void)0); }; } else { { {luaV_arith(L, ra, rb, rc, TM_MOD);}; base = ci->u.l.base; }; } };} break;
		case OP_POW: {{ TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); if (((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) { lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__)); { TValue *io_=(ra); ((io_)->u.d__)=((pow(nb,nc))); ((void)0); }; } else { { {luaV_arith(L, ra, rb, rc, TM_POW);}; base = ci->u.l.base; }; } };} break;
		case OP_UNM: {TValue *rb = (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); if (((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) { lua_Number nb = (((rb)->u.d__)); { TValue *io_=(ra); ((io_)->u.d__)=((-(nb))); ((void)0); }; } else { { {luaV_arith(L, ra, rb, rb, TM_UNM);}; base = ci->u.l.base; }; }} break;
		case OP_NOT: {TValue *rb = (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); int res = (((((rb))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((rb))->u.i.tt__) == (0x7FF7A500 | (1))) && (((rb)->u.i.v__).b) == 0)); { TValue *io=(ra); ((io)->u.i.v__).b=(res); (((io)->u.i.tt__) = (0x7FF7A500 | (1))); };} break;
		case OP_LEN: {{ {luaV_objlen(L, ra, (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))));}; base = ci->u.l.base; };} break;
		case OP_CONCAT: {int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))); int c = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))); StkId rb; L->top = base + c + 1; { {luaV_concat(L, c - b + 1);}; base = ci->u.l.base; }; ra = (base+(((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0)))))); rb = b + base; { const TValue *o2_=(rb); TValue *o1_=(ra); o1_->u = o2_->u; ((void)0); }; { {{if ((L->l_G)->GCdebt > 0) {L->top = (ra >= rb ? ra + 1 : rb); luaC_step(L);;}; ((void)0);}; {((void) 0); ((void) 0);};;}; base = ci->u.l.base; } L->top = ci->top;} break;
		case OP_JMP: {{ int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0))))); if (a > 0) luaF_close(L, ci->u.l.base + a - 1); ci->u.l.savedpc += ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))-(((1<<(9 + 9))-1)>>1)) + 0; };} break;
		case OP_EQ: {TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); { {if (((int)((((((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) : (((rb)->u.i.tt__) == ((rc)->u.i.tt__))) && luaV_equalobj_(L, rb, rc))))) != (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0)))))) ci->u.l.savedpc++; else { i = *ci->u.l.savedpc; { int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0))))); if (a > 0) luaF_close(L, ci->u.l.base + a - 1); ci->u.l.savedpc += ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))-(((1<<(9 + 9))-1)>>1)) + 1; }; };;}; base = ci->u.l.base; }} break;
		case OP_LT: {{ {if (luaV_lessthan(L, ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))), ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))))) != (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0)))))) ci->u.l.savedpc++; else { i = *ci->u.l.savedpc; { int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0))))); if (a > 0) luaF_close(L, ci->u.l.base + a - 1); ci->u.l.savedpc += ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))-(((1<<(9 + 9))-1)>>1)) + 1; }; };;}; base = ci->u.l.base; }} break;
		case OP_LE: {{ {if (luaV_lessequal(L, ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))), ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0)))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))))) != (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0)))))) ci->u.l.savedpc++; else { i = *ci->u.l.savedpc; { int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0))))); if (a > 0) luaF_close(L, ci->u.l.base + a - 1); ci->u.l.savedpc += ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))-(((1<<(9 + 9))-1)>>1)) + 1; }; };;}; base = ci->u.l.base; }} break;
		case OP_TEST: {if ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))) ? (((((ra))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((ra))->u.i.tt__) == (0x7FF7A500 | (1))) && (((ra)->u.i.v__).b) == 0)) : !(((((ra))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((ra))->u.i.tt__) == (0x7FF7A500 | (1))) && (((ra)->u.i.v__).b) == 0))) ci->u.l.savedpc++; else { i = *ci->u.l.savedpc; { int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0))))); if (a > 0) luaF_close(L, ci->u.l.base + a - 1); ci->u.l.savedpc += ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))-(((1<<(9 + 9))-1)>>1)) + 1; }; };} break;
		case OP_TESTSET: {TValue *rb = (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0)))))); if ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))) ? (((((rb))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((rb))->u.i.tt__) == (0x7FF7A500 | (1))) && (((rb)->u.i.v__).b) == 0)) : !(((((rb))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((rb))->u.i.tt__) == (0x7FF7A500 | (1))) && (((rb)->u.i.v__).b) == 0))) ci->u.l.savedpc++; else { { const TValue *o2_=(rb); TValue *o1_=(ra); o1_->u = o2_->u; ((void)0); }; { i = *ci->u.l.savedpc; { int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0))))); if (a > 0) luaF_close(L, ci->u.l.base + a - 1); ci->u.l.savedpc += ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))-(((1<<(9 + 9))-1)>>1)) + 1; }; }; }} break;
		case OP_CALL: {int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))); int nresults = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))) - 1; if (b != 0) L->top = ra+b; if (luaD_precall(L, ra, nresults)) { if (nresults >= 0) L->top = ci->top; base = ci->u.l.base; } else { ci = L->ci; ci->callstatus |= (1<<2); goto newframe; }} break;
		case OP_TAILCALL: {int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))); if (b != 0) L->top = ra+b; ((void)0); if (luaD_precall(L, ra, (-1))) base = ci->u.l.base; else { CallInfo *nci = L->ci; CallInfo *oci = nci->previous; StkId nfunc = nci->func; StkId ofunc = oci->func; StkId lim = nci->u.l.base + ((&((nfunc)->u.i.v__).gc->cl.l)->p)->numparams; int aux; if (cl->p->sizep > 0) luaF_close(L, oci->u.l.base); for (aux = 0; nfunc + aux < lim; aux++) { const TValue *o2_=(nfunc + aux); TValue *o1_=(ofunc + aux); o1_->u = o2_->u; ((void)0); }; oci->u.l.base = ofunc + (nci->u.l.base - nfunc); oci->top = L->top = ofunc + (L->top - nfunc); oci->u.l.savedpc = nci->u.l.savedpc; oci->callstatus |= (1<<6); ci = L->ci = oci; ((void)0); goto newframe; }} break;
		case OP_RETURN: {int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))); if (b != 0) L->top = ra+b-1; if (cl->p->sizep > 0) luaF_close(L, base); b = luaD_poscall(L, ra); if (!(ci->callstatus & (1<<2))) return; else { ci = L->ci; if (b) L->top = ci->top; ((void)0); ((void)0); goto newframe; }}
		case OP_FORLOOP: {lua_Number step = (((ra+2)->u.d__)); lua_Number idx = (((((ra)->u.d__)))+(step)); lua_Number limit = (((ra+1)->u.d__)); if (((0)<(step)) ? ((idx)<=(limit)) : ((limit)<=(idx))) { ci->u.l.savedpc += ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))-(((1<<(9 + 9))-1)>>1)); { TValue *io_=(ra); ((io_)->u.d__)=(idx); ((void)0); }; { TValue *io_=(ra+3); ((io_)->u.d__)=(idx); ((void)0); }; }} break;
		case OP_FORPREP: {const TValue *init = ra; const TValue *plimit = ra+1; const TValue *pstep = ra+2; if (!(((((init)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) || (((init) = luaV_tonumber(init,ra)) != ((void *)0)))) luaG_runerror(L, "'" "for" "'" " initial value must be a number"); else if (!(((((plimit)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) || (((plimit) = luaV_tonumber(plimit,ra+1)) != ((void *)0)))) luaG_runerror(L, "'" "for" "'" " limit must be a number"); else if (!(((((pstep)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) || (((pstep) = luaV_tonumber(pstep,ra+2)) != ((void *)0)))) luaG_runerror(L, "'" "for" "'" " step must be a number"); { TValue *io_=(ra); ((io_)->u.d__)=((((((ra)->u.d__)))-((((pstep)->u.d__))))); ((void)0); }; ci->u.l.savedpc += ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))-(((1<<(9 + 9))-1)>>1));} break;
		case OP_TFORCALL: {StkId cb = ra + 3; { const TValue *o2_=(ra+2); TValue *o1_=(cb+2); o1_->u = o2_->u; ((void)0); }; { const TValue *o2_=(ra+1); TValue *o1_=(cb+1); o1_->u = o2_->u; ((void)0); }; { const TValue *o2_=(ra); TValue *o1_=(cb); o1_->u = o2_->u; ((void)0); }; L->top = cb + 3; { {luaD_call(L, cb, (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))), 1);}; base = ci->u.l.base; }; L->top = ci->top; i = *(ci->u.l.savedpc++); ra = (base+(((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0)))))); ((void)0); goto l_tforloop;}
		case OP_TFORLOOP: {l_tforloop: if (!((((ra + 1))->u.i.tt__) == (0x7FF7A500 | (0)))) { { const TValue *o2_=(ra + 1); TValue *o1_=(ra); o1_->u = o2_->u; ((void)0); }; ci->u.l.savedpc += ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))-(((1<<(9 + 9))-1)>>1)); }} break;
		case OP_SETLIST: {int n = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))); int c = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9)))<<(0))))); int last; Table *h; if (n == 0) n = ((int)((L->top - ra))) - 1; if (c == 0) { ((void)0); c = (((int)(((*ci->u.l.savedpc++)>>(0 + 6)) & ((~((~(Instruction)0)<<((9 + 9 + 8))))<<(0))))); } ; h = (&((ra)->u.i.v__).gc->h); last = ((c-1)*50) + n; if (last > h->sizearray) luaH_resizearray(L, h, last); for (; n > 0; n--) { TValue *val = ra+n; luaH_setint(L, h, last--, val); { if ((((((((val)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((val)->u.i.tt__) & 0xff) & (1 << 6)) && ((((((val)->u.i.v__).gc))->gch.marked) & (((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)(((((GCObject *)((h)))))))))->gch.marked) & ((1<<(2))))) luaC_barrierback_(L,(((GCObject *)((h))))); }; } L->top = ci->top;} break;
		case OP_CLOSURE: {Proto *p = cl->p->p[(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<((9 + 9))))<<(0)))))]; Closure *ncl = getcached(p, cl->upvals, base); if (ncl == ((void *)0)) pushclosure(L, p, cl->upvals, base, ra); else { TValue *io=(ra); ((io)->u.i.v__).gc=((GCObject *)((ncl))); (((io)->u.i.tt__) = (0x7FF7A500 | ((((6 | (0 << 4))) | (1 << 6))))); ((void)0); }; { {{if ((L->l_G)->GCdebt > 0) {L->top = ra + 1; luaC_step(L); L->top = ci->top;;}; ((void)0);}; {((void) 0); ((void) 0);};;}; base = ci->u.l.base; }} break;
		case OP_VARARG: {int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<(9)))<<(0))))) - 1; int j; int n = ((int)((base - ci->func))) - cl->p->numparams - 1; if (b < 0) { b = n; { {if (L->stack_last - L->top <= (n)) luaD_growstack(L, n); else ((void)0);;}; base = ci->u.l.base; }; ra = (base+(((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<(8)))<<(0)))))); L->top = ra + n; } for (j = 0; j < b; j++) { if (j < n) { { const TValue *o2_=(base - n + j); TValue *o1_=(ra + j); o1_->u = o2_->u; ((void)0); }; } else { (((ra + j)->u.i.tt__) = (0x7FF7A500 | (0))); } }} break;
		case OP_EXTRAARG: {((void)0);} break;
		}
	}
}
