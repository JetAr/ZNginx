﻿// Copyright (c) 2013 Justin Stenning
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using SharpDX;
using SharpDX.Windows;
using SharpDX.DXGI;
using SharpDX.Direct3D11;
using SharpDX.D3DCompiler;

using Common;

// Resolve class name conflicts by explicitly stating
// which class they refer to:
using Buffer = SharpDX.Direct3D11.Buffer;

namespace Ch02_01RenderingPrimitives
{
public class QuadRenderer : Common.RendererBase
{
    // The quad vertex buffer
    Buffer quadVertices;
    // The quad index buffer
    Buffer quadIndices;
    // The vertex buffer binding for the quad
    VertexBufferBinding quadBinding;

    /// <summary>
    /// Create any device dependent resources here.
    /// This method will be called when the device is first
    /// initialized or recreated after being removed or reset.
    /// </summary>
    protected override void CreateDeviceDependentResources()
    {
        // Ensure that if already set the device resources
        // are correctly disposed of before recreating
        RemoveAndDispose(ref quadVertices);
        RemoveAndDispose(ref quadIndices);

        // Retrieve our SharpDX.Direct3D11.Device1 instance
        var device = this.DeviceManager.Direct3DDevice;

        // Create a quad (two triangles)
        quadVertices = ToDispose(Buffer.Create(device, BindFlags.VertexBuffer, new[]
        {
            /*  Vertex Position                       Vertex Color */
            new Vector4(0.25f, 0.5f, -0.5f, 1.0f), new Vector4(0.0f, 1.0f, 0.0f, 1.0f), // Top-left
            new Vector4(0.75f, 0.5f, -0.5f, 1.0f), new Vector4(1.0f, 1.0f, 0.0f, 1.0f), // Top-right
            new Vector4(0.75f, 0.0f, -0.5f, 1.0f), new Vector4(1.0f, 0.0f, 0.0f, 1.0f), // Base-right
            new Vector4(0.25f, 0.0f, -0.5f, 1.0f), new Vector4(0.0f, 0.0f, 1.0f, 1.0f), // Base-left
        }));
        quadBinding = new VertexBufferBinding(quadVertices, Utilities.SizeOf<Vector4>() * 2, 0);

        // v0    v1
        // |-----|
        // | \ A |
        // | B \ |
        // |-----|
        // v3    v2
        quadIndices = ToDispose(Buffer.Create(device, BindFlags.IndexBuffer, new ushort[]
        {
            0, 1, 2, // A
            2, 3, 0  // B
        }));
    }

    protected override void DoRender()
    {
        var context = this.DeviceManager.Direct3DContext;

        // Render a quad

        // Tell the IA we are using triangles
        context.InputAssembler.PrimitiveTopology = SharpDX.Direct3D.PrimitiveTopology.TriangleList;
        // Set the index buffer
        context.InputAssembler.SetIndexBuffer(quadIndices, Format.R16_UInt, 0);
        // Pass in the quad vertices (note: only 4 vertices)
        context.InputAssembler.SetVertexBuffers(0, quadBinding);
        // Draw the 6 vertices that make up the two triangles in the quad
        // using the vertex indices
        context.DrawIndexed(6, 0, 0);
        // Note: we have called DrawIndexed so that the index buffer will be used
    }
}
}