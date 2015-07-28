////////////////////////////////////////////////////////////////////////////////
// Filename: quadtreeclass.h
////////////////////////////////////////////////////////////////////////////////
#include "quadtreeclass.h"


QuadTreeClass::QuadTreeClass()
{
    m_vertexList = 0;
    m_parentNode = 0;
}


QuadTreeClass::QuadTreeClass(const QuadTreeClass& other)
{
}


QuadTreeClass::~QuadTreeClass()
{
}


bool QuadTreeClass::Initialize(TerrainClass* terrain, ID3D11Device* device)
{
    int vertexCount;
    float centerX, centerZ, width;


    // Get the number of vertices in the terrain vertex array.
    vertexCount = terrain->GetVertexCount();

    // Store the total triangle count for the vertex list.
    m_triangleCount = vertexCount / 3;

    // Create a vertex array to hold all of the terrain vertices.
    m_vertexList = new VertexType[vertexCount];
    if(!m_vertexList)
    {
        return false;
    }

    // Copy the terrain vertices into the vertex list.
    terrain->CopyVertexArray((void*)m_vertexList);

    // Calculate the center x,z and the width of the mesh.
    CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

    // Create the parent node for the quad tree.
    m_parentNode = new NodeType;
    if(!m_parentNode)
    {
        return false;
    }

    // Recursively build the quad tree based on the vertex list data and mesh dimensions.
    CreateTreeNode(m_parentNode, centerX, centerZ, width, device);

    // Release the vertex list since the quad tree now has the vertices in each node.
    if(m_vertexList)
    {
        delete [] m_vertexList;
        m_vertexList = 0;
    }

    return true;
}


void QuadTreeClass::Shutdown()
{
    // Recursively release the quad tree data.
    if(m_parentNode)
    {
        ReleaseNode(m_parentNode);
        delete m_parentNode;
        m_parentNode = 0;
    }

    return;
}


void QuadTreeClass::Render(FrustumClass* frustum, ID3D11DeviceContext* deviceContext, TerrainShaderClass* shader)
{
    // Reset the number of triangles that are drawn for this frame.
    m_drawCount = 0;

    // Render each node that is visible starting at the parent node and moving down the tree.
    RenderNode(m_parentNode, frustum, deviceContext, shader);

    return;
}


int QuadTreeClass::GetDrawCount()
{
    return m_drawCount;
}


void QuadTreeClass::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
    int i;
    float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;


    // Initialize the center position of the mesh to zero.
    centerX = 0.0f;
    centerZ = 0.0f;

    // Sum all the vertices in the mesh.
    for(i=0; i<vertexCount; i++)
    {
        centerX += m_vertexList[i].position.x;
        centerZ += m_vertexList[i].position.z;
    }

    // And then divide it by the number of vertices to find the mid-point of the mesh.
    centerX = centerX / (float)vertexCount;
    centerZ = centerZ / (float)vertexCount;

    // Initialize the maximum and minimum size of the mesh.
    maxWidth = 0.0f;
    maxDepth = 0.0f;

    minWidth = fabsf(m_vertexList[0].position.x - centerX);
    minDepth = fabsf(m_vertexList[0].position.z - centerZ);

    // Go through all the vertices and find the maximum and minimum width and depth of the mesh.
    for(i=0; i<vertexCount; i++)
    {
        width = fabsf(m_vertexList[i].position.x - centerX);
        depth = fabsf(m_vertexList[i].position.z - centerZ);

        if(width > maxWidth)
        {
            maxWidth = width;
        }
        if(depth > maxDepth)
        {
            maxDepth = depth;
        }
        if(width < minWidth)
        {
            minWidth = width;
        }
        if(depth < minDepth)
        {
            minDepth = depth;
        }
    }

    // Find the absolute maximum value between the min and max depth and width.
    maxX = (float)max(fabs(minWidth), fabs(maxWidth));
    maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

    // Calculate the maximum diameter of the mesh.
    meshWidth = max(maxX, maxZ) * 2.0f;

    return;
}


