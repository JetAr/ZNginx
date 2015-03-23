#ifdef ___TASK_H_CYCLE__
#error Cyclic dependency discovered: task.h
#endif

#ifndef __TASK_H__


#ifndef DOXYGEN

#define __TASK_H__
#define __TASK_H_CYCLE__

/////////////////////////////////////////////////////////////////////////////////
///
///           ----------- WCP (Windows via C++) v.1.0.2.1056 ---------- 
///                                     Beta 3.
/// 
/// 
///                       Microsoft Public License (Ms-PL)
/// 
/// 
/// This license governs use of the accompanying software. If you use the software,
///   you accept this license. If you do not accept the license, do not use the 
///                                     software.
/// 
/// Definitions
/// 
/// The  terms  "reproduce,"  "reproduction," "derivative works," and "distribution"
/// have  the same meaning here as under U.S. copyright law. A "contribution" is the
/// original  software, or any additions or changes to the software. A "contributor"
/// is  any  person  that distributes its contribution under this license. "Licensed
/// patents"   are   a  contributor`s  patent  claims  that  read  directly  on  its
/// contribution.
/// 
/// Grant of Rights
/// 
/// (A) Copyright Grant- Subject to the terms of this license, including the license
/// conditions  and  limitations  in  section  3,  each  contributor  grants  you  a
/// non-exclusive,  worldwide,  royalty-free  copyright  license  to  reproduce  its
/// contribution,  prepare  derivative works of its contribution, and distribute its
/// contribution  or any derivative works that you create. 
/// (B) Patent Grant- Subject
/// to  the  terms of this license, including the license conditions and limitations
/// in   section   3,  each  contributor  grants  you  a  non-exclusive,  worldwide,
/// royalty-free  license  under its licensed patents to make, have made, use, sell,
/// offer  for  sale,  import,  and/or  otherwise dispose of its contribution in the
/// software or derivative works of the contribution in the software.
/// 
/// Conditions and Limitations
/// 
/// (A)  No  Trademark  License-  This  license does not grant you rights to use any
/// contributors' name, logo, or trademarks. 
/// (B) If you bring a patent claim against
/// any  contributor over patents that you claim are infringed by the software, your
/// patent  license from such contributor to the software ends automatically. 
/// (C) If
/// you  distribute  any  portion  of  the  software, you must retain all copyright,
/// patent, trademark, and attribution notices that are present in the software. 
/// (D)
/// If you distribute any portion of the software in source code form, you may do so
/// only  under  this license by including a complete copy of this license with your
/// distribution.  If  you  distribute  any  portion  of the software in compiled or
/// object  code  form,  you  may only do so under a license that complies with this
/// license.  
/// (E)  The  software is licensed "as-is." You bear the risk of using it.
/// The  contributors give no express warranties, guarantees, or conditions. You may
/// have  additional consumer rights under your local laws which this license cannot
/// change.  To the extent permitted under your local laws, the contributors exclude
/// the  implied warranties of merchantability, fitness for a particular purpose and
/// non-infringement.
/// 
/// 
/// 
/// 
/// This   source  code  was compiled and tested in Microsoft Visual Studio 2008. If
/// you found any bug in this source code, please e-mail me to admin@ilyns.com.
/// 
/// Copyright (c) 2009-2010
/// ILYNS. http://www.ilyns.com
/// 
/// Copyright (c) 2009-2010
/// Alexander Stoyan
/// 
/// Follow WCP at:
///      http://wcp.codeplex.com/ - latest releases
///      http://alexander-stoyan.blogspot.com/ - blog about WCP and related stuff
///
/////////////////////////////////////////////////////////////////////////////////
///
///  Revision history:
///      07/25/2009 - Initial release.
///      10/02/2009 - XML comments added.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///      11/24/2009 - Improved perfomance in task parameters handling. Added
///                   conversion safe task to unsafe and vice versa.
///      12/19/2009 - Added support of routines that accept 6 and 7 parameters.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains a task class implementation. The task class is a command
    pattern implementation. A task data cannot be shared between thread 
    safe and thread unsafe tasks.
