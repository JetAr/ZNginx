#include "d3d_obj.h"

//z 声明类中的 static 变量。
// Declare our static variables
// So why have "mInterface" and "mDevice" be public?  Well we are going to want to
// access those variables by other classes (say for instance if we add a D3D texture class),
// so we keep them public so when we build on this framework, we can access them easily
IDirect3D9* CD3DObj::mInterface = NULL;
IDirect3DDevice9* CD3DObj::mDevice = NULL;
HRESULT CD3DObj::mResult = 0;

//z 2015-10-19 16:59 初始化 d3d object 。
// Initializes our D3D object -- Returns true on success, false otherwise
bool CD3DObj::init(HWND hwnd)
{
    // Create the D3D object, which is needed to create the D3DDevice.
    //z 创建 d3d object，2015-10-19 17:00 可以用来创建 D3DDevice 。
    mInterface = Direct3DCreate9(D3D_SDK_VERSION);

    // Error Check，检查是否创建成功
    if(mInterface == NULL)
        return false;

    //z 参数，3d device 创建方式
    //z 该参数控制了3d device （用于渲染）如何创建。
    // This is the structure that defines how our 3D device (the thing that allows us
    // to render) is going to be created.
    D3DPRESENT_PARAMETERS params = {0}; // Start by zeroing out the parameters

    // There's a bunch of parameters we could set up, however for this tutorial we
    // only care about a few.
    //z 有相当多的参数可以来进行设置，目前我们只关心一小部分。
    params.Windowed = TRUE; // This parameter says if the application runs in a window (TRUE)
    // or is full screen (FALSE)
    //z 后端 buffer 的数量 2015-10-19 17:02
    params.BackBufferCount = 1; // The number of back buffers for the application
    //z D3D 应用如何和 back buffers 进行交互。
    //z 目前只有一个 back buffer，使用的方式是 flip 。相当快，只交换下指向的指针即可。
    params.SwapEffect = D3DSWAPEFFECT_FLIP; // The swap effect flag handles how the D3D application
    // interacts with the back buffers.  By specifying the
    // D3DSWAPEFFECT_FLIP flag what will happen is drawing
    // will occur to our current back buffer, which we
    // choose to only have one.  Once rendering is completed, the
    // buffer is flipped with the current frame buffer and becomes
    // the frame buffer, while the current frame buffer becomes
    // the back buffer.  Flipping is really fast, because it avoids
    // copying the back buffer to the front buffer

    //z 设置为NULL，那么在创建时会使用当前桌面的显示格式。
    params.BackBufferFormat = D3DFMT_UNKNOWN; // Here we set what we want our D3DFORMAT to be.
    // An example format would be D3DFMT_R8G8B8 which
    // is a 24-bit RGB format.  However we choose
    // D3DFMT_UNKNOWN so that when the device is created
    // the format that will be selected is the current
    // desktop display format.

    //z 2015-10-19 17:04 完成了设备参数设置；接着来创建设备。
    // All right we're done setting up our device parameters.  Time to make the
    // device.  So by parameter:
    // D3DADAPTER_DEFAULT -- Specifies which adapter to use.  D3DADAPTER_DEFAULT means always
    //						 use the primary adapter.
    //z 使用何种类型的设备来进行光栅化；这里使用硬件来进行光栅化。
    // D3DDEVTYPE_HAL -- This is the D3DDEVTYPE.  It specifies what the device type will
    //       			 be.  D3DDEVTYPE_HAL says the device will use hardware rasterization.
    
    //z 2015-10-19 17:06 句柄，指向device的focus所属的窗口。哪个窗口拥有这个设备。
    //z 设备得有归属
    // hwnd -- This is the HWND to the window that the focus of the device belongs to.  Basically
    //		   this answers the question "Which window owns this device?"
    //z 通过传递进去的 flag 控制了设备的行为
    // D3DCREATE_HARDWARE_VERTEXPROCESSING -- This is a D3DCREATE flag.  It controls the behavior
    //										  of the device.  This specific flag says, "Do the
    //										  vertex processing in hardware."
    //z 指向 presentation parameters 的地址，指示了设备如何将其呈现到屏幕。
    // &params -- This is the address of the devices "presentation parameters".  It dictates how
    //			  the device will present itself to the screen.  So such things as back buffers,
    //			  refresh rate, and stencil format can be specified in this struct.
    //z 所创建的设备 2015-10-19 17:10
    // &mDevice -- This is the address of a pointer to a IDirect3DDevice9.  This will get
    //				  filled in with the created device.
    //z 通过 3d object 来创建 3d device 。
    mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT,
                                       D3DDEVTYPE_HAL,
                                       hwnd,
                                       D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                       &params,
                                       &mDevice);

    // It's possible we'll get an error because not all vid-cards can handle vertex processing
    // So in the event we do get an error we'll try to make the device again.  We will
    // only change D3DCREATE_HARDWARE_VERTEXPROCESSING to D3DCREATE_SOFTWARE_VERTEXPROCESSING
    // which says, "Do the vertex processing in software"
    //z 如果显卡不具备D3DCREATE_HARDWARE_VERTEXPROCESSING 的能力，检查创建结果
    if(mResult != D3D_OK)
    {
        mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT,
                                           D3DDEVTYPE_HAL,
                                           hwnd,
                                           D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                           &params,
                                           &mDevice);
        if(mResult != D3D_OK)
            return false; // Couldn't create a D3D 9.0 device
    }
    //z 成功创建了 device 
    return true; // We got loaded
}

// Begins the scene
void CD3DObj::begin()
{
    //z 开始 scene
    // This begins our scene.
    mResult = mDevice->BeginScene();
    assert(mResult == D3D_OK);
}