void QuadTreeClass::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device)
{
    int numTriangles, i, count, vertexCount, index, vertexIndex;
    float offsetX, offsetZ;
    VertexType* vertices;
    unsigned long* indices;
    bool result;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;


    // Store the node position and size.
    node->positionX = positionX;
    node->positionZ = positionZ;
    node->width = width;

    // Initialize the triangle count to zero for the node.
    node->triangleCount = 0;

    // Initialize the vertex and index buffer to null.
    node->vertexBuffer = 0;
    node->indexBuffer = 0;

    // Initialize the vertex array to null.
    node->vertexArray = 0;

    // Initialize the children nodes of this node to null.
    node->nodes[0] = 0;
    node->nodes[1] = 0;
    node->nodes[2] = 0;
    node->nodes[3] = 0;

    // Count the number of triangles that are inside this node.
    numTriangles = CountTriangles(positionX, positionZ, width);

    // Case 1: If there are no triangles in this node then return as it is empty and requires no processing.
    if(numTriangles == 0)
    {
        return;
    }

    // Case 2: If there are too many triangles in this node then split it into four equal sized smaller tree nodes.
    if(numTriangles > MAX_TRIANGLES)
    {
        for(i=0; i<4; i++)
        {
            // Calculate the position offsets for the new child node.
            offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
            offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

            // See if there are any triangles in the new node.
            count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
            if(count > 0)
            {
                // If there are triangles inside where this new node would be then create the child node.
                node->nodes[i] = new NodeType;

                // Extend the tree starting from this new child node now.
                CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device);
            }
        }

        return;
    }

    // Case 3: If this node is not empty and the triangle count for it is less than the max then
    // this node is at the bottom of the tree so create the list of triangles to store in it.
    node->triangleCount = numTriangles;

    // Calculate the number of vertices.
    vertexCount = numTriangles * 3;

    // Create the vertex array.
    vertices = new VertexType[vertexCount];

    // Create the index array.
    indices = new unsigned long[vertexCount];

    // Create the vertex array.
    node->vertexArray = new VectorType[vertexCount];

    // Initialize the index for this new vertex and index array.
    index = 0;

    // Go through all the triangles in the vertex list.
    for(i=0; i<m_triangleCount; i++)
    {
        // If the triangle is inside this node then add it to the vertex array.
        result = IsTriangleContained(i, positionX, positionZ, width);
        if(result == true)
        {
            // Calculate the index into the terrain vertex list.
            vertexIndex = i * 3;

            // Get the three vertices of this triangle from the vertex list.
            vertices[index].position = m_vertexList[vertexIndex].position;
            vertices[index].texture = m_vertexList[vertexIndex].texture;
            vertices[index].normal = m_vertexList[vertexIndex].normal;
            indices[index] = index;

            // Also store the vertex position information in the node vertex array.
            node->vertexArray[index].x = m_vertexList[vertexIndex].position.x;
            node->vertexArray[index].y = m_vertexList[vertexIndex].position.y;
            node->vertexArray[index].z = m_vertexList[vertexIndex].position.z;

            // Increment the indexes.
            index++;
            vertexIndex++;

            // Do the same for the next point.
            vertices[index].position = m_vertexList[vertexIndex].position;
            vertices[index].texture = m_vertexList[vertexIndex].texture;
            vertices[index].normal = m_vertexList[vertexIndex].normal;
            indices[index] = index;
            node->vertexArray[index].x = m_vertexList[vertexIndex].position.x;
            node->vertexArray[index].y = m_vertexList[vertexIndex].position.y;
            node->vertexArray[index].z = m_vertexList[vertexIndex].position.z;
            index++;
            vertexIndex++;

            // Do the same for the next point also.
            vertices[index].position = m_vertexList[vertexIndex].position;
            vertices[index].texture = m_vertexList[vertexIndex].texture;
            vertices[index].normal = m_vertexList[vertexIndex].normal;
            indices[index] = index;
            node->vertexArray[index].x = m_vertexList[vertexIndex].position.x;
            node->vertexArray[index].y = m_vertexList[vertexIndex].position.y;
            node->vertexArray[index].z = m_vertexList[vertexIndex].position.z;
            index++;
        }
    }

    // Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now finally create the vertex buffer.
    device->CreateBuffer(&vertexBufferDesc, &vertexData, &node->vertexBuffer);

    // Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    device->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

    // Release the vertex and index arrays now that the data is stored in the buffers in the node.
    delete [] vertices;
    vertices = 0;

    delete [] indices;
    indices = 0;

    return;
}


