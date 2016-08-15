const TValue *luaV_tonumber(const TValue *obj, TValue *n)
{
	lua_Number num;
	if(ttisnumber(obj)) return obj;
	if(ttisstring(obj) && luaO_str2d(svalue(obj), tsvalue(obj)->len, &num))
	{
		setnvalue(n, num);
		return n;
	}
	else
		return ((void *)0);
}


int luaV_tostring(lua_State *L, StkId obj)
{
	if(!ttisnumber(obj))
		return 0;
	else
	{
		char s[LUAI_MAXNUMBER2STR];
		lua_Number n = nvalue(obj);
		int l = lua_number2str(s, n);
		setsvalue2s(L, obj, luaS_newlstr(L, s, l));
		return 1;
	}
}


static void traceexec(lua_State *L)
{
	CallInfo *ci = L->ci;
	lu_byte mask = L->hookmask;
	if((mask & LUA_MASKCOUNT) && L->hookcount == 0)
	{
		resethookcount(L);
		luaD_hook(L, LUA_HOOKCOUNT, -1);
	}
	if(mask & LUA_MASKLINE)
	{
		Proto *p = ci_func(ci)->p;
		int npc = pcRel(ci->u.l.savedpc, p);
		int newline = getfuncline(p, npc);
		if(npc == 0 ||
			ci->u.l.savedpc <= L->oldpc ||
			newline != getfuncline(p, pcRel(L->oldpc, p)))
			luaD_hook(L, LUA_HOOKLINE, newline);
	}
	L->oldpc = ci->u.l.savedpc;
	if(L->status == LUA_YIELD)
	{
		ci->u.l.savedpc--;
		luaD_throw(L, LUA_YIELD);
	}
}


static void callTM(lua_State *L, const TValue *f, const TValue *p1,
				   const TValue *p2, TValue *p3, int hasres)
{
	ptrdiff_t result = savestack(L, p3);
	setobj2s(L, L->top++, f);
	setobj2s(L, L->top++, p1);
	setobj2s(L, L->top++, p2);
	if(!hasres)
		setobj2s(L, L->top++, p3);
	luaD_checkstack(L, 0);

	luaD_call(L, L->top - (4 - hasres), hasres, isLua(L->ci));
	if(hasres)
	{
		p3 = restorestack(L, result);
		setobjs2s(L, p3, --L->top);
	}
}


