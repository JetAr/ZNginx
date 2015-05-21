//z 2015-05-21 15:53:00 L.224'29220 T3218067532.K F429593811 
//z 验证下项目代码，demo。
class Base
{
public:
	virtual void A() {printf("Base:A()\r\n");}
};

class D1 : public Base
{
public:
	void A() {printf("D1:A()\r\n");}
};

class D2 : public Base
{
public:
	void A() {printf("D2:A()\r\n");}
};

void Callee(Base* pBase)
{
	pBase->A();
}

void Test_Caller()
{
	D1 * pD1 = new D1();
	D2 * pD2 = new D2();

	Callee(pD1);
	Callee(pD2);
}
