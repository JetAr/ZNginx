#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

const TValue *luaV_tonumber(const TValue *obj, TValue *n)
{
	lua_Number num;
	if(((((obj)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) return obj;
	if(((((obj))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) && luaO_str2d(((const char *)(((&((obj)->u.i.v__).gc->ts)) + 1)), (&(&((obj)->u.i.v__).gc->ts)->tsv)->len, &num))
	{
		{
			TValue *io_ = (n);
			((io_)->u.d__) = (num);
			
		};
		return n;
	}
	else
		return ((void *)0);
}

int luaV_tostring(lua_State *L, StkId obj)
{
	if(!((((obj)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
		return 0;
	else
	{
		char s[32];
		lua_Number n = (((obj)->u.d__));
		int l = sprintf((s), "%.14g", (n));
		{
			TValue *io = (obj);
			((io)->u.i.v__).gc = ((GCObject *)((luaS_newlstr(L, s, l))));
			(((io)->u.i.tt__) = (0x7FF7A500 | (((4) | (1 << 6)))));
			
		};
		return 1;
	}
}

static void traceexec(lua_State *L)
{
	CallInfo *ci = L->ci;
	lu_byte mask = L->hookmask;
	if((mask & 0x1000) && L->hookcount == 0)
	{
		(L->hookcount = L->basehookcount);
		luaD_hook(L, 3, -1);
	}
	if(mask & 0x0100)
	{
		Proto *p = ((&(((ci)->func)->u.i.v__).gc->cl.l))->p;
		int npc = (((int)((ci->u.l.savedpc) - (p)->code)) - 1);
		int newline = (((p)->lineinfo) ? (p)->lineinfo[npc] : 0);
		if(npc == 0 ||
			ci->u.l.savedpc <= L->oldpc ||
			newline != (((p)->lineinfo) ? (p)->lineinfo[(((int)((L->oldpc) - (p)->code)) - 1)] : 0))
			luaD_hook(L, 2, newline);
	}
	L->oldpc = ci->u.l.savedpc;
	if(L->status == 1)
	{
		ci->u.l.savedpc--;
		luaD_throw(L, 1);
	}
}

static void callTM(lua_State *L, const TValue *f, const TValue *p1,
				   const TValue *p2, TValue *p3, int hasres)
{
	ptrdiff_t result = ((char *)(p3) - (char *)L->stack);
	{
		const TValue *o2_ = (f);
		TValue *o1_ = (L->top++);
		o1_->u = o2_->u;
		
	};
	{
		const TValue *o2_ = (p1);
		TValue *o1_ = (L->top++);
		o1_->u = o2_->u;
		
	};
	{
		const TValue *o2_ = (p2);
		TValue *o1_ = (L->top++);
		o1_->u = o2_->u;
		
	};
	if(!hasres)
	{
		const TValue *o2_ = (p3);
		TValue *o1_ = (L->top++);
		o1_->u = o2_->u;
		
	};
	if(L->stack_last - L->top <= (0)) luaD_growstack(L, 0);
	else;

	luaD_call(L, L->top - (4 - hasres), hasres, ((L->ci)->callstatus & (1 << 0)));
	if(hasres)
	{
		p3 = ((TValue *)((char *)L->stack + (result)));
		{
			const TValue *o2_ = (--L->top);
			TValue *o1_ = (p3);
			o1_->u = o2_->u;
			
		};
	}
}

void luaV_gettable(lua_State *L, const TValue *t, TValue *key, StkId val)
{
	int loop;
	for(loop = 0; loop < 100; loop++)
	{
		const TValue *tm;
		if(((((t))->u.i.tt__) == (0x7FF7A500 | (((5) | (1 << 6))))))
		{
			Table *h = (&((t)->u.i.v__).gc->h);
			const TValue *res = luaH_get(h, key);
			if(!((((res))->u.i.tt__) == (0x7FF7A500 | (0))) ||
				(tm = ((h->metatable) == ((void *)0) ? ((void *)0) : ((h->metatable)->flags & (1u << (TM_INDEX))) ? ((void *)0) : luaT_gettm(h->metatable, TM_INDEX, ((L->l_G))->tmname[TM_INDEX]))) == ((void *)0))
			{
				{
					const TValue *o2_ = (res);
					TValue *o1_ = (val);
					o1_->u = o2_->u;
					
				};
				return;
			}

		}
		else if(((((tm = luaT_gettmbyobj(L, t, TM_INDEX)))->u.i.tt__) == (0x7FF7A500 | (0))))
			luaG_typeerror(L, t, "index");
		if((((((((tm)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((tm)->u.i.tt__) & 0xff) & 0x0F) == 6))
		{
			callTM(L, tm, t, key, val, 1);
			return;
		}
		t = tm;
	}
	luaG_runerror(L, "loop in gettable");
}

void luaV_settable(lua_State *L, const TValue *t, TValue *key, StkId val)
{
	int loop;
	for(loop = 0; loop < 100; loop++)
	{
		const TValue *tm;
		if(((((t))->u.i.tt__) == (0x7FF7A500 | (((5) | (1 << 6))))))
		{
			Table *h = (&((t)->u.i.v__).gc->h);
			TValue *oldval = ((TValue *)(luaH_get(h, key)));

			if(!((((oldval))->u.i.tt__) == (0x7FF7A500 | (0))) ||

				((tm = ((h->metatable) == ((void *)0) ? ((void *)0) : ((h->metatable)->flags & (1u << (TM_NEWINDEX))) ? ((void *)0) : luaT_gettm(h->metatable, TM_NEWINDEX, ((L->l_G))->tmname[TM_NEWINDEX]))) == ((void *)0) &&

				(oldval != (&luaO_nilobject_) ||

				(oldval = luaH_newkey(L, h, key), 1))))
			{

				{
					const TValue *o2_ = (val);
					TValue *o1_ = (oldval);
					o1_->u = o2_->u;
					
				};
				((h)->flags = 0);
				{
					if((((((((val)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((val)->u.i.tt__) & 0xff) & (1 << 6)) && ((((((val)->u.i.v__).gc))->gch.marked) & (((1 ) | (1 << (1)))))) && ((((((GCObject *)(((((GCObject *)((h)))))))))->gch.marked) & ((1 << (2))))) luaC_barrierback_(L, (((GCObject *)((h)))));
				};
				return;
			}

		}
		else if(((((tm = luaT_gettmbyobj(L, t, TM_NEWINDEX)))->u.i.tt__) == (0x7FF7A500 | (0))))
			luaG_typeerror(L, t, "index");

		if((((((((tm)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((tm)->u.i.tt__) & 0xff) & 0x0F) == 6))
		{
			callTM(L, tm, t, key, val, 0);
			return;
		}
		t = tm;
	}
	luaG_runerror(L, "loop in settable");
}

static int call_binTM(lua_State *L, const TValue *p1, const TValue *p2,
					  StkId res, TMS event)
{
	const TValue *tm = luaT_gettmbyobj(L, p1, event);
	if(((((tm))->u.i.tt__) == (0x7FF7A500 | (0))))
		tm = luaT_gettmbyobj(L, p2, event);
	if(((((tm))->u.i.tt__) == (0x7FF7A500 | (0)))) return 0;
	callTM(L, tm, p1, p2, res, 1);
	return 1;
}

static const TValue *get_equalTM(lua_State *L, Table *mt1, Table *mt2,
								 TMS event)
{
	const TValue *tm1 = ((mt1) == ((void *)0) ? ((void *)0) : ((mt1)->flags & (1u << (event))) ? ((void *)0) : luaT_gettm(mt1, event, ((L->l_G))->tmname[event]));
	const TValue *tm2;
	if(tm1 == ((void *)0)) return ((void *)0);
	if(mt1 == mt2) return tm1;
	tm2 = ((mt2) == ((void *)0) ? ((void *)0) : ((mt2)->flags & (1u << (event))) ? ((void *)0) : luaT_gettm(mt2, event, ((L->l_G))->tmname[event]));
	if(tm2 == ((void *)0)) return ((void *)0);
	if(((((((tm1)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? ((((tm2)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) : (((tm1)->u.i.tt__) == ((tm2)->u.i.tt__))) && luaV_equalobj_(((void *)0), tm1, tm2)))
		return tm1;
	return ((void *)0);
}

static int call_orderTM(lua_State *L, const TValue *p1, const TValue *p2,
						TMS event)
{
	if(!call_binTM(L, p1, p2, L->top, event))
		return -1;
	else
		return !(((((L->top))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((L->top))->u.i.tt__) == (0x7FF7A500 | (1))) && (((L->top)->u.i.v__).b) == 0));
}

static int l_strcmp(const TString *ls, const TString *rs)
{
	const char *l = ((const char *)((ls) + 1));
	size_t ll = ls->tsv.len;
	const char *r = ((const char *)((rs) + 1));
	size_t lr = rs->tsv.len;
	for(;;)
	{
		int temp = strcoll(l, r);
		if(temp != 0) return temp;
		else
		{
			size_t len = strlen(l);
			if(len == lr)
				return (len == ll) ? 0 : 1;
			else if(len == ll)
				return -1;

			len++;
			l += len;
			ll -= len;
			r += len;
			lr -= len;
		}
	}
}

int luaV_lessthan(lua_State *L, const TValue *l, const TValue *r)
{
	int res;
	if(((((l)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((r)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
		return (((((l)->u.d__))) < ((((r)->u.d__))));
	else if(((((l))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) && ((((r))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))))
		return l_strcmp((&((l)->u.i.v__).gc->ts), (&((r)->u.i.v__).gc->ts)) < 0;
	else if((res = call_orderTM(L, l, r, TM_LT)) < 0)
		luaG_ordererror(L, l, r);
	return res;
}

int luaV_lessequal(lua_State *L, const TValue *l, const TValue *r)
{
	int res;
	if(((((l)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((r)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
		return (((((l)->u.d__))) <= ((((r)->u.d__))));
	else if(((((l))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) && ((((r))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))))
		return l_strcmp((&((l)->u.i.v__).gc->ts), (&((r)->u.i.v__).gc->ts)) <= 0;
	else if((res = call_orderTM(L, l, r, TM_LE)) >= 0)
		return res;
	else if((res = call_orderTM(L, r, l, TM_LT)) < 0)
		luaG_ordererror(L, l, r);
	return !res;
}


int luaV_equalobj_(lua_State *L, const TValue *t1, const TValue *t2)
{
	const TValue *tm;
	
	switch(((((((t1)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((t1)->u.i.tt__) & 0xff) & 0x3F))
	{
	case 0:
		return 1;
	case 3:
		return (((((t1)->u.d__))) == ((((t2)->u.d__))));
	case 1:
		return (((t1)->u.i.v__).b) == (((t2)->u.i.v__).b);
	case 2:
		return (((t1)->u.i.v__).p) == (((t2)->u.i.v__).p);
	case(6 | (1 << 4)):
		return (((t1)->u.i.v__).f) == (((t2)->u.i.v__).f);
	case 4:
		return (((&((t1)->u.i.v__).gc->ts)) == ((&((t2)->u.i.v__).gc->ts)));
	case 7:
		{
			if((&(&((t1)->u.i.v__).gc->u)->uv) == (&(&((t2)->u.i.v__).gc->u)->uv)) return 1;
			else if(L == ((void *)0)) return 0;
			tm = get_equalTM(L, (&(&((t1)->u.i.v__).gc->u)->uv)->metatable, (&(&((t2)->u.i.v__).gc->u)->uv)->metatable, TM_EQ);
			break;
		}
	case 5:
		{
			if((&((t1)->u.i.v__).gc->h) == (&((t2)->u.i.v__).gc->h)) return 1;
			else if(L == ((void *)0)) return 0;
			tm = get_equalTM(L, (&((t1)->u.i.v__).gc->h)->metatable, (&((t2)->u.i.v__).gc->h)->metatable, TM_EQ);
			break;
		}
	default:
		
		return (((t1)->u.i.v__).gc) == (((t2)->u.i.v__).gc);
	}
	if(tm == ((void *)0)) return 0;
	callTM(L, tm, t1, t2, L->top, 1);
	return !(((((L->top))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((L->top))->u.i.tt__) == (0x7FF7A500 | (1))) && (((L->top)->u.i.v__).b) == 0));
}

void luaV_concat(lua_State *L, int total)
{
	
	do
	{
		StkId top = L->top;
		int n = 2;
		if(!(((((top - 2))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) || ((((top - 2)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500)) || !(((((top - 1))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) || (luaV_tostring(L, top - 1))))
		{
			if(!call_binTM(L, top - 2, top - 1, top - 2, TM_CONCAT))
				luaG_concaterror(L, top - 2, top - 1);
		}
		else if((&(&((top - 1)->u.i.v__).gc->ts)->tsv)->len == 0)
			(void)(((((top - 2))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) || (luaV_tostring(L, top - 2)));
		else if(((((top - 2))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) && (&(&((top - 2)->u.i.v__).gc->ts)->tsv)->len == 0)
		{
			{
				TValue *io = (top - 2);
				((io)->u.i.v__).gc = ((GCObject *)(((&((top - 1)->u.i.v__).gc->ts))));
				(((io)->u.i.tt__) = (0x7FF7A500 | (((4) | (1 << 6)))));
				
			};
		}
		else
		{

			size_t tl = (&(&((top - 1)->u.i.v__).gc->ts)->tsv)->len;
			char *buffer;
			int i;

			for(i = 1; i < total && (((((top - i - 1))->u.i.tt__) == (0x7FF7A500 | (((4) | (1 << 6))))) || (luaV_tostring(L, top - i - 1))); i++)
			{
				size_t l = (&(&((top - i - 1)->u.i.v__).gc->ts)->tsv)->len;
				if(l >= (((size_t)(~(size_t)0) - 2) / sizeof(char)) - tl)
					luaG_runerror(L, "string length overflow");
				tl += l;
			}
			buffer = luaZ_openspace(L, &(L->l_G)->buff, tl);
			tl = 0;
			n = i;
			do
			{
				size_t l = (&(&((top - i)->u.i.v__).gc->ts)->tsv)->len;
				memcpy(buffer + tl, ((const char *)(((&((top - i)->u.i.v__).gc->ts)) + 1)), l * sizeof(char));
				tl += l;
			}
			while(--i > 0);
			{
				TValue *io = (top - n);
				((io)->u.i.v__).gc = ((GCObject *)((luaS_newlstr(L, buffer, tl))));
				(((io)->u.i.tt__) = (0x7FF7A500 | (((4) | (1 << 6)))));
				
			};
		}
		total -= n - 1;
		L->top -= n - 1;
	}
	while(total > 1);
}

void luaV_objlen(lua_State *L, StkId ra, const TValue *rb)
{
	const TValue *tm;
	switch(((((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((rb)->u.i.tt__) & 0xff) & 0x0F))
	{
	case 5:
		{
			Table *h = (&((rb)->u.i.v__).gc->h);
			tm = ((h->metatable) == ((void *)0) ? ((void *)0) : ((h->metatable)->flags & (1u << (TM_LEN))) ? ((void *)0) : luaT_gettm(h->metatable, TM_LEN, ((L->l_G))->tmname[TM_LEN]));
			if(tm) break;
			{
				TValue *io_ = (ra);
				((io_)->u.d__) = (((lua_Number)((luaH_getn(h)))));
				
			};
			return;
		}
	case 4:
		{
			{
				TValue *io_ = (ra);
				((io_)->u.d__) = (((lua_Number)(((&(&((rb)->u.i.v__).gc->ts)->tsv)->len))));
				
			};
			return;
		}
	default:
		{
			tm = luaT_gettmbyobj(L, rb, TM_LEN);
			if(((((tm))->u.i.tt__) == (0x7FF7A500 | (0))))
				luaG_typeerror(L, rb, "get length of");
			break;
		}
	}
	callTM(L, tm, rb, rb, ra, 1);
}

void luaV_arith(lua_State *L, StkId ra, const TValue *rb,
				const TValue *rc, TMS op)
{
	TValue tempb, tempc;
	const TValue *b, *c;
	if((b = luaV_tonumber(rb, &tempb)) != ((void *)0) &&
		(c = luaV_tonumber(rc, &tempc)) != ((void *)0))
	{
		lua_Number res = luaO_arith(op - TM_ADD + 0, (((b)->u.d__)), (((c)->u.d__)));
		{
			TValue *io_ = (ra);
			((io_)->u.d__) = (res);
			
		};
	}
	else if(!call_binTM(L, rb, rc, ra, op))
		luaG_aritherror(L, rb, rc);
}
static Closure *getcached(Proto *p, UpVal **encup, StkId base)
{
	Closure *c = p->cache;
	if(c != ((void *)0))
	{
		int nup = p->sizeupvalues;
		Upvaldesc *uv = p->upvalues;
		int i;
		for(i = 0; i < nup; i++)
		{
			TValue *v = uv[i].instack ? base + uv[i].idx : encup[uv[i].idx]->v;
			if(c->l.upvals[i]->v != v)
				return ((void *)0);
		}
	}
	return c;
}

static void pushclosure(lua_State *L, Proto *p, UpVal **encup, StkId base,
						StkId ra)
{
	int nup = p->sizeupvalues;
	Upvaldesc *uv = p->upvalues;
	int i;
	Closure *ncl = luaF_newLclosure(L, p);
	{
		TValue *io = (ra);
		((io)->u.i.v__).gc = ((GCObject *)((ncl)));
		(((io)->u.i.tt__) = (0x7FF7A500 | ((((6 | (0 << 4))) | (1 << 6)))));
		
	};
	for(i = 0; i < nup; i++)
	{
		if(uv[i].instack)
			ncl->l.upvals[i] = luaF_findupval(L, base + uv[i].idx);
		else
			ncl->l.upvals[i] = encup[uv[i].idx];
	}
	{
		if(((((((GCObject *)((p)))))->gch.marked) & ((1 << (2))))) luaC_barrierproto_(L, p, ncl);
	};
	p->cache = ncl;
}


void luaV_finishOp(lua_State *L)
{
	CallInfo *ci = L->ci;
	StkId base = ci->u.l.base;
	Instruction inst = *(ci->u.l.savedpc - 1);
	OpCode op = (((OpCode)(((inst) >> 0) & ((~((~(Instruction)0) << (6))) ))));
	switch(op)
	{
	case OP_ADD:
	case OP_SUB:
	case OP_MUL:
	case OP_DIV:
	case OP_MOD:
	case OP_POW:
	case OP_UNM:
	case OP_LEN:
	case OP_GETTABUP:
	case OP_GETTABLE:
	case OP_SELF:
		{
			{
				const TValue *o2_ = (--L->top);
				TValue *o1_ = (base + (((int)(((inst) >> (0 + 6)) & ((~((~(Instruction)0) << (8))) )))));
				o1_->u = o2_->u;
				
			};
			break;
		}
	case OP_LE:
	case OP_LT:
	case OP_EQ:
		{
			int res = !(((((L->top - 1))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((L->top - 1))->u.i.tt__) == (0x7FF7A500 | (1))) && (((L->top - 1)->u.i.v__).b) == 0));
			L->top--;

			
			if(op == OP_LE &&
				((((luaT_gettmbyobj(L, base + (((int)(((inst) >> 23) & ((~((~(Instruction)0) << (9))) )))), TM_LE)))->u.i.tt__) == (0x7FF7A500 | (0))))
				res = !res;
			
			if(res != (((int)(((inst) >> (0 + 6)) & ((~((~(Instruction)0) << (8))) )))))
				ci->u.l.savedpc++;
			break;
		}
	case OP_CONCAT:
		{
			StkId top = L->top - 1;
			int b = (((int)(((inst) >> 23) & ((~((~(Instruction)0) << (9))) ))));
			int total = ((int)((top - 1 - (base + b))));
			{
				const TValue *o2_ = (top);
				TValue *o1_ = (top - 2);
				o1_->u = o2_->u;
				
			};
			if(total > 1)
			{
				L->top = top - 1;
				luaV_concat(L, total);
			}

			{
				const TValue *o2_ = (L->top - 1);
				TValue *o1_ = (ci->u.l.base + (((int)(((inst) >> (0 + 6)) & ((~((~(Instruction)0) << (8))) )))));
				o1_->u = o2_->u;
				
			};
			L->top = ci->top;
			break;
		}
	case OP_TFORCALL:
		{
			
			L->top = ci->top;
			break;
		}
	case OP_CALL:
		{
			if((((int)(((inst) >> 14) & ((~((~(Instruction)0) << (9))) )))) - 1 >= 0)
				L->top = ci->top;
			break;
		}
	case OP_TAILCALL:
	case OP_SETTABUP:
	case OP_SETTABLE:
		break;
	default:
		break;
	}
}

void luaV_execute(lua_State *L)
{
	CallInfo *ci = L->ci;
	LClosure *cl;
	TValue *k;
	StkId base;
newframe:
	
	cl = (&((ci->func)->u.i.v__).gc->cl.l);
	k = cl->p->k;
	base = ci->u.l.base;

	for(;;)
	{
		Instruction i = *(ci->u.l.savedpc++);
		StkId ra;
		if((L->hookmask & 0x1100 ) &&
			(--L->hookcount == 0 || L->hookmask & 0x0100))
		{
			traceexec(L);
			base = ci->u.l.base; ;
		}

		{
			Instruction temp = ((~((~(Instruction)0) << (8))) );
			//temp=255;
			__nop();
		}

		ra = base + (int)
			((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ));
		switch((OpCode)((i >> 0)
			& ((~((~(Instruction)0) << (6))) )))
		{
		case OP_MOVE:
			{
				const TValue *o2_ = base + (int)
					(i >> 23 & (~(~(Instruction)0) << 9) << 0);
				TValue *o1_ = (ra);
				o1_->u = o2_->u;
				;
			}
			break;
		case OP_LOADK:
			{
				TValue *rb = k + (((int)((i >> 14) & ((~((~(Instruction)0) << 18)) ))));
				{
					const TValue *o2_ = (rb);
					TValue *o1_ = (ra);
					o1_->u = o2_->u;
					
				};
			}
			break;
		case OP_LOADKX:
			{
				TValue *rb;
				
				rb = k + (((int)(((*ci->u.l.savedpc++) >> (0 + 6)) & ((~((~(Instruction)0) << ((9 + 9 + 8)))) ))));
				{
					const TValue *o2_ = (rb);
					TValue *o1_ = (ra);
					o1_->u = o2_->u;
					
				};
			}
			break;
		case OP_LOADBOOL:
			{
				{
					TValue *io = (ra);
					((io)->u.i.v__).b = ((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
					(((io)->u.i.tt__) = (0x7FF7A500 | (1)));
				};
				if((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) ci->u.l.savedpc++;
			}
			break;
		case OP_LOADNIL:
			{
				int b = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))));
				do
				{
					(((ra++)->u.i.tt__) = (0x7FF7A500 | (0)));
				}
				while(b--);
			}
			break;
		case OP_GETUPVAL:
			{
				int b = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))));
				{
					const TValue *o2_ = (cl->upvals[b]->v);
					TValue *o1_ = (ra);
					o1_->u = o2_->u;
					
				};
			}
			break;
		case OP_GETTABUP:
			{
				int b = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))));
				{
					{
						luaV_gettable(L, cl->upvals[b]->v, ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))), ra);
					};
					base = ci->u.l.base;
				};
			}
			break;
		case OP_GETTABLE:
			{
				{
					{
						luaV_gettable(L, (base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))), ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))), ra);
					};
					base = ci->u.l.base;
				};
			}
			break;
		case OP_SETTABUP:
			{
				int a = (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))));
				{
					{
						luaV_settable(L, cl->upvals[a]->v, ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))), ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))));
					};
					base = ci->u.l.base;
				};
			}
			break;
		case OP_SETUPVAL:
			{
				UpVal *uv = cl->upvals[(((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))];
				{
					const TValue *o2_ = (ra);
					TValue *o1_ = (uv->v);
					o1_->u = o2_->u;
					
				};
				{
					if((((((((ra)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((ra)->u.i.tt__) & 0xff) & (1 << 6)) && ((((((ra)->u.i.v__).gc))->gch.marked) & (((1 ) | (1 << (1)))))) && ((((((GCObject *)((uv)))))->gch.marked) & ((1 << (2))))) luaC_barrier_(L, (((GCObject *)((uv)))), (((ra)->u.i.v__).gc));
				};
			}
			break;
		case OP_SETTABLE:
			{
				{
					{
						luaV_settable(L, ra, ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))), ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))));
					};
					base = ci->u.l.base;
				};
			}
			break;
		case OP_NEWTABLE:
			{
				int b = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))));
				int c = (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))));
				Table *t = luaH_new(L);
				{
					TValue *io = (ra);
					((io)->u.i.v__).gc = ((GCObject *)((t)));
					(((io)->u.i.tt__) = (0x7FF7A500 | (((5) | (1 << 6)))));
					
				};
				if(b != 0 || c != 0) luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
				{
					{
						{
							if((L->l_G)->GCdebt > 0)
							{
								L->top = ra + 1;
								luaC_step(L);
								L->top = ci->top;;
							};
							
						};
						{
							((void) 0);
							((void) 0);
						};;
					};
					base = ci->u.l.base;
				}
			}
			break;
		case OP_SELF:
			{
				StkId rb = (base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
				{
					const TValue *o2_ = (rb);
					TValue *o1_ = (ra + 1);
					o1_->u = o2_->u;
					
				};
				{
					{
						luaV_gettable(L, rb, ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))), ra);
					};
					base = ci->u.l.base;
				};
			}
			break;
		case OP_ADD:
			{
				{
					TValue *rb = ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
					TValue *rc = ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))));
					if(((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
					{
						lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__));
						{
							TValue *io_ = (ra);
							((io_)->u.d__) = (((nb) + (nc)));
							
						};
					}
					else
					{
						{
							{
								luaV_arith(L, ra, rb, rc, TM_ADD);
							};
							base = ci->u.l.base;
						};
					}
				};
			}
			break;
		case OP_SUB:
			{
				{
					TValue *rb = ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
					TValue *rc = ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))));
					if(((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
					{
						lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__));
						{
							TValue *io_ = (ra);
							((io_)->u.d__) = (((nb) - (nc)));
							
						};
					}
					else
					{
						{
							{
								luaV_arith(L, ra, rb, rc, TM_SUB);
							};
							base = ci->u.l.base;
						};
					}
				};
			}
			break;
		case OP_MUL:
			{
				{
					TValue *rb = ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
					TValue *rc = ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))));
					if(((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
					{
						lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__));
						{
							TValue *io_ = (ra);
							((io_)->u.d__) = (((nb) * (nc)));
							
						};
					}
					else
					{
						{
							{
								luaV_arith(L, ra, rb, rc, TM_MUL);
							};
							base = ci->u.l.base;
						};
					}
				};
			}
			break;
		case OP_DIV:
			{
				{
					TValue *rb = ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
					TValue *rc = ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))));
					if(((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
					{
						lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__));
						{
							TValue *io_ = (ra);
							((io_)->u.d__) = (((nb) / (nc)));
							
						};
					}
					else
					{
						{
							{
								luaV_arith(L, ra, rb, rc, TM_DIV);
							};
							base = ci->u.l.base;
						};
					}
				};
			}
			break;
		case OP_MOD:
			{
				{
					TValue *rb = ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
					TValue *rc = ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))));
					if(((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
					{
						lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__));
						{
							TValue *io_ = (ra);
							((io_)->u.d__) = (((nb) - floor((nb) / (nc)) * (nc)));
							
						};
					}
					else
					{
						{
							{
								luaV_arith(L, ra, rb, rc, TM_MOD);
							};
							base = ci->u.l.base;
						};
					}
				};
			}
			break;
		case OP_POW:
			{
				{
					TValue *rb = ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
					TValue *rc = ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))));
					if(((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) && ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
					{
						lua_Number nb = (((rb)->u.d__)), nc = (((rc)->u.d__));
						{
							TValue *io_ = (ra);
							((io_)->u.d__) = ((pow(nb, nc)));
							
						};
					}
					else
					{
						{
							{
								luaV_arith(L, ra, rb, rc, TM_POW);
							};
							base = ci->u.l.base;
						};
					}
				};
			}
			break;
		case OP_UNM:
			{
				TValue *rb = (base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
				if(((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500))
				{
					lua_Number nb = (((rb)->u.d__));
					{
						TValue *io_ = (ra);
						((io_)->u.d__) = ((-(nb)));
						
					};
				}
				else
				{
					{
						{
							luaV_arith(L, ra, rb, rb, TM_UNM);
						};
						base = ci->u.l.base;
					};
				}
			}
			break;
		case OP_NOT:
			{
				TValue *rb = (base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
				int res = (((((rb))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((rb))->u.i.tt__) == (0x7FF7A500 | (1))) && (((rb)->u.i.v__).b) == 0));
				{
					TValue *io = (ra);
					((io)->u.i.v__).b = (res);
					(((io)->u.i.tt__) = (0x7FF7A500 | (1)));
				};
			}
			break;
		case OP_LEN:
			{
				{
					{
						luaV_objlen(L, ra, (base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))));
					};
					base = ci->u.l.base;
				};
			}
			break;
		case OP_CONCAT:
			{
				int b = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))));
				int c = (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))));
				StkId rb;
				L->top = base + c + 1;
				{
					{
						luaV_concat(L, c - b + 1);
					};
					base = ci->u.l.base;
				};
				ra = (base + (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) )))));
				rb = b + base;
				{
					const TValue *o2_ = (rb);
					TValue *o1_ = (ra);
					o1_->u = o2_->u;
					
				};
				{
					{
						{
							if((L->l_G)->GCdebt > 0)
							{
								L->top = (ra >= rb ? ra + 1 : rb);
								luaC_step(L);;
							};
							
						};
						{
							((void) 0);
							((void) 0);
						};;
					};
					base = ci->u.l.base;
				}
				L->top = ci->top;
			}
			break;
		case OP_JMP:
			{
				{
					int a = (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))));
					if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
					ci->u.l.savedpc += ((((int)((i >> 14) & ((~((~(Instruction)0) << 18)) )))) - (((1 << 18) - 1) >> 1)) + 0;
				};
			}
			break;
		case OP_EQ:
			{
				TValue *rb = ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
				TValue *rc = ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))));
				{
					{
						if(((int)((((((((rb)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? ((((rc)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) : (((rb)->u.i.tt__) == ((rc)->u.i.tt__))) && luaV_equalobj_(L, rb, rc))))) != (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))))) ci->u.l.savedpc++;
						else
						{
							i = *ci->u.l.savedpc;
							{
								int a = (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))));
								if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
								ci->u.l.savedpc += ((((int)((i >> 14) & ((~((~(Instruction)0) << 18)) )))) - (((1 << 18) - 1) >> 1)) + 1;
							};
						};;
					};
					base = ci->u.l.base;
				}
			}
			break;
		case OP_LT:
			{
				{
					{
						if(luaV_lessthan(L, ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))), ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))))) != (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))))) ci->u.l.savedpc++;
						else
						{
							i = *ci->u.l.savedpc;
							{
								int a = (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))));
								if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
								ci->u.l.savedpc += ((((int)((i >> 14) & ((~((~(Instruction)0) << 18)) )))) - (((1 << 18) - 1) >> 1)) + 1;
							};
						};;
					};
					base = ci->u.l.base;
				}
			}
			break;
		case OP_LE:
			{
				{
					{
						if(luaV_lessequal(L, ((((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))))), ((((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & (1 << (9 - 1))) ? k + ((int)((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))))) & ~(1 << (9 - 1))) : base + (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))))) != (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))))) ci->u.l.savedpc++;
						else
						{
							i = *ci->u.l.savedpc;
							{
								int a = (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))));
								if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
								ci->u.l.savedpc += ((((int)((i >> 14) & ((~((~(Instruction)0) << 18)) )))) - (((1 << 18) - 1) >> 1)) + 1;
							};
						};;
					};
					base = ci->u.l.base;
				}
			}
			break;
		case OP_TEST:
			{
				if((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))) ? (((((ra))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((ra))->u.i.tt__) == (0x7FF7A500 | (1))) && (((ra)->u.i.v__).b) == 0)) : !(((((ra))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((ra))->u.i.tt__) == (0x7FF7A500 | (1))) && (((ra)->u.i.v__).b) == 0))) ci->u.l.savedpc++;
				else
				{
					i = *ci->u.l.savedpc;
					{
						int a = (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))));
						if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
						ci->u.l.savedpc += ((((int)((i >> 14) & ((~((~(Instruction)0) << 18)) )))) - (((1 << 18) - 1) >> 1)) + 1;
					};
				};
			}
			break;
		case OP_TESTSET:
			{
				TValue *rb = (base + (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))));
				if((((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))) ? (((((rb))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((rb))->u.i.tt__) == (0x7FF7A500 | (1))) && (((rb)->u.i.v__).b) == 0)) : !(((((rb))->u.i.tt__) == (0x7FF7A500 | (0))) || (((((rb))->u.i.tt__) == (0x7FF7A500 | (1))) && (((rb)->u.i.v__).b) == 0))) ci->u.l.savedpc++;
				else
				{
					{
						const TValue *o2_ = (rb);
						TValue *o1_ = (ra);
						o1_->u = o2_->u;
						
					};
					{
						i = *ci->u.l.savedpc;
						{
							int a = (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) ))));
							if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
							ci->u.l.savedpc += ((((int)((i >> 14) & ((~((~(Instruction)0) << 18)) )))) - (((1 << 18) - 1) >> 1)) + 1;
						};
					};
				}
			}
			break;
		case OP_CALL:
			{
				int b = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))));
				int nresults = (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))) - 1;
				if(b != 0) L->top = ra + b;
				if(luaD_precall(L, ra, nresults))
				{
					if(nresults >= 0) L->top = ci->top;
					base = ci->u.l.base;
				}
				else
				{
					ci = L->ci;
					ci->callstatus |= 0x0100;
					goto newframe;
				}
			}
			break;
		case OP_TAILCALL:
			{
				int b = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))));
				if(b != 0) L->top = ra + b;
				
				if(luaD_precall(L, ra, (-1))) base = ci->u.l.base;
				else
				{
					CallInfo *nci = L->ci;
					CallInfo *oci = nci->previous;
					StkId nfunc = nci->func;
					StkId ofunc = oci->func;
					StkId lim = nci->u.l.base + ((&((nfunc)->u.i.v__).gc->cl.l)->p)->numparams;
					int aux;
					if(cl->p->sizep > 0) luaF_close(L, oci->u.l.base);
					for(aux = 0; nfunc + aux < lim; aux++)
					{
						const TValue *o2_ = (nfunc + aux);
						TValue *o1_ = (ofunc + aux);
						o1_->u = o2_->u;
						
					};
					oci->u.l.base = ofunc + (nci->u.l.base - nfunc);
					oci->top = L->top = ofunc + (L->top - nfunc);
					oci->u.l.savedpc = nci->u.l.savedpc;
					oci->callstatus |= (1 << 6);
					ci = L->ci = oci;
					
					goto newframe;
				}
			}
			break;
		case OP_RETURN:
			{
				int b = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))));
				if(b != 0) L->top = ra + b - 1;
				if(cl->p->sizep > 0) luaF_close(L, base);
				b = luaD_poscall(L, ra);
				if(!(ci->callstatus & 0x0100)) return;
				else
				{
					ci = L->ci;
					if(b) L->top = ci->top;
					
					
					goto newframe;
				}
			}
		case OP_FORLOOP:
			{
				lua_Number step = (((ra + 2)->u.d__));
				lua_Number idx = (((((ra)->u.d__))) + (step));
				lua_Number limit = (((ra + 1)->u.d__));
				if(((0) < (step)) ? ((idx) <= (limit)) : ((limit) <= (idx)))
				{
					ci->u.l.savedpc += ((((int)((i >> 14) & ((~((~(Instruction)0) << 18)) )))) - (((1 << 18) - 1) >> 1));
					{
						TValue *io_ = (ra);
						((io_)->u.d__) = (idx);
						
					};
					{
						TValue *io_ = (ra + 3);
						((io_)->u.d__) = (idx);
						
					};
				}
			}
			break;
		case OP_FORPREP:
			{
				const TValue *init = ra;
				const TValue *plimit = ra + 1;
				const TValue *pstep = ra + 2;
				if(!(((((init)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) || (((init) = luaV_tonumber(init, ra)) != ((void *)0)))) luaG_runerror(L, "'" "for" "'" " initial value must be a number");
				else if(!(((((plimit)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) || (((plimit) = luaV_tonumber(plimit, ra + 1)) != ((void *)0)))) luaG_runerror(L, "'" "for" "'" " limit must be a number");
				else if(!(((((pstep)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) || (((pstep) = luaV_tonumber(pstep, ra + 2)) != ((void *)0)))) luaG_runerror(L, "'" "for" "'" " step must be a number");
				{
					TValue *io_ = (ra);
					((io_)->u.d__) = ((((((ra)->u.d__))) - ((((pstep)->u.d__)))));
					
				};
				ci->u.l.savedpc += ((((int)((i >> 14) & ((~((~(Instruction)0) << 18)) )))) - (((1 << 18) - 1) >> 1));
			}
			break;
		case OP_TFORCALL:
			{
				StkId cb = ra + 3;
				{
					const TValue *o2_ = (ra + 2);
					TValue *o1_ = (cb + 2);
					o1_->u = o2_->u;
					
				};
				{
					const TValue *o2_ = (ra + 1);
					TValue *o1_ = (cb + 1);
					o1_->u = o2_->u;
					
				};
				{
					const TValue *o2_ = (ra);
					TValue *o1_ = (cb);
					o1_->u = o2_->u;
					
				};
				L->top = cb + 3;
				{
					{
						luaD_call(L, cb, (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) )))), 1);
					};
					base = ci->u.l.base;
				};
				L->top = ci->top;
				i = *(ci->u.l.savedpc++);
				ra = (base + (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) )))));
				
				goto l_tforloop;
			}
		case OP_TFORLOOP:
			{
l_tforloop:
				if(!((((ra + 1))->u.i.tt__) == (0x7FF7A500 | (0))))
				{
					{
						const TValue *o2_ = (ra + 1);
						TValue *o1_ = (ra);
						o1_->u = o2_->u;
						
					};
					ci->u.l.savedpc += ((((int)((i >> 14) & ((~((~(Instruction)0) << 18)) )))) - (((1 << 18) - 1) >> 1));
				}
			}
			break;
		case OP_SETLIST:
			{
				int n = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) ))));
				int c = (((int)((i >> 14) & ((~((~(Instruction)0) << (9))) ))));
				int last;
				Table *h;
				if(n == 0) n = ((int)((L->top - ra))) - 1;
				if(c == 0)
				{
					
					c = (((int)(((*ci->u.l.savedpc++) >> (0 + 6)) & ((~((~(Instruction)0) << ((9 + 9 + 8)))) ))));
				} ;
				h = (&((ra)->u.i.v__).gc->h);
				last = ((c - 1) * 50) + n;
				if(last > h->sizearray) luaH_resizearray(L, h, last);
				for(; n > 0; n--)
				{
					TValue *val = ra + n;
					luaH_setint(L, h, last--, val);
					{
						if((((((((val)->u.i.tt__) & 0x7FFFFF00) != 0x7FF7A500) ? 3 : ((val)->u.i.tt__) & 0xff) & (1 << 6)) && ((((((val)->u.i.v__).gc))->gch.marked) & (((1 ) | (1 << (1)))))) && ((((((GCObject *)(((((GCObject *)((h)))))))))->gch.marked) & ((1 << (2))))) luaC_barrierback_(L, (((GCObject *)((h)))));
					};
				}
				L->top = ci->top;
			}
			break;
		case OP_CLOSURE:
			{
				Proto *p = cl->p->p[(((int)((i >> 14) & ((~((~(Instruction)0) << 18)) ))))];
				Closure *ncl = getcached(p, cl->upvals, base);
				if(ncl == ((void *)0)) pushclosure(L, p, cl->upvals, base, ra);
				else
				{
					TValue *io = (ra);
					((io)->u.i.v__).gc = ((GCObject *)((ncl)));
					(((io)->u.i.tt__) = (0x7FF7A500 | ((((6 | (0 << 4))) | (1 << 6)))));
					
				};
				{
					{
						{
							if((L->l_G)->GCdebt > 0)
							{
								L->top = ra + 1;
								luaC_step(L);
								L->top = ci->top;;
							};
							
						};
						{
							((void) 0);
							((void) 0);
						};;
					};
					base = ci->u.l.base;
				}
			}
			break;
		case OP_VARARG:
			{
				int b = (((int)((i >> 23) & ((~((~(Instruction)0) << (9))) )))) - 1;
				int j;
				int n = ((int)((base - ci->func))) - cl->p->numparams - 1;
				if(b < 0)
				{
					b = n;
					{
						{
							if(L->stack_last - L->top <= (n)) luaD_growstack(L, n);
							else;
						};
						base = ci->u.l.base;
					};
					ra = (base + (((int)((i >> (0 + 6)) & ((~((~(Instruction)0) << (8))) )))));
					L->top = ra + n;
				}
				for(j = 0; j < b; j++)
				{
					if(j < n)
					{
						{
							const TValue *o2_ = (base - n + j);
							TValue *o1_ = (ra + j);
							o1_->u = o2_->u;
							
						};
					}
					else
					{
						(((ra + j)->u.i.tt__) = (0x7FF7A500 | (0)));
					}
				}
			}
			break;
		case OP_EXTRAARG:
			{
				
			}
			break;
		}
	}
}
