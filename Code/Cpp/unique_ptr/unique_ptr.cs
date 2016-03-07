r# 1. 智能指针概念

智能指针是基于RAII机制实现的类(模板)，具有指针的行为(重载了operator*与operator->操作符)，可以“智能”地销毁其所指对象。C++11中有unique_ptr、shared_ptr与weak_ptr等智能指针，可以对动态资源进行管理

r# 2. unique_ptr概念

unique_ptr“唯一”拥有其所指对象，同一时刻只能有一个unique_ptr指向给定对象（通过禁止拷贝语义、只有移动语义来实现）。
unique_ptr指针本身的生命周期：从unique_ptr指针创建时开始，直到离开作用域。离开作用域时，若其指向对象，则将其所指对象销毁(默认使用delete操作符，用户可指定其他操作)。
unique_ptr指针与其所指对象的关系：在智能指针生命周期内，可以改变智能指针所指对象，如创建智能指针时通过构造函数指定、通过reset方法重新指定、通过release方法释放所有权、通过移动语义转移所有权。

r# 3. unique_ptr的基本操作：
[cpp] view plain copy
//智能指针的创建
unique_ptr<int> u_i; //创建
u_i.reset(new int(3)); //"绑定”动态对象
unique_ptr<int> u_i2(new int(4));//创建时指定动态对象

//所有权的变化
int *p_i = u_i2.release(); //释放所有权
unique_ptr<string> u_s(new string("abc"));
unique_ptr<string> u_s2 = std::move(u_s);
//所有权转移(通过移动语义)，u_s所有权转移后，变成“空指针”
u_s2=nullptr;//显式销毁所指对象，同时智能指针变为空指针。与u_s2.reset()等价

r# 4. unique_ptr的使用场景

(1) 动态资源的异常安全保证(利用其RAII特性)：
[cpp] view plain copy
void foo()
{
    //不安全的代码
    X *px = new X;
    // do something, exception may occurs
    delete px; // may not go here
}

//z 这种情况倒是确实是一个问题。
[cpp] view plain copy
void foo()
{       
//异常安全的代码。无论是否异常发生，只要px指针成功创建，其析构函数都会被调用，确保动态资源被释放  
    unique_ptr<X> px(new X);  
    // do something,  
}

(2) 返回函数内创建的动态资源
[cpp] view plain copy
unique_ptr<X> foo()
{
    unique_ptr<X> px(new X);

    // do something
    return px; //移动语义
}

(3) 可放在容器中(弥补了auto_ptr不能作为容器元素的缺点)
方式一：
[cpp] view plain copy
vector<unique_ptr<string>> vs { new string{“Doug”}, new string{“Adams”} };

方式二：
[cpp] view plain copy
vector<unique_ptr<string>> v;
unique_ptr<string> p1(new string("abc"));
v.push_back(std::move(p1));//这里需要显式的移动语义，因为unique_ptr并无copy语义

(4) 管理动态数组，因为unique_ptr有unique_ptr<X[]>重载版本，销毁动态对象时调用delete[]
[cpp] view plain copy
unique_ptr<int[]> p (new int[3]{1,2,3});
p[0] = 0;// 重载了operator[]

r# 5. 自定义资源删除操作(Deleter):

unique_ptr默认的资源删除操作是delete/delete[]，若需要，可以进行自定义：
[cpp] view plain copy
void end_connection(connection *p) { disconnect(*p); } //资源清理函数  
unique_ptr<connection, decltype(end_connection)*> //资源清理器的“类型”  
        p(&c, end_connection);// 传入函数名，会自动转换为函数指针  

r# 6 auto_ptr 与 unique_ptr 的区别
在C++11环境下，auto_ptr被看做“遗留的”，他们有如下区别：
auto_ptr有拷贝语义，拷贝后源对象变得无效；unique_ptr则无拷贝语义，但提供了移动语义
auto_ptr不可作为容器元素，unique_ptr可以作为容器元素
auto_ptr不可指向动态数组(尽管不会报错，但不会表现出正确行为)，unique_ptr可以指向动态数组
