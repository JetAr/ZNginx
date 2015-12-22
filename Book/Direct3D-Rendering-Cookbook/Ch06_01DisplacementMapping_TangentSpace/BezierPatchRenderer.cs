﻿using System;
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

namespace Ch06_01DisplacementMapping
{
// 4x4 bezier patch (a single patch consists of 16 control points or 9 quads)
public class BezierPatchRenderer: Common.RendererBase
{
    Buffer vertexBuffer;
    VertexBufferBinding vertexBinding;

    protected override void CreateDeviceDependentResources()
    {
        base.CreateDeviceDependentResources();

        RemoveAndDispose(ref vertexBuffer);

        // Retrieve our SharpDX.Direct3D11.Device1 instance
        var device = this.DeviceManager.Direct3DDevice;

        // Patch layout
        // * U ------------>
        // V cp0--cp1--cp2--cp3
        //    |    |    |    |
        // | cp4--cp5--cp6--cp7
        // |  |    |    |    |
        // | cp8--cp9--cp10-cp11
        // V  |    |    |    |
        //   cp12-cp13-cp14-cp15
        //
        // (cp = control point)

        // Create the bezier surface
        // Note: the normals are discarded and instead calculated from the bezier surface during tessellation
        vertexBuffer = ToDispose(Buffer.Create(device, BindFlags.VertexBuffer, new[]
        {
            //          x, y, z        u, v texture coord
            new Vertex(-1, 0, 1,       0, 0),
            new Vertex(-0.34f, 0, 1,   1, 0),
            new Vertex(0.34f, 0.5f, 1, 2, 0),
            new Vertex(1, 2, 1,        3, 0),

            new Vertex(-1, 0, 0.34f,       0, 1),
            new Vertex(-0.34f, 0, 0.34f,   1, 1),
            new Vertex(0.34f, 0.1f, 0.34f, 2, 1),
            new Vertex(1, 0.5f, 0.34f,     3, 1),

            new Vertex(-1, 0, -0.34f,     0, 2),
            new Vertex(-0.34f, 0, -0.34f, 1, 2),
            new Vertex(0.34f, 0, -0.34f,  2, 2),
            new Vertex(1, 0, -0.34f,      3, 2),

            new Vertex(-1, 0, -1,     0, 3),
            new Vertex(-0.34f, 0, -1, 1, 3),
            new Vertex(0.34f, 0, -1,  2, 3),
            new Vertex(1, 0, -1,      3, 3),
        }));
        vertexBinding = new VertexBufferBinding(vertexBuffer, Utilities.SizeOf<Vertex>(), 0);
    }


    protected override void DoRender()
    {
        var context = this.DeviceManager.Direct3DContext;

        // Render a bezier patch

        // Tell the IA we are using a list of patches with 16 control points each
        context.InputAssembler.PrimitiveTopology = SharpDX.Direct3D.PrimitiveTopology.PatchListWith16ControlPoints;
        // Pass in the vertices
        context.InputAssembler.SetVertexBuffers(0, vertexBinding);
        context.Draw(16, 0);

        // (DEBUG) Output the control points as points
        context.HullShader.Set(null);
        context.DomainShader.Set(null);
        context.GeometryShader.Set(null);
        context.VertexShader.Set((this.Target as D3DApp).vertexShader);
        context.InputAssembler.PrimitiveTopology = SharpDX.Direct3D.PrimitiveTopology.PointList;
        // Pass in the vertices
        context.InputAssembler.SetVertexBuffers(0, vertexBinding);
        context.Draw(16, 0);

    }
}
}