*/


#include <vector>

#include "any.h"



namespace WCP_NS 
{
    template<class PtrTemplateProvider>
    class basic_task;

    /// <summary>
    /// Thread unsafe task object.
    /// </summary>
    typedef basic_task<shared_ptr_template_provider> task;
    


    /// <summary>
    /// Thread safe task object.
    /// </summary>
    typedef basic_task<safe_shared_ptr_template_provider> safe_task;



    /// <summary>
    /// Represents a runable task object.
    /// </summary>
    /// <param name='PtrTemplateProvider'>Template provider for a smart pointer that holds an task related data.</param>
    /// <remarks>
    /// <b>Example:</b>
    /// <code><pre class='_code'> 
    ///    void fputstds(const wcp::reference&lt;std::string&gt; str, FILE* file)
    ///    { fputs(str.deref().c_str(), file); }
    /// &nbsp;
    ///    void main()
    ///    {
    ///        std::string str;
    ///        wcp::task t1(str, wcp::method_ptr(&std::string::push_back), '$'),
    ///                  t2(str, wcp::method_ptr(&std::string::push_back), 0),
    ///                  t3(fputstds, wcp::reference&lt;std::string&gt;(str), stdout);
    /// &nbsp;
    ///        t1(); t1(); t1();
    ///        t2();
    ///        t3();
    ///    }
    /// </pre></code>
    /// Output is:
    /// <code><pre class='_code'> 
    /// $$$
    /// </pre></code>
    /// </remarks>
    template<class PtrTemplateProvider>
    class basic_task
    {
        // Runable object interface.
        struct abstract_runable
        {
            // Invokes a runable object.
            virtual void operator () () = 0;

            // Runable object destructor.
            virtual ~abstract_runable() = 0 { }

            // Clones a runable object.
            virtual std::auto_ptr<abstract_runable> clone() = 0;
        };


        // Parameter set type.
        typedef std::vector<any> params_t;


        // Runable object for a class method.
        template<class C, class M>
        struct runable_mem : public abstract_runable
        {
            // Constructs a runable object with a reference to an object to call a method for.
            runable_mem(C& obj)
                : obj(obj) { }

            // Object reference to call a method for.
            C& obj;
            
            // Method pointer.
            M fn;

            // Selected invoker.
            void (runable_mem::* invoker)();
            
            // Set of method parameters.
            params_t params;

            // Clones a runable object.
            std::auto_ptr<abstract_runable> clone()
            {
                runable_mem<C, M>* pclone = new runable_mem<C, M>(obj);
                std::auto_ptr<abstract_runable> p(pclone);
                pclone->fn = fn;
                pclone->params = params;
                return p;
            }

            // Invokes an invoker selected.
            virtual void operator () ()
            { (this->*invoker)(); }

            // An invoker for a class method that has no parameters.
            void invoker0()
            { (obj.*fn)(); }

            // An invoker for a class method that has 1 parameter.
            template<class P1>
            void invoker1()
            { 
                (obj.*fn)(params.front().get<P1>()); 
            }

            // An invoker for a class method that has 2 parameters.
            template<class P1, class P2>
            void invoker2()
            { 
                any* ar = &params.front();
                (obj.*fn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>()); 
            }

            // An invoker for a class method that has 3 parameters.
            template<class P1, class P2, class P3>
            void invoker3()
            { 
                any* ar = &params.front();
                (obj.*fn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>()); 
            }

            // An invoker for a class method that has 4 parameters.
            template<class P1, class P2, class P3, class P4>
            void invoker4()
            { 
                any* ar = &params.front();
                (obj.*fn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>(),
                    ar[3].get<P4>()); 
            }

            // An invoker for a class method that has 5 parameters.
            template<class P1, class P2, class P3, class P4, class P5>
            void invoker5()
            { 
                any* ar = &params.front();
                (obj.*fn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>(),
                    ar[3].get<P4>(),
                    ar[4].get<P5>()); 
            }