int QuadTreeClass::CountTriangles(float positionX, float positionZ, float width)
{
    int count, i;
    bool result;


    // Initialize the count to zero.
    count = 0;

    // Go through all the triangles in the entire mesh and check which ones should be inside this node.
    for(i=0; i<m_triangleCount; i++)
    {
        // If the triangle is inside the node then increment the count by one.
        result = IsTriangleContained(i, positionX, positionZ, width);
        if(result == true)
        {
            count++;
        }
    }

    return count;
}


bool QuadTreeClass::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
    float radius;
    int vertexIndex;
    float x1, z1, x2, z2, x3, z3;
    float minimumX, maximumX, minimumZ, maximumZ;


    // Calculate the radius of this node.
    radius = width / 2.0f;

    // Get the index into the vertex list.
    vertexIndex = index * 3;

    // Get the three vertices of this triangle from the vertex list.
    x1 = m_vertexList[vertexIndex].position.x;
    z1 = m_vertexList[vertexIndex].position.z;
    vertexIndex++;

    x2 = m_vertexList[vertexIndex].position.x;
    z2 = m_vertexList[vertexIndex].position.z;
    vertexIndex++;

    x3 = m_vertexList[vertexIndex].position.x;
    z3 = m_vertexList[vertexIndex].position.z;

    // Check to see if the minimum of the x coordinates of the triangle is inside the node.
    minimumX = min(x1, min(x2, x3));
    if(minimumX > (positionX + radius))
    {
        return false;
    }

    // Check to see if the maximum of the x coordinates of the triangle is inside the node.
    maximumX = max(x1, max(x2, x3));
    if(maximumX < (positionX - radius))
    {
        return false;
    }

    // Check to see if the minimum of the z coordinates of the triangle is inside the node.
    minimumZ = min(z1, min(z2, z3));
    if(minimumZ > (positionZ + radius))
    {
        return false;
    }

    // Check to see if the maximum of the z coordinates of the triangle is inside the node.
    maximumZ = max(z1, max(z2, z3));
    if(maximumZ < (positionZ - radius))
    {
        return false;
    }

    return true;
}


void QuadTreeClass::ReleaseNode(NodeType* node)
{
    int i;


    // Recursively go down the tree and release the bottom nodes first.
    for(i=0; i<4; i++)
    {
        if(node->nodes[i] != 0)
        {
            ReleaseNode(node->nodes[i]);
        }
    }

    // Release the vertex buffer for this node.
    if(node->vertexBuffer)
    {
        node->vertexBuffer->Release();
        node->vertexBuffer = 0;
    }

    // Release the index buffer for this node.
    if(node->indexBuffer)
    {
        node->indexBuffer->Release();
        node->indexBuffer = 0;
    }

    // Release the vertex array for this node.
    if(node->vertexArray)
    {
        delete [] node->vertexArray;
        node->vertexArray = 0;
    }

    // Release the four child nodes.
    for(i=0; i<4; i++)
    {
        if(node->nodes[i])
        {
            delete node->nodes[i];
            node->nodes[i] = 0;
        }
    }

    return;
}