void luaV_gettable(lua_State *L, const TValue *t, TValue *key, StkId val)
{
	int loop;
	for(loop = 0; loop < 100; loop++)
	{
		const TValue *tm;
		if(ttistable(t))
		{
			Table *h = hvalue(t);
			const TValue *res = luaH_get(h, key);
			if(!ttisnil(res) ||
				(tm = fasttm(L, h->metatable, TM_INDEX)) == ((void *)0))
			{
				setobj2s(L, val, res);
				return;
			}
		}
		else if(ttisnil(tm = luaT_gettmbyobj(L, t, TM_INDEX)))
			luaG_typeerror(L, t, "index");
		if(ttisfunction(tm))
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
		if(ttistable(t))
		{
			Table *h = hvalue(t);
			TValue *oldval = cast(TValue *, luaH_get(h, key));


			if(!ttisnil(oldval) ||

				((tm = fasttm(L, h->metatable, TM_NEWINDEX)) == ((void *)0) &&

				(oldval != luaO_nilobject ||


				(oldval = luaH_newkey(L, h, key), 1))))
			{

				setobj2t(L, oldval, val);
				invalidateTMcache(h);
				luaC_barrierback(L, obj2gco(h), val);
				return;
			}
		}
		else if(ttisnil(tm = luaT_gettmbyobj(L, t, TM_NEWINDEX)))
			luaG_typeerror(L, t, "index");

		if(ttisfunction(tm))
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
	if(ttisnil(tm))
		tm = luaT_gettmbyobj(L, p2, event);
	if(ttisnil(tm)) return 0;
	callTM(L, tm, p1, p2, res, 1);
	return 1;
}


static const TValue *get_equalTM(lua_State *L, Table *mt1, Table *mt2,
								 TMS event)
{
	const TValue *tm1 = fasttm(L, mt1, event);
	const TValue *tm2;
	if(tm1 == ((void *)0)) return ((void *)0);
	if(mt1 == mt2) return tm1;
	tm2 = fasttm(L, mt2, event);
	if(tm2 == ((void *)0)) return ((void *)0);
	if(luaV_rawequalobj(tm1, tm2))
		return tm1;
	return ((void *)0);
}


static int call_orderTM(lua_State *L, const TValue *p1, const TValue *p2,
						TMS event)
{
	if(!call_binTM(L, p1, p2, L->top, event))
		return -1;
	else
		return !l_isfalse(L->top);
}


static int l_strcmp(const TString *ls, const TString *rs)
{
	const char *l = getstr(ls);
	size_t ll = ls->tsv.len;
	const char *r = getstr(rs);
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
	if(ttisnumber(l) && ttisnumber(r))
		return luai_numlt(L, nvalue(l), nvalue(r));
	else if(ttisstring(l) && ttisstring(r))
		return l_strcmp(rawtsvalue(l), rawtsvalue(r)) < 0;
	else if((res = call_orderTM(L, l, r, TM_LT)) < 0)
		luaG_ordererror(L, l, r);
	return res;
}


int luaV_lessequal(lua_State *L, const TValue *l, const TValue *r)
{
	int res;
	if(ttisnumber(l) && ttisnumber(r))
		return luai_numle(L, nvalue(l), nvalue(r));
	else if(ttisstring(l) && ttisstring(r))
		return l_strcmp(rawtsvalue(l), rawtsvalue(r)) <= 0;
	else if((res = call_orderTM(L, l, r, TM_LE)) >= 0)
		return res;
	else if((res = call_orderTM(L, r, l, TM_LT)) < 0)
		luaG_ordererror(L, l, r);
	return !res;
}





int luaV_equalobj_(lua_State *L, const TValue *t1, const TValue *t2)
{
	const TValue *tm;
	switch(ttype(t1))
	{
	case LUA_TNIL:
		return 1;
	case LUA_TNUMBER:
		return luai_numeq(nvalue(t1), nvalue(t2));
	case LUA_TBOOLEAN:
		return bvalue(t1) == bvalue(t2);
	case LUA_TLIGHTUSERDATA:
		return pvalue(t1) == pvalue(t2);
	case LUA_TLCF:
		return fvalue(t1) == fvalue(t2);
	case LUA_TSTRING:
		return eqstr(rawtsvalue(t1), rawtsvalue(t2));
	case LUA_TUSERDATA:
		{
			if(uvalue(t1) == uvalue(t2)) return 1;
			else if(L == ((void *)0)) return 0;
			tm = get_equalTM(L, uvalue(t1)->metatable, uvalue(t2)->metatable, TM_EQ);
			break;
		}
	case LUA_TTABLE:
		{
			if(hvalue(t1) == hvalue(t2)) return 1;
			else if(L == ((void *)0)) return 0;
			tm = get_equalTM(L, hvalue(t1)->metatable, hvalue(t2)->metatable, TM_EQ);
			break;
		}
	default:
		return gcvalue(t1) == gcvalue(t2);
	}
	if(tm == ((void *)0)) return 0;
	callTM(L, tm, t1, t2, L->top, 1);
	return !l_isfalse(L->top);
}


void luaV_concat(lua_State *L, int total)
{
	lua_assert(total >= 2);
	do
	{
		StkId top = L->top;
		int n = 2;
		if(!(ttisstring(top - 2) || ttisnumber(top - 2)) || !tostring(L, top - 1))
		{
			if(!call_binTM(L, top - 2, top - 1, top - 2, TM_CONCAT))
				luaG_concaterror(L, top - 2, top - 1);
		}
		else if(tsvalue(top - 1)->len == 0)
			(void)tostring(L, top - 2);
		else if(ttisstring(top - 2) && tsvalue(top - 2)->len == 0)
		{
			setsvalue2s(L, top - 2, rawtsvalue(top - 1));
		}
		else
		{

			size_t tl = tsvalue(top - 1)->len;
			char *buffer;
			int i;

			for(i = 1; i < total && tostring(L, top - i - 1); i++)
			{
				size_t l = tsvalue(top - i - 1)->len;
				if(l >= (MAX_SIZET / sizeof(char)) - tl)
					luaG_runerror(L, "string length overflow");
				tl += l;
			}
			buffer = luaZ_openspace(L, &G(L)->buff, tl);
			tl = 0;
			n = i;
			do
			{
				size_t l = tsvalue(top - i)->len;
				memcpy(buffer + tl, svalue(top - i), l * sizeof(char));
				tl += l;
			}
			while(--i > 0);
			setsvalue2s(L, top - n, luaS_newlstr(L, buffer, tl));
		}
		total -= n - 1;
		L->top -= n - 1;
	}
	while(total > 1);
}


void luaV_objlen(lua_State *L, StkId ra, const TValue *rb)
{
	const TValue *tm;
	switch(ttypenv(rb))
	{
	case LUA_TTABLE:
		{
			Table *h = hvalue(rb);
			tm = fasttm(L, h->metatable, TM_LEN);
			if(tm) break;
			setnvalue(ra, cast_num(luaH_getn(h)));
			return;
		}
	case LUA_TSTRING:
		{
			setnvalue(ra, cast_num(tsvalue(rb)->len));
			return;
		}
	default:
		{
			tm = luaT_gettmbyobj(L, rb, TM_LEN);
			if(ttisnil(tm))
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
		lua_Number res = luaO_arith(op - TM_ADD + LUA_OPADD, nvalue(b), nvalue(c));
		setnvalue(ra, res);
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
	setclLvalue(L, ra, ncl);
	for(i = 0; i < nup; i++)
	{
		if(uv[i].instack)
			ncl->l.upvals[i] = luaF_findupval(L, base + uv[i].idx);
		else
			ncl->l.upvals[i] = encup[uv[i].idx];
	}
	luaC_barrierproto(L, p, ncl);
	p->cache = ncl;
}





void luaV_finishOp(lua_State *L)
{
	CallInfo *ci = L->ci;
	StkId base = ci->u.l.base;
	Instruction inst = *(ci->u.l.savedpc - 1);
	OpCode op = GET_OPCODE(inst);
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
			setobjs2s(L, base + GETARG_A(inst), --L->top);
			break;
		}
	case OP_LE:
	case OP_LT:
	case OP_EQ:
		{
			int res = !l_isfalse(L->top - 1);
			L->top--;

			lua_assert(!ISK(GETARG_B(inst)));
			if(op == OP_LE &&
				ttisnil(luaT_gettmbyobj(L, base + GETARG_B(inst), TM_LE)))
				res = !res;
			lua_assert(GET_OPCODE(*ci->u.l.savedpc) == OP_JMP);
			if(res != GETARG_A(inst))
				ci->u.l.savedpc++;
			break;
		}
	case OP_CONCAT:
		{
			StkId top = L->top - 1;
			int b = GETARG_B(inst);
			int total = cast_int(top - 1 - (base + b));
			setobj2s(L, top - 2, top);
			if(total > 1)
			{
				L->top = top - 1;
				luaV_concat(L, total);
			}
			setobj2s(L, ci->u.l.base + GETARG_A(inst), L->top - 1);
			L->top = ci->top;
			break;
		}
	case OP_TFORCALL:
		{
			lua_assert(GET_OPCODE(*ci->u.l.savedpc) == OP_TFORLOOP);
			L->top = ci->top;
			break;
		}
	case OP_CALL:
		{
			if(GETARG_C(inst) - 1 >= 0)
				L->top = ci->top;
			break;
		}
	case OP_TAILCALL:
	case OP_SETTABUP:
	case OP_SETTABLE:
		break;
	default:
		lua_assert(0);
	}
}


void luaV_execute(lua_State *L)
{
	CallInfo *ci = L->ci;
	LClosure *cl;
	TValue *k;
	StkId base;
newframe:
	cl = clLvalue(ci->func);
	k = cl->p->k;
	base = ci->u.l.base;

	for(;;)
	{
		Instruction i = *(ci->u.l.savedpc++);
		StkId ra;
		if((L->hookmask & (LUA_MASKLINE | LUA_MASKCOUNT)) &&
			(--L->hookcount == 0 || L->hookmask & LUA_MASKLINE))
		{
			traceexec(L);
			base = ci->u.l.base;
		}
		ra = (base + GETARG_A(i));

		switch(GET_OPCODE(i))
		{
		case OP_MOVE:
			{
				setobjs2s(L, ra, check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i)));
			}
			break;

		case OP_LOADK:
			{
				TValue *rb = k + GETARG_Bx(i);
				setobj2s(L, ra, rb);
			}
			break;
		case OP_LOADKX:
			{
				TValue *rb;
				lua_assert(GET_OPCODE(*ci->u.l.savedpc) == OP_EXTRAARG);
				rb = k + GETARG_Ax(*ci->u.l.savedpc++);
				setobj2s(L, ra, rb);
			}
			break;

		case OP_LOADBOOL:
			{
				setbvalue(ra, GETARG_B(i));
				if(GETARG_C(i)) ci->u.l.savedpc++;
			}
			break;

		case OP_LOADNIL:
			{
				int b = GETARG_B(i);
				do
				{
					setnilvalue(ra++);
				}
				while(b--);
			}
			break;

		case OP_GETUPVAL:
			{
				int b = GETARG_B(i);
				setobj2s(L, ra, cl->upvals[b]->v);
			}
			break;

		case OP_GETTABUP:
			{
				int b = GETARG_B(i);
				luaV_gettable(L, cl->upvals[b]->v, check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i)), ra);
				base = ci->u.l.base;
			}
			break;
		case OP_GETTABLE:
			{

				luaV_gettable(L, check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i)), check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i)), ra);
				base = ci->u.l.base;
			}
			break;

		case OP_SETTABUP:
			{
				int a = GETARG_A(i);
				luaV_settable(L, cl->upvals[a]->v, check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i)), check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i)));
				base = ci->u.l.base;
			}
			break;

		case OP_SETUPVAL:
			{
				UpVal *uv = cl->upvals[GETARG_B(i)];
				setobj(L, uv->v, ra);
				luaC_barrier(L, uv, ra);
			}
			break;

		case OP_SETTABLE:
			{
				luaV_settable(L, ra, check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i)), check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i)));
				base = ci->u.l.base;
			}
			break;

		case OP_NEWTABLE:
			{
				int b = GETARG_B(i);
				int c = GETARG_C(i);
				Table *t = luaH_new(L);
				sethvalue(L, ra, t);
				if(b != 0 || c != 0) luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
				{
					{
						luaC_condGC(L, L->top = ra + 1; luaC_step(L); L->top = ci->top;);
						luai_threadyield(L);;
					};
					base = ci->u.l.base;
				}
			}
			break;

		case OP_SELF:
			{
				StkId rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i));
				setobjs2s(L, ra + 1, rb);
				luaV_gettable(L, rb, check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i)), ra);
				base = ci->u.l.base;
			}
			break;

		case OP_ADD:
			{
				{
					TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i));
					TValue *rc = check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i));
					if(ttisnumber(rb) && ttisnumber(rc))
					{
						lua_Number nb = nvalue(rb), nc = nvalue(rc);
						setnvalue(ra, luai_numadd(L, nb, nc));
					}
					else
					{
						luaV_arith(L, ra, rb, rc, TM_ADD);
						base = ci->u.l.base;
					}
				};
			}
			break;

		case OP_SUB:
			{
				{
					TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i));
					TValue *rc = check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i));
					if(ttisnumber(rb) && ttisnumber(rc))
					{
						lua_Number nb = nvalue(rb), nc = nvalue(rc);
						setnvalue(ra, luai_numsub(L, nb, nc));
					}
					else
					{
						luaV_arith(L, ra, rb, rc, TM_SUB);
						base = ci->u.l.base;
					}
				};
			}
			break;

		case OP_MUL:
			{
				{
					TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i));
					TValue *rc = check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i));
					if(ttisnumber(rb) && ttisnumber(rc))
					{
						lua_Number nb = nvalue(rb), nc = nvalue(rc);
						setnvalue(ra, luai_nummul(L, nb, nc));
					}
					else
					{
						luaV_arith(L, ra, rb, rc, TM_MUL);
						base = ci->u.l.base;
					}
				};
			}
			break;

		case OP_DIV:
			{
				{
					TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i));
					TValue *rc = check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i));
					if(ttisnumber(rb) && ttisnumber(rc))
					{
						lua_Number nb = nvalue(rb), nc = nvalue(rc);
						setnvalue(ra, luai_numdiv(L, nb, nc));
					}
					else
					{
						luaV_arith(L, ra, rb, rc, TM_DIV);
						base = ci->u.l.base;
					}
				};
			}
			break;

		case OP_MOD:
			{
				{
					TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i));
					TValue *rc = check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i));
					if(ttisnumber(rb) && ttisnumber(rc))
					{
						lua_Number nb = nvalue(rb), nc = nvalue(rc);
						setnvalue(ra, luai_nummod(L, nb, nc));
					}
					else
					{
						luaV_arith(L, ra, rb, rc, TM_MOD);
						base = ci->u.l.base;
					}
				};
			}
			break;

		case OP_POW:
			{
				{
					TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i));
					TValue *rc = check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i));
					if(ttisnumber(rb) && ttisnumber(rc))
					{
						lua_Number nb = nvalue(rb), nc = nvalue(rc);
						setnvalue(ra, luai_numpow(L, nb, nc));
					}
					else
					{
						luaV_arith(L, ra, rb, rc, TM_POW);
						base = ci->u.l.base;
					}
				};
			}
			break;

		case OP_UNM:
			{
				TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i));
				if(ttisnumber(rb))
				{
					lua_Number nb = nvalue(rb);
					setnvalue(ra, luai_numunm(L, nb));
				}
				else
				{
					luaV_arith(L, ra, rb, rb, TM_UNM);
					base = ci->u.l.base;
				};
			}
			break;

		case OP_NOT:
			{
				TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i));
				int res = l_isfalse(rb);
				setbvalue(ra, res);
			}
			break;

		case OP_LEN:
			{
				luaV_objlen(L, ra, check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i)));
				base = ci->u.l.base;
			}
			break;

		case OP_CONCAT:
			{
				int b = GETARG_B(i);
				int c = GETARG_C(i);
				StkId rb;
				L->top = base + c + 1;
				luaV_concat(L, c - b + 1);
				base = ci->u.l.base;
				ra = (base + GETARG_A(i));
				rb = b + base;
				setobjs2s(L, ra, rb);
				luaC_condGC(L, L->top = (ra >= rb ? ra + 1 : rb); luaC_step(L););
				luai_threadyield(L);;
				base = ci->u.l.base;
				L->top = ci->top;
			}
			break;

		case OP_JMP:
			{
				int a = GETARG_A(i);
				if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
				ci->u.l.savedpc += GETARG_sBx(i) + 0;
			}
			break;

		case OP_EQ:
			{
				TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i));
				TValue *rc = check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i));
				if(cast_int(equalobj(L, rb, rc)) != GETARG_A(i)) ci->u.l.savedpc++;
				else
				{
					i = *ci->u.l.savedpc;
					{
						int a = GETARG_A(i);
						if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
						ci->u.l.savedpc += GETARG_sBx(i) + 1;
					};
				};;
				base = ci->u.l.base;
			}
			break;

		case OP_LT:
			{
				if(luaV_lessthan(L, check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i)), check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i))) != GETARG_A(i)) ci->u.l.savedpc++;
				else
				{
					i = *ci->u.l.savedpc;
					int a = GETARG_A(i);
					if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
					ci->u.l.savedpc += GETARG_sBx(i) + 1;
				}
				base = ci->u.l.base;
			}
			break;

		case OP_LE:
			{
				if(luaV_lessequal(L, check_exp(getBMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i)), check_exp(getCMode(GET_OPCODE(i)) == OpArgK, ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i))) != GETARG_A(i)) ci->u.l.savedpc++;
				else
				{
					i = *ci->u.l.savedpc;
					int a = GETARG_A(i);
					if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
					ci->u.l.savedpc += GETARG_sBx(i) + 1;
				};
				base = ci->u.l.base;
			}
			break;

		case OP_TEST:
			{
				if(GETARG_C(i) ? l_isfalse(ra) : !l_isfalse(ra)) ci->u.l.savedpc++;
				else
				{
					i = *ci->u.l.savedpc;
					int a = GETARG_A(i);
					if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
					ci->u.l.savedpc += GETARG_sBx(i) + 1;
				};
			}
			break;

		case OP_TESTSET:
			{
				TValue *rb = check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i));
				if(GETARG_C(i) ? l_isfalse(rb) : !l_isfalse(rb)) ci->u.l.savedpc++;
				else
				{
					setobjs2s(L, ra, rb);
					i = *ci->u.l.savedpc;
					int a = GETARG_A(i);
					if(a > 0) luaF_close(L, ci->u.l.base + a - 1);
					ci->u.l.savedpc += GETARG_sBx(i) + 1;
				}
			}
			break;

		case OP_CALL:
			{
				int b = GETARG_B(i);
				int nresults = GETARG_C(i) - 1;
				if(b != 0) L->top = ra + b;
				if(luaD_precall(L, ra, nresults))
				{
					if(nresults >= 0) L->top = ci->top;
					base = ci->u.l.base;
				}
				else
				{
					ci = L->ci;
					ci->callstatus |= CIST_REENTRY;
					goto newframe;
				}
			}
			break;

		case OP_TAILCALL:
			{
				int b = GETARG_B(i);
				if(b != 0) L->top = ra + b;
				lua_assert(GETARG_C(i) - 1 == LUA_MULTRET);
				if(luaD_precall(L, ra, LUA_MULTRET)) base = ci->u.l.base;
				else
				{
					CallInfo *nci = L->ci;
					CallInfo *oci = nci->previous;
					StkId nfunc = nci->func;
					StkId ofunc = oci->func;
					StkId lim = nci->u.l.base + getproto(nfunc)->numparams;
					int aux;
					if(cl->p->sizep > 0) luaF_close(L, oci->u.l.base);
					for(aux = 0; nfunc + aux < lim; aux++) setobjs2s(L, ofunc + aux, nfunc + aux);
					oci->u.l.base = ofunc + (nci->u.l.base - nfunc);
					oci->top = L->top = ofunc + (L->top - nfunc);
					oci->u.l.savedpc = nci->u.l.savedpc;
					oci->callstatus |= CIST_TAIL;
					ci = L->ci = oci;
					lua_assert(L->top == oci->u.l.base + getproto(ofunc)->maxstacksize);
					goto newframe;
				}
			}
			break;

		case OP_RETURN:
			{
				int b = GETARG_B(i);
				if(b != 0) L->top = ra + b - 1;
				if(cl->p->sizep > 0) luaF_close(L, base);
				b = luaD_poscall(L, ra);
				if(!(ci->callstatus & CIST_REENTRY)) return;
				else
				{
					ci = L->ci;
					if(b) L->top = ci->top;
					lua_assert(isLua(ci));
					lua_assert(GET_OPCODE(*((ci)->u.l.savedpc - 1)) == OP_CALL);
					goto newframe;
				}
			}

		case OP_FORLOOP:
			{
				lua_Number step = nvalue(ra + 2);
				lua_Number idx = luai_numadd(L, nvalue(ra), step);
				lua_Number limit = nvalue(ra + 1);
				if(luai_numlt(L, 0, step) ? luai_numle(L, idx, limit) : luai_numle(L, limit, idx))
				{
					ci->u.l.savedpc += GETARG_sBx(i);
					setnvalue(ra, idx);
					setnvalue(ra + 3, idx);
				}
			}
			break;

		case OP_FORPREP:
			{
				const TValue *init = ra;
				const TValue *plimit = ra + 1;
				const TValue *pstep = ra + 2;
				if(!tonumber(init, ra)) luaG_runerror(L, LUA_QL("for") " initial value must be a number");
				else if(!tonumber(plimit, ra + 1)) luaG_runerror(L, LUA_QL("for") " limit must be a number");
				else if(!tonumber(pstep, ra + 2)) luaG_runerror(L, LUA_QL("for") " step must be a number");
				setnvalue(ra, luai_numsub(L, nvalue(ra), nvalue(pstep)));
				ci->u.l.savedpc += GETARG_sBx(i);
			}
			break;

		case OP_TFORCALL:
			{
				StkId cb = ra + 3;
				setobjs2s(L, cb + 2, ra + 2);
				setobjs2s(L, cb + 1, ra + 1);
				setobjs2s(L, cb, ra);
				L->top = cb + 3;
				luaD_call(L, cb, GETARG_C(i), 1);
				base = ci->u.l.base;
				L->top = ci->top;
				i = *(ci->u.l.savedpc++);
				ra = (base + GETARG_A(i));
				lua_assert(GET_OPCODE(i) == OP_TFORLOOP);
				goto l_tforloop;
			}

		case OP_TFORLOOP:
			{
l_tforloop:
				if(!ttisnil(ra + 1))
				{
					setobjs2s(L, ra, ra + 1);
					ci->u.l.savedpc += GETARG_sBx(i);
				}
			}
			break;

		case OP_SETLIST:
			{
				int n = GETARG_B(i);
				int c = GETARG_C(i);
				int last;
				Table *h;
				if(n == 0) n = cast_int(L->top - ra) - 1;
				if(c == 0)
				{
					lua_assert(GET_OPCODE(*ci->u.l.savedpc) == OP_EXTRAARG);
					c = GETARG_Ax(*ci->u.l.savedpc++);
				} ;
				h = hvalue(ra);
				last = ((c - 1) * LFIELDS_PER_FLUSH) + n;
				if(last > h->sizearray) luaH_resizearray(L, h, last);
				for(; n > 0; n--)
				{
					TValue *val = ra + n;
					luaH_setint(L, h, last--, val);
					luaC_barrierback(L, obj2gco(h), val);
				}
				L->top = ci->top;
			}
			break;

		case OP_CLOSURE:
			{
				Proto *p = cl->p->p[GETARG_Bx(i)];
				Closure *ncl = getcached(p, cl->upvals, base);
				if(ncl == ((void *)0)) pushclosure(L, p, cl->upvals, base, ra);
				else setclLvalue(L, ra, ncl);
				{
					luaC_condGC(L, L->top = ra + 1; luaC_step(L); L->top = ci->top;);
					luai_threadyield(L);;
					base = ci->u.l.base;
				}
			}
			break;

		case OP_VARARG:
			{
				int b = GETARG_B(i) - 1;
				int j;
				int n = cast_int(base - ci->func) - cl->p->numparams - 1;
				if(b < 0)
				{
					b = n;
					luaD_checkstack(L, n);
					base = ci->u.l.base;
					ra = (base + GETARG_A(i));
					L->top = ra + n;
				}
				for(j = 0; j < b; j++)
				{
					if(j < n)
					{
						setobjs2s(L, ra + j, base - n + j);
					}
					else
					{
						setnilvalue(ra + j);
					}
				}
			}
			break;
		case OP_EXTRAARG:
			{
				lua_assert(0);
			}
			break;
		}
	}
}