            // An invoker for a class method that has 6 parameters.
            template<class P1, class P2, class P3, class P4, class P5, class P6>
            void invoker6()
            { 
                any* ar = &params.front();
                (obj.*fn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>(),
                    ar[3].get<P4>(),
                    ar[4].get<P5>(),
                    ar[5].get<P6>()); 
            }

            // An invoker for a class method that has 7 parameters.
            template<class P1, class P2, class P3, class P4, class P5, class P6, class P7>
            void invoker7()
            { 
                any* ar = &params.front();
                (obj.*fn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>(),
                    ar[3].get<P4>(),
                    ar[4].get<P5>(),
                    ar[5].get<P6>(),
                    ar[6].get<P7>()); 
            }
        };

        // Runable object for a function.
        template<class F>
        struct runable_fn : public abstract_runable
        {
            // Function pointer.
            F* pfn;
            
            // An invoker selected.
            void (runable_fn::* invoker)();
            
            // Set of function parameters.
            params_t params;

            // Clones a runable object.
            std::auto_ptr<abstract_runable> clone()
            {
                runable_fn<F>* pclone = new runable_fn<F>();
                std::auto_ptr<abstract_runable> p(pclone);
                pclone->pfn = pfn;
                pclone->params = params;
                return p;
            }

            // Invokes an invoker selected.
            virtual void operator () ()
            { (this->*invoker)(); }

            // An invoker for a function that has no parameters.
            void invoker0()
            { (*pfn)(); }

            // An invoker for a function that has 1 parameter.
            template<class P1>
            void invoker1()
            { 
                (*pfn)(params.front().get<P1>()); 
            }

            // An invoker for a function that has 2 parameters.
            template<class P1, class P2>
            void invoker2()
            { 
                any* ar = &params.front();
                (*pfn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>()); 
            }

            // An invoker for a function that has 3 parameters.
            template<class P1, class P2, class P3>
            void invoker3()
            { 
                any* ar = &params.front();
                (*pfn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>()); 
            }

            // An invoker for a function that has 4 parameters.
            template<class P1, class P2, class P3, class P4>
            void invoker4()
            { 
                any* ar = &params.front();
                (*pfn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>(),
                    ar[3].get<P4>()); 
            }

            // An invoker for a function that has 5 parameters.
            template<class P1, class P2, class P3, class P4, class P5>
            void invoker5()
            { 
                any* ar = &params.front();
                (*pfn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>(),
                    ar[3].get<P4>(),
                    ar[4].get<P5>()); 
            }

            // An invoker for a function that has 6 parameters.
            template<class P1, class P2, class P3, class P4, class P5, class P6>
            void invoker6()
            { 
                any* ar = &params.front();
                (*pfn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>(),
                    ar[3].get<P4>(),
                    ar[4].get<P5>(),
                    ar[5].get<P6>()); 
            }

            // An invoker for a function that has 7 parameters.
            template<class P1, class P2, class P3, class P4, class P5, class P6, class P7>
            void invoker7()
            { 
                any* ar = &params.front();
                (*pfn)(
                    ar[0].get<P1>(),
                    ar[1].get<P2>(),
                    ar[2].get<P3>(),
                    ar[3].get<P4>(),
                    ar[4].get<P5>(),
                    ar[5].get<P6>(),
                    ar[6].get<P7>()); 
            }
        };

        // Smart pointer type.
        typedef typename PtrTemplateProvider
            ::template template_provider<abstract_runable>::type ptr_t;

        // Pointer to a runable object.
        mutable ptr_t runable;

    public:

        /// <summary>
        /// Construct an empty task.
        /// </summary>
        basic_task() { }

        /// <summary>
        /// Copies a pointer to runable object from a task specified.
        /// </summary>
        /// <param name='rhs'>Task object to construct from.</param>
        basic_task(const basic_task& rhs)
            : runable(rhs.runable)
        { }