void QuadTreeClass::RenderNode(NodeType* node, FrustumClass* frustum, ID3D11DeviceContext* deviceContext, TerrainShaderClass* shader)
{
    bool result;
    int count, i, indexCount;
    unsigned int stride, offset;


    // Check to see if the node can be viewed, height doesn't matter in a quad tree.
    result = frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f));

    // If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
    if(!result)
    {
        return;
    }

    // If it can be seen then check all four child nodes to see if they can also be seen.
    count = 0;
    for(i=0; i<4; i++)
    {
        if(node->nodes[i] != 0)
        {
            count++;
            RenderNode(node->nodes[i], frustum, deviceContext, shader);
        }
    }

    // If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
    if(count != 0)
    {
        return;
    }

    // Otherwise if this node can be seen and has triangles in it then render these triangles.

    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Determine the number of indices in this node.
    indexCount = node->triangleCount * 3;

    // Call the terrain shader to render the polygons in this node.
    shader->RenderShader(deviceContext, indexCount);

    // Increase the count of the number of polygons that have been rendered during this frame.
    m_drawCount += node->triangleCount;

    return;
}


bool QuadTreeClass::GetHeightAtPosition(float positionX, float positionZ, float& height)
{
    float meshMinX, meshMaxX, meshMinZ, meshMaxZ;


    meshMinX = m_parentNode->positionX - (m_parentNode->width / 2.0f);
    meshMaxX = m_parentNode->positionX + (m_parentNode->width / 2.0f);

    meshMinZ = m_parentNode->positionZ - (m_parentNode->width / 2.0f);
    meshMaxZ = m_parentNode->positionZ + (m_parentNode->width / 2.0f);

    // Make sure the coordinates are actually over a polygon.
    if((positionX < meshMinX) || (positionX > meshMaxX) || (positionZ < meshMinZ) || (positionZ > meshMaxZ))
    {
        return false;
    }

    // Find the node which contains the polygon for this position.
    FindNode(m_parentNode, positionX, positionZ, height);

    return true;
}


void QuadTreeClass::FindNode(NodeType* node, float x, float z, float& height)
{
    float xMin, xMax, zMin, zMax;
    int count, i, index;
    float vertex1[3], vertex2[3], vertex3[3];
    bool foundHeight;


    // Calculate the dimensions of this node.
    xMin = node->positionX - (node->width / 2.0f);
    xMax = node->positionX + (node->width / 2.0f);

    zMin = node->positionZ - (node->width / 2.0f);
    zMax = node->positionZ + (node->width / 2.0f);

    // See if the x and z coordinate are in this node, if not then stop traversing this part of the tree.
    if((x < xMin) || (x > xMax) || (z < zMin) || (z > zMax))
    {
        return;
    }

    // If the coordinates are in this node then check first to see if children nodes exist.
    count = 0;

    for(i=0; i<4; i++)
    {
        if(node->nodes[i] != 0)
        {
            count++;
            FindNode(node->nodes[i], x, z, height);
        }
    }

    // If there were children nodes then return since the polygon will be in one of the children.
    if(count > 0)
    {
        return;
    }

    // If there were no children then the polygon must be in this node.  Check all the polygons in this node to find
    // the height of which one the polygon we are looking for.
    for(i=0; i<node->triangleCount; i++)
    {
        index = i * 3;
        vertex1[0] = node->vertexArray[index].x;
        vertex1[1] = node->vertexArray[index].y;
        vertex1[2] = node->vertexArray[index].z;

        index++;
        vertex2[0] = node->vertexArray[index].x;
        vertex2[1] = node->vertexArray[index].y;
        vertex2[2] = node->vertexArray[index].z;

        index++;
        vertex3[0] = node->vertexArray[index].x;
        vertex3[1] = node->vertexArray[index].y;
        vertex3[2] = node->vertexArray[index].z;

        // Check to see if this is the polygon we are looking for.
        foundHeight = CheckHeightOfTriangle(x, z, height, vertex1, vertex2, vertex3);

        // If this was the triangle then quit the function and the height will be returned to the calling function.
        if(foundHeight)
        {
            return;
        }
    }

    return;
}


