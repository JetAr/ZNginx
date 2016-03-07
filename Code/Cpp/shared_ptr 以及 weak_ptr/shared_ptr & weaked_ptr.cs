r# 2016-03-07 19:50
r# 1. shared_ptr、weak_ptr概念
shared_ptr与weak_ptr智能指针均是C++ RAII的一种应用，可用于动态资源管理
shared_ptr基于“引用计数”模型实现，多个shared_ptr可指向同一个动态对象，并维护了一个共享的引用计数器，记录了引用同一对象的shared_ptr实例的数量。当最后一个指向动态对象的shared_ptr销毁时，会自动销毁其所指对象(通过delete操作符)。

shared_ptr的默认能力是管理动态内存，但支持自定义的Deleter以实现个性化的资源释放动作。
weak_ptr用于解决“引用计数”模型循环依赖问题，weak_ptr指向一个对象，并不增减该对象的引用计数器

r# 2. shared_ptr的基本操作
涉及到的操作无非是：shared_ptr的创建、拷贝、绑定对象的变更(reset)、shared_ptr的销毁(手动赋值为nullptr或离开作用域)、指定deleter等操作。

(1) shared_ptr的创建。
 可以使用两种方式：一是辅助函数make_shared(会根据传递的参数调用动态对象的构造函数)；二是构造函数(可从原生指针、unique_ptr、另一个shared_ptr创建)
[cpp] view plain copy
/** 通过make_shared辅助函数创建 **/  
auto s_s = make_shared<string>("hello");//动态创建string对象，调用string(const char *str)构造器，并将其“绑定”到新创建的shared_ptr实例  
shared_ptr<int> s_i; //创建指向int类型对象的”空指针“  

/** 通过构造函数构建 **/  
auto s_s2 = s_s;//拷贝构造。引用计数器加1  

int pa = new int(1);  
shared_ptr s_i2(pa); //通过原生指针构造  
unique_ptr u_i(new int(2));
shared_ptr s_i3(u_i); //通过unique_ptr构造，u_i变成“空指针”  
string *ps = s_s.get(); //获取shared_ptr指向的动态对象的指针  

(2) 作为条件。
  若为“空智能指针”，即不指向任何对象，则为false，否则为true
[cpp] view plain copy
shared_ptr<vector<string>> up;  
if (up)  //false
{
    cout << "not null" << endl;
}

(3)定制资源销毁操作deleter
可以通过两种方式指定deleter，一是构造shared_ptr时，二是使用reset方法时
[cpp] view plain copy
void end_connection(connection *p)
{
    disconnect(*p);
}

void f(destination &d /* other parameters */)
{
    connection c = connect(&d);
    shared_ptr<connection> p(&c, end_connection);
    // use the connection
    // when f exits, even if by an exception, the connection will be properly closed
}

(4)重载的operator->, operator *，以及其他辅助操作如unique()、use_count(), get()等成员方法。

r# 3. weak_ptr
weak_ptr用于配合shared_ptr使用，并不影响动态对象的生命周期，即其存在与否并不影响对象的引用计数器。
weak_ptr并没有重载operator->和operator *操作符，因此不可直接通过weak_ptr使用对象。

提供了expired()与lock()成员函数，前者用于判断weak_ptr指向的对象是否已被销毁，后者返回其所指对象的shared_ptr智能指针(对象销毁时返回”空“shared_ptr)。

循环引用的场景：如二叉树中父节点与子节点的循环引用，容器与元素之间的循环引用等

4. shared_ptr的使用场景
对象之间“共享数据”，对象创建与销毁“分离”
放入容器中的动态对象，使用shared_ptr包装，比unique_ptr更合适

管理“动态数组”时，需要制定Deleter以使用delete[]操作符销毁内存，因为shared_ptr并没有针对数组的特化版本（unique_ptr有针对数组的特化版本）