        /// <summary>
        /// Constructs a task from an object %reference and a pointer 
        // to a class member that has no parameters.
        /// </summary>
        /// <typeparam name='C'>Class type to invoke a method from.</typeparam>
        /// <typeparam name='M'>Method to invoke type.</typeparam>
        /// <param name='obj'>Class object to invoke a method for.</param>
        /// <param name='fn'>Metho to invoke.</param>
        template<class C, class M>
        basic_task(C& obj, M fn)
        { 
            runable_mem<C, M>* p = new runable_mem<C, M>(obj);
            runable.reset(p);
            p->fn = fn;
            p->invoker = &runable_mem<C, M>::invoker0;
        }

        /// <summary>
        /// Constructs a task from an object %reference and 
        /// a pointer to a class member that has 1 parameter.
        /// </summary>
        /// <typeparam name='C'>Class type to invoke a method from.</typeparam>
        /// <typeparam name='M'>Method to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st method parameter type.</typeparam>
        /// <param name='obj'>Class object to invoke a method for.</param>
        /// <param name='fn'>Metho to invoke.</param>
        /// <param name='p1'>1-st parameter.</param>
        template<class C, class M, class P1>
        basic_task(C& obj, M fn, const P1& p1)
        { 
            runable_mem<C, M>* p = new runable_mem<C, M>(obj);
            runable.reset(p);
            p->fn = fn;
            p->invoker = &runable_mem<C, M>::invoker1<P1>;
            p->params.resize(1);
            p->params.front() = p1;
        }

        /// <summary>
        /// Constructs a task from an object %reference and 
        /// a pointer to a class member that has 2 parameters.
        /// </summary>
        /// <typeparam name='C'>Class type to invoke a method from.</typeparam>
        /// <typeparam name='M'>Method to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st method parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd method parameter type.</typeparam>
        /// <param name='obj'>Class object to invoke a method for.</param>
        /// <param name='fn'>Metho to invoke.</param>
        /// <param name='p1'>1-st parameter.</param>
        /// <param name='p2'>2-nd parameter.</param>
        template<class C, class M, class P1, class P2>
        basic_task(C& obj, M fn, const P1& p1, const P2& p2)
        { 
            runable_mem<C, M>* p = new runable_mem<C, M>(obj);
            runable.reset(p);
            p->fn = fn;
            p->invoker = &runable_mem<C, M>::invoker2<P1, P2>;
            p->params.resize(2);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
        }

        /// <summary>
        /// Constructs a task from an object %reference and 
        /// a pointer to a class member that has 3 parameters.
        /// </summary>
        /// <typeparam name='C'>Class type to invoke a method from.</typeparam>
        /// <typeparam name='M'>Method to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st method parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd method parameter type.</typeparam>
        /// <typeparam name='P3'>3-d method parameter type.</typeparam>
        /// <param name='obj'>Class object to invoke a method for.</param>
        /// <param name='fn'>Metho to invoke.</param>
        /// <param name='p1'>1-st parameter.</param>
        /// <param name='p2'>2-nd parameter.</param>
        /// <param name='p3'>3-d parameter.</param>
        template<class C, class M, class P1, class P2, class P3>
        basic_task(C& obj, M fn, const P1& p1, const P2& p2, const P3& p3)
        { 
            runable_mem<C, M>* p = new runable_mem<C, M>(obj);
            runable.reset(p);
            p->fn = fn;
            p->invoker = &runable_mem<C, M>::invoker3<P1, P2, P3>;
            p->params.resize(3);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
        }