bool QuadTreeClass::CheckHeightOfTriangle(float x, float z, float& height, float v0[3], float v1[3], float v2[3])
{
    float startVector[3], directionVector[3], edge1[3], edge2[3], normal[3];
    float Q[3], e1[3], e2[3], e3[3], edgeNormal[3], temp[3];
    float magnitude, D, denominator, numerator, t, determinant;


    // Starting position of the ray that is being cast.
    startVector[0] = x;
    startVector[1] = 0.0f;
    startVector[2] = z;

    // The direction the ray is being cast.
    directionVector[0] =  0.0f;
    directionVector[1] = -1.0f;
    directionVector[2] =  0.0f;

    // Calculate the two edges from the three points given.
    edge1[0] = v1[0] - v0[0];
    edge1[1] = v1[1] - v0[1];
    edge1[2] = v1[2] - v0[2];

    edge2[0] = v2[0] - v0[0];
    edge2[1] = v2[1] - v0[1];
    edge2[2] = v2[2] - v0[2];

    // Calculate the normal of the triangle from the two edges.
    normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
    normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
    normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

    magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
    normal[0] = normal[0] / magnitude;
    normal[1] = normal[1] / magnitude;
    normal[2] = normal[2] / magnitude;

    // Find the distance from the origin to the plane.
    D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));

    // Get the denominator of the equation.
    denominator = ((normal[0] * directionVector[0]) + (normal[1] * directionVector[1]) + (normal[2] * directionVector[2]));

    // Make sure the result doesn't get too close to zero to prevent divide by zero.
    if(fabs(denominator) < 0.0001f)
    {
        return false;
    }

    // Get the numerator of the equation.
    numerator = -1.0f * (((normal[0] * startVector[0]) + (normal[1] * startVector[1]) + (normal[2] * startVector[2])) + D);

    // Calculate where we intersect the triangle.
    t = numerator / denominator;

    // Find the intersection vector.
    Q[0] = startVector[0] + (directionVector[0] * t);
    Q[1] = startVector[1] + (directionVector[1] * t);
    Q[2] = startVector[2] + (directionVector[2] * t);

    // Find the three edges of the triangle.
    e1[0] = v1[0] - v0[0];
    e1[1] = v1[1] - v0[1];
    e1[2] = v1[2] - v0[2];

    e2[0] = v2[0] - v1[0];
    e2[1] = v2[1] - v1[1];
    e2[2] = v2[2] - v1[2];

    e3[0] = v0[0] - v2[0];
    e3[1] = v0[1] - v2[1];
    e3[2] = v0[2] - v2[2];

    // Calculate the normal for the first edge.
    edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
    edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
    edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

    // Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
    temp[0] = Q[0] - v0[0];
    temp[1] = Q[1] - v0[1];
    temp[2] = Q[2] - v0[2];

    determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

    // Check if it is outside.
    if(determinant > 0.001f)
    {
        return false;
    }

    // Calculate the normal for the second edge.
    edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
    edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
    edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

    // Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
    temp[0] = Q[0] - v1[0];
    temp[1] = Q[1] - v1[1];
    temp[2] = Q[2] - v1[2];

    determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

    // Check if it is outside.
    if(determinant > 0.001f)
    {
        return false;
    }

    // Calculate the normal for the third edge.
    edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
    edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
    edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

    // Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
    temp[0] = Q[0] - v2[0];
    temp[1] = Q[1] - v2[1];
    temp[2] = Q[2] - v2[2];

    determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

    // Check if it is outside.
    if(determinant > 0.001f)
    {
        return false;
    }

    // Now we have our height.
    height = Q[1];

    return true;
}