// End the scene and draw it
void CD3DObj::end()
{
    // This ends the scene
    mResult = mDevice->EndScene();
    assert(mResult == D3D_OK);

    // This presents (draws) the next back buffer (we only have one in our case) to the screen
    // By parameter:
    // NULL -- Must be NULL unless a swap chain was created with D3DSWAPEFFECT_COPY
    //		   **NOTE** We used D3DSWAPEFFECT_DISCARD
    // NULL -- Must be NULL unless a swap chain was created with D3DSWAPEFFECT_COPY
    // NULL -- HWND whose client area is taken as the target for this presentation.  If this is
    //		   NULL then the HWND set in the D3DPRESENT_PARAMETERS (set in our init() function)
    //		   is used
    // NULL -- Must be NULL unless a swap chain was created with D3DSWAPEFFECT_COPY
    //z 渲染结束后，将渲染记过呈现到屏幕。 2015-10-19 17:13
    //z 绘制 next back buffer 到屏幕
    mResult = mDevice->Present(NULL, NULL, NULL, NULL);
    assert(mResult == D3D_OK);
}

//z 渲染一组顶点数据
// This renders a list of SVertex type vertices that is "numVerts" long
bool CD3DObj::render(SVertex *vertList, int numVerts)
{
    //z 2015-10-19 17:14 
    //z 在渲染之前我们必须告诉 d3d 我们将使用何种类型的 vertices 。
    //z 传给 device 我们的顶点格式
    // The first thing we have to do before rendering is tell D3D what
    // type of vertices we'll be rendering.  We do this by passing the
    // #define that stipulates our vertex type.
    //z vertices 的格式
    mDevice->SetFVF(SVertexType);

    //z 2015-07-02 13:42 通过使用 d3d vertex buffer，能够提升 rendering 速度。
    // Now we are ready to render.  We can do this with one function call.
    
    //z 2015-10-19 17:16
    //z 函数名中的 up ，表示 “user pointer（用户指针）”。
    //z 一个指向vertices队列的指针（渲染对象）。
    //z 另外一种方式是使用 d3d vertex buffer；使用buffer的形式，速度会更快。
    // You'll notice the "UP" in the function name.  This stands for "user pointer" and
    // means that we are passing D3D a pointer to a list of vertices to be rendered.
    // Optionally we could use a D3D vertex buffer, this is a more specialized class that
    // increases rendering speeds.
    
    //z 使用 vertex arrays 以及 index arrays，然后将之渲染到屏幕。
    // This function takes vertex arrays and index arrays (declared in typical C++ fashion)
    // and renders them to the screen.  By parameter:
    //z 图元的类型
    // D3DPT_TRIANGLELIST -- The type of primitive to render
    //z 多个图元
    // numVerts / 3 -- Number of primitives (in our case triangles) to render
    //z 指向 index array 的指针
    // iList -- Pointer to index array
    //z 指向 vertex array 的指针
    // vertList -- Pointer to our vertex array
    // vList -- Point to the vertex array
    //z 每一个 vertex 的大小
    // sizeof(SVertex) -- Size of an individual vertex
    mResult = mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, numVerts / 3, vertList, sizeof(SVertex));
    return (mResult == D3D_OK); // Returns status of DrawPrimitiveUP()
}

//z 2015-10-19 17:19 将viewport 的颜色设置为一个指定的颜色（具有ARGB值）
// Clears the viewport to a specified ARGB color
bool CD3DObj::clearColor(int color)
{
    // This clears our viewport to the color passed in.
    // By parameter:
    // 0 -- Number of RECT(s) passed in the second parameter.  Since we pass NULL
    //		this MUST be 0
    //z 2015-10-19 17:20 传入NULL表示我们要清除整个 viewport。
    // NULL -- An array of D3DRECT(s) that specify the area(s) to be cleared in the
    //		   viewport.  By passing NULL we're saying "We want to clear the entire viewport"
    //z 我们要清理的 viewport 具体数据类型s
    // D3DCLEAR_TARGET -- This is flag that says what exactly we're clearing in the viewport
    //					  it can be a combination of these three flags:
    //z 2015-10-19 17:22 清理 stencil buffer (模版缓存区)
    //z D3DCLEAR_TARGET （渲染目标的颜色）
    //z D3DCLEAR_ZBUFFER 清理 z buffer 
    //					  D3DCLEAR_STENCIL -- This clears the stencil buffer
    //					  D3DCLEAR_TARGET -- This clears the render targets color
    //					  D3DCLEAR_ZBUFFER -- This clears the depth buffer (Z-Buffer)
    //z 将 render target 渲染为什么颜色
    // color -- The ARGB color to clear the render target to
    //z 
    // 1.0f -- The value to clear the depth buffer to
    //		   **NOTE** Since we didn't specify that flag this parameter is ignored
    // 0 --	The value to clear the stencil buffer to
    //		**NOTE** Since we didn't specify that flag this parameter is ignored
    //z 只清理渲染目标的颜色；至于模板和深度，这里没有考虑。
    mResult = mDevice->Clear(0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0);
    return (mResult == D3D_OK);
}

// Free up all the memory
void CD3DObj::deinit()
{
    //z 使用完毕后，释放所占用的资源
    if(mDevice != NULL)
        mDevice->Release();

    if(mInterface != NULL)
        mInterface->Release();

    // Zero out our D3D interface and device
    mDevice = NULL;
    mInterface = NULL;
}

CD3DObj theD3DObj; // Create our 3D Object
CD3DObj *g3D = &theD3DObj; // Set the global pointer to our 3D Object