        /// <summary>
        /// Constructs a task from an object %reference and 
        /// a pointer to a class member that has 4 parameters.
        /// </summary>
        /// <typeparam name='C'>Class type to invoke a method from.</typeparam>
        /// <typeparam name='M'>Method to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st method parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd method parameter type.</typeparam>
        /// <typeparam name='P3'>3-d method parameter type.</typeparam>
        /// <typeparam name='P4'>4-th method parameter type.</typeparam>
        /// <param name='obj'>Class object to invoke a method for.</param>
        /// <param name='fn'>Metho to invoke.</param>
        /// <param name='p1'>1-st parameter.</param>
        /// <param name='p2'>2-nd parameter.</param>
        /// <param name='p3'>3-d parameter.</param>
        /// <param name='p4'>4-th parameter.</param>
        template<class C, class M, class P1, class P2, 
            class P3, class P4>
        basic_task(C& obj, M fn, const P1& p1, const P2& p2, 
            const P3& p3, const P4& p4)
        { 
            runable_mem<C, M>* p = new runable_mem<C, M>(obj);
            runable.reset(p);
            p->fn = fn;
            p->invoker = &runable_mem<C, M>::invoker4<P1, P2, P3, P4>;
            p->params.resize(4);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
            ar[3] = p4;
        }

        /// <summary>
        /// Constructs a task from an object %reference and 
        /// a pointer to a class member that has 5 parameters.
        /// </summary>
        /// <typeparam name='C'>Class type to invoke a method from.</typeparam>
        /// <typeparam name='M'>Method to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st method parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd method parameter type.</typeparam>
        /// <typeparam name='P3'>3-d method parameter type.</typeparam>
        /// <typeparam name='P4'>4-th method parameter type.</typeparam>
        /// <typeparam name='P5'>5-th method parameter type.</typeparam>
        /// <param name='obj'>Class object to invoke a method for.</param>
        /// <param name='fn'>Metho to invoke.</param>
        /// <param name='p1'>1-st parameter.</param>
        /// <param name='p2'>2-nd parameter.</param>
        /// <param name='p3'>3-d parameter.</param>
        /// <param name='p4'>4-th parameter.</param>
        /// <param name='p5'>5-th parameter.</param>
        template<class C, class M, class P1, class P2, 
            class P3, class P4, class P5>
        basic_task(C& obj, M fn, const P1& p1, const P2& p2, 
            const P3& p3, const P4& p4, const P5& p5)
        { 
            runable_mem<C, M>* p = new runable_mem<C, M>(obj);
            runable.reset(p);
            p->fn = fn;
            p->invoker = &runable_mem<C, M>::invoker5<P1, P2, P3, P4, P5>;
            p->params.resize(5);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
            ar[3] = p4;
            ar[4] = p5;
        }

        /// <summary>
        /// Constructs a task from an object %reference and 
        /// a pointer to a class member that has 6 parameters.
        /// </summary>
        /// <typeparam name='C'>Class type to invoke a method from.</typeparam>
        /// <typeparam name='M'>Method to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st method parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd method parameter type.</typeparam>
        /// <typeparam name='P3'>3-d method parameter type.</typeparam>
        /// <typeparam name='P4'>4-th method parameter type.</typeparam>
        /// <typeparam name='P5'>5-th method parameter type.</typeparam>
        /// <typeparam name='P6'>6-th method parameter type.</typeparam>
        /// <param name='obj'>Class object to invoke a method for.</param>
        /// <param name='fn'>Metho to invoke.</param>
        /// <param name='p1'>1-st parameter.</param>
        /// <param name='p2'>2-nd parameter.</param>
        /// <param name='p3'>3-d parameter.</param>
        /// <param name='p4'>4-th parameter.</param>
        /// <param name='p5'>5-th parameter.</param>
        /// <param name='p6'>6-th parameter.</param>
        template<class C, class M, class P1, class P2, 
            class P3, class P4, class P5, class P6>
        basic_task(C& obj, M fn, const P1& p1, const P2& p2, 
            const P3& p3, const P4& p4, const P5& p5, const P6& p6)
        { 
            runable_mem<C, M>* p = new runable_mem<C, M>(obj);
            runable.reset(p);
            p->fn = fn;
            p->invoker = &runable_mem<C, M>::invoker6<P1, P2, P3, P4, P5, P6>;
            p->params.resize(6);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
            ar[3] = p4;
            ar[4] = p5;
            ar[5] = p6;
        }

        /// <summary>
        /// Constructs a task from an object %reference and 
        /// a pointer to a class member that has 7 parameters.
        /// </summary>
        /// <typeparam name='C'>Class type to invoke a method from.</typeparam>
        /// <typeparam name='M'>Method to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st method parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd method parameter type.</typeparam>
        /// <typeparam name='P3'>3-d method parameter type.</typeparam>
        /// <typeparam name='P4'>4-th method parameter type.</typeparam>
        /// <typeparam name='P5'>5-th method parameter type.</typeparam>
        /// <typeparam name='P6'>6-th method parameter type.</typeparam>
        /// <typeparam name='P7'>7-th method parameter type.</typeparam>
        /// <param name='obj'>Class object to invoke a method for.</param>
        /// <param name='fn'>Metho to invoke.</param>
        /// <param name='p1'>1-st parameter.</param>
        /// <param name='p2'>2-nd parameter.</param>
        /// <param name='p3'>3-d parameter.</param>
        /// <param name='p4'>4-th parameter.</param>
        /// <param name='p5'>5-th parameter.</param>
        /// <param name='p6'>6-th parameter.</param>
        /// <param name='p7'>7-th parameter.</param>
        template<class C, class M, class P1, class P2, 
            class P3, class P4, class P5, class P6, class P7>
        basic_task(C& obj, M fn, const P1& p1, const P2& p2, 
            const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7)
        { 
            runable_mem<C, M>* p = new runable_mem<C, M>(obj);
            runable.reset(p);
            p->fn = fn;
            p->invoker = &runable_mem<C, M>::invoker7<P1, P2, P3, P4, P5, P6, P7>;
            p->params.resize(7);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
            ar[3] = p4;
            ar[4] = p5;
            ar[5] = p6;
            ar[6] = p7;
        }

        /// <summary>
        /// Constructs a task from a pointer to a 
        /// function that has no parameters.
        /// </summary>
        /// <typeparam name='F'>Function to invoke type.</typeparam>
        /// <param name='pfn'>Function to invoke.</param>
        template<class F>
        explicit basic_task(F* pfn)
        { 
            runable_fn<F>* p = new runable_fn<F>();
            runable.reset(p);
            p->pfn = pfn;
            p->invoker = &runable_fn<F>::invoker0;
        }
        
        /// <summary>
        /// Constructs a task from a pointer to 
        /// a function that has 1 parameter.
        /// </summary>
        /// <typeparam name='F'>Function to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st function parameter type.</typeparam>
        /// <param name='pfn'>Function to invoke.</param>
        /// <param name='p1'>1-st function parameter.</param>
        template<class F, class P1>
        basic_task(F* pfn, const P1& p1)
        { 
            runable_fn<F>* p = new runable_fn<F>();
            runable.reset(p);        
            p->pfn = pfn;
            p->invoker = &runable_fn<F>::invoker1<P1>;
            p->params.resize(1);
            p->params.front() = p1;
        }

        /// <summary>
        /// Constructs a task from a pointer to 
        /// a function that has 2 parameters.
        /// </summary>
        /// <typeparam name='F'>Function to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st function parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd function parameter type.</typeparam>
        /// <param name='pfn'>Function to invoke.</param>
        /// <param name='p1'>1-st function parameter.</param>
        /// <param name='p2'>2-nd function parameter.</param>
        template<class F, class P1, class P2>
        basic_task(F* pfn, const P1& p1, const P2& p2)
        { 
            runable_fn<F>* p = new runable_fn<F>();
            runable.reset(p);
            p->pfn = pfn;
            p->invoker = &runable_fn<F>::invoker2<P1, P2>;
            p->params.resize(2);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
        }

        /// <summary>
        /// Constructs a task from a pointer to 
        /// a function that has 3 parameters.
        /// </summary>
        /// <typeparam name='F'>Function to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st function parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd function parameter type.</typeparam>
        /// <typeparam name='P3'>3-d function parameter type.</typeparam>
        /// <param name='pfn'>Function to invoke.</param>
        /// <param name='p1'>1-st function parameter.</param>
        /// <param name='p2'>2-nd function parameter.</param>
        /// <param name='p3'>3-d function parameter.</param>
        template<class F, class P1, class P2, class P3>
        basic_task(F* pfn, const P1& p1, const P2& p2, const P3& p3)
        { 
            runable_fn<F>* p = new runable_fn<F>();
            runable.reset(p);
            p->pfn = pfn;
            p->invoker = &runable_fn<F>::invoker3<P1, P2, P3>;
            p->params.resize(3);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
        }

        /// <summary>
        /// Constructs a task from a pointer to 
        /// a function that has 4 parameters.
        /// </summary>
        /// <typeparam name='F'>Function to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st function parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd function parameter type.</typeparam>
        /// <typeparam name='P3'>3-d function parameter type.</typeparam>
        /// <typeparam name='P4'>4-th function parameter type.</typeparam>
        /// <param name='pfn'>Function to invoke.</param>
        /// <param name='p1'>1-st function parameter.</param>
        /// <param name='p2'>2-nd function parameter.</param>
        /// <param name='p3'>3-d function parameter.</param>
        /// <param name='p4'>4-th function parameter.</param>
        template<class F, class P1, class P2, 
            class P3, class P4>
        basic_task(F* pfn, const P1& p1, const P2& p2, 
            const P3& p3, const P4& p4)
        { 
            runable_fn<F>* p = new runable_fn<F>();
            runable.reset(p);
            p->pfn = pfn;
            p->invoker = &runable_fn<F>::invoker4<P1, P2, P3, P4>;
            p->params.resize(4);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
            ar[3] = p4;
        }

        /// <summary>
        /// Constructs a task from a pointer to 
        /// a function that has 5 parameters.
        /// </summary>
        /// <typeparam name='F'>Function to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st function parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd function parameter type.</typeparam>
        /// <typeparam name='P3'>3-d function parameter type.</typeparam>
        /// <typeparam name='P4'>4-th function parameter type.</typeparam>
        /// <typeparam name='P5'>5-th function parameter type.</typeparam>
        /// <param name='pfn'>Function to invoke.</param>
        /// <param name='p1'>1-st function parameter.</param>
        /// <param name='p2'>2-nd function parameter.</param>
        /// <param name='p3'>3-d function parameter.</param>
        /// <param name='p4'>4-th function parameter.</param>
        /// <param name='p5'>5-th function parameter.</param>
        template<class F, class P1, class P2, 
            class P3, class P4, class P5>
        basic_task(F* pfn, const P1& p1, const P2& p2, 
            const P3& p3, const P4& p4, const P5& p5)
        { 
            runable_fn<F>* p = new runable_fn<F>();
            runable.reset(p);
            p->pfn = pfn;
            p->invoker = &runable_fn<F>::invoker5<P1, P2, P3, P4, P5>;
            p->params.resize(5);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
            ar[3] = p4;
            ar[4] = p5;
        }

        /// <summary>
        /// Constructs a task from a pointer to 
        /// a function that has 6 parameters.
        /// </summary>
        /// <typeparam name='F'>Function to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st function parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd function parameter type.</typeparam>
        /// <typeparam name='P3'>3-d function parameter type.</typeparam>
        /// <typeparam name='P4'>4-th function parameter type.</typeparam>
        /// <typeparam name='P5'>5-th function parameter type.</typeparam>
        /// <typeparam name='P6'>6-th function parameter type.</typeparam>
        /// <param name='pfn'>Function to invoke.</param>
        /// <param name='p1'>1-st function parameter.</param>
        /// <param name='p2'>2-nd function parameter.</param>
        /// <param name='p3'>3-d function parameter.</param>
        /// <param name='p4'>4-th function parameter.</param>
        /// <param name='p5'>5-th function parameter.</param>
        /// <param name='p6'>6-th function parameter.</param>
        template<class F, class P1, class P2, 
            class P3, class P4, class P5, class P6>
        basic_task(F* pfn, const P1& p1, const P2& p2, 
            const P3& p3, const P4& p4, const P5& p5, const P6& p6)
        { 
            runable_fn<F>* p = new runable_fn<F>();
            runable.reset(p);
            p->pfn = pfn;
            p->invoker = &runable_fn<F>::invoker6<P1, P2, P3, P4, P5, P6>;
            p->params.resize(6);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
            ar[3] = p4;
            ar[4] = p5;
            ar[5] = p6;
        }

        /// <summary>
        /// Constructs a task from a pointer to 
        /// a function that has 7 parameters.
        /// </summary>
        /// <typeparam name='F'>Function to invoke type.</typeparam>
        /// <typeparam name='P1'>1-st function parameter type.</typeparam>
        /// <typeparam name='P2'>2-nd function parameter type.</typeparam>
        /// <typeparam name='P3'>3-d function parameter type.</typeparam>
        /// <typeparam name='P4'>4-th function parameter type.</typeparam>
        /// <typeparam name='P5'>5-th function parameter type.</typeparam>
        /// <typeparam name='P6'>6-th function parameter type.</typeparam>
        /// <typeparam name='P7'>7-th function parameter type.</typeparam>
        /// <param name='pfn'>Function to invoke.</param>
        /// <param name='p1'>1-st function parameter.</param>
        /// <param name='p2'>2-nd function parameter.</param>
        /// <param name='p3'>3-d function parameter.</param>
        /// <param name='p4'>4-th function parameter.</param>
        /// <param name='p5'>5-th function parameter.</param>
        /// <param name='p6'>6-th function parameter.</param>
        /// <param name='p7'>7-th function parameter.</param>
        template<class F, class P1, class P2, 
            class P3, class P4, class P5, class P6, class P7>
        basic_task(F* pfn, const P1& p1, const P2& p2, 
            const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7)
        { 
            runable_fn<F>* p = new runable_fn<F>();
            runable.reset(p);
            p->pfn = pfn;
            p->invoker = &runable_fn<F>::invoker7<P1, P2, P3, P4, P5, P6, P7>;
            p->params.resize(7);
            any* ar = &p->params.front();
            ar[0] = p1;
            ar[1] = p2;
            ar[2] = p3;
            ar[3] = p4;
            ar[4] = p5;
            ar[5] = p6;
            ar[6] = p7;
        }

        /// <summary>
        /// Invokes a runable object.
        /// </summary>
        void operator() () const
        {
            if(runable)
                (*runable)();
        }

        /// <summary>
        /// Checks if a task can be run.
        /// </summary>
        /// <returns>true if can be run.</returns>
        operator bool () const
        { return runable; }

        /// <summary>
        /// Copies a runable object pointer from another task.
        /// </summary>
        /// <param name='rhs'>Task to copy from.</param>
        /// <returns>Reference to itself.</returns>
        basic_task& operator = (const basic_task& rhs)
        {
            runable = rhs.runable;
            return *this;
        }

        friend task convert_task_to_unsafe(safe_task);
        friend safe_task convert_task_to_safe(task);
    };

    /// <summary>
    /// Converts safe task to unsafe.
    /// </summary>
    /// <param name='st'>Safe task.</param>
    /// <returns>Unsafe task.</returns>
    inline task convert_task_to_unsafe(safe_task st)
    {
        task t;
        t.runable.reset(st.runable->clone().release());
        return t;
    }

    /// <summary>
    /// Converts unsafe task to safe.
    /// </summary>
    /// <param name='t'>Unsafe task.</param>
    /// <returns>Safe task.</returns>
    inline safe_task convert_task_to_safe(task t)
    {
        safe_task st;
        st.runable.reset(t.runable->clone().release());
        return st;
    }

} // namespace WCP_NS



#undef __TASK_H_CYCLE__
#endif /*__TASK_H__*/
